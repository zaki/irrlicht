// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "Color.h"
#include "Vector3D.h"
#include "Matrix4.h"
#include "ITexture.h"
#include "Material.h"
#include "Position2D.h"
#include "Rect.h"
#include "Vertex3D.h"
#include "Triangle3D.h"
#include "Box3D.h"
#include "Light.h"
#include "IAnimatedMesh.h"
#include "ISceneNode.h"
#include "ICameraSceneNode.h"
#include "IAnimatedMeshSceneNode.h"
#include "ILightSceneNode.h"
#include "IBillboardSceneNode.h"
#include "IParticleSystemSceneNode.h"
#include "IGUIFont.h"

namespace Irrlicht
{
public __gc class IrrlichtDevice;

namespace Scene
{
	public __gc class ITriangleSelector;
	public __gc class IMetaTriangleSelector;
	public __gc class ISceneCollisionManager;
	public __gc class ITerrainSceneNode;
	public __gc class IMeshManipulator;

	public __gc class ISceneManager
	{
	public:

		/// <summary>
		/// You should access the ISceneManager 
		/// through the Irrlicht::IrrlichtDevice.SceneMAnager property. Simply don't use
		/// this constructor.
		///</summary>
		///<param name="manager">The real, unmanaged C++ scene manager</param>
		ISceneManager(irr::scene::ISceneManager* manager);

		/// <summary>
		/// Gets pointer to an animateable mesh. Loads it if needed.
		/// Currently there are the following mesh formats supported:
		/// .obj(Alias Wavefront Maya), .ms3d(Milkshape3D), .bsp(Quake3 Level),
		/// .md2(Quake2 Model), .3ds(3D Studio), .x(Microsoft DirectX) More formats coming soon,
		/// make a feature request on the Irrlicht Engine homepage if you like.
		/// Special thanks go to Dean P. Macri who extended the Quake 3
		/// .bsp loader with the curved surfaces feature.
		/// </summary>
		/// <param name="filename"> Filename of the mesh to load.</param>
		/// <returns> Returns NULL if failed and the pointer to the mesh if 
		/// successful. </returns>
		IAnimatedMesh* GetMesh(System::String* filename);

		/// <summary>
		/// Adds a cube scene node to the scene. It is of (1,1,1) size. 
		/// </summary>
		/// <param name="size"> Size of the cube.</param>
		/// <param name="parent"> Parent of the scene node. Can be NULL if no parent.</param>
		/// <param name="id"> Id of the node. This id can be used to identify the scene node.</param>
		/// <param name="position"> Position of the space relative to its parent where the
		/// scene node will be placed.</param>
		/// <param name="rotation"> Initital rotation of the scene node.</param>
		/// <param name="scale"> Initial scale of the scene node.</param>
		/// <returns> Returns pointer to the created test scene node.</returns>
		ISceneNode* AddCubeSceneNode(float size, ISceneNode* parent, int id,
			 Core::Vector3D position, Core::Vector3D rotation, Core::Vector3D scale);
		ISceneNode* AddCubeSceneNode(float size, ISceneNode* parent, int id, Core::Vector3D position);

		/// <summary>
		/// Adds a scene node for rendering an animated mesh model.
		/// </summary>
		/// <param name="mesh"> Pointer to the loaded animated mesh to be displayed.</param>
		/// <param name="parent"> Parent of the scene node. Can be NULL if no parent.</param>
		/// <param name="id"> Id of the node. This id can be used to identify the scene node.</param>
		/// <param name="position"> Position of the space relative to its parent where the
		/// scene node will be placed.</param>
		/// <param name="rotation"> Initital rotation of the scene node.</param>
		/// <param name="scale">: Initial scale of the scene node.</param>
		/// <returns> Returns pointer to the created scene node.</returns>
		IAnimatedMeshSceneNode* AddAnimatedMeshSceneNode(IAnimatedMesh* mesh, ISceneNode* parent, int id,
			 Core::Vector3D position, Core::Vector3D rotation, Core::Vector3D scale);
		IAnimatedMeshSceneNode* AddAnimatedMeshSceneNode(IAnimatedMesh* mesh, ISceneNode* parent, int id);

		/// <summary>
		/// Adds a scene node for rendering a static mesh.
		/// </summary>
		/// <param name="mesh"> Pointer to the loaded static mesh to be displayed.</param>
		/// <param name="parent"> Parent of the scene node. Can be NULL if no parent.</param>
		/// <param name="id"> Id of the node. This id can be used to identify the scene node.</param>
		/// <param name="position"> Position of the space relative to its parent where the
		/// scene node will be placed.</param>
		/// <param name="rotation"> Initital rotation of the scene node.</param>
		/// <param name="scale"> Initial scale of the scene node.</param>
		/// <returns> Returns pointer to the created scene node.</returns>
		ISceneNode* AddMeshSceneNode(IMesh* mesh, ISceneNode* parent, int id);

		/// <summary>
		/// Adds a scene node for rendering a static mesh.
		/// </summary>
		/// <param name="mesh"> Pointer to the loaded static mesh to be displayed.</param>
		/// <param name="parent"> Parent of the scene node. Can be NULL if no parent.</param>
		/// <param name="id"> Id of the node. This id can be used to identify the scene node.</param>
		/// <param name="position"> Position of the space relative to its parent where the
		/// scene node will be placed.</param>
		/// <param name="rotation"> Initital rotation of the scene node.</param>
		/// <param name="scale"> Initial scale of the scene node.</param>
		/// <returns> Returns pointer to the created scene node.</returns>
		ISceneNode* AddMeshSceneNode(IMesh* mesh, ISceneNode* parent, int id,
			 Core::Vector3D position, Core::Vector3D rotation, Core::Vector3D scale);

		/// <summary>
		/// Adds a scene node for rendering a animated water surface mesh.
		/// Looks really good when the Material type EMT_TRANSPARENT_REFLECTION
		/// is used.
		/// </summary>
		/// <param name="waveHeight"> Height of the water waves. A good value would be 2.0</param>
		/// <param name="waveSpeed"> Speed of the water waves. A good value would be 300.0.</param>
		/// <param name="waveLenght"> Lenght of a water wave. A good value would be 10.0</param>
		/// <param name="mesh"> Pointer to the loaded static mesh to be displayed with water waves on it.</param>
		/// <param name="parent"> Parent of the scene node. Can be NULL if no parent.</param>
		/// <param name="id"> Id of the node. This id can be used to identify the scene node.</param>
		/// <param name="position"> Position of the space relative to its parent where the
		/// scene node will be placed.</param>
		/// <param name="rotation"> Initital rotation of the scene node.</param>
		/// <param name="scale"> Initial scale of the scene node.</param>
		/// <returns> Returns pointer to the created scene node.</returns>
		ISceneNode* AddWaterSurfaceSceneNode(IMesh* mesh,
			float waveHeight, float waveSpeed, float waveLenght, 
			ISceneNode* parent, int id);

		/// <summary>
		/// Adds a scene node for rendering using a octtree to the scene graph. This a good method for rendering 
		/// scenes with lots of geometry. The Octree is built on the fly from the mesh, much
		/// faster then a bsp tree.
		/// </summary>
		/// <param name="mesh"> The mesh containing all geometry from which the octtree will be build.
		/// If this animated mesh has more than one frames in it, the first frame is taken.</param>
		/// <param name="parent"> Parent node of the octtree node. Can be null for no parent.</param>
		/// <param name="id"> id of the node. This id can be used to identify the node.</param>
		/// <param name="minimalPolysPerNode"> Specifies the minimal polygons contained a octree node.
		/// If a node gets less polys the this value, it will not be splitted into
		/// smaller nodes. (Default: 128)</param>
		/// <returns> Returns the pointer to the octtree if successful, otherwise 0. 
		/// </returns>
		ISceneNode* AddOctTreeSceneNode(IAnimatedMesh* mesh, ISceneNode* parent, 
			int id);
		ISceneNode* AddOctTreeSceneNode(IAnimatedMesh* mesh, ISceneNode* parent, 
			int id, int minimalPolysPerNode);

		/// <summary>
		/// Adds a scene node for rendering using a octtree to the scene graph. This a good method for rendering 
		/// scenes with lots of geometry. The Octree is built on the fly from the mesh, much
		/// faster then a bsp tree.
		/// </summary>
		/// <param name="mesh"> The mesh containing all geometry from which the octtree will be build.</param>
		/// <param name="parent"> Parent node of the octtree node. Can be null for no parent.</param>
		/// <param name="id"> id of the node. This id can be used to identify the node.</param>
		/// <param name="minimalPolysPerNode"> Specifies the minimal polygons contained a octree node.
		/// If a node gets less polys the this value, it will not be splitted into
		/// smaller nodes. (Default: 128)</param>
		/// <returns> Returns the pointer to the octtree if successful, otherwise 0. 
		/// </returns>
		ISceneNode* AddOctTreeSceneNode(IMesh* mesh, ISceneNode* parent, int id);

		/// <summary>
		/// Adds a scene node for rendering using a octtree to the scene graph. This a good method for rendering 
		/// scenes with lots of geometry. The Octree is built on the fly from the mesh, much
		/// faster then a bsp tree.
		/// </summary>
		/// <param name="mesh"> The mesh containing all geometry from which the octtree will be build.</param>
		/// <param name="parent"> Parent node of the octtree node. Can be null for no parent.</param>
		/// <param name="id"> id of the node. This id can be used to identify the node.</param>
		/// <param name="minimalPolysPerNode"> Specifies the minimal polygons contained a octree node.
		/// If a node gets less polys the this value, it will not be splitted into
		/// smaller nodes. (Default: 128)</param>
		/// <returns> Returns the pointer to the octtree if successful, otherwise 0. 
		/// </returns>
		ISceneNode* AddOctTreeSceneNode(IMesh* mesh, ISceneNode* parent, int id, int minimalPolysPerNode);
		
		/// <summary>
		/// Adds a camera scene node to the scene graph and sets it as active camera.
		/// </summary>
		/// <param name="position"> Position of the space relative to its parent where the camera will be placed.</param>
		/// <param name="lookat"> Position where the camera will look at. Also known as target.</param>
		/// <param name="parent"> Parent scene node of the camera. Can be null. If the parent moves,
		/// the camera will move too.</param>
		/// <param name="id"> id of the camera. This id can be used to identify the camera.</param>
		/// <returns> Returns pointer to interface to camera if successful, otherwise 0.</returns>
		ICameraSceneNode* AddCameraSceneNode(ISceneNode* parent,
			 Core::Vector3D position, Core::Vector3D lookat, int id);

		/// <summary>
		/// Adds a camera scene node which is able to be controlled with the mouse similar
		/// like in the 3D Software Maya by Alias Wavefront.
		/// </summary>
		/// <param name="parent"> Parent scene node of the camera. Can be null.</param>
		/// <param name="rotateSpeed"> Rotation speed of the camera. Default value is -1500.0f.</param>
		/// <param name="zoomSpeed"> Zoom speed of the camera. Default is 200.0f.</param>
		/// <param name="tranlationSpeed"> TranslationSpeed of the camera. Default is 1500.0f</param>
		/// <param name="id"> id of the camera. This id can be used to identify the camera.</param>
		/// <returns> Returns a pointer to the interface of the camera if successful, otherwise 0.</returns>
		ICameraSceneNode* AddCameraSceneNodeMaya(ISceneNode* parent,
			float rotateSpeed, float zoomSpeed, float translationSpeed, int id);

		/// <summary>
		/// Adds a camera scene node which is able to be controled with the mouse and keys
		/// like in most first person shooters (FPS): 
		/// Look with the mouse, move with cursor keys. If you do not like the default
		/// key layout, you may want to specify your own. For example to make the camera
		/// be controlled by the cursor keys AND the keys W,A,S, and D, do something
		/// like this:
		/// </summary>
		/// <param name="parent"> Parent scene node of the camera. Can be null.</param>
		/// <param name="rotateSpeed"> Speed with wich the camera is rotated. This can be done
		/// only with the mouse. (Default 100)</param>
		/// <param name="movespeed"> Speed with which the camera is moved. Movement is done with
		/// the cursor keys. (Default: 500)</param>
		/// <param name="id"/> id of the camera. This id can be used to identify the camera.
		/// <param name="keyMapArray"/> Optional pointer to an array of a keymap, specifying what
		/// keys should be used to move the camera. If this is null, the default keymap
		/// is used. You can define actions more then one time in the array, to bind 
		/// multiple keys to the same action.
		/// <param name="keyMapSize"> Amount of items in the keymap array.</param>
		/// <returns> Returns a pointer to the interface of the camera if successful, otherwise 0.
		/// This pointer should not be dropped. See IUnknown::drop() for more information.</returns>
		ICameraSceneNode* AddCameraSceneNodeFPS();

		/// <summary>
		/// Adds a camera scene node which is able to be controled with the mouse and keys
		/// like in most first person shooters (FPS): 
		/// Look with the mouse, move with cursor keys. If you do not like the default
		/// key layout, you may want to specify your own. For example to make the camera
		/// be controlled by the cursor keys AND the keys W,A,S, and D, do something
		/// like this:
		/// </summary>
		/// <param name="parent"> Parent scene node of the camera. Can be null.</param>
		/// <param name="rotateSpeed"> Speed with wich the camera is rotated. This can be done
		/// only with the mouse. (Default 100)</param>
		/// <param name="movespeed"> Speed with which the camera is moved. Movement is done with
		/// the cursor keys. (Default: 500)</param>
		/// <param name="id"/> id of the camera. This id can be used to identify the camera.
		/// <param name="keyMapArray"/> Optional pointer to an array of a keymap, specifying what
		/// keys should be used to move the camera. If this is null, the default keymap
		/// is used. You can define actions more then one time in the array, to bind 
		/// multiple keys to the same action.
		/// <param name="keyMapSize"> Amount of items in the keymap array.</param>
		/// <returns> Returns a pointer to the interface of the camera if successful, otherwise 0.
		/// This pointer should not be dropped. See IUnknown::drop() for more information.</returns>
		ICameraSceneNode* AddCameraSceneNodeFPS(ISceneNode* parent,
			float rotateSpeed, float moveSpeed, int id);
	
		/// <summary>
		/// Adds a dynamic light scene node to the scene graph. The light will cast dynamic light on all
		/// other scene nodes in the scene, which have the material flag LIGHTING
		/// turned on. (This is the default setting in most scene nodes).
		/// </summary>
		/// <param name="parent"> Parent scene node of the light. Can be null. If the parent moves,
		/// the light will move too.</param>
		/// <param name="position"> Position of the space relative to its parent where the light will be placed.</param>
		/// <param name="color"> Diffuse color of the light. Ambient or Specular colors can be set manually with
		/// the ILightSceneNode::getLightData() method.</param>
		/// <param name="radius"> Radius of the light. Default is 100.</param>
		/// <param name="id"> id of the node. This id can be used to identify the node.</param>
		/// <returns> Returns pointer to the interface of the light if successful, otherwise NULL.</returns>
		ILightSceneNode* AddLightSceneNode(ISceneNode* parent,
			Core::Vector3D position,
			Video::Colorf color, float radius, int id);

		/// <summary>
		/// Adds a billboard scene node to the scene graph. A billboard is like a 3d sprite: A 2d element,
		/// which always looks to the camera. It is usually used for things like explosions, fire,
		/// lensflares and things like that.
		/// </summary>
		/// <param name="parent"> Parent scene node of the billboard. Can be null. If the parent moves,
		/// the billboard will move too.</param>
		/// <param name="position"> Position of the space relative to its parent where the billboard will be placed.</param>
		/// <param name="size"> Size of the billboard. This size is 2 dimensional because a billboard only has
		/// width and height.</param>
		/// <param name="id"> An id of the node. This id can be used to identify the node.</param>
		/// <returns> Returns pointer to the billboard if successful, otherwise NULL.</returns>
		IBillboardSceneNode* AddBillboardSceneNode(ISceneNode* parent,
			Core::Dimension2Df size, Core::Vector3D position, int id);

		/// <summary>
		/// Adds a skybox scene node to the scene graph. A skybox is a big cube with 6 textures on it and
		/// is drawed around the camera position. 
		/// </summary>
		/// <param name="top"> Texture for the top plane of the box.</param>
		/// <param name="bottom"> Texture for the bottom plane of the box.</param>
		/// <param name="left"> Texture for the left plane of the box.</param>
		/// <param name="right"> Texture for the right plane of the box.</param>
		/// <param name="front"> Texture for the front plane of the box.</param>
		/// <param name="parent"> Parent scene node of the skybox. A skybox usually has no parent,
		/// so this should be null. Note: If a parent is set to the skybox, the box will not 
		/// change how it is drawed.</param>
		/// <param name="id"> An id of the node. This id can be used to identify the node.</param>
		/// <returns> Returns a pointer to the sky box if successful, otherwise NULL.</returns>
		ISceneNode* AddSkyBoxSceneNode(Video::ITexture* top, Video::ITexture* bottom,
			Video::ITexture* left, Video::ITexture* right, Video::ITexture* front,
			Video::ITexture* back, ISceneNode* parent, int id);

		/// <summary>
		/// Adds a terrain scene node to the scene graph.
		/// This node 
		/// implements is a simple terrain renderer which uses
		/// a technique known as geo mip mapping
		/// for reducing the detail of triangle blocks which are far away.
		/// The code for the TerrainSceneNode is based on the terrain renderer by Soconne and 
	    /// the GeoMipMapSceneNode developed by Spinz. They made their code available for Irrlicht 
	    /// and allowed it to be  distributed under this licence. I only modified some parts.
	    /// A lot of thanks go to them. 
		///
		/// This scene node is capable of very quickly loading
		/// terrains and updating the indices at runtime to enable viewing very large terrains.  It uses a
		/// CLOD (Continuous Level of Detail) algorithm which updates the indices for each patch based on 
		/// a LOD (Level of Detail) which is determined based on a patch's distance from the camera.
		///
		/// The patch size of the terrain must always be a size of ( 2^N+1, i.e. 8+1(9), 16+1(17), etc. ).
		/// The MaxLOD available is directly dependent on the patch size of the terrain.  LOD 0 contains all
		/// of the indices to draw all the triangles at the max detail for a patch.  As each LOD goes up by 1
		/// the step taken, in generating indices increases by - 2^LOD, so for LOD 1, the step taken is 2, for
		/// LOD 2, the step taken is 4, LOD 3 - 8, etc.  The step can be no larger than the size of the patch,
		/// so having a LOD of 8, with a patch size of 17, is asking the algoritm to generate indices every 
		/// 2^8 ( 256 ) vertices, which is not possible with a patch size of 17.  The maximum LOD for a patch
		/// size of 17 is 2^4 ( 16 ).  So, with a MaxLOD of 5, you'll have LOD 0 ( full detail ), LOD 1 ( every
		/// 2 vertices ), LOD 2 ( every 4 vertices ), LOD 3 ( every 8 vertices ) and LOD 4 ( every 16 vertices ).
		/// </summary>
		/// <param name="heightMapFile"> The location of the file on disk, to read vertex data from. This should
		/// be a gray scale bitmap.</param>
		/// <param name="position"> The absolute position of this node.</param>
		/// <param name="scale"> The scale factor for the terrain.  If you're using a heightmap of size 128x128 and would like
		/// your terrain to be 12800x12800 in game units, then use a scale factor of ( core::vector ( 100.0f, 100.0f, 100.0f ).
		/// If you use a Y scaling factor of 0.0f, then your terrain will be flat.</param>
		/// <param name="vertexColor"> The default color of all the vertices. If no texture is associated
		/// with the scene node, then all vertices will be this color. Defaults to white.</param>
		/// <returns>Returns pointer to the created scene node. Can be null if the 
		/// terrain could not be created, for example because the heightmap could not be loaded.</returns>
		ITerrainSceneNode* AddTerrainSceneNode(
			System::String* heightMapFileName, 
			ISceneNode* parent, int id, 
			Core::Vector3D position,
			Core::Vector3D scale,
			Video::Color color	);

		/// <summary>
		/// Adds a particle system scene node to the scene graph.</summary>
		/// <param name="withDefaultEmitter"> Creates a default working point emitter
		/// which emitts some particles. Set this to true to see a particle system
		/// in action. If set to false, you'll have to set the emitter you want by 
		/// calling IParticleSystemSceneNode::setEmitter().</param>
		/// <param name="parent"> Parent of the scene node. Can be NULL if no parent.</param>
		/// <param name="id"> Id of the node. This id can be used to identify the scene node.</param>
		/// <param name="position"> Position of the space relative to its parent where the
		/// scene node will be placed.</param>
		/// <param name="rotation"> Initital rotation of the scene node.</param>
		/// <param name="scale"> Initial scale of the scene node.</param>
		/// <returns> Returns pointer to the created scene node.</returns>
		IParticleSystemSceneNode* AddParticleSystemSceneNode(
			 bool withDefaultEmitter, ISceneNode* parent, int id,
			 Core::Vector3D position,
			 Core::Vector3D rotation,
			 Core::Vector3D scale );

		/// <summary>
		/// Adds an empty scene node. Can be used for doing advanced transformations
		/// or structuring the scene graph.
		/// </summary>
		/// <returns> Returns pointer to the created scene node.</returns>
		ISceneNode* AddEmptySceneNode(ISceneNode* parent, int id);

		/// <summary>
		/// Adds a text scene node, which is able to display  2d text at a position in three dimensional space
		/// </summary>
		ISceneNode* AddTextSceneNode(GUI::IGUIFont* font, System::String* text,
			Video::Color color, ISceneNode* parent,	const Core::Vector3D position, int id);

		/// <summary>
		/// Adds a Hill Plane mesh to the mesh pool. The mesh is generated on the fly
		/// and looks like a plane with some hills on it. It is uses mostly for quick
		/// tests of the engine only. You can specify how many hills there should be 
		/// on the plane and how high they should be. Also you must specify a name for
		/// the mesh, because the mesh is added to the mesh pool, and can be retieved
		/// again using ISceneManager::getMesh() with the name as parameter.
		/// </summary>
		/// <param name="name"> The name of this mesh which must be specified in order
		/// to be able to retrieve the mesh later with ISceneManager::getMesh().</param>
		/// <param name="tileSize"> Size of a tile of the mesh. (10.0f, 10.0f) would be a
		/// good value to start, for example.</param>
		/// <param name="tileCount"> Specifies how much tiles there will be. If you specifiy 
		/// for example that a tile has the size (10.0f, 10.0f) and the tileCount is
		/// (10,10), than you get a field of 100 tiles wich has the dimension 100.0fx100.0f.</param>
		/// <param name="material"/> Material of the hill mesh.</param>
		/// <param name="hillHeight"/> Height of the hills. If you specify a negative value
		/// you will get holes instead of hills. If the height is 0, no hills will be
		/// created.</param>
		/// <param name="countHills"> Amount of hills on the plane. There will be countHills.X
		/// hills along the X axis and countHills.Y along the Y axis. So in total there 
		/// will be countHills.X * countHills.Y hills.</param>
		/// <param name="textureRepeatCount"> Defines how often the texture will be repeated in
		/// x and y direction.</param>
        /// <returns> Returns null if the creation failed. The reason could be that you 
		/// specified some invalid parameters or that a mesh with that name already
		/// exists. </returns>
		IAnimatedMesh* ISceneManager::AddHillPlaneMesh(System::String* name, 
			Core::Dimension2Df tileSize, Core::Dimension2D tileCount, 
			Video::Material material, float hillHeight, Core::Dimension2Df countHills, 
			Core::Dimension2Df textureRepeatCount );

		/// <summary>
		/// Adds a dummy transformation scene node to the scene graph.
		/// This scene node does not render itself, and does not respond to set/getPosition,
		/// set/getRotation and set/getScale. Its just a simple scene node that takes a 
		/// matrix as relative transformation, making it possible to insert any transformation
		/// anywhere into the scene graph.
		/// </summary>
		/// <returns> Returns pointer to the created scene node.</returns>
		//IDummyTransformationSceneNode* addDummyTransformationSceneNode(
		//	ISceneNode* parent, int id);


		/// <summary>
		/// Adds a static terrain mesh to the mesh pool. The mesh is generated on the fly
		/// from a texture file and a height map file. Both files may be huge
		/// (8000x8000 pixels would be no problem) because the generator splits the
		/// files into smaller textures if necessary. 
		/// You must specify a name for the mesh, because the mesh is added to the mesh pool,
		/// and can be retieved again using ISceneManager::getMesh() with the name as parameter.
		/// <param name="meshname"/> The name of this mesh which must be specified in order
		/// to be able to retrieve the mesh later with ISceneManager::getMesh().
		/// <param name="texture"/> Texture for the terrain. Please note that this is not a 
		/// hardware texture as usual (ITexture), but an IImage software texture.
		/// You can load this texture with IVideoDriver::createImageFromFile(). 
		/// <param name="heightmap"/> A grayscaled heightmap image. Like the texture,
		/// it can be created with IVideoDriver::createImageFromFile(). The amount
		/// of triangles created depends on the size of this texture, so use a small
		/// heightmap to increase rendering speed.
		/// <param name="stretchSize"/> Parameter defining how big a is pixel on the heightmap. 
		/// <param name="maxHeight"/> Defines how height a white pixel on the heighmap is.
		/// \return Returns null if the creation failed. The reason could be that you 
		/// specified some invalid parameters, that a mesh with that name already
		/// exists, or that a texture could not be found. If successful, a pointer to the mesh is returned. 
		/// This pointer should not be dropped. See IUnknown::drop() for more information.
		 //IAnimatedMesh* addTerrainMesh( c8* meshname,
		//	video::IImage* texture, video::IImage* heightmap,
		//	 core::dimension2d<float>& stretchSize = core::dimension2d<float>(10.0f,10.0f),
		//	float maxHeight=200.0f, 
		//	 core::dimension2d<int>& defaultVertexBlockSize = core::dimension2d<int>(64,64)) = 0;

		/// <summary>
		/// Sets or returns the current active camera.
		/// </summary>
		/// <returns>The active camera is returned. Note that this can be NULL, if there
		/// was no camera created yet.</returns>		
		__property ICameraSceneNode* get_ActiveCamera();

		/// <summary>
		/// Sets the active camera. The previous active camera will be deactivated.
		/// </summary>
		/// <param name="camera"> The new camera which should be active.</param>
		__property void set_ActiveCamera(ICameraSceneNode* camera);

		/// <summary>
		/// Returns or sets the color of stencil buffers shadows drawn by the scene manager.
		/// Usually you should set this to black with a little alpha like (150,0,0,0).
		/// </summary>
		__property void set_ShadowColor(Video::Color color);

		/// <summary>
		/// Returns or sets the color of stencil buffers shadows drawn by the scene manager.
		/// Usually you should set this to black with a little alpha like (150,0,0,0).
		/// </summary>
		__property Video::Color get_ShadowColor();

		/// <summary>
		/// Returns the root scene node. This is the scene node wich is parent 
		/// of all scene nodes. The root scene node is a special scene node which
		/// only exists to manage all scene nodes. It is not rendered and cannot
		/// be removed from the scene.
		/// </summary>
		__property ISceneNode* get_RootSceneNode();

		/// <summary>
		/// Returns the first scene node with the specified id.
		/// <param name="id"/> The id to search for
		/// <param name="start"/> Scene node to start from. All children of this scene
		/// node are searched. If null is specified, the root scene node is
		/// taken.
		/// \return Returns pointer to the first scene node with this id,
		/// and null if no scene node could be found.
		 //ISceneNode* getSceneNodeFromId(int id, ISceneNode* start=0) = 0;

		/// <summary>
		/// Registers a node for rendering it at a specific time.
		/// This method should only be used by SceneNodes when they get a 
		/// ISceneNode::OnPreRender() call.
		 //void registerNodeForRendering(ISceneNode* node, ESceneNodeRenderTime time = SNRT_DEFAULT) = 0;

		/// <summary>
		/// Draws all the scene nodes. This can only be invoked between
		/// IVideoDriver::BeginScene() and IVideoDriver::EndScene().
		/// </summary>
		 void DrawAll();

		/// <summary>
		/// Creates a rotation animator, which rotates the attached scene node around itself.
		/// </summary>
		/// <param name="rotationPerSecond"> Specifies the speed of the animation </param>
		/// <returns> Returns the animator. Attach it to a scene node with ISceneNode::AddAnimator()
		/// and the animator will animate it.</returns>
		ISceneNodeAnimator* CreateRotationAnimator(Core::Vector3D rotationPerSecond);

		/// <summary>
		/// Creates a fly circle animator, which lets the attached scene node fly 
		/// around a center.
		/// </summary>
		/// <param name="center"> Center of the circle.</param>
		/// <param name="radius"> Radius of the circle.</param>
		/// <param name="speed"> Specifies the speed of the flight. Default: 0.001f</param>
		/// <returns> Returns the animator. Attach it to a scene node with ISceneNode::addAnimator()
		/// and the animator will animate it.
		/// </returns>
		ISceneNodeAnimator* CreateFlyCircleAnimator( Core::Vector3D center, 
			float radius, float speed);

		/// <summary>
		/// Creates a fly straight animator, which lets the attached scene node
		/// fly or move along a line between two points.
		/// </summary>
		/// <param name="startPoint"> Start point of the line.</param>
		/// <param name="endPoint"> End point of the line.</param>
		/// <param name="timeForWay"> Time in milli seconds how long the node should need to
		/// move from the start point to the end point.</param>
		/// <param name="loop"> If set to false, the node stops when the end point is reached.
		/// If loop is true, the node begins again at the start.</param>
		/// <returns> Returns the animator. Attach it to a scene node with ISceneNode::addAnimator()
		/// and the animator will animate it.</returns>
		ISceneNodeAnimator* CreateFlyStraightAnimator( Core::Vector3D startPoint,
			 Core::Vector3D endPoint, int timeForWay, bool loop);

		/// <summary>
		/// Creates a texture animator, which switches the textures of the target scene
		/// node based on a list of textures.
		/// </summary>
		/// <param name="textures"> List of textures to use.</param>
		/// <param name="timePerFrame"> Time in milliseconds, how long any texture in the list
		/// should be visible.</param>
		/// <param name="loop"> If set to to false, the last texture remains set, and the animation
		/// stops. If set to true, the animation restarts with the first texture.</param>
		/// <returns> Returns the animator. Attach it to a scene node with ISceneNode::addAnimator()
		/// and the animator will animate it.</returns>
		ISceneNodeAnimator* CreateTextureAnimator( Video::ITexture* textures[],
			int timePerFrame, bool loop);

		/// <summary>
		/// Creates a scene node animator, which deletes the scene node after
		/// some time automaticly.
		/// </summary>
		/// <param name="when"> Time in milliseconds, after when the node will be deleted.</param>
		/// <returns> Returns the animator. Attach it to a scene node with ISceneNode::addAnimator()
		/// and the animator will animate it.</returns>
		ISceneNodeAnimator* CreateDeleteAnimator(int timeMs);

		/// <summary>
		/// Creates a special scene node animator for doing automatic collision detection 
		/// and response. See ISceneNodeAnimatorCollisionResponse for details.
		/// </summary>
		/// <param name="world"> Triangle selector holding all triangles of the world with which
		/// the scene node may collide. You can create a triangle selector with 
		/// ISceneManager::createTriangleSelector();</param>
		/// <param name="sceneNode"> SceneNode which should be manipulated. After you added this animator
		/// to the scene node, the scene node will not be able to move through walls and is
		/// affected by gravity.</param>
		/// <param name="ellipsoidRadius"> Radius of the ellipsoid with which collision detection and
		/// response is done.  </param>
		/// <param name="gravityPerSecond"> Sets the gravity of the environment. A good example value would be
		/// Core::Vector3D(0,-100.0f,0) for letting gravity affect all object to
		/// fall down. For bigger gravity, make increase the length of the vector.
		/// You can disable gravity by setting it to Core::Vector3D(0,0,0).</param>
		/// <param name="ellipsoidTranslation"> By default, the ellipsoid for collision detection is created around
		/// the center of the scene node, which means that the ellipsoid surrounds
		/// it completely. If this is not what you want, you may specify a translation
		/// for the ellipsoid.</param>
		/// <param name="slidingValue">Value for sliding, default: 0.0005f</param>
		/// <returns> Returns the animator. Attach it to a scene node with ISceneNode::addAnimator()
		/// and the animator will cause it to do collision detection and response.
		/// </returns>
		ISceneNodeAnimator* CreateCollisionResponseAnimator(
			ITriangleSelector* world, ISceneNode* sceneNode, 
			Core::Vector3D ellipsoidRadius,
			Core::Vector3D gravityPerSecond,
			Core::Vector3D ellipsoidTranslation,
			float slidingValue);

		/// <summary>
		/// Creates a follow spline animator. The animator modifies the position of
		/// the attached scene node to make it follow a hermite spline. 
		/// The code of the is based on a scene node
		/// Matthias Gall sent in. Thanks! I adapted the code just a little bit. Matthias 
		/// wrote:
 		/// Uses a subset of hermite splines: either cardinal splines (tightness != 0.5) or catmull-rom-splines (tightness == 0.5)
		/// but this is just my understanding of this stuff, I'm not a mathematician, so this might be wrong ;)
		/// </summary>
        ISceneNodeAnimator* CreateFollowSplineAnimator(int startTime,
			Core::Vector3D points[],
			float speed, float tightness);

		/// <summary>
		/// Creates a simple ITriangleSelector, based on a mesh. Triangle selectors
		/// can be used for doing collision detection. Don't use this selector
		/// for a huge amount of triangles like in Quake3 maps.
		/// Instead, use for example ISceneManager::createOctTreeTriangleSelector().
		/// Please note that the created triangle selector is not automaticly attached
		/// to the scene node. You will have to call ISceneNode::setTriangleSelector()
		/// for this. 
		/// </summary>
		/// <param name="mesh"> Mesh of which the triangles are taken.</param>
		/// <param name="node"> Scene node of which visibility and transformation is used.</param>
		/// <returns> Returns the selector, or null if not successful.
		/// If you no longer need the selector, you should call ITriangleSelector::drop().</returns>
		ITriangleSelector* CreateTriangleSelector(IMesh* mesh, ISceneNode* node);

		/// <summary>
		/// Creates a simple dynamic ITriangleSelector, based on a axis aligned bounding box. Triangle selectors
		/// can be used for doing collision detection. Every time when triangles are 
		/// queried, the triangle selector gets the bounding box of the scene node,
		/// an creates new triangles. In this way, it works good with animated scene nodes.
		/// </summary>
		/// <param name="node"> Scene node of which the bounding box, visibility and 
		/// transformation is used.</param>
		/// <returns> Returns the selector, or null if not successful.</param>
		ITriangleSelector* CreateTriangleSelectorFromBoundingBox(ISceneNode* node);

		/// <summary>
		/// Creates a simple ITriangleSelector, based on a mesh. Triangle selectors
		/// can be used for doing collision detection. This triangle selector is
		/// optimized for huge amounts of triangle, it organizes them in an octtree.
		/// Please note that the created triangle selector is not automaticly attached
		/// to the scene node. You will have to call ISceneNode::setTriangleSelector()
		/// for this. 
		/// </summary>
		/// <param name="mesh"> Mesh of which the triangles are taken.</param>
		/// <param name="node"> Scene node of which visibility and transformation is used.</param>
		/// <param name="minimalPolysPerNode"> Specifies the minimal polygons contained a octree node.
		/// If a node gets less polys the this value, it will not be splitted into
		/// smaller nodes. Defaukt value: 32</param>
		/// <returns> Returns the selector, or null if not successful.</returns>
		ITriangleSelector* CreateOctTreeTriangleSelector(IMesh* mesh,
			ISceneNode* node, int minimalPolysPerNode);

		/// <summary>
		/// Creates a meta triangle selector which is nothing more than a 
		/// collection of one or more triangle selectors providing together
		/// the interface of one triangle selector. In this way, 
		/// collision tests can be done with different triangle soups in one pass.
		/// </summary>
		/// <returns> Returns the selector, or null if not successful.</returns>
		IMetaTriangleSelector* CreateMetaTriangleSelector();

		/// <summary>
		/// Creates a triangle selector which can select triangles from a terrain scene node.
		/// </summary>
		/// <param name="node">Pointer to the created terrain scene node</param>
		/// <param name="LOD">Level of detail, 0 for highest detail.</param>
		ITriangleSelector* CreateTerrainTriangleSelector(
			ITerrainSceneNode* node, int LOD);

		/// <summary>
		/// Returns a pointer to the scene collision manager.
		/// </summary>
		__property ISceneCollisionManager* get_SceneCollisionManager();

		/// <summary>
		/// Adds an external mesh loader. If you want the engine to be extended with
		/// file formats it currently is not able to load (e.g. .cob), just implement
		/// the IMeshLoader interface in your loading class and add it with this method.
		/// </summary>
		/// <param name="externalLoader"/> Implementation of a new mesh loader.</param>
		 // void addExternalMeshLoader(IMeshLoader* externalLoader) = 0;

		/// <summary>
		/// Returns a pointer to the mesh manipulator.
		/// </summary>
		__property IMeshManipulator* get_MeshManipulator();

		/// <summary>
		/// Adds a scene node to the deletion queue. The scene node is immediatly
		/// deleted when it's secure. Which means when the scene node does not
		/// execute animators and things like that. This method is for example
		/// used for deleting scene nodes by their scene node animators. In
		/// most other cases, a ISceneNode::remove() call is enough, using this
		/// deletion queue is not necessary.
		/// See ISceneManager::createDeleteAnimator() for details.
		/// </summary>
		/// <param name="node"> Node to detete.</param>
		  //void addToDeletionQueue(ISceneNode* node) = 0;

		/// <summary>
		/// Posts an input event to the environment. Usually you do not have to
		/// use this method, it is used by the internal engine.
		/// </summary>
		 // bool postEventFromUser(SEvent event) = 0;

		/// <summary>
		/// Clears the whole scene. All scene nodes are removed. 
		/// </summary>
		void Clear();

		/// <summary>
		/// Loads a scene from an .irr file, the Irrlicht editor file format.
		/// </summary>
		void LoadScene(System::String* filename);

		/// <summary>
		/// Saves a scene to an .irr file, the Irrlicht editor file format.
		/// </summary>
		void SaveScene(System::String* filename);

	private:

		irr::scene::ISceneManager* Manager;
		ISceneCollisionManager* SCM;
		IMeshManipulator* Manipulator;
		ISceneNode* Root;
	};

}
}
