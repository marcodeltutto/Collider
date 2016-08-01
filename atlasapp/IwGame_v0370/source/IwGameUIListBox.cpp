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

#include "IwGameUIListBox.h"
#include "IwGameUtil.h"
#include "IwGameString.h"
#include "IwGameTemplates.h"
#include "IwGame.h"

//
//
//
//
//
// CIwGameUIListBox implementation
//
//
//
//
//

CIwGameUIListBox::~CIwGameUIListBox()
{
	for (_Iterator it = begin(); it != end(); ++it)
		delete *it;
	Children.clear();
}

void CIwGameUIListBox::setCaretIndex(int index)
{
	CIwGameUIBase* ui = NULL;

	if (CaretIndex < 0)
		CaretIndex = 0;
	else
	{
		ui = Children.element_at(CaretIndex)->Actor;
		ui->resetColourOffset();
	}

	if (index < 0) index = Children.size() - 1;
	if (index > Children.size() - 1) index = 0;

	ui = Children.element_at(index)->Actor;
	ui->setColourOffset(CaretColourOffset);
	ScrollToIndex(index);

	CaretIndex = index;
}

void CIwGameUIListBox::ScrollToIndex(int index)
{
	CIwGameUIBase* actor = Children.element_at(index)->Actor;

	CIwFVec2 pos =  CalculateItemPosition(index);

	if (Orientation == Orientation_Horizontal)
	{
		if (ScrollRange.w != 0)
		{
			if (pos.x < (float)ScrollRange.x)
				pos.x = (float)ScrollRange.x;
			else
			if (pos.x > (float)ScrollRange.x + ScrollRange.w)
				pos.x = (float)ScrollRange.x + ScrollRange.w;
		}
		setScrollPos(pos.x, ScrollPos.y);
	}
	else
	{
		if (ScrollRange.h != 0)
		{
			if (pos.y < (float)ScrollRange.y)
				pos.y = (float)ScrollRange.y;
			else
			if (pos.y > (float)ScrollRange.y + ScrollRange.h)
				pos.y = (float)ScrollRange.y + ScrollRange.h;
		}
		setScrollPos(ScrollPos.x, pos.y);
	}

	LayoutDirty = true;
}

void CIwGameUIListBox::addItem(CIwGameUIListBoxItem* item)
{
	if (item->Actor != NULL)
		item->Actor->setLinkedTo(this);
}

void CIwGameUIListBox::removeItem(CIwGameUIListBoxItem* item)
{
	for (_Iterator it = begin(); it != end(); ++it)
	{
		if ((*it)->Actor != NULL)
		{
			(*it)->Actor->getScene()->removeActor((*it)->Actor);
		}
		delete *it;
		Children.erase(it);
		break;
	}
}

CIwGameUIListBoxItem* CIwGameUIListBox::findItem(CIwGameUIBase* actor)
{
	for (_Iterator it = begin(); it != end(); ++it)
	{
		if ((*it)->Actor == actor)
			return *it;
	}

	return NULL;
}

void CIwGameUIListBox::setItemsData(CIwGameXomlVariableArray* data, unsigned int target_type_hash)
{
	int size = data->getSize();

	// If type of data is different then destroy previous grid actors and re-create new ones
	if (ItemsDataTypeHash != target_type_hash && ItemsData != NULL)
	{
		RemoveAllActors();
		ItemsDataTypeHash = target_type_hash;
		ItemsData = data;
		CreateAllActors();
	}

	UpdateAllActors();
}

void CIwGameUIListBox::ClearAllSelections(CIwGameUIBase* ignore)
{
	for (_Iterator it = begin(); it != end(); ++it)
	{
		CIwGameUIBase* actor = (*it)->Actor;
		if (actor != ignore && actor != NULL)
		{
			((CIwGameUIBase*)actor)->setSelected(false);
		}
	}
}

void CIwGameUIListBox::ClearAllToggles(CIwGameUIBase* ignore)
{
	for (_Iterator it = begin(); it != end(); ++it)
	{
		CIwGameUIBase* actor = (*it)->Actor;
		if (actor != ignore && actor != NULL)
		{
			((CIwGameUIBase*)actor)->setToggledOn(false);
		}
	}
}

void CIwGameUIListBox::LinkChanged(CIwGameActor* child, bool remove)
{
	if (child->getCategory() != AC_UI)
	{
#if defined(_DEBUG)
		CIwGameError::LogError("Error: Only actors that are derived from UI actor classes can be added to list box - ", child->getName().c_str());
#endif	// _DEBUG
		return;
	}

	if (remove)
	{
		for (_Iterator it = begin(); it != end(); ++it)
		{
			if ((*it)->Actor == child)
			{
				delete *it;
				Children.erase(it);
				break;
			}
		}
	}
	else
	{
		CIwGameUIListBoxItem* item = new CIwGameUIListBoxItem();
		item->Actor = (CIwGameUIBase*)child;
		Children.push_back(item);
	}
}

void CIwGameUIListBox::InitListBox()
{
	CreateAllActors();
	UpdateAllActors();
}

void CIwGameUIListBox::RemoveAllActors()
{
	if (begin() == end())
		return;

	int count = Children.size();
	for (int t = 0; t < count; t++)
	{
		_Iterator it = begin();

		if ((*it)->Actor != NULL)
			(*it)->Actor->getScene()->removeActor((*it)->Actor);
	}
	Children.clear();
}

void CIwGameUIListBox::CreateAllActors()
{
	// Find items template
	CIwGameTemplate* temp = (CIwGameTemplate*)Scene->getResourceManager()->findResource(ItemsTemplateHash, CIwGameXomlNames::Template_Hash);
	if (temp == NULL)
		return;

	if (ItemsData == NULL)
	{
#if defined(_DEBUG)
		CIwGameError::LogError("Warning: ListBox has a template but no items data - ", Name.c_str());
#endif	// _DEBUG
		return;
	}
	
	// Remove all previous actors
	RemoveAllActors();

	// Create a set of XML attributes that will replace the template parameters
	CIwGameXmlNode* replacements = new CIwGameXmlNode();
	replacements->Managed = false;

	// Set base template paramater
	CIwGameXmlAttribute* index_attrib = new CIwGameXmlAttribute();
	index_attrib->Managed = false;
	index_attrib->setName("index");
	replacements->AddAttribute(index_attrib);

	// Create new actors
	int index = 0;
	for (int t = 0; t < ItemsData->getSize(); t++)
	{
		if (ItemsData->getElement(t) != NULL)
		{
			// Instantiate actor from item template
			index_attrib->setValue(CIwGameString(index).c_str());
			if (!temp->Instantiate(this, replacements))
			{
#if defined(_DEBUG)
				CIwGameError::LogError("Error: Could not instantiate list box actor from items template - ", temp->getName().c_str());
#endif	// _DEBUG
				delete replacements;
				return;
			}
			index++;
		}
	}

	ItemsCount = ItemsData->getSize();
	LayoutDirty = true;

	// Clean up replacement attributes
	delete replacements;
}

void CIwGameUIListBox::UpdateAllActors()
{
	if (ItemsData == NULL)
		return;

	int index = 0;
	for (_Iterator it = begin(); it != end(); ++it)
	{
		CIwGameUIBase* actor = (*it)->Actor;
		if (actor != NULL)
		{
			CIwGameXomlVariable* var = ItemsData->getElement(index);
			if (var != NULL && var->Modified)
				actor->setProperty(ItemsDataTypeHash, var->getValue(), false);
		}
		index++;

		// If too many items then exit
		if (index >= ItemsData->getSize())
			break;
	}
}

CIwFVec2 CIwGameUIListBox::CalculateItemPosition(int item_index)
{
	int index = 0;
	
	_Iterator it = begin();
	if (it == end())
		return CIwFVec2(0, 0);

	int left = -Size.x / 2 + ClippingMargin.x;
	int right = Size.x / 2 - ClippingMargin.y;
	int top = -Size.y / 2 + ClippingMargin.w;
	int bottom = Size.y / 2 - ClippingMargin.h;

	CIwRect margin;

	if (Orientation == Orientation_Vertical)
	{
		float y = (float)(-Size.y / 2 + (*it)->Actor->getSize().y / 2);

		for (; it != end(); ++it)
		{
			CIwGameUIBase* actor = (*it)->Actor;
			if (actor != NULL)
			{
				CIwVec2 size = actor->getSize();
				margin = actor->getMargin();
				if ((*it)->Visible)
				{
					int sx = size.x >> 1;
					int sy = size.y >> 1;
					y += (float)margin.w;
					y += (float)(size.y + margin.h);
					if (index == item_index)
					{
						int xo = 0;

						if (AlignH == AlignH_Left)
							xo = -Size.x / 2 + sx / 2 + margin.x;
						else
						if (AlignH == AlignH_Right)
							xo = Size.x / 2 - sx / 2 - margin.y;

						return CIwFVec2((float)xo, y);
					}
					index++;
				}
			}
		}
	}
	else
	{
		float x = (float)(-Size.x / 2 + (*it)->Actor->getSize().x / 2);

		ScrollRange.w = 0;
		for (; it != end(); ++it)
		{
			CIwGameUIBase* actor = (*it)->Actor;
			if (actor != NULL)
			{
				CIwVec2 size = actor->getSize();
				margin = actor->getMargin();
				if ((*it)->Visible)
				{
					int sx = size.x >> 1;
					int sy = size.y >> 1;

					x += (float)margin.x;
					x += (float)(size.x + margin.y);
					if (index == item_index)
					{
						int yo = 0;

						if (AlignV == AlignV_Top)
							yo = -Size.y / 2 + sy / 2 + margin.w;
						else
						if (AlignV == AlignV_Bottom)
							yo = Size.y / 2 - sy / 2 - margin.h;

						return CIwFVec2(x, (float)yo);
					}
					index++;
				}
			}
		}
	}

	return CIwFVec2(0, 0);
}

bool CIwGameUIListBox::UpdateLayout()
{
	int index = 0;
	
	_Iterator it = begin();
	if (it == end())
		return true;

	int left = -Size.x / 2 + ClippingMargin.x;
	int right = Size.x / 2 - ClippingMargin.y;
	int top = -Size.y / 2 + ClippingMargin.w;
	int bottom = Size.y / 2 - ClippingMargin.h;

	CIwRect margin;

	if (Orientation == Orientation_Vertical)
	{
		float y = (float)(-Size.y / 2 + (*it)->Actor->getSize().y / 2) - ScrollPos.y;

		ScrollRange.h = 0;
		for (; it != end(); ++it)
		{
			CIwGameUIBase* actor = (*it)->Actor;
			if (actor != NULL)
			{
				CIwVec2 size = actor->getSize();
				margin = actor->getMargin();
				if ((*it)->Visible)
				{
					int sx = size.x >> 1;
					int sy = size.y >> 1;
					int xo = 0;

					if (AlignH == AlignH_Left)
						xo = -Size.x / 2 + sx / 2 + margin.x;
					else
					if (AlignH == AlignH_Right)
						xo = Size.x / 2 - sx / 2 - margin.y;

					y += (float)margin.w;
					actor->setPosition((float)xo - ScrollPos.x, y);
					if ((y + sy) < top || (y - sy) > bottom)
						actor->setVisible(false);
					else
						actor->setVisible(true);
					index++;
					y += (float)(size.y + margin.h);
				}
				else
				{
					CIwGameUIBase* actor = (*it)->Actor;
					if (actor != NULL)
						actor->setVisible(false);
				}
				ScrollRange.h += size.y + margin.w + margin.h;
			}
		}

		// Calculate vertical scroll range 
		ScrollRange.w = 0;
		ScrollRange.h -= Size.y;
		if (ScrollRange.h < 0)
			ScrollRange.h = 0;
	}
	else
	{
		float x = (float)(-Size.x / 2 + (*it)->Actor->getSize().x / 2) - ScrollPos.x;

		ScrollRange.w = 0;
		for (; it != end(); ++it)
		{
			CIwGameUIBase* actor = (*it)->Actor;
			if (actor != NULL)
			{
				CIwVec2 size = actor->getSize();
				margin = actor->getMargin();
				if ((*it)->Visible)
				{
					int sx = size.x >> 1;
					int sy = size.y >> 1;
					int yo = 0;

					if (AlignV == AlignV_Top)
						yo = -Size.y / 2 + sy / 2 + margin.w;
					else
					if (AlignV == AlignV_Bottom)
						yo = Size.y / 2 - sy / 2 - margin.h;

					x += (float)margin.x;
					actor->setPosition(x, (float)yo - ScrollPos.y);
					if ((x + sx) < left || (x - sx) > right)
						actor->setVisible(false);
					else
						actor->setVisible(true);
					index++;
					x += (float)(size.x + margin.y);
				}
				else
				{
					CIwGameUIBase* actor = (*it)->Actor;
					if (actor != NULL)
						actor->setVisible(false);
				}
				ScrollRange.w += size.x + margin.x + margin.y;
			}
		}

		// Calculate vertical scroll range 
		ScrollRange.h = 0;
		ScrollRange.w -= Size.x;
		if (ScrollRange.w < 0)
			ScrollRange.w = 0;
	}

	return CIwGameUIBase::UpdateLayout();
}

bool CIwGameUIListBox::Update(float dt)
{
	if (!CIwGameUIBase::Update(dt))
		return false;

	if (ItemsData != NULL &&  ItemsData->getSize() != ItemsCount)
		InitListBox();

	if (Scene->getParent()->getKeyFocus() == this && IW_GAME_INPUT->isKeysAvailable())
	{
		s3eKey key = IW_GAME_INPUT->getLastKey();

		if (key != 0)
		{
			if (key == s3eKeyUp || key == s3eKeyAbsUp || key == s3eKeyLeft || key == s3eKeyAbsLeft)
				setCaretIndex(getCaretIndex() - 1);
			else
			if (key == s3eKeyDown || key == s3eKeyAbsDown || key == s3eKeyRight || key == s3eKeyAbsRight)
				setCaretIndex(getCaretIndex() + 1);
			else
			if (key == s3eKeyOk || key == s3eKeyAbsOk || key == s3eKeyAbsGameA || key == s3eKeyEnter)
				setSelectionIndex(CaretIndex, true);
		}
	}

	// Check to see if any of the contained actors have been tapped
	CIwGameActor* last_tapped = Scene->getParent()->getLastTappedActor(0);
	if (last_tapped != NULL)
	{
		int index = 0;
		for (_Iterator it = begin(); it != end(); ++it)
		{
			if ((*it)->Actor != NULL)
			{
				CIwGameUIBase* actor = (CIwGameUIBase*)(*it)->Actor;
				if (actor == last_tapped)
				{
					if (MultiSelect)
					{
//						actor->setSelected(!actor->isSelected());
					}
					else
					{
						ClearAllSelections(actor);
						ClearAllToggles(actor);
//						actor->setSelected(true);
					}
					OnSelectionChanged(SelectionIndex, index);
					SelectionIndex = index;
					Selection = actor;
					break;
				}
			}
			index++;
		}
	}

	// Set clip rect for the list box
	CIwRect rc;
	rc.x = (-Size.x >> 1) + ClippingMargin.x;
	rc.y = (-Size.y >> 1) + ClippingMargin.w;
	rc.w = Size.x - (ClippingMargin.x + ClippingMargin.y);
	rc.h = Size.y - (ClippingMargin.w + ClippingMargin.h);
	Visual->setClipRect(rc);

	UpdateAllActors();

	return true;
}


void CIwGameUIListBox::setSelectionIndex(int index, bool call_events)
{
	if (!MultiSelect)
	{
		ClearAllSelections(NULL);
		ClearAllToggles(NULL);
	}
	if (index < 0)
	{
		if (Selection != NULL)
			Selection->setSelected(false);
		index = -1;
		Selection = NULL;
        SelectionIndex = index;
		return;
	}
	if (index >= Children.size())
		return;

	SelectionIndex = index;
	Selection = Children.element_at(index)->Actor;
	Selection->setSelected(true);

	if (call_events)	
		Selection->ProcessEventActions(CIwGameXomlNames::OnTapped_Hash);
}

bool CIwGameUIListBox::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - ListBox needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameUIBase::LoadFromXoml(parent, false, node))
		return false;

	CIwFVec2		grid_size = CIwFVec2(0, 0);
	CIwGameString*	items_data = NULL, *items_template = NULL, *items_target_type = NULL, *selection = NULL;
	int				selection_index = -1;

	// Process grid specific attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

		if (name_hash == CIwGameXomlNames::Orientation_Hash)
		{
			unsigned int hash = (*it)->GetValue().getHash();
			if (hash == CIwGameString::CalculateHash("horizontal"))
				Orientation = Orientation_Horizontal;
			else
				Orientation = Orientation_Vertical;
		}
		else
		if (name_hash == CIwGameXomlNames::AlignH_Hash)
		{
			unsigned int align_hash = (*it)->GetValue().getHash();
			if (align_hash == CIwGameXomlNames::left_Hash)
				AlignH = AlignH_Left;
			else
			if (align_hash == CIwGameXomlNames::right_Hash)
				AlignH = AlignH_Right;
			else
				AlignH = AlignH_Centre;
		}
		else
		if (name_hash == CIwGameXomlNames::AlignV_Hash)
		{
			unsigned int align_hash = (*it)->GetValue().getHash();
			if (align_hash == CIwGameXomlNames::top_Hash)
				AlignV = AlignV_Top;
			else
			if (align_hash == CIwGameXomlNames::bottom_Hash)
				AlignV = AlignV_Bottom;
			else
				AlignV = AlignV_Middle;
		}
		else
		if (name_hash == CIwGameXomlNames::ItemsTargetType_Hash)
			items_target_type = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::ItemsData_Hash)
			items_data = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::ItemsTemplate_Hash)
			items_template = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::MultiSelect_Hash)
			MultiSelect = (*it)->GetValueAsBool();
		else
		if (name_hash == CIwGameXomlNames::OnSelectionChanged_Hash)
			EventsManager->addEvent("OnSelectionChanged", (*it)->GetValue().c_str());
		else
		if (name_hash == CIwGameXomlNames::Selection_Hash)
			selection = &(*it)->GetValue();
		else
		if (name_hash == CIwGameXomlNames::SelectedIndex_Hash)
			selection_index = (*it)->GetValueAsInt();
		else
		if (name_hash == CIwGameXomlNames::CaretColourOffset_Hash)
		{
			if (!(*it)->GetValueAsRect(CaretColourOffset))
				CIwGameError::LogError("Warning: XOML - Invalid value for Actor::CaretColourOffset, expected rect");
		}
	}

	// Find and attach the items data array
	if (items_data != NULL)
	{
		CIwGameXomlVariable* items_var = CIwGameXomlVariable::GetVariable(*items_data, Scene);
		if (items_var != NULL)
		{
			if (!items_var->isArray())
				CIwGameError::LogError("Error: ListBox ItemsData should be an array variable type");
			else
				ItemsData = (CIwGameXomlVariableArray*)items_var;
		}
		else
			CIwGameError::LogError("Error: ListBox ItemsData could not be found - ", items_data->c_str());
	}

	// Find and attach the selection variable
	if (selection != NULL)
	{
		CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(*selection, Scene);
		if (var == NULL)
			CIwGameError::LogError("Warning: Selection binding variable not found - ", selection->c_str());
		SelectionVar = var;
	}

	if (items_target_type != NULL)
	{
		// Set Items target type
#if defined(_DEBUG)
		ItemsDataType = *items_target_type;
#endif	// _DEBUG
		ItemsDataTypeHash = items_target_type->getHash();
	}

	// Attach the items template
	if (items_template != NULL)
	{
#if defined(_DEBUG)
		ItemsTemplate = *items_template;
#endif // _DEBUG
		ItemsTemplateHash = items_template->getHash();
	}

	// Initialise the text box
	InitListBox();

	// Process inner tags
	if (load_children)
	{
		if (!IW_GAME_XOML->Process(this, node))
			return false;
	}

	// Set default selection
	if (selection_index >= 0)
		setSelectionIndex(selection_index);

	return true;
}


void CIwGameUIListBox::OnSelectionChanged(int old_index, int new_index)
{
	ProcessEventActions(CIwGameXomlNames::OnSelectionChanged_Hash);

	// Update attached variable
	if (SelectionVar != NULL)
	{
		SelectionVar->setValue(CIwGameString(new_index).c_str());
	}

}








