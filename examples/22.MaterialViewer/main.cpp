/** Example 022 Material Viewer

This example can be used to play around with material settings and watch the results.
Only the default non-shader materials are used in here.

You have two nodes to make it easier to see which difference your settings will make.
Additionally you have one lightscenenode and you can set the global ambient values.
*/

#include <irrlicht.h>
#include "driverChoice.h"

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

/*
	Variables within the empty namespace are globals which are restricted to this file.
*/
namespace
{
	const wchar_t* const DriverTypeNames[] =
	{
		L"NULL",
		L"SOFTWARE",
		L"BURNINGSVIDEO",
		L"DIRECT3D8",
		L"DIRECT3D9",
		L"OPENGL",
		0,
	};

	// For the gui id's
	enum EGUI_IDS
	{
		GUI_ID_OPEN_TEXTURE = 1,
		GUI_ID_QUIT,
		GUI_ID_MAX
	};

	// Name used in texture selection to clear the textures on the node
	const core::stringw CLEAR_TEXTURE = L"CLEAR texture";

	// some useful color constants
	const video::SColor SCOL_BLACK     = video::SColor(255, 0,   0,   0);
	const video::SColor SCOL_BLUE      = video::SColor(255, 0,   0,  255);
	const video::SColor SCOL_CYAN      = video::SColor(255, 0,  255, 255);
	const video::SColor SCOL_GRAY      = video::SColor(255, 128,128, 128);
	const video::SColor SCOL_GREEN     = video::SColor(255, 0,  255,  0);
	const video::SColor SCOL_MAGENTA   = video::SColor(255, 255, 0,  255);
	const video::SColor SCOL_RED       = video::SColor(255, 255, 0,   0);
	const video::SColor SCOL_YELLOW    = video::SColor(255, 255, 255, 0);
	const video::SColor SCOL_WHITE     = video::SColor(255, 255, 255, 255);
};	// namespace

/*
	Returns a new unique number on each call.
*/
s32 makeUniqueId()
{
	static int unique = GUI_ID_MAX;
	++unique;
	return unique;
}

/*
	Find out which vertex-type is needed for the given material type.
*/
video::E_VERTEX_TYPE getVertexTypeForMaterialType(video::E_MATERIAL_TYPE materialType)
{
	using namespace video;

	switch ( materialType )
	{
		case EMT_SOLID:
			return EVT_STANDARD;

		case EMT_SOLID_2_LAYER:
			return EVT_STANDARD;

		case EMT_LIGHTMAP:
		case EMT_LIGHTMAP_ADD:
		case EMT_LIGHTMAP_M2:
		case EMT_LIGHTMAP_M4:
		case EMT_LIGHTMAP_LIGHTING:
		case EMT_LIGHTMAP_LIGHTING_M2:
		case EMT_LIGHTMAP_LIGHTING_M4:
			return EVT_2TCOORDS;

		case EMT_DETAIL_MAP:
			return EVT_2TCOORDS;

		case EMT_SPHERE_MAP:
			return EVT_STANDARD;

		case EMT_REFLECTION_2_LAYER:
			return EVT_2TCOORDS;

		case EMT_TRANSPARENT_ADD_COLOR:
			return EVT_STANDARD;

		case EMT_TRANSPARENT_ALPHA_CHANNEL:
			return EVT_STANDARD;

		case EMT_TRANSPARENT_ALPHA_CHANNEL_REF:
			return EVT_STANDARD;

		case EMT_TRANSPARENT_VERTEX_ALPHA:
			return EVT_STANDARD;

		case EMT_TRANSPARENT_REFLECTION_2_LAYER:
			return EVT_2TCOORDS;

		case EMT_NORMAL_MAP_SOLID:
		case EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR:
		case EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA:
		case EMT_PARALLAX_MAP_SOLID:
		case EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR:
		case EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA:
			return EVT_TANGENTS;

		case EMT_ONETEXTURE_BLEND:
			return EVT_STANDARD;

		case EMT_FORCE_32BIT:
			return EVT_STANDARD;
	}
	return EVT_STANDARD;
}

/*
	Custom GUI-control to edit colorvalues.
*/
class CColorControl : public gui::IGUIElement
{
public:
	// Constructor
	CColorControl(gui::IGUIEnvironment* guiEnv, const core::position2d<s32> & pos, const wchar_t *text, IGUIElement* parent, s32 id=-1 )
		: gui::IGUIElement(gui::EGUIET_ELEMENT, guiEnv, parent,id, core::rect< s32 >(pos, pos+core::dimension2d<s32>(80, 75)))
		, DirtyFlag(true)
		, ColorStatic(0)
		, EditAlpha(0)
		, EditRed(0)
		, EditGreen(0)
		, EditBlue(0)
	{
		using namespace gui;
		ButtonSetId = makeUniqueId();

		const core::rect< s32 > rectControls(0,0,AbsoluteRect.getWidth(),AbsoluteRect.getHeight() );
		IGUIStaticText * groupElement =	guiEnv->addStaticText (L"", rectControls, true, false, this, -1, false);
		groupElement->setNotClipped(true);

		guiEnv->addStaticText (text, core::rect<s32>(0,0,80,15), false, false, groupElement, -1, false);

		EditAlpha = addEditForNumbers(guiEnv, core::position2d<s32>(0,15), L"a", -1, groupElement );
		EditRed = addEditForNumbers(guiEnv, core::position2d<s32>(0,30), L"r", -1, groupElement );
		EditGreen = addEditForNumbers(guiEnv, core::position2d<s32>(0,45), L"g", -1, groupElement );
		EditBlue = addEditForNumbers(guiEnv, core::position2d<s32>(0,60), L"b", -1, groupElement );

		ColorStatic = guiEnv->addStaticText (L"", core::rect<s32>(60,15,80,75), true, false, groupElement, -1, true);

		guiEnv->addButton (core::rect<s32>(60,35,80,50), groupElement, ButtonSetId, L"set");
		SetEditsFromColor(Color);
	}

	// event receiver
	virtual bool OnEvent(const SEvent &event)
	{
		if ( event.EventType != EET_GUI_EVENT )
			return false;

		if ( event.GUIEvent.Caller->getID() == ButtonSetId && event.GUIEvent.EventType == gui::EGET_BUTTON_CLICKED )
		{
			Color = GetColorFromEdits();
			SetEditsFromColor(Color);
		}

		return false;
	}

	// set the color values
	void setColor(const video::SColor& col)
	{
		DirtyFlag = true;
		Color = col;
		SetEditsFromColor(Color);
	}

	// get the color values
	const video::SColor& getColor() const
	{
		return Color;
	}

	// To reset the dirty flag
	void resetDirty()
	{
		DirtyFlag = false;
	}

	// when the color was changed the dirty flag is set
	bool isDirty() const
	{
		return DirtyFlag;
	};

protected:

	// Add a staticbox for a description + an editbox so users can enter numbers
	gui::IGUIEditBox* addEditForNumbers(gui::IGUIEnvironment* guiEnv, const core::position2d<s32> & pos, const wchar_t *text, s32 id, gui::IGUIElement * parent)
	{
		using namespace gui;

		core::rect< s32 > rect(pos, pos+core::dimension2d<s32>(10, 15));
		guiEnv->addStaticText (text, rect, false, false, parent, -1, false);
		rect += core::position2d<s32>( 20, 0 );
		rect.LowerRightCorner.X += 20;
		gui::IGUIEditBox* edit = guiEnv->addEditBox(L"0", rect, true, parent, id);
		return edit;
	}

	// Get the color value from the editfields
	video::SColor GetColorFromEdits()
	{
		video::SColor col;

		if (EditAlpha)
		{
			u32 alpha = core::strtoul10(core::stringc(EditAlpha->getText()).c_str());
			if (alpha > 255)
				alpha = 255;
			col.setAlpha(alpha);
		}

		if (EditRed)
		{
			u32 red = core::strtoul10(core::stringc(EditRed->getText()).c_str());
			if (red > 255)
				red = 255;
			col.setRed(red);
		}

		if (EditGreen)
		{
			u32 green = core::strtoul10(core::stringc(EditGreen->getText()).c_str());
			if (green > 255)
				green = 255;
			col.setGreen(green);
		}

		if (EditBlue)
		{
			u32 blue = core::strtoul10(core::stringc(EditBlue->getText()).c_str());
			if (blue > 255)
				blue = 255;
			col.setBlue(blue);
		}

		return col;
	}

	// Fill the editfields with the value for the given color
	void SetEditsFromColor(video::SColor col)
	{
		DirtyFlag = true;
		if ( EditAlpha )
			EditAlpha->setText( core::stringw(col.getAlpha()).c_str() );
		if ( EditRed )
			EditRed->setText( core::stringw(col.getRed()).c_str() );
		if ( EditGreen )
			EditGreen->setText( core::stringw(col.getGreen()).c_str() );
		if ( EditBlue )
			EditBlue->setText( core::stringw(col.getBlue()).c_str() );
		if ( ColorStatic )
			ColorStatic->setBackgroundColor(col);
	}

private:

	bool DirtyFlag;
	video::SColor Color;
	s32 ButtonSetId;
	gui::IGUIStaticText * ColorStatic;
	gui::IGUIEditBox * EditAlpha;
	gui::IGUIEditBox * EditRed;
	gui::IGUIEditBox * EditGreen;
	gui::IGUIEditBox * EditBlue;
};

/*
	Custom GUI-control for to edit all colors typically used in materials and lights
*/
class CAllColorsControl : public gui::IGUIElement
{
public:
	// Constructor
	CAllColorsControl(gui::IGUIEnvironment* guiEnv, const core::position2d<s32> & pos, const wchar_t * description, bool hasEmissive, IGUIElement* parent, s32 id=-1)
		: gui::IGUIElement(gui::EGUIET_ELEMENT, guiEnv, parent,id, core::rect<s32>(pos,pos+core::dimension2d<s32>(60,250)))
		, ControlAmbientColor(0), ControlDiffuseColor(0), ControlSpecularColor(0), ControlEmissiveColor(0)
	{
		core::rect<s32> rect(0, 0, 60, 15);
		guiEnv->addStaticText (description, rect, false, false, this, -1, false);
		createColorControls(guiEnv, core::position2d<s32>(0, 15), hasEmissive);
	}

	// Destructor
	virtual ~CAllColorsControl()
	{
		ControlAmbientColor->drop();
		ControlDiffuseColor->drop();
		if ( ControlEmissiveColor )
			ControlEmissiveColor->drop();
		ControlSpecularColor->drop();
	}

	// Set the color values to those within the material
	void setColorsToMaterialColors(const video::SMaterial & material)
	{
		ControlAmbientColor->setColor(material.AmbientColor);
		ControlDiffuseColor->setColor(material.DiffuseColor);
		ControlEmissiveColor->setColor(material.EmissiveColor);
		ControlSpecularColor->setColor(material.SpecularColor);
	}

	// Update all changed colors in the material
	void updateMaterialColors(video::SMaterial & material)
	{
		if ( ControlAmbientColor->isDirty() )
			material.AmbientColor = ControlAmbientColor->getColor();
		if ( ControlDiffuseColor->isDirty() )
			material.DiffuseColor = ControlDiffuseColor->getColor();
		if ( ControlEmissiveColor->isDirty() )
			material.EmissiveColor = ControlEmissiveColor->getColor();
		if ( ControlSpecularColor->isDirty() )
			material.SpecularColor = ControlSpecularColor->getColor();
	}

	// Set the color values to those from the light data
	void setColorsToLightDataColors(const video::SLight & lightData)
	{
		ControlAmbientColor->setColor(lightData.AmbientColor.toSColor());
		ControlAmbientColor->setColor(lightData.DiffuseColor.toSColor());
		ControlAmbientColor->setColor(lightData.SpecularColor.toSColor());
	}

	// Update all changed colors in the light data
	void updateLightColors(video::SLight & lightData)
	{
		if ( ControlAmbientColor->isDirty() )
			lightData.AmbientColor = video::SColorf( ControlAmbientColor->getColor() );
		if ( ControlDiffuseColor->isDirty() )
			lightData.DiffuseColor = video::SColorf( ControlDiffuseColor->getColor() );
		if ( ControlSpecularColor->isDirty() )
			lightData.SpecularColor = video::SColorf(ControlSpecularColor->getColor() );
	}

	// To reset the dirty flags
	void resetDirty()
	{
		ControlAmbientColor->resetDirty();
		ControlDiffuseColor->resetDirty();
		ControlSpecularColor->resetDirty();
		if ( ControlEmissiveColor )
			ControlEmissiveColor->resetDirty();
	}

protected:
	void createColorControls(gui::IGUIEnvironment* guiEnv, const core::position2d<s32> & pos, bool hasEmissive)
	{
		ControlAmbientColor = new CColorControl( guiEnv, pos, L"ambient", this);
		ControlDiffuseColor = new CColorControl( guiEnv, pos + core::position2d<s32>(0, 75), L"diffuse", this );
		ControlSpecularColor = new CColorControl( guiEnv, pos + core::position2d<s32>(0, 150), L"specular", this );
		if ( hasEmissive )
		{
			ControlEmissiveColor = new CColorControl( guiEnv, pos + core::position2d<s32>(0, 225), L"emissive", this );
		}
	}

private:
	CColorControl*	ControlAmbientColor;
	CColorControl*	ControlDiffuseColor;
	CColorControl*	ControlSpecularColor;
	CColorControl*	ControlEmissiveColor;
};

/*
	GUI-Control to offer a selection of available textures.
*/
class CTextureControl : public gui::IGUIElement
{
public:
	CTextureControl(gui::IGUIEnvironment* guiEnv, video::IVideoDriver * driver, const core::position2d<s32> & pos, IGUIElement* parent, s32 id=-1)
	: gui::IGUIElement(gui::EGUIET_ELEMENT, guiEnv, parent,id, core::rect<s32>(pos,pos+core::dimension2d<s32>(100,15)))
	, DirtyFlag(true), ComboTexture(0)
	{
		core::rect<s32> rectCombo(0, 0, AbsoluteRect.getWidth(),AbsoluteRect.getHeight());
		ComboTexture = guiEnv->addComboBox (rectCombo, this);
		updateTextures(driver);
	}

	virtual bool OnEvent(const SEvent &event)
	{
		if ( event.EventType != EET_GUI_EVENT )
			return false;

		if ( event.GUIEvent.Caller == ComboTexture && event.GUIEvent.EventType == gui::EGET_COMBO_BOX_CHANGED )
		{
			DirtyFlag = true;
		}

		return false;
	}

	// Workaround for a problem with comboboxes.
	// We have to get in front when the combobox wants to get in front or combobox-list might be drawn below other elements.
	virtual bool bringToFront(IGUIElement* element)
	{
		bool result = gui::IGUIElement::bringToFront(element);
		if ( Parent && element == ComboTexture )
			result &= Parent->bringToFront(this);
		return result;
	}

	// return selected texturename (if any, otherwise 0)
	const wchar_t * getSelectedTextureName() const
	{
		s32 selected = ComboTexture->getSelected();
		if ( selected < 0 )
			return 0;
		return ComboTexture->getItem(selected);
	}

	// reset the dirty flag
	void resetDirty()
	{
		DirtyFlag = false;
	}

	// when the texture was changed the dirty flag is set
	bool isDirty() const
	{
		return DirtyFlag;
	};

	// Put the names of all currently loaded textures in a combobox
	void updateTextures(video::IVideoDriver * driver)
	{
		s32 oldSelected = ComboTexture->getSelected();
		s32 selectNew = -1;
		core::stringw oldTextureName;
		if ( oldSelected >= 0 )
		{
			oldTextureName = ComboTexture->getItem(oldSelected);
		}
		ComboTexture->clear();
		for ( u32 i=0; i < driver->getTextureCount(); ++i )
		{
			video::ITexture * texture = driver->getTextureByIndex(i);
			core::stringw name( texture->getName() );
			ComboTexture->addItem( name.c_str() );
			if ( !oldTextureName.empty() && selectNew < 0 && name == oldTextureName )
				selectNew = i;
		}

		// add another name which can be used to clear the texture
		ComboTexture->addItem( CLEAR_TEXTURE.c_str() );
		if ( CLEAR_TEXTURE == oldTextureName )
			selectNew = ComboTexture->getItemCount()-1;

		if ( selectNew >= 0 )
			ComboTexture->setSelected(selectNew);

		DirtyFlag = true;
	}

private:
	bool DirtyFlag;
	gui::IGUIComboBox * ComboTexture;
};

/*
	Control which allows setting some of the material values for a meshscenenode
*/
struct SMeshNodeControl
{
	// constructor
	SMeshNodeControl()
		: Initialized(false), Driver(0), MeshManipulator(0), SceneNode(0), SceneNode2T(0), SceneNodeTangents(0)
		, AllColorsControl(0), ButtonLighting(0), InfoLighting(0), ComboMaterial(0), TextureControl1(0), TextureControl2(0), ControlVertexColors(0)
	{
	}

	// Destructor
	virtual ~SMeshNodeControl()
	{
		if ( TextureControl1 )
			TextureControl1->drop();
		if ( TextureControl2 )
			TextureControl2->drop();
		if ( ControlVertexColors )
			ControlVertexColors->drop();
		if ( AllColorsControl )
			AllColorsControl->drop();
	}

	void init(scene::IMeshSceneNode* node, IrrlichtDevice * device, const core::position2d<s32> & pos, const wchar_t * description)
	{
		if ( Initialized || !node || !device) // initializing twice or with invalid data not allowed
			return;

		Driver = device->getVideoDriver ();
		gui::IGUIEnvironment* guiEnv = device->getGUIEnvironment();
		scene::ISceneManager* smgr = device->getSceneManager();
		MeshManipulator = smgr->getMeshManipulator();

		SceneNode = node;
		scene::IMeshManipulator * meshManip = smgr->getMeshManipulator();

		scene::IMesh * mesh2T = meshManip->createMeshWith2TCoords(node->getMesh());
		SceneNode2T = smgr->addMeshSceneNode(mesh2T, 0, -1, SceneNode->getPosition(), SceneNode->getRotation(), SceneNode->getScale() );
		mesh2T->drop();

		scene::IMesh * meshTangents = meshManip->createMeshWithTangents(node->getMesh(), false, false, false);
		SceneNodeTangents = smgr->addMeshSceneNode(meshTangents, 0, -1
											, SceneNode->getPosition(), SceneNode->getRotation(), SceneNode->getScale() );
		meshTangents->drop();

		video::SMaterial & material = SceneNode->getMaterial(0);
		material.Lighting = true;
		AllColorsControl = new CAllColorsControl(guiEnv, pos, description, true, guiEnv->getRootGUIElement());
		AllColorsControl->setColorsToMaterialColors(material);

		core::rect<s32> rectBtn(pos + core::position2d<s32>(0, 320), core::dimension2d<s32>(60, 15));
		ButtonLighting = guiEnv->addButton (rectBtn, 0, -1, L"Lighting");
		ButtonLighting->setIsPushButton(true);
		ButtonLighting->setPressed(material.Lighting);
		core::rect<s32> rectInfo( rectBtn.LowerRightCorner.X, rectBtn.UpperLeftCorner.Y, rectBtn.LowerRightCorner.X+40, rectBtn.UpperLeftCorner.Y+15 );
		InfoLighting = guiEnv->addStaticText(L"", rectInfo, true, false );
		InfoLighting->setTextAlignment(gui::EGUIA_CENTER, gui::EGUIA_CENTER );

		core::rect<s32> rectCombo(pos.X, rectBtn.LowerRightCorner.Y, pos.X+100, rectBtn.LowerRightCorner.Y+15);
		ComboMaterial = guiEnv->addComboBox (rectCombo);
		for ( int i=0; i <= (int)video::EMT_ONETEXTURE_BLEND; ++i )
		{
			ComboMaterial->addItem( core::stringw(video::sBuiltInMaterialTypeNames[i]).c_str() );
		}
		ComboMaterial->setSelected( (s32)material.MaterialType );

		core::position2d<s32> posTex(rectCombo.UpperLeftCorner.X,rectCombo.LowerRightCorner.Y);
		TextureControl1 = new CTextureControl(guiEnv, Driver, posTex, guiEnv->getRootGUIElement());
		posTex.Y += 15;
		TextureControl2 = new CTextureControl(guiEnv, Driver, posTex, guiEnv->getRootGUIElement());

		core::position2d<s32> posVertexColors( posTex.X, posTex.Y + 15);
		ControlVertexColors = new CColorControl( guiEnv, posVertexColors, L"Vertex colors", guiEnv->getRootGUIElement());

		video::S3DVertex * vertices =  (video::S3DVertex *)node->getMesh()->getMeshBuffer(0)->getVertices();
		if ( vertices )
		{
			ControlVertexColors->setColor(vertices[0].Color);
		}

		Initialized = true;
	}

	void update()
	{
		if ( !Initialized )
			return;

		video::SMaterial & material = SceneNode->getMaterial(0);
		video::SMaterial & material2T = SceneNode2T->getMaterial(0);
		video::SMaterial & materialTangents = SceneNodeTangents->getMaterial(0);

		s32 selectedMaterial = ComboMaterial->getSelected();
		if ( selectedMaterial >= (s32)video::EMT_SOLID && selectedMaterial <= (s32)video::EMT_ONETEXTURE_BLEND)
		{
			video::E_VERTEX_TYPE vertexType = getVertexTypeForMaterialType((video::E_MATERIAL_TYPE)selectedMaterial);
			switch ( vertexType )
			{
				case video::EVT_STANDARD:
					material.MaterialType = (video::E_MATERIAL_TYPE)selectedMaterial;
					SceneNode->setVisible(true);
					SceneNode2T->setVisible(false);
					SceneNodeTangents->setVisible(false);
					break;
				case video::EVT_2TCOORDS:
					material2T.MaterialType = (video::E_MATERIAL_TYPE)selectedMaterial;
					SceneNode->setVisible(false);
					SceneNode2T->setVisible(true);
					SceneNodeTangents->setVisible(false);
					break;
				case video::EVT_TANGENTS:
					materialTangents.MaterialType = (video::E_MATERIAL_TYPE)selectedMaterial;
					SceneNode->setVisible(false);
					SceneNode2T->setVisible(false);
					SceneNodeTangents->setVisible(true);
					break;
			}
		}

		updateMaterial(material);
		updateMaterial(material2T);
		updateMaterial(materialTangents);

		if ( ButtonLighting->isPressed() )
			InfoLighting->setText(L"on");
		else
			InfoLighting->setText(L"off");

		AllColorsControl->resetDirty();
		TextureControl1->resetDirty();
		TextureControl2->resetDirty();
		ControlVertexColors->resetDirty();
	}

	void updateTextures()
	{
		TextureControl1->updateTextures(Driver);
		TextureControl2->updateTextures(Driver);
	}

protected:

	void updateMaterial(video::SMaterial & material)
	{
		AllColorsControl->updateMaterialColors(material);
		material.Lighting = ButtonLighting->isPressed();
		if ( TextureControl1->isDirty() )
		{
			material.TextureLayer[0].Texture = Driver->getTexture( io::path(TextureControl1->getSelectedTextureName()) );
		}
		if ( TextureControl2->isDirty() )
		{
			material.TextureLayer[1].Texture = Driver->getTexture( io::path(TextureControl2->getSelectedTextureName()) );
		}
		if ( ControlVertexColors->isDirty() )
		{
			MeshManipulator->setVertexColors (SceneNode->getMesh(), ControlVertexColors->getColor());
			MeshManipulator->setVertexColors (SceneNode2T->getMesh(), ControlVertexColors->getColor());
			MeshManipulator->setVertexColors (SceneNodeTangents->getMesh(), ControlVertexColors->getColor());
		}
	}

	bool Initialized;
	video::IVideoDriver * 		Driver;
	scene::IMeshManipulator* 	MeshManipulator;
	scene::IMeshSceneNode* 		SceneNode;
	scene::IMeshSceneNode* 		SceneNode2T;
	scene::IMeshSceneNode* 		SceneNodeTangents;
	CAllColorsControl* 			AllColorsControl;
	gui::IGUIButton * 			ButtonLighting;
	gui::IGUIStaticText* 		InfoLighting;
	gui::IGUIComboBox * 		ComboMaterial;
	CTextureControl* 			TextureControl1;
	CTextureControl* 			TextureControl2;
	CColorControl*				ControlVertexColors;
};

/*
	Control to allow setting the color values of a lightscenenode.
*/
struct SLightNodeControl
{
	// constructor
	SLightNodeControl() : Initialized(false), SceneNode(0), AllColorsControl(0)
	{
	}

	virtual ~SLightNodeControl()
	{
		if ( AllColorsControl )
			AllColorsControl->drop();
	}

	void init(scene::ILightSceneNode* node, gui::IGUIEnvironment* guiEnv, const core::position2d<s32> & pos, const wchar_t * description)
	{
		if ( Initialized || !node || !guiEnv) // initializing twice or with invalid data not allowed
			return;
		SceneNode = node;
		AllColorsControl = new CAllColorsControl(guiEnv, pos, description, false, guiEnv->getRootGUIElement());
		const video::SLight & lightData = SceneNode->getLightData();
		AllColorsControl->setColorsToLightDataColors(lightData);
		Initialized = true;
	}

	void update()
	{
		if ( !Initialized )
			return;

		video::SLight & lightData = SceneNode->getLightData();
		AllColorsControl->updateLightColors(lightData);
	}

protected:
	bool Initialized;
	scene::ILightSceneNode* SceneNode;
	CAllColorsControl* AllColorsControl;
};

/*
	Application configuration
*/
struct SConfig
{
	SConfig()
	: RenderInBackground(true)
	, DriverType(video::EDT_BURNINGSVIDEO)
	, ScreenSize(640, 480)
	{
	}

	bool RenderInBackground;
	video::E_DRIVER_TYPE DriverType;
	core::dimension2d<u32> ScreenSize;
};

/*
	Main application class
*/
class CApp : public IEventReceiver
{
	friend int main(int argc, char *argv[]);

public:
	// constructor
	CApp()
	: IsRunning(false)
	, Device(0)
	, Camera(0)
	, GlobalAmbient(0)
	{
	}

	// destructor
	~CApp()
	{
	}

	// stop running - will quit at end of mainloop
	void stopApp()
	{
		IsRunning = false;
	}

	// Event handler
	virtual bool OnEvent(const SEvent &event)
	{
		if (event.EventType == EET_GUI_EVENT)
		{
			gui::IGUIEnvironment* env = Device->getGUIEnvironment();

			switch(event.GUIEvent.EventType)
			{
				case gui::EGET_MENU_ITEM_SELECTED:
				{
					gui::IGUIContextMenu* menu = (gui::IGUIContextMenu*)event.GUIEvent.Caller;
					s32 id = menu->getItemCommandId(menu->getSelectedItem());

					switch(id)
					{
						case GUI_ID_OPEN_TEXTURE: // File -> Open Texture
							env->addFileOpenDialog(L"Please select a texture file to open");
						break;
						case GUI_ID_QUIT: // File -> Quit
							stopApp();
						break;
					}
				}
				break;

				case gui::EGET_FILE_SELECTED:
				{
					// load the model file, selected in the file open dialog
					gui::IGUIFileOpenDialog* dialog =
						(gui::IGUIFileOpenDialog*)event.GUIEvent.Caller;
					loadTexture(io::path(dialog->getFileName()).c_str());
				}
				break;

				default:
				break;
			}
		}

		return false;
	}

protected:

	// Application initialization
	// returns true when it was successful initialized, otherwise false.
	bool init(int argc, char *argv[])
	{
		// ask user for driver
		Config.DriverType=driverChoiceConsole();
		if (Config.DriverType==video::EDT_COUNT)
			return false;

		// create the device with the settings from our config
		Device = createDevice(Config.DriverType, Config.ScreenSize);
		if (!Device)
			return false;
		Device->setWindowCaption( DriverTypeNames[Config.DriverType] );
		Device->setEventReceiver(this);

		scene::ISceneManager* smgr = Device->getSceneManager();
		video::IVideoDriver * driver = Device->getVideoDriver ();
		gui::IGUIEnvironment* guiEnv = Device->getGUIEnvironment();

		// set a nicer font
		gui::IGUISkin* skin = guiEnv->getSkin();
		gui::IGUIFont* font = guiEnv->getFont("../../media/fonthaettenschweiler.bmp");
		if (font)
			skin->setFont(font);

		// remove some alpha value because it makes those menus harder to read otherwise
		video::SColor col3dHighLight( skin->getColor(gui::EGDC_APP_WORKSPACE) );
		col3dHighLight.setAlpha(255);
		video::SColor colHighLight( col3dHighLight );
		skin->setColor(gui::EGDC_HIGH_LIGHT, colHighLight );
		skin->setColor(gui::EGDC_3D_HIGH_LIGHT, col3dHighLight );

		// Add some textures which are useful to test material settings
		createDefaultTextures(driver);

		// create a menu
		gui::IGUIContextMenu * menuBar = guiEnv->addMenu();
		menuBar->addItem(L"File", -1, true, true);

		gui::IGUIContextMenu* subMenuFile = menuBar->getSubMenu(0);
		subMenuFile->addItem(L"Open texture ...", GUI_ID_OPEN_TEXTURE);
		subMenuFile->addSeparator();
		subMenuFile->addItem(L"Quit", GUI_ID_QUIT);

		// a static camera
		Camera = smgr->addCameraSceneNode (0, core::vector3df(0, 0, 0),
											core::vector3df(0, 0, 100),
											-1);

		// add the nodes which are used to show the materials
		scene::IMeshSceneNode* nodeL = smgr->addCubeSceneNode (30.0f, 0, -1,
										   core::vector3df(-35, 0, 100),
										   core::vector3df(0, 0, 0),
										   core::vector3df(1.0f, 1.0f, 1.0f));
		NodeLeft.init( nodeL, Device, core::position2d<s32>(10,20), L"left node" );

		scene::IMeshSceneNode* nodeR = smgr->addCubeSceneNode (30.0f, 0, -1,
										   core::vector3df(35, 0, 100),
										   core::vector3df(0, 0, 0),
										   core::vector3df(1.0f, 1.0f, 1.0f));
		NodeRight.init( nodeR, Device, core::position2d<s32>(530,20), L"right node" );

		// add one light
		scene::ILightSceneNode* nodeLight = smgr->addLightSceneNode(0, core::vector3df(0, 0, 0),
														video::SColorf(1.0f, 1.0f, 1.0f),
														100.0f);
		LightControl.init(nodeLight, guiEnv, core::position2d<s32>(270,20), L"light" );

		// one large cube around everything. That's mainly to make the light more obvious.
		scene::IMeshSceneNode* backgroundCube = smgr->addCubeSceneNode (200.0f, 0, -1, core::vector3df(0, 0, 0),
										   core::vector3df(45, 0, 0),
										   core::vector3df(1.0f, 1.0f, 1.0f));
		backgroundCube->getMaterial(0).BackfaceCulling = false;	 		// we are within the cube, so we have to disable backface culling to see it
		backgroundCube->getMaterial(0).EmissiveColor.set(255,50,50,50);	// we keep some self lighting to keep texts visible

		// set the ambient light value
		GlobalAmbient = new CColorControl( guiEnv, core::position2d<s32>(270, 300), L"global ambient", guiEnv->getRootGUIElement());
		GlobalAmbient->setColor( smgr->getAmbientLight().toSColor() );

		return true;
	}

	// Update one frame
	bool update()
	{
		using namespace irr;

		video::IVideoDriver* videoDriver =  Device->getVideoDriver();
		if ( !Device->run() )
			return false;

		if ( Device->isWindowActive() || Config.RenderInBackground )
		{
			gui::IGUIEnvironment* guiEnv = Device->getGUIEnvironment();
			scene::ISceneManager* smgr = Device->getSceneManager();
			gui::IGUISkin * skin = guiEnv->getSkin();

			// update our controls
			NodeLeft.update();
			NodeRight.update();
			LightControl.update();

			// update ambient light settings
			if ( GlobalAmbient->isDirty() )
			{
				smgr->setAmbientLight( GlobalAmbient->getColor() );
				GlobalAmbient->resetDirty();
			}

			// draw everything
			video::SColor bkColor( skin->getColor(gui::EGDC_APP_WORKSPACE) );
			videoDriver->beginScene(true, true, bkColor);

			smgr->drawAll();
			guiEnv->drawAll();

			videoDriver->endScene();
		}

		return true;
	}

	// Run the application. Our main loop.
	void run()
	{
		IsRunning = true;

		if ( !Device )
			return;

		// main application loop
		while(IsRunning)
		{
			if ( !update() )
				break;

			Device->sleep( 5 );
		}
	}

	// Close down the application
	void quit()
	{
		IsRunning = false;
		GlobalAmbient->drop();
		GlobalAmbient = NULL;
		if ( Device )
		{
			Device->closeDevice();
			Device->drop();
			Device = NULL;
		}
	}

	// Create some useful textures.
	// Note that the function put readability over speed, you shouldn't use setPixel at runtime but for initialization it's nice.
	void createDefaultTextures(video::IVideoDriver * driver)
	{
		const u32 width = 256;
		const u32 height = 256;
		video::IImage * imageA8R8G8B8 = driver->createImage (video::ECF_A8R8G8B8, core::dimension2d<u32>(width, height));
		if ( !imageA8R8G8B8 )
			return;
		const u32 pitch = imageA8R8G8B8->getPitch();

		// some nice square-pattern with 9 typical colors
		for ( u32 y = 0; y < height; ++ y )
		{
			for ( u32 x = 0; x < pitch; ++x )
			{
				if ( y < height/3 )
				{
					if ( x < width/3 )
						imageA8R8G8B8->setPixel (x, y, SCOL_BLACK);
					else if ( x < 2*width/3 )
						imageA8R8G8B8->setPixel (x, y, SCOL_BLUE);
					else
						imageA8R8G8B8->setPixel (x, y, SCOL_CYAN);
				}
				else if ( y < 2*height/3 )
				{
					if ( x < width/3 )
						imageA8R8G8B8->setPixel (x, y, SCOL_GRAY);
					else if ( x < 2*width/3 )
						imageA8R8G8B8->setPixel (x, y, SCOL_GREEN);
					else
						imageA8R8G8B8->setPixel (x, y, SCOL_MAGENTA);
				}
				else
				{
					if ( x < width/3 )
						imageA8R8G8B8->setPixel (x, y, SCOL_RED);
					else if ( x < 2*width/3 )
						imageA8R8G8B8->setPixel (x, y, SCOL_YELLOW);
					else
						imageA8R8G8B8->setPixel (x, y, SCOL_WHITE);
				}
			}
		}
		driver->addTexture (io::path("CARO_A8R8G8B8"), imageA8R8G8B8);

		// all white
		imageA8R8G8B8->fill(SCOL_WHITE);
		driver->addTexture (io::path("WHITE_A8R8G8B8"), imageA8R8G8B8);

		// all black
		imageA8R8G8B8->fill(SCOL_BLACK);
		driver->addTexture (io::path("BLACK_A8R8G8B8"), imageA8R8G8B8);

		// gray-scale
		for ( u32 y = 0; y < height; ++ y )
		{
			for ( u32 x = 0; x < pitch; ++x )
			{
				imageA8R8G8B8->setPixel (x, y, video::SColor(y, x,x,x) );
			}
		}
		driver->addTexture (io::path("GRAYSCALE_A8R8G8B8"), imageA8R8G8B8);

		imageA8R8G8B8->drop();
	}

	// Load a texture and make sure nodes know it when more textures are available.
	void loadTexture(const io::path &name)
	{
		Device->getVideoDriver()->getTexture(name);
		NodeLeft.updateTextures();
		NodeRight.updateTextures();
	}

private:
	SConfig						Config;
	volatile bool				IsRunning;
	IrrlichtDevice * 			Device;
	scene::ICameraSceneNode *	Camera;
	SMeshNodeControl			NodeLeft;
	SMeshNodeControl			NodeRight;
	SLightNodeControl			LightControl;
	CColorControl *				GlobalAmbient;
};

/*
  A very short main as we do everything else in classes.
*/
int main(int argc, char *argv[])
{
	CApp APP;

	if ( !APP.init(argc, argv) )
	{
		printf("init failed\n");
		return 1;
	}

	APP.run();
	APP.quit();

	return 0;
}

/*
**/
