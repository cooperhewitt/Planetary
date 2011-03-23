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
#include "CinderIPod.h"

class NodeTrack : public Node
{
  public:
	NodeTrack( Node *parent, int index, const ci::Font &font );
    void initVertexArray();
	void updateAudioData( double currentPlayheadTime );
	void update( const ci::Matrix44f &mat );
	void drawEclipseGlow();
	void drawPlanet( const std::vector< ci::gl::Texture> &planets );
	void drawClouds( const std::vector< ci::gl::Texture> &clouds );
	void drawOrbitRing( float pinchAlphaOffset, GLfloat *ringVertsLowRes, GLfloat *ringVertsHighRes );
	void buildPlayheadProgressVertexArray();
	void drawPlayheadProgress( const ci::gl::Texture &tex );
	void setData( ci::ipod::TrackRef track, ci::ipod::PlaylistRef album );
	string getName();
    uint64_t getId();

	// TODO: should this be from a getData() function? or private?
	ci::ipod::TrackRef      mTrack;
	ci::ipod::PlaylistRef   mAlbum;	
	
private:
	float		mEclipseStrength;
	float		mTrackLength;
	int			mPlayCount;
	int			mStarRating;
	int			mNumTracks;
	ci::Vec3f	mStartPos, mTransStartPos, mStartRelPos;
	vector<ci::Vec3f> mOrbitPath;
	
	double		mStartTime;
	double		mPlaybackTime;
	double		mPercentPlayed;
	
	bool		mHasClouds;
	bool		mIsPopulated;
	ci::gl::Texture mAlbumArt;
	
	int			mTotalOrbitVertices;
    int			mPrevTotalOrbitVertices;
	GLfloat		*mOrbitVerts;
	GLfloat		*mOrbitTexCoords;
	GLfloat		*mOrbitColors;
};