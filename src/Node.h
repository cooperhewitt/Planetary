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
#include "BloomSphere.h"
#include "OrbitRing.h"
#include "PlanetRing.h"

using bloom::BloomSphere;

class Node {
  public:

	Node( Node *parent, int index, const ci::Font &font, const ci::Font &smallFont, const ci::Surface &hiResSurfaces, const ci::Surface &loResSurfaces, const ci::Surface &noAlbumArt );
    
	virtual ~Node()
    { 
		for( std::vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
			delete (*nodeIt);
		}
		mChildNodes.clear();
	}	
	
	// METHODS
	void			setSphereData( BloomSphere *hiSphere, BloomSphere *mdSphere, BloomSphere *loSphere, BloomSphere *tySphere );
	void			createNameTexture();
	virtual void	update( float param1, float param2 );
	virtual void	updateGraphics( const ci::CameraPersp &cam, const ci::Vec2f &center, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp, const float &w, const float &h );
	virtual void	drawEclipseGlow();
	virtual void	drawPlanet( const ci::gl::Texture &tex ) {};
	virtual void	drawExtraGlow( const ci::gl::Texture &texGlow, const ci::gl::Texture &texCore ) {};
	virtual void	drawClouds( const std::vector< ci::gl::Texture> &clouds ) {};
//	virtual void	drawAtmosphere( const ci::Vec2f &center, const ci::gl::Texture &tex, const ci::gl::Texture &directionalTex, float pinchAlphaPer ) {};
	virtual void	drawAtmosphere( const ci::Vec3f &camEye, const ci::Vec2f &center, const ci::gl::Texture &tex, const ci::gl::Texture &directionalTex, float pinchAlphaPer ) {};
	virtual void	drawRings( const ci::gl::Texture &tex, const PlanetRing &planetRing, float camZPos );
	virtual void	findShadows( float camAlpha ) {};
	virtual void	drawOrbitRing( float pinchAlphaOffset, float camAlpha, const OrbitRing &orbitRing );

	void			drawName( const ci::CameraPersp &cam, float pinchAlphaOffset, float angle );
	void			wasTapped(){ mIsTapped = true; mHighlightStrength = 1.0f; }
	void			drawTouchHighlight( float zoomAlpha );
	void			checkForNameTouch( std::vector<Node*> &nodes, const ci::Vec2f &pos );

	void			setIsDying( bool isDying );
    bool            isDying() { return mIsDying; }
    bool            isDead() { return mIsDead; }
	
	virtual bool	isMostPlayed(){ return false; }
	virtual float	getReleaseYear(){ return 0.0f; };
	virtual int		getTrackNumber(){ return -1; };
	
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
	ci::Vec2f			mScreenPos;			// screen position
	ci::Vec3f			mRelPos;			// relative position
	ci::Vec3f			mVel;               // velocity based on mPos for helping the camera
	ci::Vec3f			mBbRight, mBbUp;
	
// CHARACTERISTICS
    
// RADII
	float				mRadiusInit;		// Radius + input from slider
	float				mRadius;			// Radius of the Node
	float				mInvRadius;			// 1.0f/radius
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
	ci::Vec2f			mScreenDirToCenter;	// Direction from screenpos to center
	float				mScreenDistToCenterPer;
    
// MUSIC LIB DATA
	float				mPercentPlayed;		// Track: percent of playback (perhaps this can be pulled directly from player?)
	float				mHighestPlayCount;	// Album: used to normalize track playcount data
	float				mLowestPlayCount;	// Album: used to normalize track playcount data
	std::string			mGenre;				// Genre

    float               mZoomPer;           // 0.0 to 1.0 based on G_ZOOM vs mGen   
    
	int					mPlanetTexIndex;	// Which of the planet textures is used
	int					mCloudTexIndex;		// Which of the cloud textures is used
	float				mIdealCameraDist;	// Ideal distance from node to camera
	
	// NAME
	ci::Font			mFont, mSmallFont;
	ci::gl::Texture		mNameTex;			// Texture of the name
	ci::Surface			mHighResSurfaces;	// Images for Track moon surface
	ci::Surface			mLowResSurfaces;	// Images for Track moon surface
	ci::Surface			mNoAlbumArtSurface;
	ci::Rectf			mHitArea;			// name hit area
	ci::Rectf			mSphereHitArea;		// node hit area
	
	ci::Sphere			mSphere;
	float				mSphereScreenRadius;// mSphere radius in screenspace
   
    
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
	float				mClosenessFadeAlpha; // makes objects fade out if they are too close to the camera. prevents clipping poops
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
    
protected:
    
	bool				mIsDying;
	bool				mIsDead;
	
// SPHERE DATA (owned by World)
    BloomSphere *mHiSphere;
    BloomSphere *mMdSphere;
    BloomSphere *mLoSphere; 
    BloomSphere *mTySphere;
};