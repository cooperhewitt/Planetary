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
	NodeTrack( Node *parent, int index, const ci::Font &font );
    void initVertexArray();
	void update( const ci::Matrix44f &mat );
	void drawPlanet( const std::vector< ci::gl::Texture> &planets );
	void drawClouds( const std::vector< ci::gl::Texture> &clouds );
	void drawRings( const ci::gl::Texture &tex );
	void drawOrbitRing();
	void setPlaying(bool playing);
	void setData( ci::ipod::TrackRef track, ci::ipod::PlaylistRef album );
	string getName();

	// TODO: should this be from a getData() function? or private?
	ci::ipod::TrackRef      mTrack;
	ci::ipod::PlaylistRef   mAlbum;	
	
private:	
	float		mTrackLength;
	int			mPlayCount;
	int			mStarRating;
	int			mNumTracks;
	
	int			mTotalVerts;
	GLfloat		*mVerts;
	GLfloat		*mTexCoords;
	ci::gl::Texture mAlbumArt;
	
	bool		mHasRings;
	bool 		mIsPlaying;
};