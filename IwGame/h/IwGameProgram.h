// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. You must ensure that this whole notice is present in all files and derivatives, so the reader knows its origin.
// If you use this engine in your product then please ensure that you credit IwGame's usage appropriately. Please see http://www.drmop.com/index.php/iwgame-engine/ for licensing details and support
//
//

#if !defined(_IW_GAME_PROGRAM_H_)
#define _IW_GAME_PROGRAM_H_

#include "IwGameUtil.h"
#include "IwGameString.h"
#include "IwGameXoml.h"

class CIwGameProgram;
class CIwGameProgramManager;

// 
// 
//
//
// IIwGameCommandExecutor - Common base classs used by objects that execute a list of CIwGameCommand's
// 
// 
// 
//
class IIwGameCommandExecutor
{
public:
	// Public access to iteration
	typedef CIwList<IIwGameCommandExecutor*>::iterator _Iterator;
	_Iterator				begin()		{ return Commands.begin(); }
	_Iterator				end()		{ return Commands.end(); }

protected:
	// Properties
	CIwGameProgram*						Program;			// Parent container program
	IIwGameCommandExecutor*				Parent;				// Parent command executor
	CIwList<IIwGameCommandExecutor*>	Commands;			// A collection of commands
	int									CurrentCommand;		// Index of currently executing command
	bool								Parallel;			// if true then child commands will be executed in parallel
	unsigned int						Conditions;			// Name of conditions variable that defines a set of conditions that must be met for this command to be executed
	CIwGameString						_Conditions;
	bool								Not;				// if true then result of condition will be inversed
#if defined (_DEBUG)
	CIwGameString						CommandName;		// Command name
#endif	// _DEBUG
	unsigned int						CommandNameHash;	// Command name hash
	bool								Initialised;		// Initialised state
	bool								Finished;			// Command has finished and is in a waiting state
	int									ReturnValue;		// Return value
	IIwGameCommandExecutor*				IfReturn;			// Command that we check return value of
	int									IfReturnValue;		// Value to check return again against
	int									CommandIndex;		// Command index in parent program

public:
	void					setProgram(CIwGameProgram* program)			{ Program = program; }
	CIwGameProgram*			getProgram()								{ return Program; }
	void					setParent(IIwGameCommandExecutor* parent)	{ Parent = parent; }
	IIwGameCommandExecutor*	getParent()									{ return Parent; }
	void					setCommandName(const char* name)
	{
#if defined (_DEBUG)
		CommandName = name;
#endif	// _DEBUG
		CommandNameHash = CIwGameString::CalculateHash(name);
	}
#if defined (_DEBUG)
	CIwGameString&			getCommandName()					{ return CommandName; }
#endif	// _DEBUG
	unsigned int			getCommandNameHash()				{ return CommandNameHash; }
	void					setInitialised(bool init)			{ Initialised = init; }
	bool					isInitialised() const				{ return Initialised; }
	void					setFinished(bool finished)			{ Finished = finished; }
	bool					isFinished() const					{ return Finished; }
	void					setParallel(bool parallel)			{ Parallel = parallel; }
	bool					isParallel() const					{ return Parallel; }
	void					addCommand(IIwGameCommandExecutor* command);
	bool					removeCommand(IIwGameCommandExecutor* command);
	IIwGameCommandExecutor*	findCommand(unsigned int name_hash);
	IIwGameCommandExecutor*	findCommand(const char* name);
	bool					gotoCommand();
	virtual void			restart();
	void					clearCommands();
	bool					nextCommand();
	void					setCurrentCommand(IIwGameCommandExecutor* command);
	void					setCurrentCommand(int index);
	int						getReturnValue() const				{ return ReturnValue; }
	IIwGameCommandExecutor*	getIfReturn()						{ return IfReturn; }
	void					setIfReturn(IIwGameCommandExecutor* command, int value);
	int						getIfReturnValue() const			{ return IfReturnValue; }
	void					setCommandIndex(int index)			{ CommandIndex = index; }
	int						getCommandIndex() const				{ return CommandIndex; }
	void					setConditions(const char* cond, bool _not = false)
	{
		_Conditions = cond;
		Conditions = IW_GAME_HASH(cond);
		Not = _not;
	}
	unsigned int			getConditions()	const	{ return Conditions; }

	// Properties end
protected:
public:
	IIwGameCommandExecutor() : Parent(NULL), Initialised(false), Parallel(false), Finished(false), CommandNameHash(0), Program(NULL), ReturnValue(0), IfReturnValue(0), IfReturn(NULL), Conditions(0), Not(false)	{ CurrentCommand = 0; CommandIndex = -1; }
	virtual ~IIwGameCommandExecutor()							{ clearCommands(); }

	virtual void			Init() { Finished = false; }		// Initialise the command
	virtual bool			Execute(float dt) = 0;				// Execute the command
	void					Uninit();							// Unitialises this command all children

	// Utility
	void					RebuildCommandIndices();
};

//
//
//
//
// CIwGameCommand - A program command - Derive program commands from CIwGameCommand so they can be added to a CIwGameProgram or another CIwGameCommand derived command
//
//
//
//
class CIwGameCommand : public IIwGameCommandExecutor
{
public:
	// Public access to iteration

	// Properties
protected:
	CIwGameString			Params[5];					// Commands parameters
	CIwGameXomlVariable*	Variables[5];				// Cached variable parameters (prevents continually searching for variables)
public:
	void					setParameter(int index, CIwGameString& data)	{ Params[index] = data; }
	void					setParameter(int index, const char* data)		{ Params[index] = data; }
	// Properties End
public:
	CIwGameCommand() : IIwGameCommandExecutor() { for (int t = 0; t < 5; t++) Variables[t] = (CIwGameXomlVariable*)1; } // Chances of pointer at address 1 is none existent 
	CIwGameString&		getParameter1(IIwGameXomlResource* parent);
	CIwGameString&		getParameter2(IIwGameXomlResource* parent);
	CIwGameString&		getParameter3(IIwGameXomlResource* parent);
	CIwGameString&		getParameter4(IIwGameXomlResource* parent);
	CIwGameString&		getParameter5(IIwGameXomlResource* parent);
};

// 
// 
//
//
// CIwGameCommandCreator - Base class that is used by classes that create an instance of a type of command
//
//
//
//
class CIwGameCommandCreator
{
#if defined(_DEBUG)
	CIwGameString			CommandName;				// Name of command
#endif
	unsigned int			CommandNameHash;			// Hashed name of class
public:
	void					setCommandName(const char* name)
	{
#if defined(_DEBUG)
		CommandName = name;
#endif
		CommandNameHash = CIwGameString::CalculateHash(name);
	}
	unsigned int			getCommandNameHash() const				{ return CommandNameHash; }
#if defined(_DEBUG)
	CIwGameString&			getCommandtName()						{ return CommandName; }
#endif
public:
	virtual CIwGameCommand* CreateInstance() = 0;
};


// 
// 
//
//
// IIwGameProgram - Manages a collection of commands (a program)
//
//
//
//
class CIwGameProgram : public IIwGameXomlResource, public IIwGameCommandExecutor
{
public:
	enum CIwGameProgram_Status
	{
		PS_Stopped, 
		PS_Running, 
		PS_Paused, 
	};

protected:
	// Properties
	CIwGameProgramManager*		Manager;			// Parent program manager
	CIwGameProgram_Status		Status;				// Status of the program
	IIwGameCommandExecutor*		LastCommand;		// Last executed command
	CIwGameProgram*				Caller;				// If this program was called from another then Caller represents the calling program
public:
	void					setManager(CIwGameProgramManager* manager)		{ Manager = manager; }
	CIwGameProgramManager*	getManager()									{ return Manager; }
	void					setCaller(CIwGameProgram* program)				{ Caller = program; }
	CIwGameProgram*			getCaller()										{ return Caller; }
	void					stop()											{ Status = PS_Stopped; }
	void					start()											{ Status = PS_Running; }
	void					pause()											{ Status = PS_Paused; }
	void					restart();
	bool					isRunning() const								{ return Status == PS_Running; }
	bool					isPaused() const								{ return Status == PS_Paused; }
	bool					isStopped() const								{ return Status == PS_Stopped; }
	void					call(CIwGameProgram* program);
	void					returnToCaller();
	void					gotoNextCommand();
	void					setLastCommand(IIwGameCommandExecutor* command)	{ LastCommand = command; }
	IIwGameCommandExecutor*	getLastCommand()								{ return LastCommand; }

	// Properties end
protected:
	bool					ProcessCommands(CIwGameCommand* parent, bool load_children, CIwGameXmlNode* node);

public:
	CIwGameProgram() : IIwGameXomlResource(), Status(PS_Paused), Manager(NULL), LastCommand(NULL), Caller(NULL) { setClassType("program"); CurrentCommand = 0; }
	~CIwGameProgram() { clearCommands(); }

	bool					Execute(float dt);

	// Implementation of IIwGameXomlResource interface
	bool					LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);
};

//
//  CIwGameProgramCreator - Creates an instance of a program
//
class CIwGameProgramCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameProgramCreator()
	{
		setClassName("program");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameProgram(); }
};

// 
// 
//
//
// CIwGameProgramManager - Manages a collection of programs
//
//
//
//
class CIwGameProgramManager
{
public:
	// Public access to iteration
	typedef CIwList<CIwGameProgram*>::iterator _Iterator;
	_Iterator				begin()		{ return Programs.begin(); }
	_Iterator				end()		{ return Programs.end(); }

protected:
	// Properties
	IIwGameXomlResource*	Parent;					// Parent container scene
	CIwGameProgram*			PriorityProgram;		// This program has priority and is ran before all other programs
	CIwList<CIwGameProgram*> Programs;				// A collection of programms
public:
	void					setPriorityProgram(CIwGameProgram* program)	{ PriorityProgram = program; }
	CIwGameProgram*			getPriorityProgram()						{ return PriorityProgram; }
	void					addProgram(CIwGameProgram* Program);
	bool					removeProgram(CIwGameProgram* Program);
	CIwGameProgram*			findProgram(unsigned int name_hash);
	CIwGameProgram*			findProgram(const char* name);
	void					clearPrograms();
	void					stopAll();
	void					startAll();
	void					pauseAll();
	void					setParent(IIwGameXomlResource* scene)		{ Parent = scene; }
	IIwGameXomlResource*	getParent()									{ return Parent; }
	// Properties end
protected:

public:
	CIwGameProgramManager() : PriorityProgram(NULL), Parent(NULL) {}
	~CIwGameProgramManager() { clearPrograms(); }

	void					Execute(float dt);

	// Utility
	static CIwGameProgram*	FindProgram(const char* name, IIwGameXomlResource* container);
	static CIwGameProgram*	FindProgram(unsigned int name_hash, IIwGameXomlResource* container);

};

// 
// 
//
//
// CIwGamePrograms - CIwGamePrograms is the main controller responsible for storing command creators and creating instances of commands
// 
// 
// 
//
#define IW_GAME_PROGRAMS	CIwGamePrograms::getInstance()
class CIwGamePrograms
{
public:
	CDEFINE_SINGLETON(CIwGamePrograms)

	// Public access to command creator iteration
	typedef CIwList<CIwGameCommandCreator*>::iterator _Iterator;
	_Iterator				begin() { return CommandCreators.begin(); }
	_Iterator				end() { return CommandCreators.end(); }
protected:
	// Properties
public:
	void						addCommand(CIwGameCommandCreator* creator)
	{
#if defined(_DEBUG)
		if (findCreator(creator->getCommandNameHash()))
		{
			CIwGameError::LogError("Warning: Command creator already exists in command creator list - ", creator->getCommandtName().c_str());
			delete creator;
			return;
		}
#endif	//(_DEBUG)
		CommandCreators.push_back(creator);
	}
	CIwGameCommandCreator*		findCreator(unsigned int name_hash);
	CIwGameCommandCreator*		findCreator(const char* name);
	// Properties end

protected:
	CIwList<CIwGameCommandCreator*> CommandCreators;

public:
	void			Init();
	void			Release();
};

//
//
//
// CIwGameCommandNop - Nop command - This command does nothing and is used for commands with no method
// 
// 
//
class CIwGameCommandNop : public CIwGameCommand
{
public:
	void			Init();
	bool			Execute(float dt);
};
class CIwGameCommandNopCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandNopCreator()
	{
		setCommandName("nop");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandNop(); }
};

//
//
//
// CIwGameCommandGoto - Goto command - This command changed execution from the current command to the spcific command
// 
// 
//
class CIwGameCommandGoto : public CIwGameCommand
{
public:
	bool			Execute(float dt);
};
class CIwGameCommandGotoCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandGotoCreator()
	{
		setCommandName("goto");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandGoto(); }
};

//
//
//
// CIwGameCommandStop - Stop command - This command stops execution of the program
// 
// 
//
class CIwGameCommandStop : public CIwGameCommand
{
public:
	bool			Execute(float dt)
	{
		if (!IIwGameCommandExecutor::Execute(dt))
			return false;
		Program->stop();
		return false;
	}
};
class CIwGameCommandStopCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandStopCreator()
	{
		setCommandName("stop");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandStop(); }
};

//
//
//
// CIwGameCommandPriority - Priority command - This command changes the current program that has priority exection
// 
// 
//
class CIwGameCommandPriority : public CIwGameCommand
{
public:
	bool			Execute(float dt);
};
class CIwGameCommandPriorityCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandPriorityCreator()
	{
		setCommandName("priority");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandPriority(); }
};

//
//
//
// CIwGameCommandCall - Call command - Calls another program, pausing the current program
// 
// 
//
class CIwGameCommandCall : public CIwGameCommand
{
public:
	bool			Execute(float dt);
};
class CIwGameCommandCallCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandCallCreator()
	{
		setCommandName("call");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandCall(); }
};

//
//
//
// CIwGameCommanReturn - Return command - Returns another program, pausing the current program
// 
// 
//
class CIwGameCommandReturn : public CIwGameCommand
{
public:
	bool			Execute(float dt)
	{
		if (!IIwGameCommandExecutor::Execute(dt))
			return false;

		Program->returnToCaller();

		return false;
	}
};
class CIwGameCommandReturnCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandReturnCreator()
	{
		setCommandName("return");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandReturn(); }
};

//
//
//
// CIwGameCommandRunActions - Run Actions - Applies an actions list to a scene or actor. Param 1 - action to execute, Param 2 - Actor or scene to apply actions, Param3 - Scene in which actor lives (optional)
// 
// 
//
class CIwGameCommandRunActions : public CIwGameCommand
{
public:
	bool			Execute(float dt);
};
class CIwGameCommandRunActionsCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandRunActionsCreator()
	{
		setCommandName("run_actions");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandRunActions(); }
};

//
//
//
// CIwGameCommandSetProperty - Set property - Sets a property of an actor. Param 1 - Property to set, Param2 - Property value,  Param 3 - Actor whos property to change, Param4 - Scene in which actor lives (optional)
// 
// 
//
class CIwGameCommandSetProperty : public CIwGameCommand
{
public:
	bool			Execute(float dt);
};
class CIwGameCommandSetPropertyCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandSetPropertyCreator()
	{
		setCommandName("set_property");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandSetProperty(); }
};

//
//
//
// CIwGameCommandAddVariable - Add variable - Adds a value onto a variable. Param 1 - variable to modify, Param2 - Value to add,  Param3 - Scene in which variable lives (optional)
// 
// 
//
class CIwGameCommandAddVariable : public CIwGameCommand
{
public:
	bool			Execute(float dt);
};
class CIwGameCommandAddVariableCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandAddVariableCreator()
	{
		setCommandName("add_var");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandAddVariable(); }
};

//
//
//
// CIwGameCommandSetVariable - Set variable - Sets a variables value. Param 1 - variable to set, Param2 - Variables value,  Param3 - Scene in which variable lives (optional)
// 
// 
//
class CIwGameCommandSetVariable : public CIwGameCommand
{
public:
	bool			Execute(float dt);
};
class CIwGameCommandSetVariableCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandSetVariableCreator()
	{
		setCommandName("set_var");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandSetVariable(); }
};

//
//
//
// CIwGameCommandGetVariable - Get variable - Gets a variables value. Param 1 - variable to get, Param3 - Scene in which variable lives (optional)
// 
// 
//
class CIwGameCommandGetVariable : public CIwGameCommand
{
public:
	bool			Execute(float dt);
};
class CIwGameCommandGetVariableCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandGetVariableCreator()
	{
		setCommandName("get_var");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandGetVariable(); }
};

//
// CIwGameCommandIfVar - Checks variable against a value - Param1 - variable to check, Param2 - operator, Param3 - value, Param4 - Scene where variable is located (optional)
//
class CIwGameCommandIfVar : public CIwGameCommand
{
public:
	bool			Execute(float dt);
};
class CIwGameCommandIfVarCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandIfVarCreator()
	{
		setCommandName("if_var");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandIfVar(); }
};

//
// CIwGameCommandWaitVarIsValue - Wait for a variable to becoem a specific value - Param1 - variable to check, Param2 - Value that variable should be to continue, Param3 - Scene where variable is located (optional)
//
class CIwGameCommandWaitVarIsValue : public CIwGameCommand
{
public:
	bool			Execute(float dt);
};
class CIwGameCommandWaitVarIsValueCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandWaitVarIsValueCreator()
	{
		setCommandName("wait_var_is_value");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandWaitVarIsValue(); }
};

//
// CIwGameCommandCallScript - Calls a script - Param1 - script function to call, Param2 - scene to pass, Param3, Param4, Param5 - Params to pass to script function
//
class CIwGameCommandCallScript : public CIwGameCommand
{
public:
	bool			Execute(float dt);
};
class CIwGameCommandCallScriptCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandCallScriptCreator()
	{
		setCommandName("call_script");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandCallScript(); }
};

//
// CIwGameCommandCallGlobalScript - Calls a global script - Param1 - script function to call, Param2, Param3, Parm4 - Params to pass to script function
//
class CIwGameCommandCallGlobalScript : public CIwGameCommand
{
public:
	bool			Execute(float dt);
};
class CIwGameCommandCallGlobalScriptCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandCallGlobalScriptCreator()
	{
		setCommandName("call_global_script");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandCallGlobalScript(); }
};

//
// CIwGameCommandInline - Calls a script - Param1 - script to run, Param2 - scene to run script in, if not supplied then global script space is used
//
class CIwGameCommandInline : public CIwGameCommand
{
public:
	bool			Execute(float dt);
};
class CIwGameCommandInlineCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandInlineCreator()
	{
		setCommandName("inline");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandInline(); }
};

//
// CIwGameCommandFromTemplate - Instantiates a template - Param1 - template name, Param2 - Parameters to pass to template (separated by colon character), Param3 - Scene to instantiate template into (optional)
//
class CIwGameCommandFromTemplate : public CIwGameCommand
{
public:
	bool			Execute(float dt);
};
class CIwGameCommandFromTemplateCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandFromTemplateCreator()
	{
		setCommandName("from_template");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandFromTemplate(); }
};

//
// CIwGameCommandMusic - Play or stop music, Param1 - command (play or stop), Param2 - music file name to play, Param3 - Repeat count (0 play forever)
//
class CIwGameCommandMusic : public CIwGameCommand
{
public:
	bool			Execute(float dt);
};
class CIwGameCommandMusicCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandMusicCreator()
	{
		setCommandName("music");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandMusic(); }
};

//
// CIwGameCommandSound - Play sound effect, Param1 - Sound effect name to play, Param2 - volume, Param3 - pitch, Param4 - pan, Param5 - specific scene (optional)
//
class CIwGameCommandSound : public CIwGameCommand
{
public:
	bool			Execute(float dt);
};
class CIwGameCommandSoundCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandSoundCreator()
	{
		setCommandName("sound");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandSound(); }
};

//
// CIwGameCommandWait - Waits specified amount of time, Param1 - Time to wait in seconds
//
class CIwGameCommandWait : public CIwGameCommand
{
protected:
	CIwGameTimer	Timer;
public:
	void			Init();
	bool			Execute(float dt);
};
class CIwGameCommandWaitCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandWaitCreator()
	{
		setCommandName("wait");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandWait(); }
};

//
// CIwGameCommandDebugText - Writes dbug text to the output, Param1 - text / var, Param2 = text / var
//
class CIwGameCommandDebugText : public CIwGameCommand
{
protected:
public:
	bool			Execute(float dt);
};
class CIwGameCommandDebugTextCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandDebugTextCreator()
	{
		setCommandName("debug_text");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandDebugText(); }
};

//
// CIwGameCommandRemoteReq - Requests remote data, Param1 - RemoteReq resource name, Param2 = optional data to pass to request
//
class CIwGameCommandRemoteReq : public CIwGameCommand
{
protected:
public:
	bool			Execute(float dt);
};
class CIwGameCommandRemoteReqCreator : public CIwGameCommandCreator
{
public:
	CIwGameCommandRemoteReqCreator()
	{
		setCommandName("remote_req");
	}
	CIwGameCommand* CreateInstance() { return new CIwGameCommandRemoteReq(); }
};


#endif	// _IW_GAME_PROGRAM_H_
