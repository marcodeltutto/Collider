// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. The only restriction to its usage is that this header must remain intact and visible in all IwGame engine files.
// If you use this engine in your product, whilst it is not mandatory, a mention of the IwGame engine would be nice or a quick email to let us know where it is being used.
//
//

#include "IwGameProgram.h"
#include "IwGameScene.h"
#include "IwGameActor.h"
#include "IwGame.h"

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
			((CIwGameProgram*)this)->stop();

		return false;
	}

	// Skip to next command if the new command failes a return value check
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
			CIwGameString	name, method, ifreturn;
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
				if (attrib_hash == CIwGameXomlNames::Param1_Hash)
					params[0] = (*it2)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Param2_Hash)
					params[1] = (*it2)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Param3_Hash)
					params[2] = (*it2)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Param4_Hash)
					params[3] = (*it2)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Param5_Hash)
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
	IW_GAME_PROGRAMS->addCommand(new CIwGameCommandWaitVarIsValueCreator());
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
// CIwGameCommandRunActions Implementation
// 
// 
// 
//
void CIwGameCommandRunActions::Init()
{
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

	CIwGameActions* actions = CIwGameActionsManager::FindActions(Params[0].getHash(), scene);
	if (actions != NULL)
	{
		CIwGameActor* actor = scene->findActor(Params[1].getHash());
		if (actor == NULL && scene == NULL)
		{
#if defined(_DEBUG)
			CIwGameError::LogError("Warning: Command run_actions - No valid target actor or scene could be found");
#endif	// _DEBUG
			return;
		}
		if (actor == NULL)
			actions->Execute(scene);
		else
			actions->Execute(actor);
	}
#if defined(_DEBUG)
	else
	{
		CIwGameError::LogError("Warning: Command run_actions - Actions could not be found - ", Params[0].c_str());
	}
#endif	// _DEBUG

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
void CIwGameCommandSetProperty::Init()
{
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
		scene = game->findScene(Params[3].getHash());

	if (scene != NULL)
	{
		CIwGameActor* actor = scene->findActor(Params[2].getHash());
		if (actor != NULL)
			actor->setProperty(Params[0].getHash(), Params[1], false);
#if defined(_DEBUG)
		else
			CIwGameError::LogError("Warning: Command set_property - Actor does not exist - ", Params[2].c_str());
#endif	// _DEBUG
	}
	else
	{
#if defined(_DEBUG)
		CIwGameError::LogError("Warning: Command set_property - Scene does not exist - ", Params[3].c_str());
#endif	// _DEBUG
	}

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
void CIwGameCommandSetVariable::Init()
{
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

	// Set the variables value
	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[0], scene);
	if (var != NULL)
		var->setValue(Params[1].c_str());
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
void CIwGameCommandWaitVarIsValue::Init()
{
	IIwGameCommandExecutor::Init();
}
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

	CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(Params[0], scene);
	if (var->getValue().getHash() != Params[1].getHash())
		return true;

	return false;
}

