/*
 *  NodeAlbum.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "Node.h"
#include "cinder/Vector.h"

class NodeAlbum : public Node
{
  public:
	NodeAlbum( Node *parent, int index, const ci::Font &font, std::string name );
	
	void update( const ci::Matrix44f &mat, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void drawStar();
	void drawStarGlow();
	void drawOrbitalRings();
	void drawPlanet( ci::Matrix44f accelMatrix, std::vector< ci::gl::Texture*> planets );
	void drawClouds( ci::Matrix44f accelMatrix, std::vector< ci::gl::Texture*> clouds );
	void drawRings( ci::gl::Texture *tex );
	void drawAtmosphere();
	void select();
	void setData( ci::ipod::PlaylistRef album );
	//void selectNextTrack( Node *nodeSelected, string trackName );
};