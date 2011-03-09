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

class World {
 public:
	World();
	void setData( Data *data ){ mData = data; }
	void initNodes( ci::ipod::Player *player, const ci::Font &font );
	void filterNodes();
	void deselectAllNodes();
//	void checkForSphereIntersect( Node* &theNode, const ci::Ray &ray, ci::Matrix44f &mat );
	void checkForSphereIntersect( std::vector<Node*> &nodes, const ci::Ray &ray, ci::Matrix44f &mat );
	void update( const ci::Matrix44f &mat, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void updateGraphics( const ci::CameraPersp &cam );
	void drawStars();
	void drawStarGlows();
	void drawNames();
	void drawOrthoNames( const ci::CameraPersp &cam );
	void drawSpheres();
	void drawOrbitalRings();
	void drawConstellation( const ci::Matrix44f &mat );
	void buildConstellation();
	void drawPlanets( ci::Matrix44f accelMatrix, std::vector< ci::gl::Texture*> planets );
	void drawClouds( ci::Matrix44f accelMatrix, std::vector< ci::gl::Texture*> clouds );
	void drawRings( ci::gl::Texture *tex );
	void drawAtmospheres();
	
	Data *mData;
	
	std::vector<Node*> mNodes;
	std::vector<ci::gl::Texture> mNameTextures;
	
	std::vector<ci::Vec3f> mConstellation;
	std::vector<ci::ColorA> mConstellationColors;
	int mTotalVertices;
	GLfloat *mVerts;
	GLfloat *mTexCoords;
	GLfloat *mColors;
};