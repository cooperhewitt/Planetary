/*
 *  NodeTrack.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "cinder/app/AppBasic.h"
#include "NodeTrack.h"
#include "cinder/Text.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "Globals.h"

using namespace ci;
using namespace ci::ipod;
using namespace std;

NodeTrack::NodeTrack( Node *parent, int index, const Font &font )
	: Node( parent, index, font )
{	
	mIsHighlighted		= true;
	mRadius				*= 10.0f;
    mIsPlaying			= false;
	mHasClouds			= false;
	mIsPopulated		= false;
}

void NodeTrack::setData( TrackRef track, PlaylistRef album )
{
	mAlbum			= album;
	mTrack			= track;
	mTrackLength	= (*mAlbum)[mIndex]->getLength();
	mPlayCount		= (*mAlbum)[mIndex]->getPlayCount();
	mStarRating		= (*mAlbum)[mIndex]->getStarRating();
	
	/*
	mAlbumArt		= mTrack->getArtwork( Vec2i( 256, 256 ) );
	if( !mAlbumArt ){
		mAlbumArt	= gl::Texture( 256, 256 );
	}
	*/
	
	
	
	
	
	
	//normalize playcount data
	float playCountDelta	= ( mParentNode->mHighestPlayCount - mParentNode->mLowestPlayCount ) + 1.0f;
	float normPlayCount		= ( mPlayCount - mParentNode->mLowestPlayCount )/playCountDelta;
    
	// TODO: try making own texture for ringed planet. texture stripe, maybe from album art?
	mPlanetTexIndex = mIndex%( G_NUM_PLANET_TYPES * G_NUM_PLANET_TYPE_OPTIONS );//(int)( normPlayCount * ( G_NUM_PLANET_TYPES - 1 ) );
	mCloudTexIndex  = Rand::randInt( G_NUM_CLOUD_TYPES );
   // mPlanetTexIndex *= G_NUM_PLANET_TYPE_OPTIONS + Rand::randInt( G_NUM_PLANET_TYPE_OPTIONS );
	
	if( mPlayCount > 50 ){
		mCloudTexIndex = 2;
	} else if( mPlayCount > 10 ){
		mCloudTexIndex = 1;
	} else {
		mCloudTexIndex = 0;
	}
	
	if( mPlayCount > 5 ){
		mHasClouds = true;
	}
	
	
	float hue			= Rand::randFloat( 0.15f, 0.75f );
	float sat			= Rand::randFloat( 0.15f, 0.5f );
	float val			= Rand::randFloat( 0.85f, 1.00f );
	mColor				= Color( CM_HSV, hue, sat, val );
	mGlowColor			= mColor;
	mEclipseColor		= mColor;
	
	mRadius				= math<float>::max( mRadius * pow( normPlayCount + 0.5f, 2.0f ), 0.0003f ) * 0.75;
	mSphere				= Sphere( mPos, mRadius * 7.5f );
	mMass				= ( pow( mRadius, 3.0f ) * ( M_PI * 4.0f ) ) * 0.3333334f;
	
	mIdealCameraDist	= 0.01f;
	mOrbitPeriod		= mTrackLength;
	mAxialTilt			= Rand::randFloat( 5.0f, 30.0f );
    mAxialVel			= Rand::randFloat( 10.0f, 45.0f );
	
	mOrbitLineAlpha		= 0.1f + normPlayCount * 0.2f;
}

void NodeTrack::update( const Matrix44f &mat )
{	
	double playbackTime		= app::getElapsedSeconds();
	double percentPlayed	= playbackTime/mOrbitPeriod;
	double orbitAngle		= percentPlayed * TWO_PI + mStartAngle;
	
	mPrevPos	= mTransPos;
	mRelPos		= Vec3f( cos( orbitAngle ), sin( orbitAngle ), 0.0f ) * mOrbitRadius;
	mPos		= mParentNode->mPos + mRelPos;
	
    float eclipseDist = 1.0f;
    if( mParentNode->mParentNode->mDistFromCamZAxisPer > 0.0f ){
        float dist = mScreenPos.distance( mParentNode->mParentNode->mScreenPos );
        eclipseDist = constrain( dist/200.0f, 0.0f, 1.0f );
		if( G_ZOOM == G_TRACK_LEVEL ){
			mEclipseStrength	= math<float>::max( 250.0f - abs( mSphereScreenRadius - mParentNode->mParentNode->mSphereScreenRadius ), 0.0f ) / 250.0f; 
			mEclipseStrength	= pow( mEclipseStrength, 5.0f );
		}
	}
	mEclipseColor = ( mColor + Color::white() ) * 0.5f * eclipseDist;
	
	
	Node::update( mat );
	
	mVel		= mTransPos - mPrevPos;	
}


void NodeTrack::drawEclipseGlow()
{
	if( mIsSelected && mDistFromCamZAxisPer > 0.0f ){
        gl::color( ColorA( mParentNode->mParentNode->mGlowColor, mEclipseStrength * 3.0f ) );
		Vec2f radius = Vec2f( mRadius, mRadius ) * 3.25f;
		gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
	}
}


void NodeTrack::drawPlanet( const vector<gl::Texture> &planets )
{	
	glEnable( GL_RESCALE_NORMAL );
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	int numVerts;
	if( mIsSelected ){
		glVertexPointer( 3, GL_FLOAT, 0, mSphereVertsHiRes );
		glTexCoordPointer( 2, GL_FLOAT, 0, mSphereTexCoordsHiRes );
		glNormalPointer( GL_FLOAT, 0, mSphereNormalsHiRes );
		numVerts = mTotalVertsHiRes;
	} else {
		glVertexPointer( 3, GL_FLOAT, 0, mSphereVertsLoRes );
		glTexCoordPointer( 2, GL_FLOAT, 0, mSphereTexCoordsLoRes );
		glNormalPointer( GL_FLOAT, 0, mSphereNormalsLoRes );
		numVerts = mTotalVertsLoRes;
	}
	
	gl::disableAlphaBlending();
	
    gl::pushModelView();
	gl::translate( mTransPos );
	gl::scale( Vec3f( mRadius, mRadius, mRadius ) );
	gl::rotate( mMatrix );
	gl::rotate( Vec3f( 0.0f, 0.0f, mAxialTilt ) );
	gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * mAxialVel, 0.0f ) );
	gl::color( mEclipseColor );
	planets[mPlanetTexIndex].enableAndBind();
	glDrawArrays( GL_TRIANGLES, 0, numVerts );
	gl::popModelView();
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	
}

void NodeTrack::drawClouds( const vector<gl::Texture> &clouds )
{
	if( mCamDistAlpha > 0.05f && mHasClouds ){
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glEnableClientState( GL_NORMAL_ARRAY );
		int numVerts;
		if( mIsSelected ){
			glVertexPointer( 3, GL_FLOAT, 0, mSphereVertsHiRes );
			glTexCoordPointer( 2, GL_FLOAT, 0, mSphereTexCoordsHiRes );
			glNormalPointer( GL_FLOAT, 0, mSphereNormalsHiRes );
			numVerts = mTotalVertsHiRes;
		} else {
			glVertexPointer( 3, GL_FLOAT, 0, mSphereVertsLoRes );
			glTexCoordPointer( 2, GL_FLOAT, 0, mSphereTexCoordsLoRes );
			glNormalPointer( GL_FLOAT, 0, mSphereNormalsLoRes );
			numVerts = mTotalVertsLoRes;
		}
		
		gl::disableAlphaBlending();
		
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::pushModelView();
		float radius = mRadius + 0.000025f;
		gl::scale( Vec3f( radius, radius, radius ) );
		glEnable( GL_RESCALE_NORMAL );
		gl::rotate( mMatrix );
		gl::rotate( Vec3f( 0.0f, 0.0f, mAxialTilt ) );
		gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * mAxialVel * 0.6f, 0.0f ) );

		// SHADOW CLOUDS
		glDisable( GL_LIGHTING );
		gl::disableAlphaBlending();
		gl::enableAlphaBlending();
		gl::color( ColorA( 0.0f, 0.0f, 0.0f, mCamDistAlpha * 0.5f ) );
		clouds[mCloudTexIndex].enableAndBind();
		glDrawArrays( GL_TRIANGLES, 0, numVerts );
		gl::popModelView();
		glEnable( GL_LIGHTING );
		// LIT CLOUDS
		gl::pushModelView();
		radius = mRadius + 0.00005f;
		gl::scale( Vec3f( radius, radius, radius ) );
		glEnable( GL_RESCALE_NORMAL );
		gl::rotate( mMatrix );
		gl::rotate( Vec3f( 0.0f, 0.0f, mAxialTilt ) );
		gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * mAxialVel * 0.6f, 0.0f ) );
		gl::enableAdditiveBlending();
		gl::color( ColorA( mEclipseColor, mCamDistAlpha ) );
		glDrawArrays( GL_TRIANGLES, 0, numVerts );
		gl::popModelView();
		gl::popModelView();
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glDisableClientState( GL_NORMAL_ARRAY );

	}
}


void NodeTrack::drawOrbitRing( GLfloat *ringVertsLowRes, GLfloat *ringVertsHighRes )
{
	if( mIsPlaying ){
		gl::color( ColorA( 0.2f, 0.3f, 0.7f, 0.45f ) );
	} else {
		gl::color( ColorA( 0.15f, 0.2f, 0.4f, mOrbitLineAlpha ) );
	}
	gl::pushModelView();
	gl::translate( mParentNode->mTransPos );
	gl::scale( Vec3f( mOrbitRadius, mOrbitRadius, mOrbitRadius ) );
	gl::rotate( mMatrix );
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, ringVertsLowRes );
	glDrawArrays( GL_LINE_STRIP, 0, G_RING_LOW_RES );
	glDisableClientState( GL_VERTEX_ARRAY );
	gl::popModelView();
}

string NodeTrack::getName()
{
	string name = mTrack->getTitle();
	if( name.size() < 1 ) name = "Untitled";
	return name;
}

uint64_t NodeTrack::getId()
{
    return mTrack->getItemId();
}
