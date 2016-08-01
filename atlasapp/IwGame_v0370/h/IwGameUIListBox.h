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

#if !defined(_CIW_GAME_UI_LISTBOX_H_)
#define _CIW_GAME_UI_LISTBOX_H_

#include "IwGameActor.h"
#include "IwGameScene.h"
#include "IwGameUI.h"

//
//
//
//
// CIwGameUIListBox - An image based game actor hthat acts as a list box, allowing the selection from a list of items
//
//
//
//
struct CIwGameUIListBoxItem
{
	CIwGameUIBase*		Actor;			// Actor that represents this list box item
	bool				Visible;		// Visible state

	CIwGameUIListBoxItem()
	{
		Actor = NULL;
		Visible = true;
	}
};

class CIwGameUIListBox : public CIwGameUIBase
{
public:
	// Public access to items iteration
	typedef CIwList<CIwGameUIListBoxItem*>::iterator _Iterator;
	_Iterator					begin() { return Children.begin(); }
	_Iterator					end() { return Children.end(); }

	// Properties
protected:
	eIwGameUI_Orientation		Orientation;			// Stacking orientation
	eIwGameUI_AlignH			AlignH;					// Horizontal alignment of contained actors (vertical only)
	eIwGameUI_AlignV			AlignV;					// Horizontal alignment of contained actors (horizontal only)
#if defined(_DEBUG)
	CIwGameString				ItemsTemplate;			// Name of template used to instantiate items
	CIwGameString				ItemsDataType;			// Target type for items (Text, Brush etc..)
#endif	// _DEBUG
	unsigned int				ItemsTemplateHash;		// Hash name of Template used to instantiate items
	CIwGameXomlVariableArray*	ItemsData;				// Bound data array
	unsigned int				ItemsDataTypeHash;		// Target item type hash
	bool						MultiSelect;			// If true then multiple selections are allowed
	CIwGameUIBase*				Selection;				// Last selections actor
	int							SelectionIndex;			// Last selection index
	CIwGameXomlVariable*		SelectionVar;			// Variable that is updated with the current selection index
	int							CaretIndex;				// Caret index
	CIwRect						CaretColourOffset;		// Amount the caret offsets the selected control
public:
	void						setOrientation(eIwGameUI_Orientation o)	{ Orientation = o; setLayoutDirty(true); }
	eIwGameUI_Orientation		getOrientation() const					{ return Orientation; }
	void						setAlignH(eIwGameUI_AlignH align)		{ AlignH = align; setLayoutDirty(true); }
	eIwGameUI_AlignH			getAlignH() const						{ return AlignH; }
	void						setAlignV(eIwGameUI_AlignV align)		{ AlignV = align; setLayoutDirty(true); }
	eIwGameUI_AlignV			getAlignV() const						{ return AlignV; }
	int							getItemCount() const					{ return Children.size(); }
	void						addItem(CIwGameUIListBoxItem* item);
	void						removeItem(CIwGameUIListBoxItem* item);
	CIwGameUIListBoxItem*		getItem(int index)						{ return Children.element_at(index); }
	CIwGameUIListBoxItem*		findItem(CIwGameUIBase* actor);
	void						setAllVisible(bool visible);
	void						setItemsTemplate(const char* temp_name)
	{
#if defined(_DEBUG)
		ItemsTemplate = temp_name;
#endif	// _DEBUG
		ItemsTemplateHash = CIwGameString::CalculateHash(temp_name);
	}
	void						setItemsData(CIwGameXomlVariableArray* data, unsigned int  type_hash);
	CIwGameXomlVariableArray*	getItemsData()							{ return ItemsData; }
	unsigned int				getDataTypeHash() const					{ return ItemsDataTypeHash; }
	void						setMultiSelect(bool enable)				{ MultiSelect = enable; }
	bool						isMultiSelect() const					{ return MultiSelect; }
	void						setSelectionIndex(int index, bool call_events = false);
	int							getSelectionIndex() const				{ return SelectionIndex; }
	CIwGameUIBase*				getSelection()							{ return Selection; }
	void						setSelectionVar(CIwGameXomlVariable* var) { SelectionVar = var; }
	void						setCaretIndex(int index);
	int							getCaretIndex() const					{ return CaretIndex; }
	CIwRect						getCaretColourOffset() const			{ return CaretColourOffset; }
	void						setCaretColourOffset(CIwRect& offset)	{ CaretColourOffset = offset; }
	// Properties end
protected:
	int							ItemsCount;
	CIwList<CIwGameUIListBoxItem*>	Children;			// Collection of items that are linked to this list box
	void						LinkChanged(CIwGameActor* child, bool remove);
	void						RemoveActor(CIwGameActor* actor);
	void						RemoveAllActors();		// Removes and destroys all actors in the list box
	void						CreateAllActors();		// Re-creates all actors within the list box from a template
	void						UpdateAllActors();		// Moves data from the bound items data array to the list box actors
	bool						UpdateLayout();			// Updates the layout of the child items
	CIwFVec2					CalculateItemPosition(int item_index);
public:
	CIwGameUIListBox() : CIwGameUIBase(), AlignH(AlignH_Centre), ItemsData(NULL), ItemsDataTypeHash(0), ItemsTemplateHash(0), MultiSelect(false), SelectionIndex(-1), Selection(NULL), SelectionVar(NULL), 
											Orientation(Orientation_Vertical), ItemsCount(-1), CaretIndex(-1), CaretColourOffset(0, 0, 0, 0)
								{ setActualClassType("listbox"); ReceiveEventFromChild = true; }
	virtual ~CIwGameUIListBox();

	virtual void				InitListBox();
	bool						Update(float dt);

	// Event handlers
	virtual void				OnSelectionChanged(int old_index, int new_index);

	// Implementation of IIwGameXomlClass interface
	bool						LoadFromXoml(IIwGameXomlResource* parent, bool load_children, CIwGameXmlNode* node);

	// Utility
	void						ClearAllSelections(CIwGameUIBase* ignore);
	void						ClearAllToggles(CIwGameUIBase* ignore);
	void						ScrollToIndex(int index);

};

//
//  CIwGameUIListBoxCreator - Creates an instance of a list box actor object
//
class CIwGameUIListBoxCreator : public IIwGameXomlClassCreator
{
public:
	CIwGameUIListBoxCreator()
	{
		setClassName("listbox");
	}
	IIwGameXomlResource*	CreateInstance(IIwGameXomlResource* parent)	{ return new CIwGameUIListBox(); }
};



#endif	// _CIW_GAME_UI_LISTBOX_H_
