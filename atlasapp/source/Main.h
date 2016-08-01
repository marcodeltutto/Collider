#ifndef _MAIN_H_
#define _MAIN_H_

/**
\mainpage LHSee with Marmalade 


\section Intro

This is the main page of documentation for the project.

The cross-platform API <a href="http://www.madewithmarmalade.com/">Marmalade</a> is used to package our base-application for multiple platforms including iOS & Android. Marmalade is a proprietary API and for commercial release to say the Apple Marketplace or Google Play Store a license must be purchased (this will be covered by the grant). For development so far I've been using an free `educational license', available to students and academics. The only thing that we can't do with an educational license is commercially release our app (a loading screen informs anyone running the app of this fact). To obtain an educational license, send an email to their customer services desk from an email account with ".ac" in the address saying who you are and that you wish an educational account. 
Note: Probably easier to just get a proper license at this point.


On top of this, we use a free & open-source Game Engine called <a href="http://www.drmop.com/index.php/iwgame-engine/">IwGame</a> to abstract away the raw Marmalade API function calls into a much more usable framework. IwGame is designed to be a game-development platform for Marmalade that uses standard game-design patterns (actors, scenes, resource-managers etc.) and comes with an implementation of the 2-D physics engine Box2D which is abstracted into the IwGame engine as Box2DBody.

IwGame is well documented in a PDF that can be downloaded from <a href="http://www.drmop.com/wp-content/uploads/2012/09/IwGame_v0.370.zip">here</a>; in particular we make use of IwGameScene, IwUI and the HTTP manager.


<b>Marmalade 6.1</b> and <b>IwGame 0.370</b> should be used with the current build.

Once Marmalade is installed, the compiling and packaging potions <b>can only</b> be called from an IDE: either Microsoft <b>Visual Studio</b> or <b>xCode</b> for Mac. 

Once Marmalade is installed, it's scripting language mkb is interpretable by the system. To load a project defined in an mkb script (i.e. GameSceneGL.mkb); clicking on it in the directory should be sufficient to start Visual Studio or xCode.

\section Compiling-and-Packaging

Compiling the project into a binary and packaging this for a particular platform (Android .apk or whatever) are <b>two steps</b> in this workflow. 
\subsection Compiling


TODO: add here the process.


\subsection Packaging

TODO: add here the process.

\section Entry-points

The entry point for the entire application is source/Main.cpp

The patterns do inherit from the model set out in IwGame; namely a Game inherits from CIwGame and makes extensive use of CIwGameScene.

Most of the class defined in source/ can be implemented in XoML. These classes are identifiable as inheriting from IIwGameXomlResource and having a separate class named <class-name>Creator to dynamically instantiate them. The (virtual IIwGameXomlResource)  LoadFromXOML function is called to instantiate the object of this type from XML when an XML element is encountered by the XML parser with the XML tag matching the class name. The XML attributes and child elements are handled by the same LoadFromXOML function.


\section Event-display
The only class that I defined that calls any raw Marmalade API calls is IwGxScene which implements a 3-D graphics pipeline to render ATLAS events in 3-D using Marmalade API function calls. This is quite stable.

\subsection Event-display-animation
The animation of the 3-D event displays is driven by the Animation class's update function which is called once per tick in IwGxScene::Draw.


*/


#endif	// _MAIN_H_
