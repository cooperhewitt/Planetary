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
	NodeArtist( Node *parent, int index, const ci::Font &font );
	
	void update( const ci::Matrix44f &mat );
	void drawStar();
	void drawStarGlow();
	void drawOrbitRing(){ Node::drawOrbitRing(); }
	void drawPlanet( const std::vector< ci::gl::Texture> &planets );
	void drawClouds( const std::vector< ci::gl::Texture> &clouds ){ Node::drawClouds( clouds ); }
	void drawRings( const ci::gl::Texture &tex ){ Node::drawRings( tex ); }
	void select();
	string getName();
	void setData( ci::ipod::PlaylistRef playlist );

	int mNumAlbums;

  private:
	ci::ipod::PlaylistRef mPlaylist;
	
};