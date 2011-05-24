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
	NodeAlbum( Node *parent, int index, const ci::Font &font, const ci::Font &smallFont, const ci::Surface &surfaces );
	void setData( ci::ipod::PlaylistRef album );
	void update( const ci::Matrix44f &mat );
	void drawEclipseGlow();
	void drawPlanet( const std::vector< ci::gl::Texture> &planets );
	void drawClouds( const std::vector< ci::gl::Texture> &clouds );
	void drawRings( const ci::gl::Texture &tex, GLfloat *planetRingVerts, GLfloat *planetRingTexCoords, float camZPos );
	void drawAtmosphere( const ci::gl::Texture &tex, const ci::gl::Texture &directionalTex, float pinchAlphaPer );
	void drawOrbitRing( float pinchAlphaOffset, float camAlpha, const ci::gl::Texture &tex, GLfloat *ringVertsLowRes, GLfloat *ringTexLowRes, GLfloat *ringVertsHighRes, GLfloat *ringTexHighRes );
	void select();
	void setChildOrbitRadii();
	string getName();
	float getReleaseYear();
    uint64_t getId();

	// TODO: should this be private?
	int mNumTracks;
	ci::Surface	mAlbumArtSurface;

  private:
	float		mReleaseYear;
	float		mTotalLength;
	float		mAsciiPer;
	bool		mHasAlbumArt;
	bool		mHasRings;
	bool		mHasClouds;
	bool		mIsBlockedBySun;
	float		mBlockedBySunPer;
	ci::gl::Texture mAlbumArtTex;
	ci::ipod::PlaylistRef mAlbum;
	float		mCloudLayerRadius;
};