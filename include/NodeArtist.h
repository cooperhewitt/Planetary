/*
 *  NodeArtist.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "Node.h"
#include "cinder/Vector.h"

class NodeArtist : public Node
{
  public:
	NodeArtist( Node *parent, int index, const ci::Font &font, std::string name );
	
	void update( const ci::CameraPersp &cam, const ci::Matrix44f &mat, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void drawStar();
	void drawStarGlow();
	void drawOrbitalRings();
	void select();
	
	ci::Vec3f mPosDest;
};