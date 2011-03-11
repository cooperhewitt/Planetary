/*
 *  NodeTrack.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "Node.h"on
#include "Orbiter.h"
#include "cinder/Vector.h"

class Orbiter;

class NodeTrack : public Node
{
  public:
	NodeTrack( Node *parent, int index, const ci::Font &font, std::string name );
	void update( const ci::Matrix44f &mat );
	void drawPlanet( const ci::Matrix44f &accelMatrix, const std::vector< ci::gl::Texture> &planets );
	void drawClouds( const ci::Matrix44f &accelMatrix, const std::vector< ci::gl::Texture> &clouds );
	void drawRings( const ci::gl::Texture &tex );
	void drawOrbitRing();
	void drawOrbiters();
	void drawAtmosphere();
	void setPlaying(bool playing);
	void setData( ci::ipod::TrackRef track, ci::ipod::PlaylistRef album );
	
	std::vector<Orbiter> mOrbiters;
	
	float		mTrackLength;
	int			mPlayCount;
	int			mStarRating;
	int			mNumTracks;
	float		mSphereRes;
	int			mSphereIntRes;
	
	int			mTotalVerts;
	GLfloat		*mVerts;
	GLfloat		*mTexCoords;
	ci::gl::Texture mAlbumArt;
	
	ci::Color	mAtmosphereColor;
	
	float		mAxialTilt;
	float		mCamDistAlpha;
	
	bool		mHasRings;
	bool 		mIsPlaying;
};