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

#include "IwGameProgram.h"
#include "IwGameScene.h"
#include "IwGameActor.h"
#include "IwGame.h"
#include "IwGameScript.h"
#include "IwGameRemoteReq.h"

// 
// 
//
//
// IIwGameCommandExecutor implementation
//
//
//
//

void IIwGameCommandExecutor::addCommand(IIwGameCommandExecutor* command)
{
	command->setCommandIndex(Commands.size());
	Commands.push_back(command);
	command->setParent(this);
}

bool IIwGameCommandExecutor::removeCommand(IIwGameCommandExecutor* command)
{
	for (_Iterator it = Commands.begin(); it != Commands.end(); ++it)
	{
		if (*it == command)
		{
			delete *it;
			Commands.erase(it);
			RebuildCommandIndices();
			return true;
		}
	}

	return false;
}

IIwGameCommandExecutor* IIwGameCommandExecutor::findCommand(unsigned int name_hash)
{
	if (CommandNameHash == name_hash)
		return this;

	for (_Iterator it = Commands.begin(); it != Commands.end(); ++it)
	{
		IIwGameCommandExecutor* found = (*it)->findCommand(name_hash);
		if (found != NULL)
			return found;
	}

	return NULL;
}

IIwGameCommandExecutor* IIwGameCommandExecutor::findCommand(const char* name)
{
	return findCommand(CIwGameString::CalculateHash(name));
}

void IIwGameCommandExecutor::setCurrentCommand(IIwGameCommandExecutor* command)
{
	if (Parent != NULL)
	{
		int index = command->getCommandIndex();
		Parent->setCommandIndex(index);
		Parent->setCurrentCommand(index);
	}
}

bool IIwGameCommandExecutor::gotoCommand()
{
	setCurrentCommand(this);

	return true;
}

void IIwGameCommandExecutor::RebuildCommandIndices()
{
	int index = 0;
	for (_Iterator it = Commands.begin(); it != Commands.end(); ++it, index++)
		(*it)->setCommandIndex(index);
}


void IIwGameCommandExecutor::Uninit()
{
	Initialised = false;

	for (_Iterator it = Commands.begin(); it != Commands.end(); ++it)
		(*it)->Uninit();
}


void IIwGameCommandExecutor::restart()
{
	CurrentCommand = 0;
	Initialised = false;
	Finished = false;

	for (_Iterator it = Commands.begin(); it != Commands.end(); ++it)
		(*it)->restart();
}

void IIwGameCommandExecutor::clearCommands()
{
	// Remove and delete all images from the manager
	for (_Iterator it = Commands.begin(); it != Commands.end(); ++it)
	{
		(*it)->clearCommands();
		delete *it;
	}

	Commands.clear();
}

bool IIwGameCommandExecutor::nextCommand()
{
	IIwGameCommandExecutor* current_command = Commands.element_at(CurrentCommand);
	if (current_command != NULL)
	{
		// Reset the old ocmmand so it can be re-used
		current_command->restart();
	}

	if (Parallel)
	{
		// return to any previous command executor
		if (Parent != NULL)
			Parent->nextCommand();

		return false;
	}

	CurrentCommand++;
	if (CurrentCommand >= Commands.size())
	{
		// return to any previous command executor
		if (Parent != NULL)
			Parent->nextCommand();
		else
		{
			((CIwGameProgram*)this)->restart();
			((CIwGameProgram*)this)->stop();
		}

		return false;
	}

	// Skip to next command if the new command fails a return value check
	current_command = Commands.element_at(CurrentCommand);
	current_command->setInitialised(false);
	current_command->setCurrentCommand(0);
	if (current_command->getIfReturn() != NULL)
	{
		if (current_command->getIfReturn()->getReturnValue() != current_command->getIfReturnValue())
			nextCommand();
	}
	
	return true;
}

void IIwGameCommandExecutor::setCurrentCommand(int index)
{
	if (index < 0 || index >= Commands.size())
		return;

	CurrentCommand = index;
}

void IIwGameCommandExecutor::setIfReturn(IIwGameCommandExecutor* command, int value)
{
	IfReturn = command;
	IfReturnValue = value;
}

bool IIwGameCommandExecutor::Execute(float dt)
{
	if (Conditions != 0)
	{
		// Find the condition variable
		CIwGameXomlVariable* var = NULL;
		
		if (Program == NULL)
			var = CIwGameXomlVariable::GetVariable(_Conditions, NULL);
		else
		{
			unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
			if (class_hash == CIwGameXomlNames::Game_Hash)
				var = CIwGameXomlVariable::GetVariable(_Conditions, NULL);
			else
			if (class_hash == CIwGameXomlNames::Scene_Hash)
				var = CIwGameXomlVariable::GetVariable(_Conditions, (CIwGameScene*)Program->getManager()->getParent());
		}
		
		if (var != NULL)
		{
			bool res = var->isTrue();
			if (Not)
				res = !res;
			if (!res)
				return false;
		}
#if defined (_DEBUG)
		else
			CIwGameError::LogError("Warning: condition variable not found - ", _Conditions.c_str());
#endif // _DEBUG
	}

	// If commands fails its return value check and the command is part of a parallel processnig command then dont allow user to run the command
	if (IfReturn != NULL)
	{
		if (IfReturn->getReturnValue() != IfReturnValue)
		{
			if (Parent != NULL && Parent->isParallel())
				return false;
		}
	}

	if (Program != NULL)
		Program->setLastCommand(this);

	if (Commands.size() == 0)
		return true;

	// Process command children
	if (Parallel)
	{
		// Process all commands in parallel
		int num_finished = 0;
		for (_Iterator it = begin(); it != end(); ++it)
		{
			if (!(*it)->isInitialised())
			{
				(*it)->Init();
				(*it)->setInitialised(true);
				if (!(*it)->Execute(dt))
				{
					num_finished++;
				}
			}
			else
			{
				if (!(*it)->Execute(dt))
				{
					num_finished++;
				}
			}
		}

		if (num_finished == Commands.size())
			return false;
	}
	else
	{
		// Process current command in this command executor
		CIwGameCommand* command = (CIwGameCommand*)Commands.element_at(CurrentCommand);
		if (command != NULL)
		{
			if (!command->isInitialised())
			{
				command->Init();
				command->setInitialised(true);
				if (!command->Execute(dt))
				{
					nextCommand();
				}
			}
			else
			{
				if (!command->Execute(dt))
				{
					nextCommand();
				}
			}
		}
	}

	return true;
}


// 
// 
//
//
// CIwGameCommand implementation
//
//
//
//
CIwGameString& CIwGameCommand::getParameter1(IIwGameXomlResource* parent)
{
	if (Variables[0] == NULL)
		return Params[0];

	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[0], parent);
	Variables[0] = var;
	if (var != NULL)
	{
		return var->getValue();
	}

	return Params[0];
}
CIwGameString& CIwGameCommand::getParameter2(IIwGameXomlResource* parent)
{
	if (Variables[1] == NULL)
		return Params[1];

	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[1], parent);
	Variables[1] = var;
	if (var != NULL)
		return var->getValue();

	return Params[1];
}
CIwGameString& CIwGameCommand::getParameter3(IIwGameXomlResource* parent)
{
	if (Variables[2] == NULL)
		return Params[2];

	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[2], parent);
	Variables[2] = var;
	if (var != NULL)
		return var->getValue();

	return Params[2];
}
CIwGameString& CIwGameCommand::getParameter4(IIwGameXomlResource* parent)
{
	if (Variables[3] == NULL)
		return Params[3];

	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[3], parent);
	Variables[3] = var;
	if (var != NULL)
		return var->getValue();

	return Params[3];
}
CIwGameString& CIwGameCommand::getParameter5(IIwGameXomlResource* parent)
{
	if (Variables[4] == NULL)
		return Params[4];

	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[4], parent);
	Variables[4] = var;
	if (var != NULL)
		return var->getValue();

	return Params[4];
}



// 
// 
//
//
// CIwGameProgram implementation
//
//
//
//

void CIwGameProgram::restart()
{
	IIwGameCommandExecutor::restart();

	LastCommand = NULL;
	Status = PS_Running;
}

void CIwGameProgram::gotoNextCommand()
{
	bool ret = false;

	// Next command could have been called from anywhere so we find out where based on last called command
	if (LastCommand == NULL)
	{
		IIwGameCommandExecutor::nextCommand();
	}
	else
	{
		if (LastCommand->getParent() != NULL && LastCommand->getParent() != LastCommand->getProgram())
			LastCommand->getParent()->nextCommand();
		else
			nextCommand();
	}
	if (CurrentCommand >= Commands.size())
	{
		stop();
	}
}

void CIwGameProgram::call(CIwGameProgram* program)
{
	program->setCaller(this);
	program->restart();
	getManager()->setPriorityProgram(program);
	pause();
}

void CIwGameProgram::returnToCaller()
{
	if (Caller != NULL)
	{
		Caller->start();
		getManager()->setPriorityProgram(Caller);
		stop();
	}
}

bool CIwGameProgram::Execute(float dt)
{
	if (Status == PS_Running)
		return IIwGameCommandExecutor::Execute(dt);

	return false;
}

bool CIwGameProgram::ProcessCommands(CIwGameCommand* parent, bool load_children, CIwGameXmlNode* node)
{
	for (CIwGameXmlNode::_Iterator it = node->begin(); it != node->end(); ++it)
	{
		unsigned int name_hash = (*it)->GetName().getHash();
		if (name_hash == CIwGameXomlNames::Command_Hash)
		{
			CIwGameString	name, method, ifreturn, condition;
			CIwGameString	params[5];
			bool			active = true;
			bool			parallel = false;
			int				value = 0;

			for (CIwGameXmlNode::_AttribIterator it2 = (*it)->attribs_begin(); it2 != (*it)->attribs_end(); ++it2)
			{
				unsigned int attrib_hash = (*it2)->getName().getHash();

				if (attrib_hash == CIwGameXomlNames::Name_Hash)
					name = (*it2)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Method_Hash)
					method = (*it2)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Param1_Hash || attrib_hash == CIwGameXomlNames::P1_Hash)
					params[0] = (*it2)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Param2_Hash || attrib_hash == CIwGameXomlNames::P2_Hash)
					params[1] = (*it2)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Param3_Hash || attrib_hash == CIwGameXomlNames::P3_Hash)
					params[2] = (*it2)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Param4_Hash || attrib_hash == CIwGameXomlNames::P4_Hash)
					params[3] = (*it2)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Param5_Hash || attrib_hash == CIwGameXomlNames::P5_Hash)
					params[4] = (*it2)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Parallel_Hash)
					parallel = (*it2)->GetValueAsBool();
				else
				if (attrib_hash == CIwGameXomlNames::IfReturn_Hash)
					ifreturn = (*it2)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Value_Hash)
					value = (*it2)->GetValueAsInt();
				else
				if (attrib_hash == CIwGameXomlNames::Condition_Hash)
					condition = (*it2)->GetValue();
			}

			// Find the commands creator
			CIwGameCommand* command = NULL;
			if (method.IsEmpty())
				method="nop";

			CIwGameCommandCreator* creator = IW_GAME_PROGRAMS->findCreator(method.getHash());
			if (creator != NULL)
			{
				// Create the command
				command = creator->CreateInstance();
				if (command != NULL)
				{
					command->setProgram(this);
					command->setParallel(parallel);
					command->setCommandName(name.c_str());
					if (!ifreturn.IsEmpty())
					{
						IIwGameCommandExecutor* ifcommand = findCommand(ifreturn.getHash());
						if (ifcommand == NULL)
							CIwGameError::LogError("Warning: XOML - IfReturn command could not be found - ", ifreturn.c_str());
						else
							command->setIfReturn(ifcommand, value);
					}

					for (int t = 0; t < 5; t++)
						command->setParameter(t, params[t]);

					// If no parent command then add to the program else add to the parent commands child list
					if (parent == NULL)
						addCommand(command);
					else
						parent->addCommand(command);

					if (!condition.IsEmpty())
					{
						if (*(condition.c_str()) == '!')
							command->setConditions(condition.c_str() + 1, true);
						else
							command->setConditions(condition.c_str());
					}

				}
			}
			else
				CIwGameError::LogError("Warning: XOML - No creator for command method could be found - ", method.c_str());

			// If command contains sub commands then load them also
			if ((*it)->Children.size() != 0)
			{
				ProcessCommands(command, true, *it);
			}
		}
	}

	return true;
}

bool CIwGameProgram::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	CIwGameScene* scene = NULL;
	if (parent->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
	{
		CIwGameActor* actor = (CIwGameActor*)parent;
		scene = actor->getScene();
	}
	else
	if (parent->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	bool auto_run = false;
	bool priority = false;

	// Process program attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Name_Hash)
			setName((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Tag_Hash)
			setTag((*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::AutoRun_Hash)
			auto_run = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::Priority_Hash)
			priority = (*it)->GetValueAsBool();
	}

	// Prrocess commands
	ProcessCommands(NULL, true, node);

/*	// If we are declared inside a scene then program is local to the scene
	if (scene != NULL)
		scene->getResourceManager()->addResource(this);
	else
		IW_GAME_GLOBAL_RESOURCES->getResourceManager()->addResource(this);*/

	// Add the program to a program manager
	if (scene != NULL)
		scene->getProgrameManager()->addProgram(this);
	else
		IW_GAME_GLOBAL_RESOURCES->getProgrameManager()->addProgram(this);

	// if auto_run then start the program running
	if (auto_run)
		start();

	// If set as priority program then inform manager
	if (priority)
		Manager->setPriorityProgram(this);

	return true;
}

//
// 
// 
//
// CIwGameProgramManager Implementation
// 
// 
// 
//
void CIwGameProgramManager::addProgram(CIwGameProgram* program)
{
#if defined(IW_REMOVE_DUPLICATES)
	CIwGameProgram* prog = findProgram(program->getNameHash());
	if (prog != NULL)
	{
		CIwGameError::LogError("Warning: Program already exists, program was replaced - ", program->getName().c_str());
		removeProgram(program);
	}
#endif

	Programs.push_back(program);
	program->setManager(this);
}

bool CIwGameProgramManager::removeProgram(CIwGameProgram* Program)
{
	for (_Iterator it = Programs.begin(); it != Programs.end(); ++it)
	{
		if (*it == Program)
		{
			delete *it;
			Programs.erase(it);
			if (*it == PriorityProgram)
				PriorityProgram = NULL;
			return true;
		}
	}

	return false;
}

CIwGameProgram* CIwGameProgramManager::findProgram(unsigned int name_hash)
{
	for (_Iterator it = Programs.begin(); it != Programs.end(); ++it)
	{
		if ((*it)->getNameHash() == name_hash)
			return *it;
	}

	return NULL;
}

CIwGameProgram* CIwGameProgramManager::findProgram(const char* name)
{
	return findProgram(CIwGameString::CalculateHash(name));
}

void CIwGameProgramManager::clearPrograms()
{
	for (_Iterator it = Programs.begin(); it != Programs.end(); ++it)
		delete *it;
	Programs.clear();
	PriorityProgram = NULL;
}
void CIwGameProgramManager::stopAll()
{
	for (_Iterator it = Programs.begin(); it != Programs.end(); ++it)
		(*it)->stop();
}

void CIwGameProgramManager::startAll()
{
	for (_Iterator it = Programs.begin(); it != Programs.end(); ++it)
		(*it)->start();
}

void CIwGameProgramManager::pauseAll()
{
	for (_Iterator it = Programs.begin(); it != Programs.end(); ++it)
		(*it)->pause();
}

void CIwGameProgramManager::Execute(float dt)
{
	// Execute priority program first
	if (PriorityProgram != NULL)
		PriorityProgram->Execute(dt);

	// Execute other programs
	for (_Iterator it = Programs.begin(); it != Programs.end(); ++it)
	{
		if (*it != PriorityProgram)
			(*it)->Execute(dt);
	}
}

CIwGameProgram* CIwGameProgramManager::FindProgram(const char* name, IIwGameXomlResource* container)
{
	return FindProgram(CIwGameString::CalculateHash(name), container);
}

CIwGameProgram* CIwGameProgramManager::FindProgram(unsigned int name_hash, IIwGameXomlResource* container)
{
	CIwGameProgram* program = NULL;
	
	// If container class is an actor then check the actors scenes program manager
	if (container != NULL && container->getClassTypeHash() == CIwGameXomlNames::Actor_Hash)
		container = ((CIwGameActor*)container)->getScene();
	// If container class is a scene then check the scenes program manager
	if (container != NULL && container->getClassTypeHash() == CIwGameXomlNames::Scene_Hash)
		program = (CIwGameProgram*)((CIwGameScene*)container)->getProgrameManager()->findProgram((name_hash));

	// If the program has no container or the program was not found in the supplied container then check the global programs manager
	if (program == NULL)
		program = (CIwGameProgram*)IW_GAME_GLOBAL_RESOURCES->getProgrameManager()->findProgram((name_hash));

	return program;
}

//
// 
// 
//
// CIwGamePrograms Implementation
// 
// 
// 
//
CDECLARE_SINGLETON(CIwGamePrograms)

CIwGameCommandCreator* CIwGamePrograms::findCreator(unsigned int name_hash)
{
	for (_Iterator it = CommandCreators.begin(); it != CommandCreators.end(); ++it)
	{
		if ((*it)->getCommandNameHash() == name_hash)
			return *it;
	}

	return NULL;
}

CIwGameCommandCreator* CIwGamePrograms::findCreator(const char* name)
{
	return findCreator(CIwGameString::CalculateHash(name));
}

void CIwGamePrograms::Init()
{
	// Add default program commands
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandNopCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandCallCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandGotoCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandStopCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandReturnCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandPriorityCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandRunActionsCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandSetPropertyCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandSetVariableCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandGetVariableCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandAddVariableCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandIfVarCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandWaitVarIsValueCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandCallScriptCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandInlineCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandFromTemplateCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandMusicCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandSoundCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandWaitCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandCallGlobalScriptCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandDebugTextCreator());
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandRemoteReqCreator());
}

void CIwGamePrograms::Release()
{
	// Clean up command creators
	for (_Iterator it = begin(); it != end(); ++it)
	{
		delete *it;
	}
	CommandCreators.clear();
}

//
// 
// 
//
// CIwGameCommandNop Implementation
// 
// 
// 
//
void CIwGameCommandNop::Init()
{
/*	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)Program->getManager()->getParent();
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)Program->getManager()->getParent();*/

}

bool CIwGameCommandNop::Execute(float dt)
{
	if (IfReturn == NULL && Parallel == false)
		return false;
	else
		return IIwGameCommandExecutor::Execute(dt);
}

//
// 
// 
//
// CIwGameCommandGoto Implementation
// 
// 
// 
//
bool CIwGameCommandGoto::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)Program->getManager()->getParent();
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)Program->getManager()->getParent();

	CIwGameCommand* command = (CIwGameCommand*)Program->findCommand(getParameter1(scene).getHash());
	if (command != NULL)
		command->gotoCommand();
	else
	{
#if defined(_DEBUG)
		CIwGameError::LogError("Warning: Goto command - Cannot find named command - ", Params[0].c_str());
#endif	// _DEBUG
	}
	return true;
}

//
// 
// 
//
// CIwGameCommandPriority Implementation
// 
// 
// 
//
bool CIwGameCommandPriority::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)Program->getManager()->getParent();
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)Program->getManager()->getParent();

	// Find program
	CIwGameProgram* p = NULL;
		
	if (Params[0].IsEmpty())	// Passing no program will make program that ran thsi command the priority program
		p = Program;
	else
		p = Program->getManager()->findProgram(getParameter1(scene).getHash());
	if (p != NULL)
		Program->getManager()->setPriorityProgram(p);
#if defined (_DEBUG)
	else
		CIwGameError::LogError("Command priority - could not find named program - ", getParameter1(scene).c_str());
#endif // (_DEBUG)

	return false;
}

//
// 
// 
//
// CIwGameCommandCall Implementation
// 
// 
// 
//
bool CIwGameCommandCall::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)Program->getManager()->getParent();
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)Program->getManager()->getParent();

	// Find program
	CIwGameProgram* p = Program->getManager()->findProgram(getParameter1(scene).getHash());
	if (p != NULL)
		Program->call(p);
#if defined (_DEBUG)
	else
		CIwGameError::LogError("Command call - could not find named program - ", getParameter1(scene).c_str());
#endif // (_DEBUG)

	return false;
}

//
// 
// 
//
// CIwGameCommandRunActions Implementation
// 
// 
// 
//
bool CIwGameCommandRunActions::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	Finished = false;

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	CIwGameActor* actor = NULL;
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)Program->getManager()->getParent();
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)Program->getManager()->getParent();

	if (game != NULL)
		scene = game->findScene(Params[2].getHash());

	IIwGameXomlResource* cont = (actor != NULL) ? (IIwGameXomlResource*)actor : (IIwGameXomlResource*)scene;
	CIwGameActions* actions = CIwGameActionsManager::FindActions(getParameter1(cont).getHash(), scene);
	if (actions != NULL)
	{
		CIwGameActor* actor = scene->findActor(getParameter2(cont).getHash());
		if (actor == NULL && scene == NULL)
		{
#if defined(_DEBUG)
			CIwGameError::LogError("Warning: Command run_actions - No valid target actor or scene could be found");
#endif	// _DEBUG
			return false;
		}
		if (actor == NULL)
			actions->Execute(scene);
		else
			actions->Execute(actor);
	}
#if defined(_DEBUG)
	else
	{
		CIwGameError::LogError("Warning: Command run_actions - Actions could not be found - ", getParameter1(cont).c_str());
	}
#endif	// _DEBUG

	return false;
}

//
// 
// 
//
// CIwGameCommandSetProperty Implementation
// 
// 
// 
//
bool CIwGameCommandSetProperty::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	CIwGameActor* actor = NULL;
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)Program->getManager()->getParent();
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)Program->getManager()->getParent();

	IIwGameXomlResource* cont = (actor != NULL) ? (IIwGameXomlResource*)actor : (IIwGameXomlResource*)scene;
	if (Params[2].IsEmpty() && !Params[3].IsEmpty())
	{
		// User supplied a specific scene and no actor, so we set the scenes property instead
		if (game != NULL)
			scene = game->findScene(getParameter4(cont).getHash());
		else
			scene = scene->getParent()->findScene(getParameter4(cont).getHash());
		if (scene == NULL)
		{
#if defined(_DEBUG)
			CIwGameError::LogError("Warning: Command set_property - Scene does not exist - ", getParameter4(cont).c_str());
#endif	// _DEBUG
		}
		else
			scene->setProperty(getParameter1(cont).getHash(), getParameter2(cont), false);
		return false;
	}

	// Check to see if we supplied a scene that the actor lives in
	if (game != NULL && !Params[3].IsEmpty())
		scene = game->findScene(getParameter4(cont).getHash());

	if (scene != NULL)
	{
		CIwGameActor* actor = scene->findActor(getParameter3(cont).getHash());
		if (actor != NULL)
			actor->setProperty(getParameter1(cont).getHash(), getParameter2(cont), false);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Command set_property - Actor does not exist - ", getParameter3(cont).c_str());
#endif	// _DEBUG
	}
	else
	{
#if defined(_DEBUG)
		CIwGameError::LogError("Warning: Command set_property - Scene does not exist - ", getParameter4(cont).c_str());
#endif	// _DEBUG
	}

	return false;
}

//
// 
// 
//
// CIwGameCommandSetVariable Implementation
// 
// 
// 
//
bool CIwGameCommandSetVariable::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)Program->getManager()->getParent();
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)Program->getManager()->getParent();

	if (game != NULL)
		scene = game->findScene(Params[2].getHash());

	IIwGameXomlResource* cont = (IIwGameXomlResource*)scene;
	// Set the variables value
	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[0], scene);
	if (var != NULL)
	{
		unsigned int hash = getParameter2(cont).getHash();
		if (hash == CIwGameXomlNames::Rand_Hash || hash == CIwGameXomlNames::RandChar_Hash)
		{
			CIwGameString rnd;
			rnd.setAutoHash(false);
			float min, max;

			if (hash == CIwGameXomlNames::Rand_Hash)
			{
				min = getParameter4(cont).GetAsFloat();
				max = getParameter5(cont).GetAsFloat();
			}
			else
			{
				min = (float)getParameter4(cont)[0];
				max = (float)getParameter5(cont)[0];
			}

			if (var->isArray())
			{
				CIwGameXomlVariableArray* arr = (CIwGameXomlVariableArray*)var;
				for (int t = 0; t < arr->getSize(); t++)
				{
					CIwGameXomlVariable* var2 = arr->getElement(t);
					float num = min + (((float)rand() * (max - min)) / RAND_MAX);

					if (hash == CIwGameXomlNames::Rand_Hash)
					{
						if (var2->Type == VT_Bool)
							rnd = (bool)(num > 0.5f);
						else
						if (var2->Type == VT_Int)
							rnd = (int)num;
						else
							rnd = num;
					}
					else
						rnd = (char)num;
					var2->setValue(rnd.c_str());
				}
			}
			else
			{
				float num = min + (((float)rand() * (max - min)) / RAND_MAX);
				if (hash == CIwGameXomlNames::Rand_Hash)
				{
					if (var->Type == VT_Bool)
						rnd = (bool)(num > 0.5f);
					else
					if (var->Type == VT_Int)
						rnd = (int)num;
					else
						rnd = num;
				}
				else
					rnd = (char)num;
				var->setValue(rnd.c_str());
			}
		}
		else
			var->setValue(getParameter2(cont).c_str());
	}

	return false;
}

//
// 
// 
//
// CIwGameCommandGetVariable Implementation
// 
// 
// 
//
bool CIwGameCommandGetVariable::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)Program->getManager()->getParent();
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)Program->getManager()->getParent();

	if (game != NULL)
		scene = game->findScene(Params[2].getHash());

	// Set the variables value
	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[0], scene);
	if (var != NULL)
		ReturnValue = var->getValue().GetAsInt();

	return false;
}

//
// 
// 
//
// CIwGameCommandAddVariable Implementation
// 
// 
// 
//
bool CIwGameCommandAddVariable::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	Finished = false;

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	CIwGameActor* actor = NULL;
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)Program->getManager()->getParent();
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)Program->getManager()->getParent();

	if (game != NULL)
		scene = game->findScene(Params[2].getHash());

	IIwGameXomlResource* cont = (actor != NULL) ? (IIwGameXomlResource*)actor : (IIwGameXomlResource*)scene;
	// Set the variables value
	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[0], scene);
	if (var != NULL)
		var->addValue(getParameter2(cont).c_str());

	return false;
}

//
// 
// 
//
// CIwGameCommandIfVar Implementation
// 
// 
// 
//
bool CIwGameCommandIfVar::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	CIwGameActor* actor = NULL;
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)Program->getManager()->getParent();
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)Program->getManager()->getParent();

	if (game != NULL)
		scene = game->findScene(Params[3].getHash());

	IIwGameXomlResource* cont = (actor != NULL) ? (IIwGameXomlResource*)actor : (IIwGameXomlResource*)scene;
	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[0], scene);
	if (var != NULL)
	{
		CIwGameString op = getParameter2(cont);
		op.ToLower();
		if (op == "==")
			ReturnValue = (int)var->checkCondition(CO_Equal, getParameter3(cont));
		else
		if (op == "!=")
			ReturnValue = (int)var->checkCondition(CO_NotEqual, getParameter3(cont));
		else
		if (op == "gt")
			ReturnValue = (int)var->checkCondition(CO_Greater, getParameter3(cont));
		else
		if (op == "lt")
			ReturnValue = (int)var->checkCondition(CO_Less, getParameter3(cont));
		else
		if (op == "gte")
			ReturnValue = (int)var->checkCondition(CO_GreaterEqual, getParameter3(cont));
		else
		if (op == "lte")
			ReturnValue = (int)var->checkCondition(CO_LessEqual, getParameter3(cont));
		else
		if (op == "and")
			ReturnValue = (int)var->checkCondition(CO_And, getParameter3(cont));
		else
			CIwGameError::LogError("Warning: IfVar command - operator invalid - ", op.c_str());
	}
	else
		CIwGameError::LogError("Warning: IfVar command - variable not found - ", Params[0].c_str());

	return false;
}
//
// 
// 
//
// CIwGameCommandWaitVarIsValue Implementation
// 
// 
// 
//
bool CIwGameCommandWaitVarIsValue::Execute(float dt)
{
//	CIwGameError::LogError("WaitItemsCommand::Update()");
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	CIwGameActor* actor = NULL;
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)Program->getManager()->getParent();
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)Program->getManager()->getParent();

	if (game != NULL)
		scene = game->findScene(Params[2].getHash());

	IIwGameXomlResource* cont = (actor != NULL) ? (IIwGameXomlResource*)actor : (IIwGameXomlResource*)scene;
	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[0], scene);
	if (var->getValue().getHash() != getParameter2(cont).getHash())
		return true;

	return false;
}

//
// 
// 
//
// CIwGameCommandCallScript Implementation
// 
// 
// 
//
bool CIwGameCommandCallScript::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	IIwGameXomlResource* parent = Program->getManager()->getParent();
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)parent;
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	if (game != NULL && !Params[1].IsEmpty())
		scene = game->findScene(Params[1].getHash());

	if (scene->getScriptEngine() == NULL)
	{
		CIwGameError::LogError("Warning: call_script - Scene does not contain a script engine");
		return false;
	}

	IIwGameXomlResource* cont = (IIwGameXomlResource*)scene;
	if (scene != NULL)
	{
		CIwGameString *p2 = NULL;
		CIwGameString *p3 = NULL;
		CIwGameString *p4 = NULL;

		if (!Params[2].IsEmpty())
			p2 = &getParameter3(cont);
		if (!Params[3].IsEmpty())
			p3 = &getParameter4(cont);
		if (!Params[4].IsEmpty())
			p4 = &getParameter5(cont);

		if (scene != NULL)
			ReturnValue = scene->getScriptEngine()->CallFunction(scene, &getParameter1(cont), p2, p3, p4);
	}

	return false;
}

//
// 
// 
//
// CIwGameCommandCallGlobalScript Implementation
// 
// 
// 
//
bool CIwGameCommandCallGlobalScript::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	IIwGameXomlResource* parent = Program->getManager()->getParent();
	unsigned int class_hash = parent->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)parent;
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		game = ((CIwGameScene*)parent)->getParent();

	if (game != NULL && !Params[1].IsEmpty())
		scene = game->findScene(Params[1].getHash());

	IIwGameXomlResource* cont = (IIwGameXomlResource*)scene;
	CIwGameString *p2 = NULL;
	CIwGameString *p3 = NULL;
	CIwGameString *p4 = NULL;

	if (!Params[2].IsEmpty())
		p2 = &getParameter3(cont);
	if (!Params[3].IsEmpty())
		p3 = &getParameter4(cont);
	if (!Params[4].IsEmpty())
		p4 = &getParameter5(cont);

	if (scene != NULL)
		ReturnValue = IW_GAME_GLOBAL_RESOURCES->getScriptEngine()->CallFunction(scene, &getParameter1(cont), p2, p3, p4);
	else
	if (game != NULL)
		ReturnValue = IW_GAME_GLOBAL_RESOURCES->getScriptEngine()->CallFunction(game, &getParameter1(cont), p2, p3, p4);

	return false;
}

//
// 
// 
//
// CIwGameCommandInline Implementation
// 
// 
// 
//
bool CIwGameCommandInline::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	IIwGameXomlResource* parent = Program->getManager()->getParent();
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)parent;
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)parent;

	if (game == NULL)
		game = scene->getParent();

	CIwGameString* p1 = &getParameter1(scene);
	if (!Params[1].IsEmpty())	// Check for a specific scene
	{
		CIwGameString* p2 = &getParameter2(scene);
		scene = game->findScene(p2->getHash());
		if (scene == NULL)
		{
			CIwGameError::LogError("Warning: inline command - scene not found - ", p2->c_str());
			return false;
		}
		if (scene->getScriptEngine() == NULL)
		{
			CIwGameError::LogError("Warning: inline command - Scene does not contain a script engine");
			return false;
		}

		// Run script in scene script space
		scene->getScriptEngine()->RunScript(p1->c_str(), p1->GetLength());
	}
	else
	{
		// Run script in global script space
		IW_GAME_GLOBAL_RESOURCES->getScriptEngine()->RunScript(p1->c_str(), p1->GetLength());
	}

	return false;
}

//
// 
// 
//
// CIwGameCommandFromTemplate Implementation
// 
// 
// 
//
bool CIwGameCommandFromTemplate::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	CIwGameScene* target_scene = NULL;
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)Program->getManager()->getParent();
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)Program->getManager()->getParent();

	IIwGameXomlResource* cont = (IIwGameXomlResource*)scene;
	if (scene != NULL)
	{
		game = scene->getParent();
		if (!Params[2].IsEmpty())
		{
			target_scene = game->findScene(getParameter3(cont).getHash());
			if (target_scene == NULL)
			{
#if defined (_DEBUG)
				CIwGameError::LogError("Warning: Command - FromTemplate - Invalid scene - ", getParameter3(cont).c_str());
#endif
			}
			else
				CIwGameXomlLoad::CreateInstance(scene, target_scene, &getParameter1(cont), &getParameter2(cont));
		}
		else
			CIwGameXomlLoad::CreateInstance(scene, scene, &getParameter1(cont), &getParameter2(cont));
	}

	return false;
}

//
// 
// 
//
// CIwGameCommandMusic Implementation
// 
// 
// 
//
bool CIwGameCommandMusic::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	if (!Params[0].IsEmpty())
	{
		CIwGame* game = NULL;
		CIwGameScene* scene = NULL;
		unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
		if (class_hash == CIwGameXomlNames::Game_Hash)
			game = (CIwGame*)Program->getManager()->getParent();
		else
		if (class_hash == CIwGameXomlNames::Scene_Hash)
			scene = (CIwGameScene*)Program->getManager()->getParent();

		IIwGameXomlResource* cont = (IIwGameXomlResource*)scene;

		unsigned int hash = getParameter1(cont).getHash();
		if (hash == IW_GAME_HASH("play"))
		{
			IW_GAME_AUDIO->PlayMusic(getParameter2(cont).c_str(), getParameter3(cont).GetAsInt());
		}
		else
		if (hash == IW_GAME_HASH("stop"))
		{
			IW_GAME_AUDIO->StopMusic();
		}
	}

	return false;
}

//
// 
// 
//
// CIwGameCommandSound Implementation
// 
// 
// 
//
bool CIwGameCommandSound::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	if (!Params[0].IsEmpty() && !Params[1].IsEmpty() && !Params[2].IsEmpty() && !Params[3].IsEmpty())
	{
		CIwGame* game = NULL;
		CIwGameScene* scene = NULL;
		unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
		if (class_hash == CIwGameXomlNames::Game_Hash)
			game = (CIwGame*)Program->getManager()->getParent();
		else
		if (class_hash == CIwGameXomlNames::Scene_Hash)
			scene = (CIwGameScene*)Program->getManager()->getParent();

		if (game != NULL && !Params[4].IsEmpty())
			scene = game->findScene(Params[4].getHash());

		IIwGameXomlResource* cont = (IIwGameXomlResource*)scene;
		CIwGameSound* sound = (CIwGameSound*)CIwGameXomlResourceManager::FindResource(getParameter1(cont).getHash(), CIwGameXomlNames::Sound_Hash, scene);
		if (scene != NULL)
			sound->Play(getParameter2(cont).GetAsFloat(), getParameter3(cont).GetAsFloat(), getParameter4(cont).GetAsFloat());
		else
			CIwGameError::LogError("Warning: Command sound - Could not find named sound - ", getParameter1(cont).c_str());
	}
	else
		CIwGameError::LogError("Warning: Command sound - You must specify sound name, volume, pitch and pan - ", Params[0].c_str());

	return false;
}

//
// 
// 
//
// CIwGameCommandWait Implementation
// 
// 
// 
//
void CIwGameCommandWait::Init()
{
	IIwGameCommandExecutor::Init();

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)Program->getManager()->getParent();
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)Program->getManager()->getParent();

	Timer.setDuration((uint64)(getParameter1(scene).GetAsFloat() * 1000.0f));
}
bool CIwGameCommandWait::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	if (Timer.HasTimedOut())
	{
		Timer.Stop();
		return false;
	}

	return true;
}

//
// 
// 
//
// CIwGameCommandDebugText Implementation
// 
// 
// 
//
bool CIwGameCommandDebugText::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)Program->getManager()->getParent();
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)Program->getManager()->getParent();

	CIwGameError::LogError(getParameter1(scene).c_str(), getParameter2(scene).c_str());

	return false;
}

//
// 
// 
//
// CIwGameCommandRemoteReq Implementation
// 
// 
// 
//
bool CIwGameCommandRemoteReq::Execute(float dt)
{
	if (!IIwGameCommandExecutor::Execute(dt))
		return false;

	CIwGame* game = NULL;
	CIwGameScene* scene = NULL;
	unsigned int class_hash = Program->getManager()->getParent()->getClassTypeHash();
	if (class_hash == CIwGameXomlNames::Game_Hash)
		game = (CIwGame*)Program->getManager()->getParent();
	else
	if (class_hash == CIwGameXomlNames::Scene_Hash)
		scene = (CIwGameScene*)Program->getManager()->getParent();

	CIwGameString* p1 = NULL;
	CIwGameString* p2 = NULL;
	if (Params[0].IsEmpty())
	{
		CIwGameError::LogError("Error: Command remote_req requires Remote Request resource name as Param1");
		return false;
	}
	p1 = &getParameter1(scene);
	CIwGameRemoteReq* request = (CIwGameRemoteReq*)scene->getResourceManager()->findResource(p1->c_str(), CIwGameXomlNames::RemoteReq_Hash);
	if (request == NULL)
	{
		CIwGameError::LogError("Warning: Could not find named remote request resource in the scene or in the global resources - ", p1->c_str());
		return false;
	}
	if (!Params[1].IsEmpty())
	{
		p2 = &getParameter2(scene);
		request->setData(p2->c_str());
	}

	request->MakeRequest();

	return false;
}
