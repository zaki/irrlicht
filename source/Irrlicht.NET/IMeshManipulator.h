#pragma once

using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "Vector3D.h"
#include "IMesh.h"
#include "IAnimatedMesh.h"

namespace Irrlicht
{
namespace Scene
{
	/// <summary>
	/// An interface for easily manipulating meshes.
	///</summary>
	public __gc class IMeshManipulator
	{
	public:

		/// <summary>
		/// Instead of this use SceneManager.GetMeshManipulator().
		/// </summary>
		IMeshManipulator( irr::scene::IMeshManipulator* realManipulator );

		~IMeshManipulator();

		/// <summary>
		/// Flips the direction of surfaces. 
		/// </summary>
		void FlipSurfaces( Irrlicht::Scene::IMesh* mesh );

		/// <summary>
		/// Sets the alpha vertex color value of the whole mesh to a new value.
		/// </summary>
		void SetVertexColorAlpha( Irrlicht::Scene::IMesh* mesh, int alpha );

		/// <summary>
		/// Sets the colors of all vertices to one color
		/// </summary>
		void SetVertexColors( Irrlicht::Scene::IMesh* mesh, Irrlicht::Video::Color color );

		/// <summary>
		/// Recalculates all normals of the mesh.
		/// </summary>
		void RecalculateNormals( Irrlicht::Scene::IMesh* mesh );

		/// <summary>
		/// Recalculates all normals of the mesh buffer.
		/// </summary>
		/// void RecalculateNormals( Irrlicht::Scene::IMeshBuffer* buffer );

		/// <summary>
		/// Scales the whole mesh.
		/// </summary>
		void ScaleMesh( Irrlicht::Scene::IMesh* mesh, Irrlicht::Core::Vector3D scale );

		/// <summary>
		/// Clones a static IMesh into a modifyable SMesh.
		/// </summary>
		/// SMesh* createMeshCopy( Irrlicht::Scene::IMesh* mesh );

		/// <summary>
		/// Creates a planar texture mapping on the mesh
		/// </summary>
		void MakePlanarTextureMapping( Irrlicht::Scene::IMesh* mesh, float resolution );

		/// <summary>
		/// Creates a copy of the mesh, which will only consist of S3DVertexTangents vertices.
        /// This is useful if you want to draw tangent space normal mapped geometry because
		/// it calculates the tangent and binormal data which is needed there.
		/// </summary>
		Irrlicht::Scene::IMesh* CreateMeshWithTangents( Irrlicht::Scene::IMesh* mesh );

		/// <summary>
		/// Recalculates the bounding box for a meshbuffer.
		/// </summary>
		/// void RecalculateBoundingBox( Irrlicht::Scene::IMeshBuffer* buffer );

		/// <summary>
		/// Rerturns the amount of Polygons in mesh
		/// </summary>
		int GetPolyCount( Irrlicht::Scene::IMesh* mesh );

		/// <summary>
		/// Rerturns the amount of Polygons in mesh
		/// </summary>
		int GetPolyCount( Irrlicht::Scene::IAnimatedMesh* mesh );

	protected:

		irr::scene::IMeshManipulator* meshManipulator;

		inline irr::scene::IMeshManipulator* getMeshManipulator()
		{
			return (irr::scene::IMeshManipulator*)meshManipulator;
		}
	};
}
}