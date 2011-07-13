/*
 *  World.h
 *  Kepler
 *
 *  Created by Robert Hodgin on 2/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "CinderIPod.h"
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/Vector.h"
#include "cinder/Font.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"

#include "Node.h"
#include "NodeArtist.h"
#include "NodeTrack.h"

#include "Filter.h"

#include "Stars.h"
#include "StarGlows.h"
#include "OrbitRing.h"
#include "PlanetRing.h"
#include "Constellation.h"

class World {

public:
	
    World() { mSpheresInitialized = false; mIsInitialized = false; };
    ~World() {};
    
	void setup();

	void initNodes( const vector<ci::ipod::PlaylistRef> &artists, 
                    const ci::Font &font, 
                    const ci::Font &smallFont, 
                    const ci::Surface &hiResSurfaces, 
                    const ci::Surface &loResSurfaces, 
                    const ci::Surface &noAlbumArt );
    
	void setFilter(FilterRef filterRef);
    int  getNumFilteredNodes() const { return mFilteredNodes.size(); }
    
    void updateIsPlaying( uint64_t artistId, uint64_t albumId, uint64_t trackId );
    void selectHierarchy( uint64_t artistId, uint64_t albumId, uint64_t trackId );
    NodeTrack* getTrackNodeById( uint64_t artistId, uint64_t albumId, uint64_t trackId );
    
    void updateAgainstCurrentFilter();
    
	void checkForNameTouch( std::vector<Node*> &nodes, const ci::Vec2f &pos );
    
	void update( float param1, float param2 );
	void updateGraphics( const ci::CameraPersp &cam, const ci::Vec2f &center, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp, const float &zoomAlpha );

	void drawStarsVertexArray();
	void drawStarGlowsVertexArray();
	void drawNames( const ci::CameraPersp &cam, float pinchAlphaOffset, float angle );
	void drawOrbitRings( float pinchAlphaOffset, float camAlpha );
	void drawConstellation();
	void drawTouchHighlights( float zoomAlpha );
	void drawRings( const ci::gl::Texture &tex, float camZPos );
    void drawHitAreas();
    
    NodeArtist* getArtistNodeById(const uint64_t theId) { return mNodesById[theId]; }
    
	std::vector<Node*> getUnsortedNodes( int fromGen, int toGen );
    std::vector<Node*> sortNodes( std::vector<Node*> unsortedNodes );
	
	NodeTrack *mPlayingTrackNode;
	
private:

    std::vector<NodeArtist*> mNodes;
    std::map<uint64_t, NodeArtist*> mNodesById;
    std::vector<NodeArtist*> mFilteredNodes;
    FilterRef mFilterRef;

    ///////////////

    int mAge;
	int mEndRepulseAge;
    bool mIsRepulsing;
	bool mIsInitialized;
    bool mSpheresInitialized;

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
