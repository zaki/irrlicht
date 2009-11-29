/** Example 022 Material Viewer

This example can be used to play around with material settings and watch the results.
Only the default non-shader materials are used in here.
*/

#include <irrlicht.h>
#include <iostream>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

namespace	// empty namespace makes sure all variables in here are only defined local in this file.
{
    const wchar_t* const DriverTypeNames[] =
    {
        L"NULL",
        L"SOFTWARE",
        L"BURNINGSVIDEO",
        L"DIRECT3D8",
        L"DIRECT3D9",
        L"EDT_OPENGL",
        0,
    };

	enum EGUI_IDS
	{
		GUI_ID_OPEN_TEXTURE = 1,
		GUI_ID_QUIT,
	};

	const core::stringw CLEAR_TEXTURE = "CLEAR texture";

	const irr::video::SColor SCOL_BLACK     = irr::video::SColor(255, 0,   0,   0);
	const irr::video::SColor SCOL_BLUE      = irr::video::SColor(255, 0,   0,  255);
	const irr::video::SColor SCOL_CYAN      = irr::video::SColor(255, 0,  255, 255);
	const irr::video::SColor SCOL_GRAY      = irr::video::SColor(255, 128,128, 128);
	const irr::video::SColor SCOL_GREEN     = irr::video::SColor(255, 0,  255,  0);
	const irr::video::SColor SCOL_MAGENTA   = irr::video::SColor(255, 255, 0,  255);
	const irr::video::SColor SCOL_RED       = irr::video::SColor(255, 255, 0,   0);
	const irr::video::SColor SCOL_YELLOW    = irr::video::SColor(255, 255, 255, 0);
	const irr::video::SColor SCOL_WHITE     = irr::video::SColor(255, 255, 255, 255);
};	// namespace

/*
Returns a new unique number on each call
*/
s32 makeUniqueId()
{
	static int unique = 10000;
	++unique;
	return unique;
}

/*
Find out which vertex-type is needed for the given material type
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
Control for setting colors
NOTE: Like with other controls it might have been a good idea deriving this from IGUIElement and make real gui controls of those. 
This also works, but using real gui-controls might be cleaner.
*/
class CColorControl : public IEventReceiver
{
public:
	CColorControl()
		: DirtyFlag(true)
		, ColorStatic(0)
		, EditAlpha(0)
		, EditRed(0)
		, EditGreen(0)
		, EditBlue(0)
	{
		ButtonSetId = makeUniqueId();
	}

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

	void init( gui::IGUIEnvironment* guiEnv, const core::position2d<s32> & pos, const wchar_t *text )
	{
		using namespace gui;

		const core::rect< s32 > rectControls(pos, core::dimension2d<s32>(80, 75));
		IGUIStaticText * groupElement =	guiEnv->addStaticText (L"", rectControls, true, false, 0, -1, false);

		guiEnv->addStaticText (text, core::rect<s32>(0,0,80,15), false, false, groupElement, -1, false);

		EditAlpha = addEditForNumbers(guiEnv, core::position2d<s32>(0,15), L"a", -1, groupElement );
		EditRed = addEditForNumbers(guiEnv, core::position2d<s32>(0,30), L"r", -1, groupElement );
		EditGreen = addEditForNumbers(guiEnv, core::position2d<s32>(0,45), L"g", -1, groupElement );
		EditBlue = addEditForNumbers(guiEnv, core::position2d<s32>(0,60), L"b", -1, groupElement );

		ColorStatic = guiEnv->addStaticText (L"", core::rect<s32>(60,15,80,75), true, false, groupElement, -1, true);

		guiEnv->addButton (core::rect<s32>(60,35,80,50), groupElement, ButtonSetId, L"set");
		SetEditsFromColor(Color);
	}

	void setColor(const video::SColor& col)
	{
		setDirty(true);
		Color = col;
		SetEditsFromColor(Color);
	}

	const video::SColor& getColor() const
	{
		return Color;
	}

	void setDirty(bool dirty)
	{
		DirtyFlag = dirty;
	}

	// when the color was changed the dirty flag is set
	bool isDirty() const
	{
		return DirtyFlag;
	};

protected:

	gui::IGUIEditBox* addEditForNumbers(gui::IGUIEnvironment* guiEnv, const core::position2d<s32> & pos, const wchar_t *text, s32 id, gui::IGUIElement * parent)
	{
		using namespace gui;

		core::rect< s32 > rect(pos, core::dimension2d<s32>(10, 15));
		guiEnv->addStaticText (text, rect, false, false, parent, -1, false);
		rect += core::position2d<s32>( 20, 0 );
		rect.LowerRightCorner.X += 20;
		gui::IGUIEditBox* edit = guiEnv->addEditBox(L"0", rect, true, parent, id);
		return edit;
	}

	video::SColor GetColorFromEdits()
	{
		video::SColor col;

		u32 alpha=col.getAlpha();
		if ( EditAlpha )
		{
			alpha = (u32)core::strtol10(  core::stringc( EditAlpha->getText() ).c_str(), 0);
			if ( alpha > 255 )
				alpha = 255;
		}
		col.setAlpha(alpha);

		u32 red=col.getRed();
		if ( EditRed )
		{
			red = (u32)core::strtol10( core::stringc( EditRed->getText() ).c_str(), 0);
			if ( red > 255 )
				red = 255;
		}
		col.setRed(red);

		u32 green=col.getGreen();
		if ( EditGreen )
		{
			green = (u32)core::strtol10( core::stringc( EditGreen->getText() ).c_str(), 0);
			if ( green > 255 )
				green = 255;
		}
		col.setGreen(green);


		u32 blue=col.getBlue();
		if ( EditBlue )
		{
			blue = (u32)core::strtol10( core::stringc( EditBlue->getText() ).c_str(), 0);
			if ( blue > 255 )
				blue = 255;
		}
		col.setBlue(blue);

		return col;
	}

	void SetEditsFromColor(video::SColor col)
	{
		setDirty(true);
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
Control for setting colors typically used in materials
*/
struct SAllColorsControl
{
	virtual bool OnEvent(const SEvent &event)
	{
		if ( ControlAmbientColor.OnEvent(event) )
			return true;
		if ( ControlDiffuseColor.OnEvent(event) )
			return true;
		if ( ControlEmissiveColor.OnEvent(event) )
			return true;
		if ( ControlSpecularColor.OnEvent(event) )
			return true;
		return false;
	}

	void init(gui::IGUIEnvironment* guiEnv, const core::position2d<s32> & pos, const wchar_t * description, bool hasEmissive)
	{
		core::rect<s32> rect(pos, core::dimension2d<s32>(60, 15));
		guiEnv->addStaticText (description, rect, false, false, 0, -1, false);
		createColorControls(guiEnv, pos + core::position2d<s32>(0, 15), hasEmissive);
	}

	// any colors changed?
	bool isDirty() const
	{
		return ControlAmbientColor.isDirty() || ControlDiffuseColor.isDirty() || ControlSpecularColor.isDirty() || ControlEmissiveColor.isDirty();
	}

	void setDirty(bool dirty)
	{
		ControlAmbientColor.setDirty(dirty);
		ControlDiffuseColor.setDirty(dirty);
		ControlSpecularColor.setDirty(dirty);
		ControlEmissiveColor.setDirty(dirty);
	}

	CColorControl	ControlAmbientColor;
	CColorControl	ControlDiffuseColor;
	CColorControl	ControlSpecularColor;
	CColorControl	ControlEmissiveColor;

protected:
	void createColorControls(gui::IGUIEnvironment* guiEnv, const core::position2d<s32> & pos, bool hasEmissive)
	{
		ControlAmbientColor.init( guiEnv, pos, L"ambient" );
		ControlDiffuseColor.init( guiEnv, pos + core::position2d<s32>(0, 75), L"diffuse" );
		ControlSpecularColor.init( guiEnv, pos + core::position2d<s32>(0, 150), L"specular" );
		if ( hasEmissive )
		{
			ControlEmissiveColor.init( guiEnv, pos + core::position2d<s32>(0, 225), L"emissive" );
		}
	}
};

/*
Control to offer a selection of available textures
*/
struct STextureControl
{
	STextureControl() : Initialized(false), DirtyFlag(false), ComboTexture(0)
	{
	}

	virtual bool OnEvent(const SEvent &event)
	{
		if ( event.EventType != EET_GUI_EVENT )
			return false;

		if ( event.GUIEvent.Caller == ComboTexture && event.GUIEvent.EventType == gui::EGET_COMBO_BOX_CHANGED )
		{
			setDirty(true);
		}

		return false;
	}

	// return selected texturename (if any, otherwise 0)
	const wchar_t * getSelectedTextureName() const
	{
		if ( !Initialized )
			return 0;

		s32 selected = ComboTexture->getSelected();
		if ( selected < 0 )
			return 0;
		return ComboTexture->getItem(selected);
	}

	void setDirty(bool dirty)
	{
		DirtyFlag = dirty;
	}

	// when the texture was changed the dirty flag is set
	bool isDirty() const
	{
		return DirtyFlag;
	};

	void init(gui::IGUIEnvironment* guiEnv, video::IVideoDriver * driver, const core::position2d<s32> & pos)
	{
		if ( Initialized || !guiEnv || !driver )
			return;
		core::rect<s32> rectCombo(pos.X, pos.Y, pos.X+100, pos.Y+15);
		ComboTexture = guiEnv->addComboBox (rectCombo);
		Initialized = true;

		updateTextures(driver);
	}

	void updateTextures(video::IVideoDriver * driver)
	{
		if ( !Initialized )
			return;

		s32 oldSelected = ComboTexture->getSelected();
		s32 selectNew = -1;
		const wchar_t * oldTextureName = 0;
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
			if ( oldTextureName && selectNew < 0 && name == oldTextureName )
				selectNew = i;
		}

		ComboTexture->addItem( CLEAR_TEXTURE.c_str() );
		if ( CLEAR_TEXTURE == oldTextureName )
			selectNew = ComboTexture->getItemCount()-1;

		if ( selectNew >= 0 )
			ComboTexture->setSelected(selectNew);

		setDirty(true);
	}

private:
	bool Initialized;
	bool DirtyFlag;
	gui::IGUIComboBox * ComboTexture;
};

/*
Control which allows setting typical material values for a meshscenenode
*/
struct SMeshNodeControl
{
	SMeshNodeControl() : Initialized(false), Driver(0), MeshManipulator(0), SceneNode(0), SceneNode2T(0), SceneNodeTangents(0), ButtonLighting(0), ComboMaterial(0) {}

	virtual bool OnEvent(const SEvent &event)
	{
		if ( AllColorsControl.OnEvent(event) )
			return true;
		if ( Texture1.OnEvent(event) )
			return true;
		if ( Texture2.OnEvent(event) )
			return true;
		if ( ControlVertexColors.OnEvent(event) )
			return true;
		return false;
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
		AllColorsControl.init(guiEnv, pos, description, true);
		AllColorsControl.ControlAmbientColor.setColor(material.AmbientColor);
		AllColorsControl.ControlDiffuseColor.setColor(material.DiffuseColor);
		AllColorsControl.ControlEmissiveColor.setColor(material.EmissiveColor);
		AllColorsControl.ControlSpecularColor.setColor(material.SpecularColor);

		core::rect<s32> rectBtn(pos + core::position2d<s32>(0, 320), core::dimension2d<s32>(100, 15));
		ButtonLighting = guiEnv->addButton (rectBtn, 0, -1, L"Lighting");
		ButtonLighting->setIsPushButton(true);
		ButtonLighting->setPressed(material.Lighting);

		core::rect<s32> rectCombo(pos.X, rectBtn.LowerRightCorner.Y, pos.X+100, rectBtn.LowerRightCorner.Y+15);
		ComboMaterial = guiEnv->addComboBox (rectCombo);
		for ( int i=0; i <= (int)video::EMT_ONETEXTURE_BLEND; ++i )
		{
			ComboMaterial->addItem( core::stringw(video::sBuiltInMaterialTypeNames[i]).c_str() );
		}
		ComboMaterial->setSelected( (s32)material.MaterialType );

		core::position2d<s32> posTex(rectCombo.UpperLeftCorner.X,rectCombo.LowerRightCorner.Y);
		Texture1.init(guiEnv, Driver, posTex);
		posTex.Y += 15;
		Texture2.init(guiEnv, Driver, posTex);
		
		core::position2d<s32> posVertexColors( posTex.X, posTex.Y + 15);
		ControlVertexColors.init( guiEnv, posVertexColors, L"Vertex colors");
		
		video::S3DVertex * vertices =  (video::S3DVertex *)node->getMesh()->getMeshBuffer(0)->getVertices();
		if ( vertices )
		{
			ControlVertexColors.setColor(vertices[0].Color);
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

        AllColorsControl.setDirty(false);
        Texture1.setDirty(false);
        Texture2.setDirty(false);
		ControlVertexColors.setDirty(false);
	}

	void updateTextures()
	{
		Texture1.updateTextures(Driver);
		Texture2.updateTextures(Driver);
	}

protected:

    void updateMaterial(video::SMaterial & material)
    {
		if ( AllColorsControl.isDirty() )
		{
			material.AmbientColor = AllColorsControl.ControlAmbientColor.getColor();
			material.DiffuseColor = AllColorsControl.ControlDiffuseColor.getColor();
			material.EmissiveColor = AllColorsControl.ControlEmissiveColor.getColor();
			material.SpecularColor = AllColorsControl.ControlSpecularColor.getColor();
		}
		material.Lighting = ButtonLighting->isPressed();
		if ( Texture1.isDirty() )
		{
			material.TextureLayer[0].Texture = Driver->getTexture( io::path(Texture1.getSelectedTextureName()) );
		}
		if ( Texture2.isDirty() )
		{
			material.TextureLayer[1].Texture = Driver->getTexture( io::path(Texture2.getSelectedTextureName()) );
		}
	    if ( ControlVertexColors.isDirty() )
	    {
		    MeshManipulator->setVertexColors (SceneNode->getMesh(), ControlVertexColors.getColor());
		    MeshManipulator->setVertexColors (SceneNode2T->getMesh(), ControlVertexColors.getColor());
		    MeshManipulator->setVertexColors (SceneNodeTangents->getMesh(), ControlVertexColors.getColor());
	    }
    }

	bool Initialized;
	video::IVideoDriver * Driver;
	scene::IMeshManipulator* MeshManipulator;
	scene::IMeshSceneNode* SceneNode;
	scene::IMeshSceneNode* SceneNode2T;
	scene::IMeshSceneNode* SceneNodeTangents;
	SAllColorsControl AllColorsControl;
	gui::IGUIButton * ButtonLighting;
	gui::IGUIComboBox * ComboMaterial;
	STextureControl Texture1;
	STextureControl Texture2;
	CColorControl	ControlVertexColors;
};

/*
Control to allow setting the color values of a lightscenenode.
*/
struct SLightNodeControl
{
	SLightNodeControl() : Initialized(false), SceneNode(0) {}

	virtual bool OnEvent(const SEvent &event)
	{
		if ( AllColorsControl.OnEvent(event) )
			return true;
		return false;
	}

	void init(scene::ILightSceneNode* node, gui::IGUIEnvironment* guiEnv, const core::position2d<s32> & pos, const wchar_t * description)
	{
		if ( Initialized || !node || !guiEnv) // initializing twice or with invalid data not allowed
			return;
		SceneNode = node;
		AllColorsControl.init(guiEnv, pos, description, false);
		const video::SLight & lightData = SceneNode->getLightData();
		if ( AllColorsControl.isDirty() )
		{
			AllColorsControl.ControlAmbientColor.setColor(lightData.AmbientColor.toSColor());
			AllColorsControl.ControlDiffuseColor.setColor(lightData.DiffuseColor.toSColor());
			AllColorsControl.ControlSpecularColor.setColor(lightData.SpecularColor.toSColor());
			AllColorsControl.setDirty(false);
		}
		Initialized = true;
	}

	void update()
	{
		if ( !Initialized )
			return;

		video::SLight & lightData = SceneNode->getLightData();
		lightData.AmbientColor = video::SColorf( AllColorsControl.ControlAmbientColor.getColor() );
		lightData.DiffuseColor = video::SColorf( AllColorsControl.ControlDiffuseColor.getColor() );
		lightData.SpecularColor = video::SColorf( AllColorsControl.ControlSpecularColor.getColor() );
	}

protected:
	bool Initialized;
	scene::ILightSceneNode* SceneNode;
	SAllColorsControl AllColorsControl;
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
    CApp()
	: IsRunning(false)
	, Device(0)
	, Camera(0)
	{
	}

    ~CApp()
	{
	}

	// stop running - will quit at end of mainloop
    void stopApp()
	{
		IsRunning = false;
	}

	virtual bool OnEvent(const SEvent &event)
	{
		if ( NodeLeft.OnEvent(event) )
			return true;
		if ( NodeRight.OnEvent(event) )
			return true;
		if ( LightControl.OnEvent(event) )
			return true;
		if ( GlobalAmbient.OnEvent(event) )
			return true;

		if (event.EventType == EET_GUI_EVENT)
		{
			//s32 id = event.GUIEvent.Caller->getID();
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

    bool init(int argc, char *argv[])
	{
		Config.DriverType = getDriverTypeFromConsole();
		if ( (int)Config.DriverType < 0 )
			return false;

		Device = createDevice(Config.DriverType, Config.ScreenSize);
		if (!Device)
			return false;

        Device->setWindowCaption( DriverTypeNames[Config.DriverType] );
		Device->setEventReceiver(this);
		scene::ISceneManager* smgr = Device->getSceneManager();
		video::IVideoDriver * driver = Device->getVideoDriver ();
		gui::IGUIEnvironment* guiEnv = Device->getGUIEnvironment();

		createDefaultTextures(driver);

		gui::IGUIContextMenu * menuBar = guiEnv->addMenu();
		menuBar->addItem(L"File", -1, true, true);

		gui::IGUIContextMenu* subMenuFile = menuBar->getSubMenu(0);
		subMenuFile->addItem(L"Open texture ...", GUI_ID_OPEN_TEXTURE);
		subMenuFile->addSeparator();
		subMenuFile->addItem(L"Quit", GUI_ID_QUIT);

		Camera = smgr->addCameraSceneNode (0, core::vector3df(0, 0, 0),
											core::vector3df(0, 0, 100),
		                                    -1);

		scene::IMeshSceneNode* nodeL = smgr->addCubeSceneNode (10.0f, 0, -1,
		                                   core::vector3df(-35, 0, 100),
		                                   core::vector3df(0, 0, 0),
		                                   core::vector3df(3.0f, 3.0f, 3.0f));
		NodeLeft.init( nodeL, Device, core::position2d<s32>(10,20), L"left node" );

		scene::IMeshSceneNode* nodeR = smgr->addCubeSceneNode (10.0f, 0, -1,
		                                   core::vector3df(35, 0, 100),
		                                   core::vector3df(0, 0, 0),
		                                   core::vector3df(3.0f, 3.0f, 3.0f));
		NodeRight.init( nodeR, Device, core::position2d<s32>(530,20), L"right node" );

		scene::ILightSceneNode* nodeLight = smgr->addLightSceneNode(0, core::vector3df(0, 0, 0),
														video::SColorf(1.0f, 1.0f, 1.0f),
														100.0f);
		LightControl.init(nodeLight, guiEnv, core::position2d<s32>(270,20), L"light" );

		GlobalAmbient.init( guiEnv, core::position2d<s32>(270, 300), L"global ambient" );
		GlobalAmbient.setColor( smgr->getAmbientLight().toSColor() );

		return true;
	}

	video::E_DRIVER_TYPE getDriverTypeFromConsole()
	{
		printf("Please select the driver you want for this example:\n"\
			" (a) Direct3D 9.0c\n (b) Direct3D 8.1\n (c) OpenGL 1.5\n"\
			" (d) Software Renderer\n (e) Burning's Software Renderer\n"\
			" (f) NullDevice\n (otherKey) exit\n\n");

		char i;
		std::cin >> i;

		switch(i)
		{

			case 'a': return video::EDT_DIRECT3D9;
			case 'b': return video::EDT_DIRECT3D8;
			case 'c': return video::EDT_OPENGL;
			case 'd': return video::EDT_SOFTWARE;
			case 'e': return video::EDT_BURNINGSVIDEO;
			case 'f': return video::EDT_NULL;
			default: return video::E_DRIVER_TYPE(-1);
		}
	}

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

			// remove some alpha value because it makes those menus harder to read otherwise
			video::SColor col3dHighLight( skin->getColor(gui::EGDC_APP_WORKSPACE) );
			col3dHighLight.setAlpha(255);
			video::SColor colHighLight( col3dHighLight );
			skin->setColor(gui::EGDC_HIGH_LIGHT, colHighLight );
			skin->setColor(gui::EGDC_3D_HIGH_LIGHT, col3dHighLight );

			NodeLeft.update();
			NodeRight.update();
			LightControl.update();

			if ( GlobalAmbient.isDirty() )
			{
				smgr->setAmbientLight( GlobalAmbient.getColor() );
				GlobalAmbient.setDirty(false);
			}

			video::SColor bkColor( skin->getColor(gui::EGDC_APP_WORKSPACE) );
			videoDriver->beginScene(true, true, bkColor);

			smgr->drawAll();
			guiEnv->drawAll();

			videoDriver->endScene();
		}

		return true;
	}

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

    void quit()
	{
		IsRunning = false;
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

		// some nice caro with 9 useful colors
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

		imageA8R8G8B8->fill(SCOL_WHITE);
		driver->addTexture (io::path("WHITE_A8R8G8B8"), imageA8R8G8B8);

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
	}

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
	CColorControl				GlobalAmbient;
};

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
