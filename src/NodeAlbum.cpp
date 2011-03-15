/*
 *  NodeAlbum.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "cinder/app/AppBasic.h"
#include "NodeArtist.h"
#include "NodeAlbum.h"
#include "NodeTrack.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/PolyLine.h"
#include "Globals.h"

using namespace ci;
using namespace ci::ipod;
using namespace std;

NodeAlbum::NodeAlbum( Node *parent, int index, const Font &font )
	: Node( parent, index, font )
{
	mIsHighlighted	= true;

	float hue		= Rand::randFloat( 0.0f, 0.5f );
	if( hue > 0.2f && hue < 0.4f ){
		hue			= Rand::randFloat( 0.0f, 0.5f );
	}
	float sat		= 1.0f - sin( hue * 2.0f * M_PI );
	mColor			= Color( CM_HSV, hue, sat * 0.2f, 1.0f );
	mGlowColor		= Color( CM_HSV, hue, sat + 0.3f, 1.0f );
	
	// FIXME: bad c++?
	float numAlbums = ((NodeArtist*)mParentNode)->mNumAlbums;
	
	float invAlbumPer = 1.0f/(float)numAlbums;
	float albumNumPer = (float)mIndex * invAlbumPer;
	
	float minAmt		= mParentNode->mRadius * 1.0f;
	float maxAmt		= mParentNode->mRadius * 3.0f;
	float deltaAmt		= maxAmt - minAmt;
	mOrbitRadiusDest	= minAmt + deltaAmt * albumNumPer + Rand::randFloat( maxAmt * invAlbumPer );

	mIdealCameraDist = mRadius * 2.0f;
    mPlanetTexIndex = 0;
	
    mRadius         *= 0.25f;
	mSphere			= Sphere( mPos, mRadius * 7.5f );
}


void NodeAlbum::setData( PlaylistRef album )
{
	mAlbum				= album;
	mNumTracks			= mAlbum->size();
	mHighestPlayCount	= 0;
	mLowestPlayCount	= 10000;
	mEclipseColor       = mColor;

    mAxialVel       = Rand::randFloat( 10.0f, 45.0f );
    
    mPlanetTexIndex = mIndex%( G_NUM_PLANET_TYPES * G_NUM_PLANET_TYPE_OPTIONS );//(int)( normPlayCount * ( G_NUM_PLANET_TYPES - 1 ) );
	mCloudTexIndex  = Rand::randInt( G_NUM_CLOUD_TYPES );

	for (int i = 0; i < mNumTracks; i++) {
		float numPlays = (*mAlbum)[i]->getPlayCount();
		if( numPlays < mLowestPlayCount )
			mLowestPlayCount = numPlays;
		if( numPlays > mHighestPlayCount )
			mHighestPlayCount = numPlays;
	}
}


void NodeAlbum::update( const Matrix44f &mat )
{
	double playbackTime		= app::getElapsedSeconds();
	double percentPlayed	= playbackTime/mOrbitPeriod;
	double orbitAngle		= percentPlayed * TWO_PI + mStartAngle;
    
	mPrevPos	= mTransPos;
	
	mRelPos		= Vec3f( cos( orbitAngle ), sin( orbitAngle ), 0.0f ) * mOrbitRadius;
	mPos		= mParentNode->mPos + mRelPos;
    
    float eclipseDist = 1.0f;
    if( mIsSelected && mParentNode->mDistFromCamZAxisPer > 0.0f ){
        float dist = mScreenPos.distance( mParentNode->mScreenPos );
        eclipseDist = constrain( dist/200.0f, 0.0f, 1.0f );
    }
	mEclipseColor = mColor * eclipseDist;
    
    
	Node::update( mat );
	
	mVel		= mTransPos - mPrevPos;	
}


void NodeAlbum::drawOrbitRing()
{
	gl::pushModelView();
	gl::translate( mParentNode->mTransPos );
	gl::rotate( mMatrix );
	
	int ringRes;
	if( mIsSelected ){
		gl::color( ColorA( 0.15f, 0.2f, 0.4f, 0.5f ) );
		ringRes = 300;
	} else {
		gl::color( ColorA( 0.15f, 0.2f, 0.4f, 0.15f ) );
		ringRes = 150;
	}
	
	gl::drawStrokedCircle( Vec2f::zero(), mOrbitRadius, ringRes );

	gl::popModelView();
	
	Node::drawOrbitRing();
}

void NodeAlbum::drawPlanet( const vector<gl::Texture> &planets )
{	
    gl::pushModelView();
	gl::translate( mTransPos );
	gl::rotate( mMatrix );
	gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * mAxialVel, 0.0f ) );
	
	gl::disableAlphaBlending();
	
	gl::color( mEclipseColor );
	planets[mPlanetTexIndex].enableAndBind();
	gl::drawSphere( Vec3f::zero(), mRadius, mSphereResInt );
	
	gl::popModelView();
    
	Node::drawPlanet( planets );
}

void NodeAlbum::drawClouds( const vector<gl::Texture> &clouds )
{
    if( mCamDistAlpha > 0.05f ){
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::rotate( mMatrix );
        gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * mAxialVel * 0.75f, 0.0f ) );
        
        clouds[mCloudTexIndex].enableAndBind();
        
		gl::disableAlphaBlending();
		gl::enableAlphaBlending();
		gl::color( ColorA( 0.0f, 0.0f, 0.0f, mCamDistAlpha * 0.66f ) );
		gl::drawSphere( Vec3f::zero(), mRadius + 0.0001f, mSphereResInt );
        
		gl::enableAdditiveBlending();
		gl::color( ColorA( mEclipseColor, mCamDistAlpha ) );
		gl::drawSphere( Vec3f::zero(), mRadius + 0.00025f, mSphereResInt );
        
		gl::popModelView();
	}
    
	Node::drawClouds( clouds );
}

void NodeAlbum::select()
{
	if( !mIsSelected && mChildNodes.size() == 0 ){
		for (int i = 0; i < mNumTracks; i++) {
			TrackRef track		= (*mAlbum)[i];
			string name			= track->getTitle();
			std::cout << "trackname = " << name << std::endl;
			NodeTrack *newNode	= new NodeTrack( this, i, mFont );
			mChildNodes.push_back( newNode );
			newNode->setData( track, mAlbum );
		}
	}	
	Node::select();
}

string NodeAlbum::getName()
{
	return mAlbum->getAlbumTitle();
}
