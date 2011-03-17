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
#include "CinderIPodPlayer.h"
#include "cinder/Matrix.h"
#include "Data.h"
#include "Node.h"
#include "NodeTrack.h"

class World {
 public:
	World();
	void setData( Data *data ){ mData = data; }
	void initRingVertexArray();
	void initNodes( ci::ipod::Player *player, const ci::Font &font );
	void filterNodes();
	void deselectAllNodes();
	void checkForNameTouch( std::vector<Node*> &nodes, const ci::Vec2f &pos );
	void checkForSphereIntersect( std::vector<Node*> &nodes, const ci::Ray &ray, ci::Matrix44f &mat );
	void update( const ci::Matrix44f &mat );
	void updateGraphics( const ci::CameraPersp &cam, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void buildStarsVertexArray( const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void drawStarsVertexArray( const ci::Matrix44f &mat );
	void drawStars();
	void buildStarGlowsVertexArray( const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void drawStarGlowsVertexArray( const ci::Matrix44f &mat );
	void drawStarGlows();
	void drawNames( const ci::CameraPersp &cam, float pinchAlphaOffset );
	void drawOrbitRings( NodeTrack *playingNode );
	void drawConstellation( const ci::Matrix44f &mat );
	void drawTouchHighlights();
	void buildConstellation();
	void drawPlanets( const std::vector< ci::gl::Texture> &planets );
	void drawClouds( const std::vector< ci::gl::Texture> &clouds );
	void drawRings( const ci::gl::Texture &tex );
	
	Data *mData;
	
	std::vector<Node*> mNodes;
	std::vector<ci::gl::Texture> mNameTextures;
	
	std::vector<ci::Vec3f> mConstellation;
	std::vector<ci::ColorA> mConstellationColors;
	std::vector<float> mConstellationDistances;
	int mTotalVertices;
	int mPrevTotalVertices;
	GLfloat *mVerts;
	GLfloat *mTexCoords;
	GLfloat *mColors;
	
	GLfloat *mRingVertsLowRes;
	GLfloat *mRingVertsHighRes;
	
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
};
