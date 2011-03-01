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
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "Globals.h"


using namespace ci;
using namespace ci::ipod;
using namespace std;

NodeTrack::NodeTrack( ipod::Player *player, Node *parent, int index, vector<ci::Font*> fonts, std::string name )
	: Node( player, parent, index, fonts, name )
{
	mIsHighlighted	= true;
	mSphereRes		= 16;
	//mRadius			*= 0.75f;
	mIsPlaying		= false;
	
	float hue		= Rand::randFloat( 0.7f );
	float sat		= 1.0f - sin( hue * 1.6f * M_PI );
	mColor			= Color( CM_HSV, hue, sat * 0.65f, 1.0f );
	mAtmosphereColor = Color( CM_HSV, Rand::randFloat(), 0.45f, 1.0f );
}

void NodeTrack::setData( TrackRef track, PlaylistRef album )
{
	mAlbum			= album;
	mTrack			= track;
	mTrackLength	= (*mAlbum)[mIndex]->getLength();
	mPlayCount		= (*mAlbum)[mIndex]->getPlayCount();
	
	//normalize playcount data
	float playCountDelta	= (mParentNode->mHighestPlayCount - mParentNode->mLowestPlayCount) + 1.0f;
	float normPlayCount		= ( mPlayCount - mParentNode->mLowestPlayCount )/playCountDelta;
	
	mPlanetTexIndex = (int)( normPlayCount * G_NUM_PLANET_TYPES );
	
	mRadius			= mRadius * pow( normPlayCount + 0.85f, 2.0f ) * 1.5f;
	mOrbitPeriod	= mTrackLength;
	mAxisAngle		= Rand::randFloat( 5.0f, 30.0f );
	
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
	
	mPosPrev	= mPos;
	mPosRel		= Vec3f( cos( orbitAngle ), sin( orbitAngle ), 0.0f ) * mOrbitRadius;
	mPos		= mParentNode->mPos + mPosRel;
	mVel		= mPos - mPosPrev;
	
	Node::update( mat, bbRight, bbUp );
}

void NodeTrack::drawSphere( vector< gl::Texture*> texs )
{
	gl::color( mColor );
	gl::pushModelView();
	gl::translate( mTransPos );
	gl::rotate( mMatrix );
	if( mPlanetTexIndex == G_NUM_PLANET_TYPES - 1 ){
		gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * 10.0f, mAxisAngle ) );
	} else {
		gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * 50.0f, mAxisAngle ) );
	}
	texs[mPlanetTexIndex]->enableAndBind();
	gl::drawSphere( Vec3f::zero(), mRadius * 0.375f, mSphereRes );
	
	if( mIsPlaying ){
		gl::enableAdditiveBlending();
		texs[G_CLOUDS_TYPE]->enableAndBind();
		gl::color( mAtmosphereColor );
		gl::rotate( Vec3f( 0.0f, app::getElapsedSeconds() * 30.0f, 0.0f ) );
		gl::drawSphere( Vec3f::zero(), mRadius * 0.4f, mSphereRes );
		gl::rotate( Vec3f( 0.0f, app::getElapsedSeconds() * 40.0f, 180.0f ) );
		gl::drawSphere( Vec3f::zero(), mRadius * 0.42f, mSphereRes );
		gl::disableAlphaBlending();
	}
	
	gl::popModelView();
}


void NodeTrack::drawRings( vector< gl::Texture*> texs )
{
	if( mPlanetTexIndex == G_NUM_PLANET_TYPES - 1 ){
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::rotate( mMatrix );
		gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * 10.0f, mAxisAngle ) );
		texs[G_RING_TYPE]->enableAndBind();
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mTexCoords );
		glDrawArrays( GL_TRIANGLES, 0, 6 );
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		gl::popModelView();
	}
}

void NodeTrack::select()
{
	std::cout << "Index = " << mIndex << std::endl;
	std::cout << "AlbumName = " << mAlbum->getAlbumTitle() << std::endl;
	mPlayer->play( mAlbum, mIndex );
	mIsPlaying = true;
}

