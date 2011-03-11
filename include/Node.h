/*
 *  Node.h
 *  Kepler
 *
 *  Created by Robert Hodgin on 2/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once 
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Font.h"
#include "cinder/gl/Texture.h"
#include "cinder/Sphere.h"
#include "CinderIPod.h"
#include "CinderIPodPlayer.h"
#include "cinder/Camera.h"
#include <vector>

using std::vector;

class Node {
  public:
	Node();
	~Node(){ deselect(); }
	Node( Node *parent, int index, const ci::Font &font, std::string name );
	
	// METHODS
	void				init();
	void				initWithParent();
	void				createNameTexture();
	virtual void		update( const ci::Matrix44f &mat );
	virtual void		updateGraphics( const ci::CameraPersp &cam, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	virtual void		drawStar();
	virtual void		drawStarGlow();
	void				drawName();
	void				drawOrthoName( const ci::CameraPersp &cam, float pinchAlphaOffset );
	void				drawSphere();
	virtual void		drawOrbitRing();
	virtual void		drawPlanet( const ci::Matrix44f &accelMatrix, const std::vector< ci::gl::Texture> &planets );
	virtual void		drawClouds( const ci::Matrix44f &accelMatrix, const std::vector< ci::gl::Texture> &clouds );
	virtual void		drawRings( const ci::gl::Texture &tex );
	virtual void		drawAtmosphere();
	void				checkForSphereIntersect( std::vector<Node*> &nodes, const ci::Ray &ray, ci::Matrix44f &mat );
	void				checkForNameTouch( std::vector<Node*> &nodes, const ci::Vec2f &pos );
	virtual void		select();
	void				deselect();
	
	
	ci::ipod::PlaylistRef mAlbum;
	int					mCurrentTrackIndex;
	ci::ipod::TrackRef  mTrack;
	int					mNumAlbums;
	int					mNumTracks;
	
	int					mGen;
	int					mIndex;
	Node				*mParentNode;
	std::vector<Node*>	mChildNodes;
	
	
	// POSITION/VELOCITY
	ci::Vec3f			mPos;				// global position
	ci::Vec3f			mPosDest;			// artist node final position
	ci::Vec3f			mTransPos;			// global position * mMatrix
	ci::Vec2f			mScreenPos;			// screen position
	ci::Vec3f			mPosPrev;			// previous global position
	ci::Vec3f			mPosRel;			// relative position
	ci::Vec3f			mVel;				// global velocity
	float				mCamZVel;			// speed object is moving towards or away from camera
	ci::Matrix44f		mMatrix;
	ci::Vec3f			mBbRight, mBbUp;
	
	
	// CHARACTERISTICS
	float				mMass;
	float				mRadius;			// Radius of the Node
	float				mGlowRadius;		// Radius of the glow image
	float				mOrbitAngle;		// Current angle in relation to the parentNode
	float				mStartAngle;		// Starting angle in relation to the parentNode
	float				mOrbitRadius;		// Current distance from parentNode
	float				mOrbitRadiusDest;	// Final distance from parentNode
	float				mOrbitPeriod;		// Time in seconds to orbit parentNode
	float				mAngularVelocity;	// Change in angle per frame
	float				mPercentPlayed;		// Track: percent of playback (perhaps this can be pulled directly from player?)
	float				mHighestPlayCount;	// Album: used to normalize track playcount data
	float				mLowestPlayCount;	// Album: used to normalize track playcount data
	float				mDistFromCamZAxis;	// Node's distance from Cam eye
	float				mPrevDistFromCamZAxis;	// Node's previous distance from Cam eye
	float				mDistFromCamZAxisPer; // normalized range.
	float				mSphereScreenRadius;// mSphere radius in screenspace
	float				mZoomPer;			// 0.0 to 1.0 to 0.0 based on zoom level vs mgen
	int					mPlanetTexIndex;	// Which of the planet textures is used
	int					mCloudTexIndex;		// Which of the cloud textures is used
	float				mIdealCameraDist;	// Ideal distance from node to camera
	
	// NAME
	std::string			mName;				// Name of the node
	ci::Font			mFont;

	ci::gl::Texture		mNameTex;			// Texture of the name
	ci::gl::Texture		mPlanetTex;			// TODO: this is a test.
	
	
	ci::Sphere			mSphere;			// Sphere used for name label alignment
	ci::Sphere			mHitSphere;			// Sphere used for hit-tests
	ci::Color			mColor;				// Color of the node
	ci::Color			mGlowColor;			// Color of the star glow
	
	bool				mIsSelected;		// Node has been chosen
	bool				mIsHighlighted;		// Node is able to be chosen
};