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
	NodeAlbum( Node *parent, int index, const ci::Font &font );
	
	void update( const ci::Matrix44f &mat );
	void drawOrbitRing( GLfloat *ringVertsLowRes, GLfloat *ringVertsHighRes );
	void drawEclipseGlow();
	void drawPlanet( const std::vector< ci::gl::Texture> &planets );
	void drawClouds( const std::vector< ci::gl::Texture> &clouds );
	void drawRings( const ci::gl::Texture &tex, GLfloat *planetRingVerts, GLfloat *planetRingTexCoords );
	void select();
	void setChildOrbitRadii();
	void setData( ci::ipod::PlaylistRef album );
	string getName();
    uint64_t getId();

	// TODO: should this be private?
	int mNumTracks;

  private:
	float mEclipseStrength;
	
	bool mHasRings;
	ci::ipod::PlaylistRef mAlbum;
};