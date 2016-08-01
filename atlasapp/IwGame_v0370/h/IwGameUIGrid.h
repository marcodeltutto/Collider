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

#if !defined(_CIW_GAME_UI_GRID_H_)
#define _CIW_GAME_UI_GRID_H_

#include "IwGameActor.h"
#include "IwGameScene.h"
#include "IwGameUI.h"
#include "IwGameTemplates.h"

//
//
//
//
// CIwGameUIGrid - An image based game actor hthat acts as a grid, allowing other actors to be arranged into a grid
//
//
//
//
struct CIwGameUIGridColumn
{
	CIwGameString		Name;
	int					OriginalWidth;
	int					Width;
	eIwGameUI_AlignH	Align;
	bool				Visible;
	CIwGameTemplate*	ItemsTemplate;			// Items template used to instantiate grid column items
	CIwGameXomlVariableArray*	ItemsData;		// Bound data array
	unsigned int		ItemsDataTypeHash;		// Target item type hash
	int					ItemCount;				// Number of items in column
public:

	CIwGameUIGridColumn(const char* name, int width, eIwGameUI_AlignH align, bool visible)
	{
		Name = name;
		Width = width;
		Align = align;
		Visible = visible;
		ItemsTemplate = NULL;
		ItemsData = NULL;
		ItemsDataTypeHash = 0;
		ItemCount = 0;
	}
};

struct CIwGameUIGridRow
{
	CIwGameString		Name;
	int					OriginalHeight;
	int					Height;
	eIwGameUI_AlignV	Align;
	bool				Visible;

	CIwGameUIGridRow(const char* name, int height, eIwGameUI_AlignV align, bool visible)
	{
		Name = name;
		Height = height;
		Align = align;
		Visible = visible;
	}
};

class CIwGameUIGrid : public CIwGameUIBase
{
	// Properties
protected:
	int							RowCount;				// Number of rows
	int							ColumnCount;			// Number of columns
	eIwGameUI_AlignH			AlignH;					// Horizontal alignment of contained actors
	eIwGameUI_AlignV			AlignV;					// Vertical alignment of contained actors
	CIwList<CIwGameUIGridColumn*> ColumnDefinitions;	// Definition of columns
	CIwList<CIwGameUIGridRow*>	RowDefinitions;			// Definition of rows
	CIwGameTemplate*			ItemsTemplate;			// Items template used to instantiate grid actor items
	CIwGameXomlVariableArray*	ItemsData;				// Bound data array
	unsigned int				ItemsDataTypeHash;		// Target item type hash
	bool						MultiSelect;			// If true then multiple selections are allowed
	CIwGameUIBase*				Selection;				// Last selections actor
	int							SelectionIndex;			// Last selection index
	CIwGameXomlVariable*		SelectionVar;			// Variable that is updated with the current selection index
public:
	void						setAlignH(eIwGameUI_AlignH align)		{ AlignH = align; setLayoutDirty(true); }
	eIwGameUI_AlignH			getAlignH() const						{ return AlignH; }
	void						setAlignV(eIwGameUI_AlignV align)		{ AlignV = align; setLayoutDirty(true); }
	eIwGameUI_AlignV			getAlignV() const						{ return AlignV; }
	int							getRowCount() const						{ return RowCount; }
	int							getColumnCount() const					{ return ColumnCount; }
	CIwGameUIGridColumn* 		AddColumn(const char* name, int width, eIwGameUI_AlignH align, bool visible);
	CIwGameUIGridRow*			AddRow(const char* name, int height, eIwGameUI_AlignV align, bool visible);
	CIwGameUIGridRow*			getRow(int index)						{ if (index < 0 || index >= RowCount) return NULL; else return RowDefinitions.element_at(index); }
	CIwGameUIGridColumn*		getColumn(int index)					{ if (index < 0 || index >= ColumnCount) return NULL; else return ColumnDefinitions.element_at(index); }
	void						setRowVisible(int index, bool visible);
	void						setColumnVisible(int index, bool visible);
	void						setAllVisible(bool visible);
	void						setItemsTemplate(const char* temp_name)
	{
		RemoveAllActors();
		ItemsTemplate = (CIwGameTemplate*)Scene->getResourceManager()->findResource(temp_name, CIwGameXomlNames::Template_Hash);
		CreateAllActors();
	}
	void						setItemsData(CIwGameXomlVariableArray* data, unsigned int  type_hash);
	CIwGameXomlVariableArray*	getItemsData()							{ return ItemsData; }
	unsigned int				getDataTypeHash() const					{ return ItemsDataTypeHash; }
	void						setMultiSelect(bool enable)				{ MultiSelect = enable; }
	bool						isMultiSelect() const					{ return MultiSelect; }
	void						setSelectionIndex(int index);
	int							getSelectionIndex() const				{ return SelectionIndex; }
	CIwGameUIBase*				getSelection()							{ return Selection; }
	void						setSelectionVar(CIwGameXomlVariable* var) { SelectionVar = var; }
	// Properties end
protected:
	CIwGameSlotArray<CIwGameUIBase*>	Children;					// Collection of actors that are linked to this panel
	void						LinkChanged(CIwGameActor* child, bool remove);	// Links a new actor into the grid
	void						RemoveActor(CIwGameActor* actor);	// Removes the specified actor from the grid
	void						RemoveAllActors();					// Removes and destroys all actiors in the grid
	void						CreateAllActors();					// Re-creates all actors within the grid from a template
	void						UpdateAllActors();					// Moves data from the bound items data array to the actors
	void						RemoveColumnActors(int column);		// Removes all grid actors in a column
	void						CreateColumnActors(int column);		// Creates the grid actors for a column
	void						CreateAllColumnActors();			// Creates the grid actors for all columns
	void						UpdateColumnActors(int column);		// Moves data from the bound column items data array to the column actors
	void						UpdateAllColumnActors();			// Updates all grid column actors
	bool						UpdateLayout();						// Updates the layout of the child items
	void						CheckForColumnChanges();

public:
	CIwGameUIGrid() : CIwGameUIBase(), AlignH(AlignH_Centre), AlignV(AlignV_Middle), RowCount(0), ColumnCount(0), ItemsData(NULL), ItemsTemplate(NULL), ItemsDataTypeHash(0), MultiSelect(false), SelectionIndex(-1), Selection(NULL), SelectionVar(NULL)
								{ setActualClassType("grid"); ReceiveEventFromChild = true; }
	virtual ~CIwGameUIGrid();

	virtual void				InitGrid(int row_count, int column_count);
	bool						Update(float dt);

	// Event handlers
	void						OnOrientationChange(eIwGameScene_Orientation old_orientation, eIwGameScene_Orientation new_orientation);
	virtual void				OnSelectionChanged(int old_index, int new_index);

	// Implementation of IIwGameXomlClass interface
	bool						LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	// Utility
	void						ClearAllSelections(CIwGameUIBase* ignore);
};

//
//  CIwGameUIGridCreator - Creates an instance of a grid actor object
//
class CIwGameUIGridCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameUIGridCreator()
	{
		setClassName("grid");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameUIGrid(); }
};



#endif	// _CIW_GAME_UI_GRID_H_
