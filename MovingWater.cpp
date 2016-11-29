// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "MovingWater.hpp"

#include <iostream>

namespace irr
{
namespace scene
{

//! constructor
MovingWaterSceneNode::MovingWaterSceneNode(f32 waveHeight, f32 waveSpeed, f32 waveLength,
		ISceneNode* parent, ISceneManager* mgr, s32 id,
		const core::vector3df& position, const core::vector3df& rotation,
		const core::vector3df& scale)
	//: IMeshSceneNode(mesh, parent, mgr, id, position, rotation, scale),
	: IMeshSceneNode(parent, mgr, id, position, rotation, scale),
	WaveLength(waveLength), WaveSpeed(waveSpeed), WaveHeight(waveHeight)
{
	#ifdef _DEBUG
	setDebugName("MovingWaterSceneNode");
	#endif

	//FIXME: Hardcoded or defined in multiple places
	irr::f32 tileWidth = 1000; //Width in metres - Note this is used in Simulation model normalisation as 1000, so visible jumps in water are minimised
    irr::u32 segments = 20; //How many tiles per segment
    irr::f32 segmentSize = tileWidth / segments;

	mesh = mgr->addHillPlaneMesh( "myHill",
                           core::dimension2d<f32>(segmentSize,segmentSize),
                           core::dimension2d<u32>(segments,segments),
                           0,
                           0.0f,
                           core::dimension2d<f32>(0,0),
                           core::dimension2d<f32>(10,10));


}


//! destructor
MovingWaterSceneNode::~MovingWaterSceneNode()
{
	// Mesh is dropped in IMeshSceneNode destructor (??? FIXME: Probably not true!)


}


//! frame
void MovingWaterSceneNode::OnRegisterSceneNode()
{
    //std::cout << "In OnRegisterSceneNode()" << std::endl;

	if (IsVisible)
            SceneManager->registerNodeForRendering(this);

        ISceneNode::OnRegisterSceneNode();
}


void MovingWaterSceneNode::OnAnimate(u32 timeMs)
{
	//std::cout << "In OnAnimate()" << std::endl;
	if (getMesh() && IsVisible)
	{
		const u32 meshBufferCount = getMesh()->getMeshBufferCount();
		const f32 time = timeMs / WaveSpeed;

        //JAMES: new for seamless edges between multiple scene nodes
        updateAbsolutePosition();
        core::vector3df absolutePosition = getAbsolutePosition();
        core::vector3df absolutePositionXZ(absolutePosition.X, 0, absolutePosition.Z);
        //end new

		for (u32 b=0; b<meshBufferCount; ++b)
		{
			const u32 vtxCnt = getMesh()->getMeshBuffer(b)->getVertexCount();

			//for (u32 i=0; i<vtxCnt; ++i)
				//FIXME: IMPLEMENT!
				//getMesh()->getMeshBuffer(b)->getPosition(i).Y = addWave(
				//	//OriginalMesh->getMeshBuffer(b)->getPosition(i),                   //JAMES: Replaced
                //    mesh->getMeshBuffer(b)->getPosition(i)+absolutePositionXZ,    //JAMES: New for seamless edges between multiple scene nodes
				//	time);

            }// end for all mesh buffers
		getMesh()->setDirty(scene::EBT_VERTEX);

		SceneManager->getMeshManipulator()->recalculateNormals(getMesh());
	}
	IMeshSceneNode::OnAnimate(timeMs);
}


void MovingWaterSceneNode::setMesh(IMesh* mesh)
{
    //std::cout << "In setMesh()" << std::endl;
}


void MovingWaterSceneNode::render()
{

    //std::cout << "In render()" << std::endl;

    video::IVideoDriver* driver = SceneManager->getVideoDriver();

	if (!mesh || !driver)
		return;

	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

	for (u32 i=0; i<mesh->getMeshBufferCount(); ++i)
    {
        scene::IMeshBuffer* mb = mesh->getMeshBuffer(i);
        if (mb)
        {
            const video::SMaterial& material = mb->getMaterial();

            // only render transparent buffer if this is the transparent render pass
            // and solid only in solid pass
            driver->setMaterial(material);
            driver->drawMeshBuffer(mb);
        }
    }

	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

}

const core::aabbox3d<f32>& MovingWaterSceneNode::getBoundingBox() const
{
    //std::cout << "In getBoundingBox()" << std::endl;
    if (mesh) {
        return mesh->getBoundingBox();
    } else {
        return core::aabbox3d<f32>(0,0,0,1,1,1);
    }
}

IMesh* MovingWaterSceneNode::getMesh(void)
{
    //std::cout << "In getMesh()" << std::endl;
    return mesh;
}

IShadowVolumeSceneNode* MovingWaterSceneNode::addShadowVolumeSceneNode(const IMesh* shadowMesh, s32 id, bool zfailmethod, f32 infinity)
{
    //std::cout << "In addShadowVolumeSceneNode()" << std::endl;
    return 0;
}

void MovingWaterSceneNode::setReadOnlyMaterials(bool readonly)
{
    //Ignored
    //std::cout << "In setReadOnlyMaterials()" << std::endl;
}

bool MovingWaterSceneNode::isReadOnlyMaterials() const
{
    //std::cout << "In isReadOnlyMaterials()" << std::endl;
    return false; //Fixme: Check!
}

void MovingWaterSceneNode::setMaterialTexture(u32 textureLayer, video::ITexture * texture)
{
    if (textureLayer >= video::MATERIAL_MAX_TEXTURES)
        return;

    for (u32 i = 0; i<mesh->getMeshBufferCount(); i++) {
        mesh->getMeshBuffer(i)->getMaterial().setTexture(textureLayer, texture);
    }
    //for (u32 i=0; i<getMaterialCount(); ++i)
    //    getMaterial(i).setTexture(textureLayer, texture);
}


}
}