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
#include "Stars.h"
#include "StarGlows.h"
#include "OrbitRing.h"
#include "PlanetRing.h"
#include "Constellation.h"

class World {
  // TODO: clean up public/private here, perhaps spin sphere stuff off into utility lib?
  public:
	
    World() {};
    ~World() {};
    
	void setup( Data *data );

	void initNodes( const ci::Font &font, 
                    const ci::Font &smallFont, 
                    const ci::Surface &hiResSurfaces, 
                    const ci::Surface &loResSurfaces, 
                    const ci::Surface &noAlbumArt );
    
	void filterNodes();
	void deselectAllNodes();
    
    void setIsPlaying( uint64_t artistId, uint64_t albumId, uint64_t trackId );
    Node* getPlayingTrackNode( ci::ipod::TrackRef playingTrack, Node* albumNode );
    Node* getPlayingAlbumNode( ci::ipod::TrackRef playingTrack, Node* artistNode );
    Node* getPlayingArtistNode( ci::ipod::TrackRef playingTrack );
    
	void checkForNameTouch( std::vector<Node*> &nodes, const ci::Vec2f &pos );
    
	void update( float param1, float param2 );
	void updateGraphics( const ci::CameraPersp &cam, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp, const float &zoomAlpha );

	void drawStarsVertexArray();
	void drawStarGlowsVertexArray();
	void drawEclipseGlows();
	void drawNames( const ci::CameraPersp &cam, float pinchAlphaOffset, float angle );
	void drawOrbitRings( float pinchAlphaOffset, float camAlpha, const ci::gl::Texture &tex );
	void drawConstellation();
	void drawTouchHighlights( float zoomAlpha );
	void drawPlanets( const ci::gl::Texture &tex );
	void drawClouds( const std::vector< ci::gl::Texture> &clouds );
	void drawRings( const ci::gl::Texture &tex, float camZPos );
    void drawHitAreas();
    
	std::vector<Node*> getUnsortedNodes( int fromGen, int toGen );
    std::vector<Node*> sortNodes( std::vector<Node*> unsortedNodes );
	
	NodeTrack *mPlayingTrackNode;
	
private:

    std::vector<Node*> mNodes;
	Data *mData;	

    ///////////////

    int mAge;
	int mEndRepulseAge;
    bool mIsRepulsing;
	bool mIsInitialized;

    ///////////////

	void repulseNodes();    

    ///////////////
    
    // VERTEX ARRAYS
    Stars mStars;
    StarGlows mStarGlows;
    OrbitRing mOrbitRing;
    PlanetRing mPlanetRing;
    Constellation mConstellation;
    
    // SPHERE LOD VERTEX ARRAYS
    BloomSphere mTySphere, mLoSphere, mMdSphere, mHiSphere;

};

bool nodeSortFunc(Node* a, Node* b);
