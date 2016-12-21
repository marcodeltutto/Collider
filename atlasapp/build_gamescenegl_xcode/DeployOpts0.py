# coding=utf-8

###### Options ######

options={}

options["<config>"]=ur'Default'
options["<os>"]=ur'iphone'
options["appdata"]=ur'/Users/deltutto/.config/marmalade/DeployTool'
options["build"]=ur'ARM GCC Release'
options["cmdline"]=['/Applications/Marmalade.app/Contents/s3e/makefile_builder/mkb.py', '/Users/deltutto/My_Collider/atlasapp/GameSceneGL.mkb', '--buildenv=XCODE', '--release', '--arm', '--compiler=gcc', '--deploy-only']
options["configlist"]=ur'Default ,'
options["device"]=ur'"{"localdir}/Deploy.py" device "{"tempdir}/DeployOpts.py" {hasmkb}'
options["dir"]=ur'/Users/deltutto/My_Collider/atlasapp/build_gamescenegl_xcode'
options["folder"]=ur'"{"localdir}/Deploy.py" folder "{"tempdir}/DeployOpts.py" {hasmkb}'
options["hasmkb"]=ur'nomkb'
options["helpfile"]=ur'../docs/Deployment.chm'
options["helppage"]=ur'0'
options["lastdir"]=ur'/Users/deltutto/My_Collider/atlasapp/build_gamescenegl_xcode'
options["mkb"]=ur'/Users/deltutto/My_Collider/atlasapp/GameSceneGL.mkb'
options["mkbdir"]=ur'/Users/deltutto/My_Collider/atlasapp'
options["mkbfile"]=ur'GameSceneGL.mkb'
options["outdir"]=ur'/Users/deltutto/My_Collider/atlasapp/build_gamescenegl_xcode'
options["resdir"]=ur'/Applications/Marmalade.app/Contents/Applications/DeployTool.app/Contents/Resources/'
options["s3edir"]=ur'/Applications/Marmalade.app/Contents/s3e/'
options["stage"]=ur'build'
options["target"]=ur'GameSceneGL'
options["task"]=ur'Default'
options["tasktype"]=ur'Package'

###### Tasks ######

tasks=[]

### Task Default:iphone ###

t={}
tasks.append(t)
t["tasktype"]="Package"
t["<config>"]="Default"
t["os"]="iphone"
t["endDir"]="/Users/deltutto/My_Collider/atlasapp/build_gamescenegl_xcode/deployments/default/iphone/release/arm"
