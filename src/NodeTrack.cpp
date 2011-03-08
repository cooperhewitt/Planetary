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

NodeTrack::NodeTrack( Node *parent, int index, int numTracks, const Font &font, std::string name )
	: Node( parent, index, font, name )
{	
	mIsHighlighted	= true;
	mSphereRes		= 16;
	//mRadius			*= 0.75f;
	mIsPlaying		= false;
	
	float hue		= Rand::randFloat();
	float sat		= Rand::randFloat( 0.0f, 0.5f);
	float val		= Rand::randFloat( 0.85f, 1.0f);
	mColor			= Color( CM_HSV, hue, sat, val );
	mGlowColor		= mColor;
	mAtmosphereColor = mParentNode->mColor;
	mNumTracks		= numTracks;
	float invTrackPer = 1.0f/(float)mNumTracks;
	float trackNumPer = (float)mIndex * invTrackPer;
	
	mOrbitRadiusDest = ( mParentNode->mRadius * 1.5f ) * trackNumPer + ( mParentNode->mRadius * 0.5f ) + Rand::randFloat( mParentNode->mRadius * 2.0f * invTrackPer );
	
}

void NodeTrack::setData( TrackRef track, PlaylistRef album )
{
	mAlbum			= album;
	mTrack			= track;
	mTrackLength	= (*mAlbum)[mIndex]->getLength();
	mPlayCount		= (*mAlbum)[mIndex]->getPlayCount();
	//mAlbumArt		= mTrack->getArtwork( Vec2i( 256, 256 ) );
	
	//normalize playcount data
	float playCountDelta	= ( mParentNode->mHighestPlayCount - mParentNode->mLowestPlayCount ) + 1.0f;
	float normPlayCount		= ( mPlayCount - mParentNode->mLowestPlayCount )/playCountDelta;
	
	if( mPlayCount == mParentNode->mHighestPlayCount )
		mPlanetTexIndex = G_NUM_RINGED_TYPE;
	else
		mPlanetTexIndex = (int)( normPlayCount * ( G_NUM_PLANET_TYPES - 1 ) );
		
	mRadius			= mRadius * pow( normPlayCount + 0.5f, 2.0f );
	mSphere			= Sphere( mPos, mRadius * 3.0f );
	mHitSphere		= Sphere( mPos, 0.01f );
	mIdealCameraDist = mRadius * 3.0f;
	mOrbitPeriod	= mTrackLength;
	mAxialTilt		= Rand::randFloat( 5.0f, 30.0f );
	
	mVerts			= new float[18];
	mTexCoords		= new float[12];
	int i = 0;
	int t = 0;
	Vec3f corner;
	float w	= mRadius * 1.3f;
	
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

void NodeTrack::update( const Matrix44f &mat, const Vec3f &bbRight, const Vec3f &bbUp )
{
	if( mIsSelected ) mSphereRes = 32;
	else mSphereRes = 16;
	
	double playbackTime		= app::getElapsedSeconds();
	double percentPlayed	= playbackTime/mOrbitPeriod;
	double orbitAngle		= percentPlayed * TWO_PI + mStartAngle;
	
	mPosPrev	= mTransPos;
	mPosRel		= Vec3f( cos( orbitAngle ), sin( orbitAngle ), 0.0f ) * mOrbitRadius;
	mPos		= mParentNode->mPos + mPosRel;

	Node::update( mat, bbRight, bbUp );
	
	mVel		= mTransPos - mPosPrev;	
}

void NodeTrack::drawAtmosphere()
{
	if( mIsSelected && mDistFromCamZAxisPer > 0.0f ){
		gl::color( ColorA( mParentNode->mGlowColor, 1.0f ) );
		Vec2f radius = Vec2f( mRadius, mRadius ) * 0.875f;
		gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
	}
}

void NodeTrack::drawPlanet( Matrix44f accelMatrix, std::vector< gl::Texture*> texs )
{
	gl::color( mColor );
	gl::pushModelView();
	gl::translate( mTransPos );
	gl::rotate( mMatrix );
	gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * 20.0f, mAxialTilt ) );
	texs[mPlanetTexIndex]->enableAndBind();
	gl::drawSphere( Vec3f::zero(), mRadius * 0.375f, mSphereRes );
	
	
	if( mIsSelected ){
		texs[G_CLOUDS_TYPE]->enableAndBind();
		
		gl::pushModelView();
		 gl::enableAlphaBlending();
 		 gl::color( ColorA( 0.0f, 0.0f, 0.0f, 0.5f ) );
		 gl::rotate( Vec3f( 0.0f, app::getElapsedSeconds() * 3.0f, mAxialTilt ) );
		 gl::drawSphere( Vec3f::zero(), mRadius * 0.377f, mSphereRes );
		
		 gl::enableAdditiveBlending();
		 gl::color( mAtmosphereColor );
		 gl::drawSphere( Vec3f::zero(), mRadius * 0.4f, mSphereRes );
		gl::popModelView();
	}
	gl::popModelView();
	
	
	/*
	// PLANET NAME TEXTURE
	if( mIsSelected ){
		glDisable( GL_LIGHTING );
		mPlanetTex.enableAndBind();
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::color( mParentNode->mParentNode->mGlowColor );
		gl::rotate( accelMatrix );//Vec3f( 0.0f, app::getElapsedSeconds() * -5.0f, mAxialTilt ) );
		gl::drawSphere( Vec3f::zero(), mRadius * 0.41f, mSphereRes );
		gl::popModelView();
		glEnable( GL_LIGHTING );

		
	}
	*/
	
	gl::disableAlphaBlending();
}

void NodeTrack::drawRings( gl::Texture *tex )
{
	if( mPlanetTexIndex == G_NUM_PLANET_TYPES - 1 ){
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::rotate( mMatrix );
		gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * 10.0f, mAxialTilt ) );
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
		tex->enableAndBind();
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mTexCoords );
		glDrawArrays( GL_TRIANGLES, 0, 6 );
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		tex->disable();
		gl::popModelView();
	}
}

void NodeTrack::select()
{
	mPlayer->play( mAlbum, mIndex );
	mIsPlaying = true;
	
	Node::select();
}

