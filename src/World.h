/*
 *  World.h
 *  Kepler
 *
 *  Created by Robert Hodgin on 2/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "cinder/app/AppCocoaTouch.h"
#include "cinder/Vector.h"
#include "cinder/Font.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "CinderIPodPlayer.h"
#include "Data.h"
#include "Node.h"
#include "NodeTrack.h"

class World {
  // TODO: clean up public/private here, perhaps spin sphere stuff off into utility lib?
  public:
	World();
	void setup( Data *data );

	void initNodes( ci::ipod::Player *player, const ci::Font &font, const ci::Font &smallFont, const ci::Surface &hiResSurfaces, const ci::Surface &loResSurfaces, const ci::Surface &noAlbumArt );
    
	void filterNodes();
	void repulseNodes();
	void deselectAllNodes();
    
    void setIsPlaying( uint64_t artistId, uint64_t albumId, uint64_t trackId );
    Node* getPlayingTrackNode( ci::ipod::TrackRef playingTrack, Node* albumNode );
    Node* getPlayingAlbumNode( ci::ipod::TrackRef playingTrack, Node* artistNode );
    Node* getPlayingArtistNode( ci::ipod::TrackRef playingTrack );
    
	void checkForNameTouch( std::vector<Node*> &nodes, const ci::Vec2f &pos );
	void update( float param1, float param2 );
	void updateGraphics( const ci::CameraPersp &cam, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void buildStarsVertexArray( const ci::Vec3f &bbRight, const ci::Vec3f &bbUp, float zoomAlpha );
	void buildStarGlowsVertexArray( const ci::Vec3f &bbRight, const ci::Vec3f &bbUp, float zoomAlpha );

	void drawStarsVertexArray();
	void drawStarGlowsVertexArray();
	void drawEclipseGlows();
	void drawNames( const ci::CameraPersp &cam, float pinchAlphaOffset, float angle );
	void drawOrbitRings( float pinchAlphaOffset, float camAlpha, const ci::gl::Texture &tex );
	void drawConstellation();
	void drawTouchHighlights( float zoomAlpha );
	void buildConstellation();
	void drawPlanets( const ci::gl::Texture &tex );
	void drawClouds( const std::vector< ci::gl::Texture> &clouds );
	
	void drawRings( const ci::gl::Texture &tex, float camZPos );
	std::vector<Node*> getUnsortedNodes( int fromGen, int toGen );
    std::vector<Node*> sortNodes( std::vector<Node*> unsortedNodes );
	
	Data *mData;
	
	int mAge;
	int mEndRepulseAge;
	
	std::vector<Node*> mNodes;
	std::vector<ci::gl::Texture> mNameTextures;
	
	NodeTrack *mPlayingTrackNode;
	
	bool mIsRepulsing;
	bool mIsInitialized;

private:

    void initVertexArrays();
	void buildPlanetRingsVertexArray();
	void buildOrbitRingsVertexArray();

    ///////////////
    
	std::vector<ci::Vec3f> mConstellation;
	std::vector<float> mConstellationDistances;
	int mTotalConstellationVertices;
	int mPrevTotalConstellationVertices;
	GLfloat *mConstellationVerts;
	GLfloat *mConstellationTexCoords;
	
	GLfloat *mRingVertsLowRes;
	GLfloat *mRingTexLowRes;
	GLfloat *mRingVertsHighRes;
	GLfloat *mRingTexHighRes;
	
	int mTotalStarVertices;
    int mPrevTotalStarVertices; // so we only recreate frames
	GLfloat *mStarVerts;
	GLfloat *mStarTexCoords;
	GLfloat *mStarColors;
	
	int mTotalStarGlowVertices;
    int mPrevTotalStarGlowVertices; // so we only recreate frames
	GLfloat *mStarGlowVerts;
	GLfloat *mStarGlowTexCoords;
	GLfloat *mStarGlowColors;
	
	GLfloat *mPlanetRingVerts;
	GLfloat *mPlanetRingTexCoords;

    // SPHERE VERTEX ARRAYS
    BloomSphere mTySphere, mLoSphere, mMdSphere, mHiSphere;

};

bool nodeSortFunc(Node* a, Node* b);
