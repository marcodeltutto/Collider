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

#include "IwGameUIGrid.h"
#include "IwGameUtil.h"
#include "IwGameString.h"
#include "IwGame.h"

//
//
//
//
//
// CIwGameUIGrid implementation
//
//
//
//
//

CIwGameUIGrid::~CIwGameUIGrid()
{
	for (CIwList<CIwGameUIGridRow*>::iterator ity = RowDefinitions.begin(); ity != RowDefinitions.end(); ++ity)
		delete *ity;
	for (CIwList<CIwGameUIGridColumn*>::iterator itx = ColumnDefinitions.begin(); itx != ColumnDefinitions.end(); ++itx)
		delete *itx;
	ColumnDefinitions.clear();
	RowDefinitions.clear();
}

CIwGameUIGridColumn* CIwGameUIGrid::AddColumn(const char* name, int width, eIwGameUI_AlignH align, bool visible)
{
	int owidth = width;
	if (width < 0)
		width = (Size.x * -width) / 100;

	CIwGameUIGridColumn* col = new CIwGameUIGridColumn(name, width, align, visible);
	col->OriginalWidth = owidth;
	ColumnDefinitions.push_back(col);

	return col;
}	

CIwGameUIGridRow* CIwGameUIGrid::AddRow(const char* name, int height, eIwGameUI_AlignV align, bool visible)
{
	int oheight = height;
	if (height < 0)
		height = (Size.y * -height) / 100;

	CIwGameUIGridRow* row = new CIwGameUIGridRow(name, height, align, visible);
	row->OriginalHeight = oheight;
	RowDefinitions.push_back(row);

	return row;
}

void CIwGameUIGrid::setItemsData(CIwGameXomlVariableArray* data, unsigned int target_type_hash)
{
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

void CIwGameUIGrid::ClearAllSelections(CIwGameUIBase* ignore)
{
	for (int t = 0; t < RowCount * ColumnCount; t++)
	{
		CIwGameUIBase* actor = Children.element_at(t);
		if (actor != ignore && actor != NULL)
			actor->setSelected(false);
	}
}

void CIwGameUIGrid::setSelectionIndex(int index)
{
	if (index < 0)
	{
		if (Selection != NULL)
			Selection->setSelected(false);
		index = -1;
		Selection = NULL;
		return;
	}
	if (index >= Children.getSize())
		return;

	SelectionIndex = index;
	Selection = Children.element_at(index);
	if (Selection != NULL)
		Selection->setSelected(true);
}


void CIwGameUIGrid::LinkChanged(CIwGameActor* child, bool remove)
{
	if (child->getCategory() != AC_UI)
	{
#if defined(_DEBUG)
		CIwGameError::LogError("Error: Only actors that are derived from UI actor classes can be added to grid - ", child->getName().c_str());
#endif	// _DEBUG
		return;
	}

	if (remove)
		Children.remove((CIwGameUIBase*)child);
	else
	{
		unsigned int grid_pos = child->getGridPos();
		if ((grid_pos >> 16) >= (uint)ColumnCount)
		{
#if defined(_DEBUG)
			CIwGameError::LogError("Warning: Attempted to link actor into a grid but grid column was out of range - ", child->getName().c_str());
#endif	// _DEBUG
			return;
		}
		if ((grid_pos & 0xffff) >= (uint)RowCount)
		{
#if defined(_DEBUG)
			CIwGameError::LogError("Warning: Attempted to link actor into a grid but grid row was out of range - ", child->getName().c_str());
#endif	// _DEBUG
			return;
		}
		int index = (grid_pos & 0xffff) * ColumnCount + (grid_pos >> 16);

		if (Children.element_at(index) != NULL)
			Children.element_at(index)->getScene()->removeActor(Children.element_at(index));

		Children.set(index, (CIwGameUIBase*)child);
	}
}

void CIwGameUIGrid::InitGrid(int row_count, int column_count)
{
	RemoveAllActors();
//	Scene->CleanupRemovedActors();

	ColumnCount = column_count;
	RowCount = row_count;
	Children.grow(column_count * row_count);

	// Add / remov rows to balanc row cuont
	int curr_row_count = RowDefinitions.size();
	if (row_count > curr_row_count)
	{
		CIwGameUIGridRow* row0 = RowDefinitions.element_at(0);
		for (int t = 0; t < row_count - curr_row_count; t++)
			AddRow("", row0->Height,row0->Align, row0->Visible);
	}
	else
	if (row_count < curr_row_count)
	{
		CIwList<CIwGameUIGridRow*>::iterator it = RowDefinitions.begin();
		for (int t = 0; t < curr_row_count -1; t++)
			it++;
		for (int t = row_count; t < curr_row_count; t++)
		{
			CIwList<CIwGameUIGridRow*>::iterator it2 = it--;
			delete *it2;
			RowDefinitions.erase(it2);
		}
	}

	// Size columns
	// Calculate total size of all specified grid columns
	int total_size = 0;
	int not_set_count = 0;
	for (CIwList<CIwGameUIGridColumn*>::iterator it = ColumnDefinitions.begin(); it != ColumnDefinitions.end(); ++it)
	{
		int d = (*it)->Width;
		if (d > 0)
			total_size += d;
		else
			not_set_count++;
	}
	if (not_set_count != 0)
	{
		// Some grid column widths were not set so we share the remaining space equally between them
		int d = (Size.x - total_size) / not_set_count;
		for (CIwList<CIwGameUIGridColumn*>::iterator it = ColumnDefinitions.begin(); it != ColumnDefinitions.end(); ++it)
		{
			if ((*it)->Width == 0)
				(*it)->Width = d;
		}
	}

	// Size rows
	// Calculate total size of all specified grid rows
	total_size = 0;
	not_set_count = 0;
	for (CIwList<CIwGameUIGridRow*>::iterator it = RowDefinitions.begin(); it != RowDefinitions.end(); ++it)
	{
		int d = (*it)->Height;
		if (d > 0)
			total_size += d;
		else
			not_set_count++;
	}
	if (not_set_count != 0)
	{
		// Some grid row heights  were not set so we share the remainnig space equally between them
		int d = (Size.y - total_size) / not_set_count;
		for (CIwList<CIwGameUIGridRow*>::iterator it = RowDefinitions.begin(); it != RowDefinitions.end(); ++it)
		{
			if ((*it)->Height == 0)
				(*it)->Height = d;
		}
	}

	CreateAllActors();
	UpdateAllActors();
}

void CIwGameUIGrid::setRowVisible(int index, bool visible)
{
	index = index * ColumnCount;
	for (int t = 0; t < ColumnCount; t++)
	{
		CIwGameUIBase* actor = Children.element_at(index);
		if (actor != NULL)
			actor->setVisible(visible);
		index++;
	}
	RowDefinitions.element_at(index)->Visible = visible;
}

void CIwGameUIGrid::setColumnVisible(int index, bool visible)
{
	for (int t = 0; t < RowCount; t++)
	{
		CIwGameUIBase* actor = Children.element_at(index);
		if (actor != NULL)
			actor->setVisible(visible);
		index += ColumnCount;
	}
	ColumnDefinitions.element_at(index)->Visible = visible;
}

void CIwGameUIGrid::setAllVisible(bool visible)
{
	for (int t = 0; t < RowCount * ColumnCount; t++)
	{
		CIwGameUIBase* actor = Children.element_at(t);
		if (actor != NULL)
			actor->setVisible(visible);
	}
	for (CIwList<CIwGameUIGridRow*>::iterator ity = RowDefinitions.begin(); ity != RowDefinitions.end(); ++ity)
		(*ity)->Visible = visible;
	for (CIwList<CIwGameUIGridColumn*>::iterator itx = ColumnDefinitions.begin(); itx != ColumnDefinitions.end(); ++itx)
		(*itx)->Visible = visible;
}

void CIwGameUIGrid::RemoveAllActors()
{
	for (int t = 0; t < RowCount * ColumnCount; t++)
	{
		CIwGameUIBase* actor = Children.element_at(t);
		if (actor != NULL)
			actor->getScene()->removeActor(actor);
	}
	Children.clear();
}

void CIwGameUIGrid::RemoveColumnActors(int column)
{
	for (int t = 0; t < RowCount; t++)
	{
		int index = t * ColumnCount + column;
		CIwGameUIBase* actor = Children.element_at(index);
		if (actor != NULL)
		{
			actor->getScene()->removeActor(actor);
			Children.set(index, NULL);
		}
	}
}

void CIwGameUIGrid::CreateAllActors()
{
	// If not items data ttacjed the look to columns instead
	if (ItemsData == NULL)
	{
		CreateAllColumnActors();
		return;
	}

	// Find items template
	CIwGameTemplate* temp = ItemsTemplate;
	if (temp == NULL)
	{
#if defined(_DEBUG)
		CIwGameError::LogError("Error: No items template attached to the grid");
#endif	// _DEBUG
		return;
	}

	// Create a set of XML attributes that will replace the template parameters
	CIwGameXmlNode* replacements = new CIwGameXmlNode();
	replacements->Managed = false;

	// Set base template paramater
	CIwGameXmlAttribute* index_attrib = new CIwGameXmlAttribute();
	index_attrib->Managed = false;
	index_attrib->setName("index");
	replacements->AddAttribute(index_attrib);

	CIwGameXmlAttribute* gridpos_attrib = new CIwGameXmlAttribute();
	gridpos_attrib->Managed = false;
	gridpos_attrib->setName("gridpos");
	replacements->AddAttribute(gridpos_attrib);

	CIwGameString grid_pos;
	int index = 0;
	for (int y = 0; y < RowCount; y++)
	{
		for (int x = 0; x < ColumnCount; x++)
		{
			CIwGameUIBase* actor = Children.element_at(index);
			if (actor != NULL)
				actor->getScene()->removeActor(actor);

			// Instantiate actor from item template
			index_attrib->setValue(CIwGameString(index).c_str());
			grid_pos = CIwGameString(x);
			grid_pos += ",";
			grid_pos += CIwGameString(y);
			gridpos_attrib->setValue(grid_pos.c_str());
			if (!temp->Instantiate(this, replacements))
			{
#if defined(_DEBUG)
				CIwGameError::LogError("Error: Could not instantiate grid actor from items template - ", temp->getName().c_str());
#endif	// _DEBUG
				delete replacements;
				return;
			}

			index++;
		}
	}

	// Clean up replacement attributes
	delete replacements;
}

void CIwGameUIGrid::CreateColumnActors(int column)
{
	// Check to see if columns have any bindings
	bool found = false;
	for (CIwList<CIwGameUIGridColumn*>::iterator itx = ColumnDefinitions.begin(); itx != ColumnDefinitions.end(); ++itx)
	{
		if ((*itx)->ItemsData != NULL)
		{
			found = true;
			break;
		}
	}
	if (!found)
		return;

	CIwGameUIGridColumn* col = ColumnDefinitions.element_at(column);
	if (col == NULL)
	{
#if defined(_DEBUG)
		CIwGameError::LogError("Error: Column number is invalid - ", CIwGameString(column).c_str());
#endif	// _DEBUG
		return;
	}

	// Create a set of XML attributes that will replace the template parameters
	CIwGameXmlNode* replacements = new CIwGameXmlNode();
	replacements->Managed = false;

	// Set base template paramater
	CIwGameXmlAttribute* index_attrib = new CIwGameXmlAttribute();
	index_attrib->Managed = false;
	index_attrib->setName("index");
	replacements->AddAttribute(index_attrib);

	CIwGameXmlAttribute* gridpos_attrib = new CIwGameXmlAttribute();
	gridpos_attrib->Managed = false;
	gridpos_attrib->setName("gridpos");
	replacements->AddAttribute(gridpos_attrib);

	CIwGameString grid_pos;

	if (col->ItemsData != NULL)
	{
		// Find column items template
		CIwGameTemplate* temp = col->ItemsTemplate;

		// if col;umn has not items template then look to the grids template instead
		if (temp == NULL)
			temp = ItemsTemplate;

		if (temp != NULL)
		{
			int y = 0;
			for (CIwList<CIwGameUIGridRow*>::iterator ity = RowDefinitions.begin(); ity != RowDefinitions.end(); ++ity)
			{
				// Remove existing grid cell actor
				CIwGameUIBase* actor = Children.element_at(y * ColumnCount + column);
				if (actor != NULL)
					actor->getScene()->removeActor(actor);

				// Instantiate actor from item template
				index_attrib->setValue(CIwGameString(y * ColumnCount + column).c_str());
				grid_pos = CIwGameString(column);
				grid_pos += ",";
				grid_pos += CIwGameString(y);
				gridpos_attrib->setValue(grid_pos.c_str());
				if (!temp->Instantiate(this, replacements))
				{
#if defined(_DEBUG)
					CIwGameError::LogError("Error: Could not instantiate grid column actor from items template - ", temp->getName().c_str());
#endif	// _DEBUG
					delete replacements;
					return;
				}
				y++;
			}
		}
	}

	// Clean up replacement attributes
	delete replacements;
}

void CIwGameUIGrid::CheckForColumnChanges()
{
	bool changed = false;
	int max_rows = 0;
	for (CIwList<CIwGameUIGridColumn*>::iterator itx = ColumnDefinitions.begin(); itx != ColumnDefinitions.end(); ++itx)
	{
		if ((*itx)->ItemsData != NULL)
		{
			int num_items = (*itx)->ItemsData->getSize();
			if (num_items != (*itx)->ItemCount)
			{
				(*itx)->ItemCount = num_items;
				changed = true;
			}
			if (num_items > max_rows)
				max_rows = num_items;
		}
	}

	if (changed)
	{
		InitGrid(max_rows, ColumnDefinitions.size());
		UpdateLayout();
	}
}


void CIwGameUIGrid::CreateAllColumnActors()
{
	int index = 0;
	for (CIwList<CIwGameUIGridColumn*>::iterator it = ColumnDefinitions.begin(); it != ColumnDefinitions.end(); ++it)
		CreateColumnActors(index++);
}

void CIwGameUIGrid::UpdateAllActors()
{
	if (ItemsData == NULL)
	{
		UpdateAllColumnActors();
		return;
	}

	for (int t = 0; t < RowCount * ColumnCount; t++)
	{
		CIwGameUIBase* actor = Children.element_at(t);
		if (actor != NULL)
		{
			CIwGameXomlVariable* var = ItemsData->getElement(t);
			if (var != NULL && var->Modified)	// && !var->getValue().IsEmpty())
			{
				actor->setProperty(ItemsDataTypeHash, var->getValue(), false);
			}
		}
	}
}

void CIwGameUIGrid::UpdateColumnActors(int column)
{
	// Check to see if columsn have any bindings
	bool found = false;
	for (CIwList<CIwGameUIGridColumn*>::iterator itx = ColumnDefinitions.begin(); itx != ColumnDefinitions.end(); ++itx)
	{
		if ((*itx)->ItemsData != NULL)
		{
			found = true;
			break;
		}
	}
	if (!found)
		return;

	CIwGameUIGridColumn* col = ColumnDefinitions.element_at(column);
	if (col == NULL)
	{
#if defined(_DEBUG)
		CIwGameError::LogError("Error: Column number is invalid - ", CIwGameString(column).c_str());
#endif	// _DEBUG
		return;
	}

	int y = 0;
	for (CIwList<CIwGameUIGridRow*>::iterator ity = RowDefinitions.begin(); ity != RowDefinitions.end(); ++ity)
	{
		CIwGameUIBase* actor = Children.element_at(y * ColumnCount + column);
		if (actor != NULL)
		{
			CIwGameXomlVariable* var = col->ItemsData->getElement(y);
			if (var != NULL && var->Modified)
				actor->setProperty(col->ItemsDataTypeHash, var->getValue(), false);
		}
		y++;
	}
}

void CIwGameUIGrid::UpdateAllColumnActors()
{
	int index = 0;
	for (CIwList<CIwGameUIGridColumn*>::iterator it = ColumnDefinitions.begin(); it != ColumnDefinitions.end(); ++it)
		UpdateColumnActors(index++);
}

bool CIwGameUIGrid::UpdateLayout()
{
	int index = 0;
	
	// Find first visible row
	CIwList<CIwGameUIGridRow*>::iterator ity = RowDefinitions.begin();
	for (; ity != RowDefinitions.end(); ity++)
	{
		if ((*ity)->Visible)
			break;
	}
	if (ity == RowDefinitions.end())
	{
		setAllVisible(false);
		return true;
	}

	int left = -Size.x / 2 + ClippingMargin.x;
	int right = Size.x / 2 - ClippingMargin.y;
	int top = -Size.y / 2 + ClippingMargin.w;
	int bottom = Size.y / 2 - ClippingMargin.h;

	int y = -Size.y / 2 + (*ity)->Height / 2 - (int)ScrollPos.y;

	ScrollRange.h = 0;
	for (ity = RowDefinitions.begin(); ity != RowDefinitions.end(); ++ity)
	{
		if ((*ity)->Visible)
		{
			eIwGameUI_AlignV alignv = (*ity)->Align;
			int yo;

			if (alignv == AlignV_Top)
				yo = -(*ity)->Height / 2;
			else
			if (alignv == AlignV_Bottom)
				yo = (*ity)->Height / 2;
			else
				yo = 0;

			// Step to first visible column
			CIwList<CIwGameUIGridColumn*>::iterator itx = ColumnDefinitions.begin();
			for (; itx != ColumnDefinitions.end(); itx++)
			{
				if ((*itx)->Visible)
					break;
			}
			if (itx == ColumnDefinitions.end())
			{
				setAllVisible(false);
				return true;
			}

			int x = -Size.x / 2;
			for (itx = ColumnDefinitions.begin(); itx != ColumnDefinitions.end(); ++itx)
			{
				if ((*itx)->Visible)
				{
					CIwGameUIBase* actor = Children.element_at(index);
					if (actor != NULL)
					{
						CIwVec2 size = actor->getSize();
						int sx = size.x >> 1;
						int sy = size.y >> 1;
						eIwGameUI_AlignH alignh = (*itx)->Align;
						CIwRect margin = actor->getMargin();
						int xo;

						if (alignh == AlignH_Left)
							xo = sx + margin.x;
						else
						if (alignh == AlignH_Right)
							xo = (*itx)->Width - sx - margin.y;
						else
							xo = (*itx)->Width / 2;

						float dx = (float)(x + xo) - ScrollPos.x;
						float dy = (float)(y + yo);
						if (alignv == AlignV_Top)
							dy += (float)(sy + margin.w);
						else
						if (alignv == AlignV_Bottom)
							dy += (float)(-sy - margin.h);
						actor->setPosition(dx, dy);

						if ((dy + sy) < top || (dy - sy) > bottom || (dx + sx) < left || (dx - sx) > right)
							actor->setVisible(false);
						else
							actor->setVisible(true);
					}
					x += (*itx)->Width;
				}
				else
				{
					CIwGameUIBase* actor = Children.element_at(index);
					if (actor != NULL)
						actor->setVisible(false);
				}
				index++;
			}
			y += (*ity)->Height;
		}
		else
		{
			for (CIwList<CIwGameUIGridColumn*>::iterator itx = ColumnDefinitions.begin(); itx != ColumnDefinitions.end(); ++itx)
			{
				CIwGameUIBase* actor = Children.element_at(index);
				if (actor != NULL)
					actor->setVisible(false);
				index++;
			}
		}
		ScrollRange.h += (*ity)->Height;
	}
	ScrollRange.h -= Size.y;
	if (ScrollRange.h < 0)
		ScrollRange.h = 0;

	// Calculate horizontal scroll range 
	ScrollRange.w = 0;
	for (CIwList<CIwGameUIGridColumn*>::iterator itx = ColumnDefinitions.begin(); itx != ColumnDefinitions.end(); ++itx)
		ScrollRange.w += (*itx)->Width;
	ScrollRange.w -= Size.x;
	if (ScrollRange.w < 0)
		ScrollRange.w = 0;

	return CIwGameUIBase::UpdateLayout();
}

bool CIwGameUIGrid::Update(float dt)
{
	if (!CIwGameUIBase::Update(dt))
		return false;

	if (ItemsData == NULL)
	{
		CheckForColumnChanges();
	}

	// Check to see if any of the contained actors have been tapped
	CIwGameActor* last_tapped = Scene->getParent()->getLastTappedActor(0);
	if (last_tapped != NULL)
	{
		for (int t = 0; t < RowCount * ColumnCount; t++)
		{
			CIwGameUIBase* actor = Children.element_at(t);
			if (actor != NULL)
			{
				if (actor == last_tapped)
				{
					if (MultiSelect)
					{
//						actor->setSelected(!actor->isSelected());
					}
					else
					{
						ClearAllSelections(actor);
//						actor->setSelected(true);
					}
					OnSelectionChanged(SelectionIndex, t);
					SelectionIndex = t;
					Selection = actor;
					break;
				}
			}
		}
	}

	// Set clip rect for the grid
	CIwRect rc;
	rc.x = (-Size.x >> 1) + ClippingMargin.x;
	rc.y = (-Size.y >> 1) + ClippingMargin.w;
	rc.w = Size.x - (ClippingMargin.x +  + ClippingMargin.y);
	rc.h = Size.y - (ClippingMargin.w +  + ClippingMargin.h);
	Visual->setClipRect(rc);

	UpdateAllActors();

	return true;
}

bool CIwGameUIGrid::LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node)
{
	if (parent->getClassTypeHash() != CIwGameXomlNames::Scene_Hash && parent->getClassTypeHash() != CIwGameXomlNames::Actor_Hash)
	{
		CIwGameError::LogError("Error: XOML - Grid needs to be declared inside a scene or an actor");
		return false;
	}

	// Load main actor attributes
	if (!CIwGameUIBase::LoadFromXoml(parent, false, node))
		return false;

	CIwFVec2		grid_size = CIwFVec2(0, 0);
	CIwGameString*	items_data = NULL, *items_template = NULL, *items_target_type = NULL, *selection = NULL;
	bool			global = false;
	int				selection_index = -1;

	// Process grid specific attributes
	for (CIwGameXmlNode::_AttribIterator it = node->attribs_begin(); it != node->attribs_end(); it++)
	{
		unsigned int name_hash = (*it)->getName().getHash();

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
		{
			items_data = &(*it)->GetValue();
			global = true;
		}
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
	}

	// Process any inner row / column definitions
	int num_rows = 0, num_columns = 0;
	for (CIwGameXmlNode::_Iterator it2 = node->begin(); it2 != node->end(); ++it2)
	{
		unsigned int name_hash = (*it2)->GetName().getHash();
		if (name_hash == CIwGameXomlNames::RowDefinition_Hash)
		{
			CIwGameString	name;
			int				height = 0;
			eIwGameUI_AlignV aiign = AlignV_Middle;
			bool			visible = true;

			// Get row data
			for (CIwGameXmlNode::_AttribIterator it = (*it2)->attribs_begin(); it != (*it2)->attribs_end(); ++it)
			{
				unsigned int attrib_hash = (*it)->getName().getHash();

				if (attrib_hash == CIwGameXomlNames::Name_Hash)
					name = (*it)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Height_Hash)
					height = (*it)->GetValueAsInt();
				else
				if (attrib_hash == CIwGameXomlNames::AlignV_Hash)
				{
					unsigned int align_hash = (*it)->GetValue().getHash();
					if (align_hash == CIwGameXomlNames::top_Hash)
						aiign = AlignV_Top;
					else
					if (align_hash == CIwGameXomlNames::bottom_Hash)
						aiign = AlignV_Bottom;
				}
				else
				if (attrib_hash == CIwGameXomlNames::Visible_Hash)
					visible = (*it)->GetValueAsBool();
			}

			AddRow(name.c_str(), height, aiign, visible);
			num_rows++;
		}
		else
		if (name_hash == CIwGameXomlNames::ColumnDefinition_Hash)
		{
			CIwGameString	name;
			int				width = 0;
			eIwGameUI_AlignH aiign = AlignH_Centre;
			bool			visible = true;
			bool			local = false;
			CIwGameString	items_data2, items_template2, items_target_type2;

			// Get column data
			for (CIwGameXmlNode::_AttribIterator it = (*it2)->attribs_begin(); it != (*it2)->attribs_end(); ++it)
			{
				unsigned int attrib_hash = (*it)->getName().getHash();

				if (attrib_hash == CIwGameXomlNames::Name_Hash)
					name = (*it)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::Width_Hash)
					width = (*it)->GetValueAsInt();
				else
				if (attrib_hash == CIwGameXomlNames::AlignH_Hash)
				{
					unsigned int align_hash = (*it)->GetValue().getHash();
					if (align_hash == CIwGameXomlNames::left_Hash)
						aiign = AlignH_Left;
					else
					if (align_hash == CIwGameXomlNames::right_Hash)
						aiign = AlignH_Right;
				}
				else
				if (attrib_hash == CIwGameXomlNames::Visible_Hash)
					visible = (*it)->GetValueAsBool();
				else
				if (attrib_hash == CIwGameXomlNames::ItemsTargetType_Hash)
					items_target_type2 = (*it)->GetValue();
				else
				if (attrib_hash == CIwGameXomlNames::ItemsData_Hash)
				{
					items_data2 = (*it)->GetValue();
					local = true;
				}
				else
				if (attrib_hash == CIwGameXomlNames::ItemsTemplate_Hash)
					items_template2 = (*it)->GetValue();
			}

			// Add the column
			CIwGameUIGridColumn* col = AddColumn(name.c_str(), width, aiign, visible);

			if (local && global)
				CIwGameError::LogError("Warning: Grid is globally bound, column binding is ignored");
			else
			{
				// Find and attach the items data array
				if (!items_data2.IsEmpty())
				{
					CIwGameXomlVariable* items_var = CIwGameXomlVariable::GetVariable(items_data2, Scene);
					if (items_var != NULL)
					{
						if (!items_var->isArray())
							CIwGameError::LogError("Warning: Grid column ItemsData should be an array variable type");
						else
						{
//							if (((CIwGameXomlVariableArray*)items_var)->getSize() < num_rows)
//								CIwGameError::LogError("Warning: Grid column ItemsData array is not large enough, should be >= num_rows", CIwGameString(num_rows).c_str());
//							else
							{
								col->ItemsData = (CIwGameXomlVariableArray*)items_var;
								col->ItemCount = col->ItemsData->getSize();
							}
						}
					}
					else
						CIwGameError::LogError("Warning: Grid column ItemsData could not be found - ", items_data2.c_str());
				}

				// Attach the items template
				if (!items_template2.IsEmpty())
				{
					col->ItemsTemplate = (CIwGameTemplate*)Scene->getResourceManager()->findResource(items_template2.getHash(), CIwGameXomlNames::Template_Hash);
					if (col->ItemsTemplate == NULL)
						CIwGameError::LogError("Warning: Grid column ItemsTemplate could not be found - ", items_template2.c_str());
				}

				// Set Items target type
				col->ItemsDataTypeHash = items_target_type2.getHash();
			}

			num_columns++;
		}
	}

	// Find attach the items data array
	if (items_data != NULL)
	{
		CIwGameXomlVariable* items_var = CIwGameXomlVariable::GetVariable(*items_data, Scene);
		if (items_var != NULL)
		{
			if (!items_var->isArray())
				CIwGameError::LogError("Warning: Grid ItemsData should be an array variable type");
			else
			{
				if (((CIwGameXomlVariableArray*)items_var)->getSize() < num_rows * num_columns)
					CIwGameError::LogError("Warning: Grid ItemsData array is not large enough, should be >= ", CIwGameString(num_rows * num_columns).c_str());
				else
					ItemsData = (CIwGameXomlVariableArray*)items_var;
			}
		}
		else
			CIwGameError::LogError("Warning: Grid ItemsData could not be found - ", items_data->c_str());
	}

	// Attach the items template
	if (items_template != NULL)
	{
		ItemsTemplate = (CIwGameTemplate*)Scene->getResourceManager()->findResource(items_template->getHash(), CIwGameXomlNames::Template_Hash);
		if (ItemsTemplate == NULL)
			CIwGameError::LogError("Warning: Grid ItemsTemplate could not be found - ", items_template->c_str());
	}

	if (items_target_type != NULL)
	{
		// Set Items target type
		ItemsDataTypeHash = items_target_type->getHash();
	}

	// Find and attach the selection variable
	if (selection != NULL)
	{
		CIwGameXomlVariable* var = CIwGameXomlVariable::GetVariable(*selection, Scene);
		if (var == NULL)
			CIwGameError::LogError("Warning: Selection binding variable not found - ", selection->c_str());
		SelectionVar = var;
	}

	// Initialise the grid
	InitGrid(num_rows, num_columns);

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

void CIwGameUIGrid::OnOrientationChange(eIwGameScene_Orientation old_orientation, eIwGameScene_Orientation new_orientation)
{
	CIwGameUIBase::OnOrientationChange(old_orientation, new_orientation);

	// Recalculate widths and heights of columns and rows
	for (CIwList<CIwGameUIGridRow*>::iterator ity = RowDefinitions.begin(); ity != RowDefinitions.end(); ++ity)
	{
		if ((*ity)->OriginalHeight < 0)
			(*ity)->Height = (-(*ity)->OriginalHeight * Size.y) / 100;
	}
	for (CIwList<CIwGameUIGridColumn*>::iterator itx = ColumnDefinitions.begin(); itx != ColumnDefinitions.end(); ++itx)
	{
		if ((*itx)->OriginalWidth < 0)
			(*itx)->Width = (-(*itx)->OriginalWidth * Size.x) / 100;
	}

	LayoutDirty = true;
}

void CIwGameUIGrid::OnSelectionChanged(int old_index, int new_index)
{
	// Update attached variable
	if (SelectionVar != NULL)
	{
		SelectionVar->setValue(CIwGameString(new_index).c_str());
	}

	ProcessEventActions(CIwGameXomlNames::OnSelectionChanged_Hash);
}








