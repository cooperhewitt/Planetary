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
	
	void update( const ci::Matrix44f &mat, const ci::Surface &surfaces );
	void drawEclipseGlow();
	void drawPlanet( const std::vector< ci::gl::Texture> &planets );
	void drawClouds( const std::vector< ci::gl::Texture> &clouds );
	void drawRings( const ci::gl::Texture &tex, GLfloat *planetRingVerts, GLfloat *planetRingTexCoords, float camZPos );
	void drawAtmosphere( const ci::gl::Texture &tex, const ci::gl::Texture &directionalTex, float pinchAlphaPer );
	void drawOrbitRing( float pinchAlphaOffset, float camAlpha, const ci::gl::Texture &tex, GLfloat *ringVertsLowRes, GLfloat *ringTexLowRes, GLfloat *ringVertsHighRes, GLfloat *ringTexHighRes );
	void select();
	void setChildOrbitRadii();
	void setData( ci::ipod::PlaylistRef album );
	string getName();
    uint64_t getId();

	// TODO: should this be private?
	int mNumTracks;

  private:
	float		mReleaseYear;
	float		mTotalLength;
	float		mAsciiPer;
	bool		mHasAlbumArt;
	bool		mHasRings;
	bool		mHasClouds;
	bool		mIsPopulated;
	ci::gl::Texture mAlbumArt;
	ci::ipod::PlaylistRef mAlbum;
	
	float		mCloudLayerRadius;
};