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
	virtual void		update( const ci::CameraPersp &cam, const ci::Matrix44f &mat, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	virtual void		drawStar();
	virtual void		drawStarGlow();
	void				drawName();
	void				drawOrthoName( const ci::CameraPersp &cam );
	void				drawSphere();
	virtual void		drawOrbitalRings();
	void				checkForSphereIntersect( Node* &theNode, const ci::Ray &ray, ci::Matrix44f &mat );
	virtual void		select();
	void				deselect();
	
	int					mGen;
	int					mIndex;
	Node				*mParentNode;
	std::vector<Node*>	mChildNodes;
	
	
	// POSITION/VELOCITY
	ci::Vec3f			mPos;				// global position
	ci::Vec3f			mTransPos;			// global position * mMatrix
	ci::Vec3f			mPosPrev;			// previous global position
	ci::Vec3f			mPosRel;			// relative position
	ci::Vec3f			mVel;				// global velocity
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
	float				mDistFromCamZAxisPer; // normalized range.
	
	// NAME
	std::string			mName;				// Name of the node
	ci::Font			mFont;
	ci::gl::Texture		mNameTex;			// Texture of the name
	
	
	ci::Sphere			mSphere;			// Sphere object used only for hit-testing
	ci::Color			mColor;				// Color of the node
	ci::Color			mGlowColor;			// Color of the star glow
	
	bool				mIsSelected;		// Node has been chosen
	bool				mIsHighlighted;		// Node is able to be chosen
};