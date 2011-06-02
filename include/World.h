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
#include "cinder/Matrix.h"
#include "Data.h"
#include "Node.h"
#include "NodeTrack.h"

class World {
  // TODO: clean up public/private here, perhaps spin sphere stuff off into utility lib?
  public:
	World();
	void setup( Data *data );
    void initVertexArrays();
    void buildSphereVertexArray( int segments, int *numVerts, float* &sphereVerts, float* &sphereTexCoords, float* &sphereNormals );    
	void buildPlanetRingsVertexArray();
	void buildOrbitRingsVertexArray();
	void buildStarsVertexArray( const ci::Vec3f &bbRight, const ci::Vec3f &bbUp, float zoomAlpha );
	void buildStarGlowsVertexArray( const ci::Vec3f &bbRight, const ci::Vec3f &bbUp, float zoomAlpha );
	
	void initNodes( ci::ipod::Player *player, const ci::Font &font, const ci::Font &smallFont, const ci::Surface &hiResSurfaces, const ci::Surface &loResSurfaces, const ci::Surface &noAlbumArt );
	void initNodeSphereData( int totalHiVertices, float *sphereHiVerts, float *sphereHiTexCoords, float *sphereHiNormals,
							 int totalMdVertices, float *sphereMdVerts, float *sphereMdTexCoords, float *sphereMdNormals,
							 int totalLoVertices, float *sphereLoVerts, float *sphereLoTexCoords, float *sphereLoNormals,
							 int totalTyVertices, float *sphereTyVerts, float *sphereTyTexCoords, float *sphereTyNormals );
	void filterNodes();
	void repulseNodes();
	void deselectAllNodes();
    void setIsPlaying( uint64_t artistId, uint64_t albumId, uint64_t trackId );
	void checkForNameTouch( std::vector<Node*> &nodes, const ci::Vec2f &pos );
	void update( const ci::Matrix44f &mat, float param1, float param2 );
	void updateGraphics( const ci::CameraPersp &cam, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );

	void drawStarsVertexArray( const ci::Matrix44f &mat );
	void drawStarGlowsVertexArray( const ci::Matrix44f &mat );
	void drawEclipseGlows();
	void drawNames( const ci::CameraPersp &cam, float pinchAlphaOffset, float angle );
	void drawOrbitRings( float pinchAlphaOffset, float camAlpha, const ci::gl::Texture &tex );
	void drawConstellation( const ci::Matrix44f &mat );
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

    // VERTEX ARRAYS
	int mNumSphereTyResVerts;
	float *mSphereTyResVerts; 
	float *mSphereTyResNormals;
	float *mSphereTyResTexCoords;
	int mNumSphereLoResVerts;
	float *mSphereLoResVerts; 
	float *mSphereLoResNormals;
	float *mSphereLoResTexCoords;
	int mNumSphereMdResVerts;
	float *mSphereMdResVerts; 
	float *mSphereMdResNormals;
	float *mSphereMdResTexCoords;
	int mNumSphereHiResVerts;
	float *mSphereHiResVerts; 
	float *mSphereHiResNormals;
	float *mSphereHiResTexCoords;
	


};

bool nodeSortFunc(Node* a, Node* b);
