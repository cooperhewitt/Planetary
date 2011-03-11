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

NodeTrack::NodeTrack( Node *parent, int index, const Font &font, std::string name )
	: Node( parent, index, font, name )
{	
	mIsHighlighted	= true;
	mSphereRes		= 16;
	//mRadius			*= 0.75f;
	mIsPlaying		= false;
	mHasRings		= false;
	
	float invTrackPer = 1.0f/(float)mParentNode->mNumTracks;
	float trackNumPer = (float)mIndex * invTrackPer;
	
	float minAmt		= mParentNode->mRadius * 0.5f;
	float maxAmt		= mParentNode->mRadius * 2.0f;
	float deltaAmt		= maxAmt - minAmt;
	mOrbitRadiusDest	= minAmt + deltaAmt * trackNumPer + Rand::randFloat( mParentNode->mRadius * 2.0f * invTrackPer );
	
}

void NodeTrack::setData( TrackRef track, PlaylistRef album )
{
	mAlbum			= album;
	mTrack			= track;
	mTrackLength	= (*mAlbum)[mIndex]->getLength();
	mPlayCount		= (*mAlbum)[mIndex]->getPlayCount();
	mStarRating		= (*mAlbum)[mIndex]->getStarRating();
	
	/*
	for( int i=0; i<mStarRating * 4; i++ ){
		mOrbiters.push_back( Orbiter( this, i ) );
	}
	*/

	/*
	mAlbumArt		= mTrack->getArtwork( Vec2i( 256, 256 ) );
	if( !mAlbumArt ){
		mAlbumArt	= gl::Texture( 256, 256 );
	}
	*/
	
	//normalize playcount data
	float playCountDelta	= ( mParentNode->mHighestPlayCount - mParentNode->mLowestPlayCount ) + 1.0f;
	float normPlayCount		= ( mPlayCount - mParentNode->mLowestPlayCount )/playCountDelta;
	
	// try making own texture for ringed planet. texture stripe, maybe from album art?
	
	mPlanetTexIndex = (int)( normPlayCount * ( G_NUM_PLANET_TYPES - 1 ) );
	mCloudTexIndex  = mPlanetTexIndex;
	
	if( mParentNode->mHighestPlayCount == mPlayCount && mPlayCount > 5 )
		mHasRings = true;
	
	float hue		= Rand::randFloat( 0.15f, 0.75f );
	float sat		= Rand::randFloat( 0.15f, 0.25f );
	float val		= Rand::randFloat( 0.85f, 1.00f );
	mColor			= Color( CM_HSV, hue, sat, val );
	mGlowColor		= mColor;
	mAtmosphereColor = mParentNode->mColor;
	
	mRadius			= math<float>::max( mRadius * pow( normPlayCount + 0.5f, 2.0f ), 0.0002f ) * 0.3735f;
	mSphere			= Sphere( mPos, mRadius * 7.0f );
	mHitSphere		= Sphere( mPos, 0.01f );
	mIdealCameraDist = 0.004;//mRadius * 10.0f;
	mOrbitPeriod	= mTrackLength;
	mAxialTilt		= Rand::randFloat( 5.0f, 30.0f );
	
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
	
	mPosPrev	= mTransPos;
	mPosRel		= Vec3f( cos( orbitAngle ), sin( orbitAngle ), 0.0f ) * mOrbitRadius;
	mPos		= mParentNode->mPos + mPosRel;

	Node::update( mat );
	
	mVel		= mTransPos - mPosPrev;	

	if( mIsSelected ){
		mSphereRes		-= ( mSphereRes - 16 ) * 0.1f;
		mCamDistAlpha	-= ( mCamDistAlpha - 1.0f ) * 0.05f;
	} else {
		mSphereRes		-= ( mSphereRes - 7 ) * 0.1f;
		mCamDistAlpha	-= ( mCamDistAlpha - 0.0f ) * 0.05f;
	}
	mSphereIntRes = mSphereRes * 2 + 1;
	
	if( mStarRating > 0 && mIsSelected ){
		vector<Orbiter>::iterator it;
		for( it = mOrbiters.begin(); it != mOrbiters.end(); ++it ){
			it->update( mat, mTransPos );
		}
	}
}

void NodeTrack::drawAtmosphere()
{
	if( mCamDistAlpha > 0.05f && mPlanetTexIndex > 0 ){
		Vec3f perp = mBbRight.cross( mBbUp );
		gl::color( ColorA( mParentNode->mColor, mCamDistAlpha + mParentNode->mEclipsePer ) );
		Vec2f radius = Vec2f( mRadius, mRadius ) * 2.35f;
		gl::drawBillboard( mTransPos + perp * mRadius * 0.1f, radius, 0.0f, mBbRight, mBbUp );
	}
}

void NodeTrack::drawPlanet( const Matrix44f &accelMatrix, const vector<gl::Texture> &planets )
{	
	gl::pushModelView();
	gl::translate( mTransPos );
	gl::rotate( mMatrix );
	gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * 20.0f, mAxialTilt ) );
	
	gl::disableAlphaBlending();

	float c = 1.0f;
	if( mIsPlaying ) c = 1.0f - mParentNode->mEclipsePer * 0.5f;
	gl::color( mColor * c );
	
	planets[mPlanetTexIndex].enableAndBind();
	gl::drawSphere( Vec3f::zero(), mRadius, mSphereIntRes );
	
	gl::popModelView();
	
    if( mIsSelected )
		drawOrbiters();
}

void NodeTrack::drawClouds( const Matrix44f &accelMatrix, const vector<gl::Texture> &clouds )
{
	if( mCamDistAlpha > 0.05f ){
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::rotate( mMatrix );
		if( mPlanetTexIndex == 1 ) // jupiter
			gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * 20.0f, mAxialTilt ) );
		else 
			gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * 12.0f, mAxialTilt ) );
		gl::disableAlphaBlending();
		gl::enableAlphaBlending();
		
		clouds[mCloudTexIndex].enableAndBind();
		gl::color( ColorA( 0.0f, 0.0f, 0.0f, mCamDistAlpha * 0.66f ) );
		gl::drawSphere( Vec3f::zero(), mRadius + 0.00001f, mSphereIntRes );

		gl::enableAdditiveBlending();
		float c = 1.0f;
		if( mIsPlaying ) c = 1.0f - mParentNode->mEclipsePer * 0.5f;
		gl::color( ColorA( mColor * c, mCamDistAlpha ) );
		gl::drawSphere( Vec3f::zero(), mRadius + 0.00002f, mSphereIntRes );
		 
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

void NodeTrack::drawOrbitRing()
{
	gl::pushModelView();
	gl::translate( mParentNode->mTransPos );
	gl::rotate( mMatrix );


	int ringRes;
	if( mIsPlaying ){
		gl::color( ColorA( 0.15f, 0.2f, 0.4f, 0.5f ) );
		ringRes = 300;
	} else {
		gl::color( ColorA( 0.15f, 0.2f, 0.4f, 0.15f ) );
		ringRes = 150;
	}
	
	gl::drawStrokedCircle( Vec2f::zero(), mOrbitRadius, ringRes );
	gl::popModelView();
}

void NodeTrack::drawOrbiters()
{
	if( mStarRating > 0 ){
		float index = 0.0f;
		vector<Orbiter>::iterator it;
		for( it = mOrbiters.begin(); it != mOrbiters.end(); ++it ){
			it->draw( mMatrix, mTransPos, mBbRight, mBbUp );
			
			index ++;
		}
	}
}

void NodeTrack::setPlaying(bool playing)
{
	mIsPlaying = playing;
}
