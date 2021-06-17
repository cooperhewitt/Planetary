/*
 *  NodeAlbum.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2013 Smithsonian Institution. All rights reserved.
 *
 */

#pragma once

#include "cinder/Vector.h"
#include "Node.h"
#include "OrbitRing.h"
//#include "Shadow.h"

class NodeAlbum : public Node
{
  public:
	NodeAlbum( Node *parent, int index, const ci::Font &font, const ci::Font &smallFont, const ci::Surface &hiResSurfaces, const ci::Surface &loResSurfaces, const ci::Surface &noAlbumArt );
	
	void setData( ci::ipod::PlaylistRef album );
	void update( float param1, float param2 );
	void drawEclipseGlow();
	void drawPlanet( const ci::gl::Texture &tex );
	void drawClouds( const std::vector< ci::gl::Texture> &clouds );
	void drawRings( const ci::gl::Texture &tex, const PlanetRing &planetRing, float camZPos );
	void drawAtmosphere( const ci::Vec3f &camEye, const ci::Vec2f &center, const ci::gl::Texture &tex, const ci::gl::Texture &directionalTex, float pinchAlphaPer, float scaleSliderOffset );
	void drawOrbitRing( float pinchAlphaOffset, float camAlpha, const OrbitRing &orbitRing, float fadeInAlphaToArtist, float fadeInArtistToAlbum );
	void findShadows( float camAlpha );
    void buildShadowVertexArray( ci::Vec3f p1, ci::Vec3f p2, ci::Vec3f p3, ci::Vec3f p4 );
	void select();
	void setChildOrbitRadii();
	string getName();
	float getReleaseYear();
    uint64_t getId();

	ci::ipod::PlaylistRef getPlaylist() { return mAlbum; }
    
	// TODO: should this be private?
	int mNumTracks;
	ci::Surface	mAlbumArtSurface;
	
  private:
	struct VertexData {
        ci::Vec2f vertex;
        ci::Vec2f texture;
    }; 
	GLuint		mAlbumArtVbo;
	
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
	uint64_t    mId;
//    Shadow      mShadow;	
	GLfloat		*mShadowVerts;
	GLfloat		*mShadowTexCoords;
	
	
};