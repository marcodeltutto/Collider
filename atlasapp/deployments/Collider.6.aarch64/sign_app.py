#!/usr/bin/env python2.7
#
# (C) 2001-2012 Marmalade. All Rights Reserved.
#
# This document is protected by copyright, and contains information
# proprietary to Marmalade.
#
# This file consists of source code released by Marmalade under
# the terms of the accompanying End User License Agreement (EULA).
# Please do not use this program/source code before you have read the
# EULA and have agreed to be bound by its terms.
#
"""
This script takes an iphone .app (folder or zip file) or .ipa (itunes
package) and (re)-signs it with the given signing identity and provisioning
profile.

If the script is run a non-Mac then it can use a remove signing server on a Mac
to do the actual codesign.

It takes two arguments and anoptional third:
   1. the .app folder (or a deployment-produced .app.zip)
   2. The signing identity (or any prefix of it)
   3. (optional) The name of the mobile provisioning profile.
      This should only be specified if using a Distribution identity

Note: If using a .zip file for (1), the zip must have been produced by a
previous deployment. Signing may fail if you zip up a .app file yourself.

Use --help for further commands

For example:
   ./sign_app.py MyApp.app "iPhone Distribution" /path/to/distribution.mobileprovision
   ./sign_app.py MyApp.app "iPhone Development"

As part of the process the script will automatically create an entitlements
file called which is then used during code signing.

The CFBundleIdentifier in the Info.plist file must match that used in the
provisioning profile.  This can be achieved by editing the Info.plist file
directly prior to running this script.  For example, if the Info.plist file
specifies "com.mycorp.myapp" as the CFBundleIdentifier but the provisioning
profile was created for "com.publisher.*" then the plist file should
be edited to contain "com.publisher.myapp" prior to running this script.

The CFBundleExecutable in the Info.plist must match the name of the .app
folder.  This seems to be a requirement of Apple's for uploading the App Store.
"""
import os
import time
import struct
import sys
import stat
import shutil
import tempfile
import socket
import zipfile
import subprocess
import plistlib
from os.path import join
from optparse import OptionParser, make_option
import re
from xml.parsers.expat import ExpatError

protocol_version = 3
__version__ = "0.6"

options = None
skip_exe = False
parent_provisioningprofile = None

def message(msg):
    sys.stderr.write(msg)
    sys.stderr.write('\n')

from plistlib import writePlist, readPlistFromString, writePlistToString

try:
    from biplist import readPlist
except ImportError:
    message("Unable to import biplist. It won't be possible to process binary plist files.")
    from plistlib import readPlist

def trace(msg):
    if not options or options.verbose:
        message(msg)

def error(msg):
    message(msg)
    sys.exit(1)

def make_executable(executable):
    if os.name != 'nt':
        if not os.access(executable, os.X_OK):
            trace("Making %s binary executable" % os.path.basename(executable))
            mode = os.stat(executable)[stat.ST_MODE]
            os.chmod(executable, mode | stat.S_IEXEC)

def parse_provioning_profile(profile_filename):
    PLIST_START = '<?xml'
    PLIST_END = '</plist>'
    with open(profile_filename) as fp:
        data = fp.read()
    xml = data[data.find(PLIST_START) : data.find(PLIST_END) + len(PLIST_END)]
    return readPlistFromString(xml)

def make_entitlements(app_dir, distribution):
    entitlements = {}
    try:
        info_plist = readPlist(os.path.join(app_dir, "Info.plist"))
        app_id = info_plist['CFBundleIdentifier']
        message("Processing %s" % app_id)
        executable = info_plist['CFBundleExecutable']
    except (KeyError, IOError, ExpatError):
        error("Unable to obtain CFBundleIdentifier/CFBundleExecutable from Info.plist")

    basename = os.path.basename(app_dir)
    basename = os.path.splitext(basename)[0]
    if executable != basename:
        error("CFBundleExecutable (%s) must be the same as the .app (%s)" % (executable, basename))

    executable = join(app_dir, executable)
    if not os.path.exists(executable):
        error("CFBundleExecutable (%s) does not exist" % (executable))

    make_executable(executable)

    provisioning_profile_file = os.path.join(app_dir, "embedded.mobileprovision")
    if not os.path.exists(provisioning_profile_file):
        if parent_provisioningprofile:
            message(parent_provisioningprofile)
            provisioning_profile_file = parent_provisioningprofile
    if not os.path.exists(provisioning_profile_file):
        error('Unable to sign without specifying a valid provisioning profile')

    try:
        # provisioning profile is a binary file with embedded .plist xml. Need to find the xml inside the binary before parsing.
        PLIST_START = '<?xml'
        PLIST_END = '</plist>'
        with open(provisioning_profile_file, "rb") as fp:
            data = fp.read()
        xml = data[data.find(PLIST_START) : data.find(PLIST_END) + len(PLIST_END)]
        provisioning_profile = readPlistFromString(xml)
    except (IOError, ExpatError):
        error("Invalid mobile provisioning file %s " % provisioning_profile_file)

    try:
        profile_uid = provisioning_profile['ApplicationIdentifierPrefix'][0]
    except (KeyError, IndexError, IOError):
        error("Failed to find ApplicationIdentifierPrefix in provisioning profile")

    try:
        profile_entitlements = provisioning_profile['Entitlements']
        application_id = profile_entitlements['application-identifier']
    except KeyError:
        error("Failed to find ApplicationIdentifier in provisioning profile")

    if ".*" in application_id:
        message("Using wildcard provisioning profile")
    elif not application_id.endswith(app_id):
        error("App ID in Info.plist is '%s' but provisioning profile is for '%s'" % (app_id, application_id))
    else:
        message("Provisioning profile is for: %s" % application_id)

    if profile_entitlements.get('aps-environment'):
        message("Using push notification environment: %s" % profile_entitlements['aps-environment'])

    # Copy entitlements from the provisioning profile
    entitlements.update(profile_entitlements)

    # Update bundle id (as it could be different when using a wildcard provisioning profile)
    bundle_id = '%s.%s' % (profile_uid, app_id)
    entitlements['application-identifier'] = bundle_id
    entitlements['keychain-access-groups'] = [ bundle_id ]

    return entitlements

def unzip(filename, target='.'):
    message("unzipping '%s' to '%s'" % (filename, os.path.abspath(target)))
    zip = zipfile.ZipFile(filename)
    for info in zip.infolist():
        fullname = join(target, info.filename)
        mode = info.external_attr >> 16L
        data = zip.read(info.filename)
        if stat.S_ISDIR(mode):
            trace(" ... Creating dir: %s" % fullname)
            if not os.path.isdir(fullname):
                os.makedirs(fullname)
        elif stat.S_ISLNK(mode):
            _target = data
            trace(" ... Creating symlink: %s -> %s" % (info.filename, _target))
            if hasattr(os, 'symlink'):
                if not os.path.exists(os.path.dirname(fullname)):
                    fullname2 = fullname.replace("\\", "/")
                    if os.path.exists(os.path.dirname(fullname2)):
                        fullname = fullname2
                    else:
                        error("folder for symlink not found: %s" % fullname)
                os.symlink(_target, fullname)
            else:
                # In the absence of symlinks just write the target
                # filename into the source file.
                # This seems to be what windows unzip.exe does.
                # I know it seems crazy.
                open(fullname, "wb").write(data)
        else:
            trace(" ... Creating file %s" % fullname)
            open(fullname, "wb").write(data)

def unzip_to_tmp(filename):
    tmp_dir = tempfile.mkdtemp(prefix=os.path.basename(filename))
    unzip(filename, tmp_dir)
    return tmp_dir

# adds all files under path into the zip and removes the origionals
def zip_app_dir3(path, app_dir, zip, exe_name, prefix=""):
    if not skip_exe and not os.path.exists(exe_name):
        error("iPhone code not found: %s" % exe_name)
    found_exe = 0

    def add_dir(name, zipname):
        info = zipfile.ZipInfo(zipname + '/')
        st = os.stat(name)
        mode = st[stat.ST_MODE]
        info.external_attr = (mode & 0xFFFF) << 16L
        info.date_time = time.localtime(time.time())
        zip.writestr(info, '')

    # add all the directories first
    add_dir(path, join(prefix, app_dir))
    for root, dirs, files in os.walk(path, topdown=False):
        for name in dirs:
            trace("adding dir %s" % name)
            full_name = join(root, name)
            partial_name = join(prefix, app_dir, full_name[len(path)+1:])
            partial_name = partial_name.replace("\\", "/")
            add_dir(full_name, partial_name)

    # then the files
    for root, dirs, files in os.walk(path, topdown=False):
        for name in files:
            full_name = join(root, name)
            partial_name = join(prefix, app_dir, full_name[len(path)+1:])
            partial_name = partial_name.replace("\\", "/")
            #message("adding to zip: %s"  % partial_name)

            if full_name == exe_name:
                # we need to mark this file as executable in the archive or the OS won't boot it!
                attr = zipfile.ZipInfo(partial_name)
                attr.external_attr = 0100755 << 16L
                attr.create_system = 3
                attr.compress_type = zipfile.ZIP_DEFLATED
                attr.date_time = time.localtime(time.time())
                zip.writestr(attr, read_file(full_name))
                found_exe = 1
            elif os.path.islink(full_name):
                # don't zip symlinks
                pass
            else:
                zip.write(full_name, partial_name)

    if not skip_exe and not found_exe:
        error("iPhone executable not found during deployment: %s" % exe_name)

def zip_app_dir2(app_dir, zip, prefix=""):
    trace("adding app_dir to zip: %s" % (app_dir))
    exename = os.path.basename(app_dir)
    exename = os.path.splitext(exename)[0]
    code = join(app_dir, exename)
    zip_app_dir3(app_dir, exename + ".app", zip, code, prefix)

    # Create CodeResources as a symbolic link within the .app file
    dest = "_CodeSignature/CodeResources"
    if os.path.isdir(join(app_dir, "_CodeSignature")):
        trace("Creating CodeResources symbolic link")
        attr = zipfile.ZipInfo()
        attr.filename = "%s/%s/%s" % (prefix, os.path.basename(app_dir), "CodeResources")
        attr.create_system = 3
        attr.external_attr = 2716663808L # long type of hex val
                                                 # of '0xA1ED0000L'
                                                 # say, symlink attr magic..
        attr.date_time = time.localtime(time.time())
        zip.writestr(attr, dest)

def zip_app_dir(app_dir, zip_name):
    message("Creating .app zip: %s" % zip_name)
    thezip = zipfile.ZipFile(zip_name, 'w', zipfile.ZIP_DEFLATED)
    zip_app_dir2(app_dir, thezip)
    thezip.close()

class SocketWrapper(object):
    XFER_UNIT = 100*1024

    def __init__(self, host, port):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(120.0)
        self.host = host
        self.port = port

    def connect(self):
        self.sock.connect((self.host, self.port))

    def getbytes(self, num):
        ret = ""
        bytes_left = num
        while bytes_left > 0:
            data = self.sock.recv(min(self.XFER_UNIT, bytes_left))
            if not data:
                raise Exception("Couldn't receive data")
            bytes_left = bytes_left - len(data)
            ret += data
        return ret

    def sendbytes(self, data):
        ptr = 0
        dataLen = len(data)
        ret = ""
        toSend = self.XFER_UNIT
        while (dataLen > ptr):
            if (dataLen - ptr < self.XFER_UNIT):
                toSend = dataLen - ptr
            sent = self.sock.send(data[ptr:(ptr+toSend)])
            if not sent:
                raise Exception("Couldn't send data")
            ptr += sent

    def send_object(self, data):
        if data:
            self.sendbytes(struct.pack("<i", len(data)))
            self.sendbytes(data)
        else:
            self.sendbytes(struct.pack("<i", 0))

    def recv_object(self, name=None):
        len = struct.unpack("<i", self.getbytes(4))[0]
        if not len or len < 0:
            return
        return self.getbytes(len)

    def recv_file(self):
        filename = self.recv_object("filename")
        if not filename:
            return None, None
        data = self.recv_object("filedata")
        return filename, data

def read_file(filename, ok_missing=0):
    # we've had all kind of issues with the damn 't' mode being the default
    # for win32.  This function exists to make sure that files are always opened in
    # binary mode
    if ok_missing and not filename:
        return
    return open(filename, 'rb').read()

def do_sign_remote(sign_host, app_dir, sign_id, profile, distribution):
    port = 5445
    if ':' in sign_host:
        sign_host, port = sign_host.split(':')
        port = int(port)

    tmp_zip = app_dir + ".tmp.zip"
    if os.path.exists(tmp_zip):
        os.remove(tmp_zip)
    zip_app_dir(app_dir, tmp_zip)
    tmp_zip_data = open(tmp_zip, "rb").read()

    profile = read_file(profile, 1)

    message("Signing app as '%s' on remote host: %s:%d" % (sign_id, sign_host, port))

    s = SocketWrapper(sign_host, port)
    errmsg = ""

    # send the app over to the mac to get signed using tcp
    try:
        s.connect()
        start = time.time()
        s.sendbytes(struct.pack("<i", -1))
        s.sendbytes(struct.pack("<i", protocol_version))
        s.send_object(tmp_zip_data)
        s.send_object(sign_id)
        s.send_object(profile)
        s.sendbytes(struct.pack("<i", distribution))
        message("sent data to signing server [%.2f]" % (time.time() - start))

        count = 0
        while 1:
            name, data = s.recv_file()
            if not name:
                break
            count += 1
            message("got file: %s [%d]" % (name, len(data)))
            full_name = join(app_dir, name)
            dirname = os.path.dirname(full_name)
            if not os.path.isdir(dirname):
                os.makedirs(dirname)
            open(full_name, "wb").write(data)

        if not count:
            errmsg = 'remote server did not return any files'
        else:
            message("done signing")
    except socket.gaierror, e:
        if e[0] == 11001:
            errmsg = "Could not resolve: " + sign_host
        else:
            errmsg = "Socket: " + e[1]
    #except Exception, e:
        #errmsg = str(e)
    finally:
        os.remove(tmp_zip)
        s.sock.close()

    if errmsg:
        error("remote app signing failed on: %s (%s)" % (sign_host, errmsg))

def get_sign_id(res_dir, app_dir, sign_id):
    # Our codesign port has some extra requirements for local signing
    if sign_id not in ["iPhone Distribution", "iPhone Developer"]:
        error('Windows and Linux only support local signing for "iPhone Developer" and "iPhone Distribution" identities')

    distribution = 0
    if sign_id == "iPhone Distribution":
        distribution = 1

    certificates_path = join(res_dir, "certificates")
    if distribution:
        key_name = "distribution_identity.key"
    else:
        key_name = "developer_identity.key"

    # First look next to .app directory
    sign_key = join(app_dir, "..", key_name)
    sign_key = os.path.normpath(sign_key)

    # Default to "certificates" directory next to this script
    if not os.path.exists(sign_key):
        sign_key = join(certificates_path, key_name)

    # Expect key to be next to cert
    sign_cert = sign_key.replace(".key", ".cer")

    # Contents of these don't vary by application
    sign_apple_root = join(certificates_path, "Apple Root CA.cer")
    sign_apple_wwdrca = join(certificates_path, "AppleWWDRCA.cer")

    if not os.path.exists(sign_apple_root):
        error(("Signing error: Can't find the apple root certificate at %s.\n" +
               "Please see the iPhone deployment guide for how to get setup for iPhone app signing.\n"
               "You need to obtain the Apple Root CA certificate (Apple Root CA.cer) and copy it to\n%s.") % (sign_apple_root, certificates_path))

    if not os.path.exists(sign_apple_wwdrca):
        error(("Signing error: Can't find the Apple WWDR intermediate certificate at %s\n" +
               "Please download AppleWWDRCA.cer from the Certificates area of your iPhone Developer\n" +
               "Program Portal and copy it to %s") % (sign_apple_wwdrca, certificates_path))

    if not os.path.exists(sign_key):
        #if hasattr(settings, "iphone_signing_key"):
        #    error("Can't find specified iphone signing key %s" % settings.iphone_signing_key)
        #else:
        if distribution:
            error(("Signing error: Can't find your distribution signing private key at %s\n" +
                   "Please create this using the iPhone Certificate Request tool, or export it from your Mac") % sign_id)
        else:
            error(("Signing error: Can't find your developer signing private key at %s\n" +
                   "Please create this using the iPhone Certificate Request tool, or export it from your Mac") % sign_id)

    if not os.path.exists(sign_cert):
        #if hasattr(settings, "iphone_signing_key"):
        #    error(("Can't find specified iphone signing certificate %s\n" +
        #           "(Note: you must have a certificate file to match the key. If the keyfile contains both"
        #           "key and certificate, then it can be used as both certificate and key\n"
        #           % settings.iphone_signing_key.replace(".key", ".cer")))
        #else:
        if distribution:
            error(("Signing error: Can't find your distribution signing certificate at %s\n" +
                   "Please download distribution_identity.cer from the Certificates area of your iPhone\n" +
                   "Developer Program Portal and copy it to %s") % (sign_cert, os.path.dirname(sign_cert)))
        else:
            error(("Signing error: Can't find your developer signing certificate at %s\n" +
                   "Please download developer_identity.cer from the Certificates area of your\n" +
                   "iPhone Developer Program Portal and copy it to %s") % (sign_cert, os.path.dirname(sign_cert)))
    return sign_key

def sign_remote(sign_host, app_dir, sign_id, distribution):
    profile = join(app_dir, "embedded.mobileprovision")
    if not os.path.exists(profile):
        profile = None
    do_sign_remote(sign_host, app_dir, sign_id, profile, distribution)

def sign_app(app_dir, sign_id, distribution, extra_entitlements):
    trace("Codesigning %s" % os.path.basename(app_dir))
    if sys.platform == 'darwin':
        root = os.popen('xcode-select -print-path').read().strip()
        if not os.path.exists(root):
            error("xcode-select reported non-existant SDK root: %s" % root)

        codesign_allocate_searchpath = [ 
            os.environ.get('CODESIGN_ALLOCATE',''),
            join(root, 'Platforms', 'iPhoneOS.platform', 'Developer', 'usr', 'bin', 'codesign_allocate'),
            join(root, 'Toolchains', 'XcodeDefault.xctoolchain', 'usr', 'bin', 'codesign_allocate')
        ]
        for codesign_allocate in codesign_allocate_searchpath:
            if os.path.exists(codesign_allocate):
                os.environ['CODESIGN_ALLOCATE'] = codesign_allocate
                break
        if not os.path.exists(codesign_allocate):
            error("codesign_allocate not found")

        codesign = 'codesign'
        password = os.environ.get('S3E_DEPLOY_KEYCHAIN_PASSWORD')
        if password:
            execute(['security', 'unlock-keychain', '-p', password], quiet=1)
    else:
        here = os.path.dirname(os.path.abspath(__file__))
        if os.name == 'nt':
            codesign = join(os.path.dirname(os.path.dirname(here)), "codesign.exe")
        else:
            codesign = join(here, "codesign")

        if not os.path.exists(codesign):
            if os.getenv("S3E_DIR"):
                codesign = join(os.getenv("S3E_DIR"), "deploy", "codesign.exe")
            else:
                error("Local app signing on Windows or Linux requires an SDK installation, with the S3E_DIR environment variable set correctly")

        if os.name == 'nt':
            os.environ['CODESIGN_ALLOCATE'] = os.path.splitext(codesign)[0] + "_allocate.exe"
        else:
            os.environ['CODESIGN_ALLOCATE'] = codesign + "_allocate"

        sign_id = get_sign_id(here, app_dir, sign_id)

    app_dir_abs = os.path.abspath(app_dir)
    cmd = [codesign, '--verbose', '-f', '-s', sign_id, app_dir_abs]

    resource_rules = os.path.join(app_dir_abs, "ResourceRules.plist")
    if os.path.exists(resource_rules) and not distribution:
        cmd += [ '--resource-rules', resource_rules ]

    if 'S3E_CODESIGN_ARGS' in os.environ:
        cmd += os.environ['S3E_CODESIGN_ARGS'].split()

    # Creating a temporary .plist with entitlements (.xcent file) used during signing
    entitlements = make_entitlements(app_dir, distribution)
    if extra_entitlements:
        entitlements.update(extra_entitlements)

    # The temp file is created in binary mode to avoid issues with line endings on PC.
    xcentfile = tempfile.NamedTemporaryFile(suffix='.xcent', delete=False)
    try:
        trace("Creating entitlement file %s" % xcentfile.name)
        try:
            xcentfile.write(writePlistToString(entitlements))
        finally:
            xcentfile.close()
        cmd += ['--entitlements', xcentfile.name]
        execute(cmd, quiet=True, handle_error=True)
    finally:
        os.remove(xcentfile.name)

def sign_local(app_dir, sign_id, distribution, extra_entitlements={}):
    #on linux/windows, sign_id is actually the path to the signing private key

    if distribution:
        message("Signing for distribution (for App Store or AdHoc distribution)")
    else:
        message("Signing for development")

    # parent provisioning profile (for signing app extensions)
    global parent_provisioningprofile
    parent_provisioningprofile = os.path.abspath(os.path.join(app_dir, "embedded.mobileprovision"))

    # sign all plugins
    for root, dirs, files in os.walk(os.path.join(app_dir, 'PlugIns')):
        for _dir in dirs:
            exts=('.app', '.appex')
            if any(_dir.lower().endswith(ext) for ext in exts):
                plugin_app_dir = os.path.join(root, _dir)
                message("Codesigning plugin %s" % os.path.basename(plugin_app_dir))
                sign_app(plugin_app_dir, sign_id, distribution, extra_entitlements)

    # sign the main bundle
    message("Codesigning main bundle: %s" % os.path.basename(app_dir))
    sign_app(app_dir, sign_id, distribution, extra_entitlements)

def get_app_dir(root):
    listing = os.listdir(root)
    if not listing:
        error("no app folder found in expected lcoation: %s" % root)
    for f in listing:
        if f.lower().endswith(".app"):
            return join(root, f)
    error("no app folder found in expected lcoation: %s" % root)

def main():
    global options
    opts = [
         make_option("--host", action="store", help="the Mac host on which to sign the app."),
         make_option('-v', "--verbose", action="store_true", help="be more verbose."),
         make_option('-f', "--force", action="store_true", help="Deprecated option."),
         make_option("--dist",  action="store_true", help="force signing for distribution (this is normally implied in the signing identity)."),
    ]
    usage = """Usage %prog <app_dir_or_zip> [<identity> [<provisioning_profile>]]

For example:

   ./sign_app.py MyApp.app "iPhone Distribution" /path/to/distribution.mobileprovision
   ./sign_app.py MyApp.app "iPhone Development" /path/to/mydevelopment.mobileprovision
"""
    parser = OptionParser(usage=usage, version=("iPhone Signing Tool v%s" % __version__), option_list=opts)
    options, args = parser.parse_args()
    if len(args) < 1 or len(args) > 3:
        error("Usage %s <app_dir_or_zip> [<identity> [<provisioning_profile>]]" % sys.argv[0])

    ipa_name = None
    ipa_dir = None
    tmp_dir = None
    profile = None
    sign_id = 'iPhone Developer'
    app_dir = args[0]

    if len(args) > 1:
        sign_id = args[1]
    if len(args) > 2 and args[2]!='"':
        profile = args[2]

    if 'dist' in sign_id.lower():
        options.dist = 1
    elif options.dist == None:
        options.dist = 0

    if not os.path.exists(app_dir):
        error("the specified app could not be found: '%s'" % app_dir)

    zip_name = app_dir + ".zip"
    ext = os.path.splitext(app_dir)[1].lower()
    if ext == ".zip":
        zip_name = app_dir
        tmp_dir = unzip_to_tmp(zip_name)
        app_dir = get_app_dir(tmp_dir)
        ipa_name = os.path.splitext(zip_name)[0] + ".ipa"
    elif ext == '.ipa':
        ipa_name = app_dir
        tmp_dir = unzip_to_tmp(ipa_name)
        app_dir = join(tmp_dir, "Payload")
        if not os.path.exists(app_dir):
            error("Could not find Payload in the specified .ipa file.")
        app_dir = get_app_dir(app_dir)
        zip_name = os.path.splitext(ipa_name)[0] + ".app.zip"

    # Make sure any existing profile is removed
    profile_embedded = join(app_dir, "embedded.mobileprovision")
    if os.path.exists(profile_embedded):
        os.remove(profile_embedded)

    # remove CodeResouces file as it is created by codesign and must be
    # a symbolic link.  If the .app has been edited on windows then the
    # symbolic link may be have converted into a real file.
    message("Removing existing signatures from %s" % os.path.basename(app_dir))
    for root, dirs, files in os.walk(app_dir):
        for currentFile in files:
            if currentFile.endswith('CodeResources'):
                trace("Removing signature: from %s" % os.path.join(root, currentFile))
                os.remove(os.path.join(root, currentFile))

    # Profile always required for distribution, required to make some some entitlements work in development
    if not profile:
        if options.dist:
            error("provisioning profile must be specified when siging for distribution")
    else:
        shutil.copyfile(profile, join(app_dir, "embedded.mobileprovision"))

    if options.host:
        do_sign_remote(options.host, app_dir, sign_id, profile, options.dist)
    else:
        sign_local(app_dir, sign_id, options.dist)

    if os.path.exists(zip_name):
        os.remove(zip_name)

    zip_app_dir(app_dir, zip_name)

    if ipa_name:
        itunes_artwork = os.path.join(app_dir, '..', '..', 'iTunesArtwork')
        create_ipa(ipa_name, app_dir, itunes_artwork)

     # if no exceptions occur and we get this far then remove the temporary dir
    if tmp_dir and os.path.exists(tmp_dir):
        shutil.rmtree(tmp_dir)

def create_ipa(ipa_file, app_dir, artwork=None):
    message("Creating ipa: %s" % ipa_file)
    thezip = zipfile.ZipFile(ipa_file, 'w', zipfile.ZIP_DEFLATED)
    if artwork and os.path.exists(artwork):
        thezip.write(artwork, "iTunesArtwork")
    zip_app_dir2(app_dir, thezip, 'Payload')
    thezip.close()


def execute(cmd, quiet=False, handle_error=True):
    try:
        output = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as grepexc:
        message(''.join(cmd))
        if handle_error:
            message("Error code %d %s" % (grepexc.returncode, grepexc.output))
        else:
            error("Error code %d %s" % (grepexc.returncode, grepexc.output)) 

    if not quiet:
        message(cmd)
        message(output)

if __name__ == "__main__":
    main()
