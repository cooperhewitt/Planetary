/*
 *  NodeTrack.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "cinder/app/AppBasic.h"
#include "NodeAlbum.h"
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
	mRadius				*= 4.0f;
	
	// FIXME: bad C++?
	float numTracks		= ((NodeAlbum*)mParentNode)->mNumTracks + 2.0f;
	float invTrackPer	= 1.0f/numTracks;
	float trackNumPer	= (float)mIndex * invTrackPer;
	
	float minAmt		= mParentNode->mRadius * 2.0f;
	float maxAmt		= mParentNode->mRadius * 8.0f;
	float deltaAmt		= maxAmt - minAmt;
	mOrbitRadiusDest	= minAmt + deltaAmt * trackNumPer + Rand::randFloat( maxAmt * invTrackPer * 0.75f );
	
    mIsPlaying			= false;
	mHasRings			= false;
	
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

	
	if( mParentNode->mHighestPlayCount == mPlayCount && mPlayCount > 5 )
		mHasRings = true;
	
	float hue			= Rand::randFloat( 0.15f, 0.75f );
	float sat			= Rand::randFloat( 0.15f, 0.25f );
	float val			= Rand::randFloat( 0.85f, 1.00f );
	mColor				= Color( CM_HSV, hue, sat, val );
	mGlowColor			= mColor;
	mEclipseColor		= mColor;
	
	mRadius				= math<float>::max( mRadius * pow( normPlayCount + 0.5f, 2.0f ), 0.0003f ) * 0.75;
	mSphere				= Sphere( mPos, mRadius * 7.5f );
	mIdealCameraDist	= 0.008f;
	mOrbitPeriod		= mTrackLength;
	mAxialTilt			= Rand::randFloat( 5.0f, 30.0f );
    mAxialVel			= Rand::randFloat( 10.0f, 45.0f );
	
	mOrbitLineAlpha		= 0.1f + normPlayCount * 0.2f;
	
    // TODO: no reason why every track should init this vertex array
	//initVertexArray();
}

void NodeTrack::initVertexArray()
{
    mVerts			= new float[18];
	mTexCoords		= new float[12];
	int i = 0;
	int t = 0;
	Vec3f corner;
	float w	= mRadius * 3.35f;
	
	corner			= Vec3f( -w, 0.0f, -w );
	mVerts[i++]			= corner.x;
	mVerts[i++]			= corner.y;
	mVerts[i++]			= corner.z;
	mTexCoords[t++]		= 0.0f;
	mTexCoords[t++]		= 0.0f;
	
	corner			= Vec3f( w, 0.0f, -w );
	mVerts[i++]			= corner.x;
	mVerts[i++]			= corner.y;
	mVerts[i++]			= corner.z;
	mTexCoords[t++]		= 1.0f;
	mTexCoords[t++]		= 0.0f;
    
	corner			= Vec3f( w, 0.0f, w );	
	mVerts[i++]			= corner.x;
	mVerts[i++]			= corner.y;
	mVerts[i++]			= corner.z;
	mTexCoords[t++]		= 1.0f;
	mTexCoords[t++]		= 1.0f;
	
	corner			= Vec3f( -w, 0.0f, -w );
	mVerts[i++]			= corner.x;
	mVerts[i++]			= corner.y;
	mVerts[i++]			= corner.z;
	mTexCoords[t++]		= 0.0f;
	mTexCoords[t++]		= 0.0f;
	
	corner			= Vec3f( w, 0.0f, w );	
	mVerts[i++]			= corner.x;
	mVerts[i++]			= corner.y;
	mVerts[i++]			= corner.z;
	mTexCoords[t++]		= 1.0f;
	mTexCoords[t++]		= 1.0f;
    
	corner			= Vec3f( -w, 0.0f, w );	
	mVerts[i++]			= corner.x;
	mVerts[i++]			= corner.y;
	mVerts[i++]			= corner.z;
	mTexCoords[t++]		= 0.0f;
	mTexCoords[t++]		= 1.0f;
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
    if( mIsSelected && mParentNode->mParentNode->mDistFromCamZAxisPer > 0.0f ){
        float dist = mScreenPos.distance( mParentNode->mParentNode->mScreenPos );
        eclipseDist = constrain( dist/200.0f, 0.0f, 1.0f );
    }
	mEclipseColor = mColor * eclipseDist;
	
	
	Node::update( mat );
	
	mVel		= mTransPos - mPrevPos;	
}

void NodeTrack::drawPlanet( const vector<gl::Texture> &planets )
{	
	gl::disableAlphaBlending();
	gl::pushModelView();
	gl::translate( mTransPos );
	gl::rotate( mMatrix );
	gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * mAxialVel, mAxialTilt ) );
	gl::color( mEclipseColor );
	planets[mPlanetTexIndex].enableAndBind();
	gl::drawSphere( Vec3f::zero(), mRadius, mSphereResInt );
	gl::popModelView();
}

void NodeTrack::drawClouds( const vector<gl::Texture> &clouds )
{
	if( mCamDistAlpha > 0.05f ){
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::rotate( mMatrix );
		gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * mAxialVel * 0.6f, mAxialTilt ) );
		gl::disableAlphaBlending();
		gl::enableAlphaBlending();
		
		clouds[mCloudTexIndex].enableAndBind();
		gl::color( ColorA( 0.0f, 0.0f, 0.0f, mCamDistAlpha * 0.66f ) );
		gl::drawSphere( Vec3f::zero(), mRadius + 0.000025f, mSphereResInt );

		gl::enableAdditiveBlending();
		gl::color( ColorA( mEclipseColor, mCamDistAlpha ) );
		gl::drawSphere( Vec3f::zero(), mRadius + 0.00005f, mSphereResInt );
		gl::popModelView();
	}
}


void NodeTrack::drawRings( const gl::Texture &tex )
{
	if( mHasRings ){
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::rotate( mMatrix );
		gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * 10.0f, mAxialTilt ) );
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
		tex.enableAndBind();
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mTexCoords );
		glDrawArrays( GL_TRIANGLES, 0, 6 );
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		tex.disable();
		gl::popModelView();
	}
}


void NodeTrack::drawOrbitRing( NodeTrack *playingNode, GLfloat *ringVertsLowRes, GLfloat *ringVertsHighRes )
{
	// TODO: TrackId should be compared?
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


void NodeTrack::setPlaying(bool playing)
{
	mIsPlaying = playing;
}


string NodeTrack::getName()
{
	string name = mTrack->getTitle();
	if( name.size() < 1 ) name = "Untitled";
	return name;
}
