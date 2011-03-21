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
	NodeArtist( int index, const ci::Font &font );
	
	void update( const ci::Matrix44f &mat );
	void drawStar();
	void drawStarGlow();
	void drawPlanet( const std::vector< ci::gl::Texture> &planets );
	void drawClouds( const std::vector< ci::gl::Texture> &clouds );
	void select();
	void setChildOrbitRadii();
	string getName();
    uint64_t getId();
	void setData( ci::ipod::PlaylistRef playlist );

    // TODO: should this be private?
	int mNumAlbums;
	
  private:
	ci::ipod::PlaylistRef mPlaylist;
	float mHue;
	float mSat;
	float mVal;
	ci::Color mDepthDiskColor;
};