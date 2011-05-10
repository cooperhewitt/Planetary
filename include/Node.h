/*
 *  Node.h
 *  Kepler
 *
 *  Created by Robert Hodgin on 2/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once 

#include <vector>
#include <string>
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Font.h"
#include "cinder/gl/Texture.h"
#include "cinder/Sphere.h"
#include "cinder/Camera.h"

class Node {
  public:

	Node( Node *parent, int index, const ci::Font &font );
	virtual ~Node(){ 
		for( std::vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
			delete (*nodeIt);
		}
			
		mChildNodes.clear();
	}	
	
	// METHODS
	void			setSphereData( int totalHiVertices, float *sphereHiVerts, float *sphereHiTexCoords, float *sphereHiNormals, 
								  int totalLoVertices, float *sphereLoVerts, float *sphereLoTexCoords, float *sphereLoNormals );
	void			init();
	void			initWithParent();
	void			createNameTexture();
	virtual void	update( const ci::Matrix44f &mat );
	virtual void	updateGraphics( const ci::CameraPersp &cam, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	virtual void	drawEclipseGlow();
	virtual void	drawPlanet( const std::vector< ci::gl::Texture> &planets ) {};
	virtual void	drawClouds( const std::vector< ci::gl::Texture> &clouds ) {};
	virtual void	drawAtmosphere( const ci::gl::Texture &tex, const ci::gl::Texture &directionalTex, float pinchAlphaPer ) {};
	virtual void	drawRings( const ci::gl::Texture &tex, GLfloat *planetRingVerts, GLfloat *planetRingTexCoords, float camZPos );
	virtual void	drawOrbitRing( float pinchAlphaOffset, GLfloat *ringVertsLowRes, GLfloat *ringVertsHighRes );
	void			drawName( const ci::CameraPersp &cam, float pinchAlphaOffset, float angle );
	void			wasTapped(){ mIsTapped = true; mHighlightStrength = 1.0f; }
	void			drawTouchHighlight();
	void			checkForSphereIntersect( std::vector<Node*> &nodes, const ci::Ray &ray, ci::Matrix44f &mat );
	void			checkForNameTouch( std::vector<Node*> &nodes, const ci::Vec2f &pos );
	void			setIsDying( bool isDying );
	
	virtual bool	isMostPlayed(){ return false; }
	
    virtual void	select();
	void			deselect();
    
	virtual std::string getName()=0; // Name of the node *must* be provided by subclasses
    virtual uint64_t    getId()=0;  // ID of the node *must* be provided by subclasses

// TODO: clean up interface and enable privates!
//private:
	
	int					mGen;
	int					mIndex;
	Node				*mParentNode;
	std::vector<Node*>	mChildNodes;
	
	// POSITION/VELOCITY
	ci::Vec3f			mPos;				// global position
	ci::Vec3f			mPosDest;			// artist node final position
	ci::Vec3f			mAcc;				// acceleration used for initial repulsion
	ci::Vec3f			mTransPos;			// global position * mMatrix
	ci::Vec2f			mScreenPos;			// screen position
	float				mEclipsePer;		// ECLIPSE!
	ci::Vec3f			mRelPos;			// relative position
	ci::Vec3f			mTransVel;			// velocity based on mTransPos for helping the camera
	ci::Matrix44f		mMatrix;
	ci::Vec3f			mBbRight, mBbUp;
	
// CHARACTERISTICS
    
// RADII
	float				mRadius;			// Radius of the Node
	float				mRadiusDest;		// Destination radius
	float				mGlowRadius;		// Radius of the glow image
    
// ORBIT
	float				mOrbitAngle;		// Current angle in relation to the parentNode
	float				mOrbitStartAngle;
	float				mOrbitRadiusMin;
	float				mOrbitRadiusMax;
	float				mOrbitRadius;		// Current distance from parentNode
	float				mOrbitRadiusDest;	// Final distance from parentNode
	float				mOrbitPeriod;		// Time in seconds to orbit parentNode
	
// ROTATION
	float				mAngularVelocity;	// Change in angle per frame
	ci::Vec3f			mAxialRot;
    float               mAxialTilt;         // Planetary axis
    float               mAxialVel;          // Speed of rotation around mAxialTilt axis;
    
// DIST FROM CAMERA
	float				mDistFromCamZAxis;	// Node's distance from Cam eye
	float				mPrevDistFromCamZAxis;	// Node's previous distance from Cam eye
	float				mDistFromCamZAxisPer; // normalized range.
    float               mCamDistAlpha;      // Transparency change based on dist to camera
    
// MUSIC LIB DATA
	float				mPercentPlayed;		// Track: percent of playback (perhaps this can be pulled directly from player?)
	float				mHighestPlayCount;	// Album: used to normalize track playcount data
	float				mLowestPlayCount;	// Album: used to normalize track playcount data


    float               mZoomPer;           // 0.0 to 1.0 based on G_ZOOM vs mGen   
    
	int					mPlanetTexIndex;	// Which of the planet textures is used
	int					mCloudTexIndex;		// Which of the cloud textures is used
	float				mIdealCameraDist;	// Ideal distance from node to camera
	
	// NAME
	ci::Font			mFont;
	ci::gl::Texture		mNameTex;			// Texture of the name
	ci::Rectf			mHitArea;			// name hit area
	ci::Rectf			mSphereHitArea;		// node hit area
	
	ci::Sphere			mSphere;			// Sphere used for name label alignment
	float				mSphereScreenRadius;// mSphere radius in screenspace
    float               mSphereRes, mSphereResInt;
   
    
// COLORS
	float				mHue;
	float				mSat;
	float				mVal;
	ci::Color			mColor;				// Color of the node
	ci::Color			mGlowColor;			// Color of the star glow
	ci::Color           mEclipseColor;      // Color during eclipse
    float				mEclipseStrength;	// Strength of the eclipse (0, no occlusion. 1, full occlusion)
	float				mEclipseAngle;		// screenspace angle of eclipse effect
	float				mEclipseDirBasedAlpha;	// The alpha of the directional eclipse effect based on screenspace distance
	
	int					mAge;
	int					mDeathCount;
	int					mDeathThresh;
	float				mDeathPer;	
	int					mBirthPause;
	bool				mIsTapped;			// Highlight when tapped
	float				mHighlightStrength;	// Falloff for the highlight glow
	bool				mIsSelected;		// Node has been chosen
	bool				mIsHighlighted;		// Node is able to be chosen
    bool                mIsPlaying;         // Node represents something about the currently playing track (album/artist)
	bool				mIsDying;
	bool				mIsDead;
	
// SPHERE DATA
	int					mTotalVertsHiRes;
	int					mTotalVertsLoRes;
	float				*mSphereVertsHiRes;
	float				*mSphereTexCoordsHiRes;
	float				*mSphereNormalsHiRes;
	float				*mSphereVertsLoRes;
	float				*mSphereTexCoordsLoRes;
	float				*mSphereNormalsLoRes;
};