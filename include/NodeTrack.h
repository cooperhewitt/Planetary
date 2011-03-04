/*
 *  NodeTrack.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "Node.h"
#include "cinder/Vector.h"

class NodeTrack : public Node
{
  public:
	NodeTrack( Node *parent, int index, int numTracks, const ci::Font &font, std::string name );
	void update( const ci::Matrix44f &mat, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void drawStar();
	void drawPlanet( std::vector< ci::gl::Texture*> texs );
	void drawRings( ci::gl::Texture *tex );
	void select();
	void setData( ci::ipod::TrackRef track, ci::ipod::PlaylistRef album );
	
	ci::ipod::PlaylistRef mAlbum;
	ci::ipod::TrackRef mTrack;
	float mTrackLength;
	float mPlayCount;
	double lastTime;
	int mNumTracks;
	int mSphereRes;
	
	int mTotalVerts;
	GLfloat *mVerts;
	GLfloat *mTexCoords;
	ci::gl::Texture mAlbumArt;
	
	ci::Color mAtmosphereColor;
	
	float mAxialTilt;
	
	bool mIsPlaying;
};