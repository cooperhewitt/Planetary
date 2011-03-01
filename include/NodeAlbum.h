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
	NodeAlbum( ci::ipod::Player *player, Node *parent, int index, vector<ci::Font*> fonts, std::string name );
	
	void update( const ci::Matrix44f &mat, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void drawStars( float uiScale );
	void drawGlow();
	void drawSphere( std::vector< ci::gl::Texture*> texs );
	void drawRings( std::vector< ci::gl::Texture*> texs );
	void drawChildOrbits();
	void select();
	void setData( ci::ipod::PlaylistRef album );
	void selectNextTrack( Node *nodeSelected, string trackName );
	
	ci::ipod::PlaylistRef mAlbum;
	int mSphereRes;
};