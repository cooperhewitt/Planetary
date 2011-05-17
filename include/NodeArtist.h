/*
 *  NodeArtist.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "CinderIPod.h"
#include "Node.h"
#include "cinder/Vector.h"

class NodeArtist : public Node
{
  public:
	NodeArtist( int index, const ci::Font &font );
	
	void update( const ci::Matrix44f &mat );
	void drawEclipseGlow();
	void drawPlanet( const std::vector< ci::gl::Texture> &planets );
	void drawClouds( const std::vector< ci::gl::Texture> &clouds );
	void drawRings( const ci::gl::Texture &tex, GLfloat *planetRingVerts, GLfloat *planetRingTexCoords, float camZPos );
	
	void select();
	void setChildOrbitRadii();
    std::string getName();
    uint64_t getId();
	void setData( ci::ipod::PlaylistRef playlist );
	int getNumAlbums(){ return mNumAlbums; }
	
	
  private:
	void setColors();
	ci::ipod::PlaylistRef mPlaylist;
	int mNumAlbums;
};