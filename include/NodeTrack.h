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
#include "Orbiter.h"
#include "cinder/Vector.h"

class Orbiter;

class NodeTrack : public Node
{
  public:
	NodeTrack( Node *parent, int index, int numTracks, const ci::Font &font, std::string name );
	void update( const ci::Matrix44f &mat, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void drawPlanet( ci::Matrix44f accelMatrix, std::vector< ci::gl::Texture*> planets );
	void drawClouds( ci::Matrix44f accelMatrix, std::vector< ci::gl::Texture*> clouds );
	void drawRings( ci::gl::Texture *tex );
	void drawOrbiters();
	void drawAtmosphere();
	void select();
	void setData( ci::ipod::TrackRef track, ci::ipod::PlaylistRef album );
	
	std::vector<Orbiter> mOrbiters;
	
	float mTrackLength;
	int mPlayCount;
	int mStarRating;
	double lastTime;
	int mNumTracks;
	float mSphereRes;
	
	int mTotalVerts;
	GLfloat *mVerts;
	GLfloat *mTexCoords;
	ci::gl::Texture mAlbumArt;
	
	ci::Color mAtmosphereColor;
	
	float mAxialTilt;
	float mCamDistAlpha;
	
	// TODO: we'd need to fix this so it gets unset when the track changes
	// and so that it's set correctly when the track is initialized
	//bool mIsPlaying;
};