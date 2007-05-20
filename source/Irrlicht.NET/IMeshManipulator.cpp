#include "IMeshManipulator.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Scene
{

	IMeshManipulator::IMeshManipulator( irr::scene::IMeshManipulator* realManipulator )
		: meshManipulator( realManipulator )
	{
		meshManipulator->grab();
	}

	IMeshManipulator::~IMeshManipulator()
	{
		meshManipulator->drop();
	}

	void IMeshManipulator::FlipSurfaces( Irrlicht::Scene::IMesh* mesh )
	{
		getMeshManipulator()->flipSurfaces( mesh->get_NativeMesh() );
	}

	void IMeshManipulator::SetVertexColorAlpha( Irrlicht::Scene::IMesh* mesh, int alpha )
	{
		getMeshManipulator()->setVertexColorAlpha( mesh->get_NativeMesh(), alpha );
	}

	void IMeshManipulator::SetVertexColors( Irrlicht::Scene::IMesh* mesh, Irrlicht::Video::Color color )
	{
		getMeshManipulator()->setVertexColors( mesh->get_NativeMesh(), irr::NativeConverter::getNativeColor( color ) );
	}

	void IMeshManipulator::RecalculateNormals( Irrlicht::Scene::IMesh* mesh )
	{
		getMeshManipulator()->recalculateNormals( mesh->get_NativeMesh() );
	}

	void IMeshManipulator::ScaleMesh( Irrlicht::Scene::IMesh* mesh, Irrlicht::Core::Vector3D scale )
	{
		getMeshManipulator()->scaleMesh( mesh->get_NativeMesh(), irr::NativeConverter::getNativeVector( scale ) );
	}

	void IMeshManipulator::MakePlanarTextureMapping( Irrlicht::Scene::IMesh* mesh, float resolution )
	{
		getMeshManipulator()->makePlanarTextureMapping( mesh->get_NativeMesh(), resolution );
	}

	Irrlicht::Scene::IMesh* IMeshManipulator::CreateMeshWithTangents( Irrlicht::Scene::IMesh* mesh )
	{
		irr::scene::IMesh* real = 
			getMeshManipulator()->createMeshWithTangents( mesh->get_NativeMesh() );

		if(!real)
			return 0;

		return new IMesh( real );
	}

	int IMeshManipulator::GetPolyCount( Irrlicht::Scene::IAnimatedMesh* mesh )
	{
		return getMeshManipulator()->getPolyCount( mesh->get_NativeAnimatedMesh() );
	}

	int IMeshManipulator::GetPolyCount( Irrlicht::Scene::IMesh* mesh )
	{
		return getMeshManipulator()->getPolyCount( mesh->get_NativeMesh() );
	}
}

}