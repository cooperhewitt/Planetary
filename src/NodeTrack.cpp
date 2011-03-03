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

NodeTrack::NodeTrack( Node *parent, int index, const Font &font, std::string name )
	: Node( parent, index, font, name )
{
	mIsHighlighted	= true;
	mSphereRes		= 16;
	//mRadius			*= 0.75f;
	mIsPlaying		= false;
	
	float hue		= Rand::randFloat();
	float sat		= Rand::randFloat( 0.2f, 0.7f);
	float val		= Rand::randFloat( 0.7f, 1.0f);
	mColor			= Color( CM_HSV, hue, sat, val );
	mAtmosphereColor = Color( CM_HSV, Rand::randFloat(), 0.45f, 1.0f );
	mOrbitRadiusDest = Rand::randFloat( mParentNode->mRadius * 0.5f, mParentNode->mRadius * 2.0f );
	mIdealCameraDist = mSphere.getRadius() * 0.5f;
}

void NodeTrack::setData( TrackRef track, PlaylistRef album )
{
	mAlbum			= album;
	mTrack			= track;
	mTrackLength	= (*mAlbum)[mIndex]->getLength();
	mPlayCount		= (*mAlbum)[mIndex]->getPlayCount();
	
	//normalize playcount data
	float playCountDelta	= ( mParentNode->mHighestPlayCount - mParentNode->mLowestPlayCount ) + 1.0f;
	float normPlayCount		= ( mPlayCount - mParentNode->mLowestPlayCount )/playCountDelta;

	mPlanetTexIndex = (int)( normPlayCount * G_NUM_PLANET_TYPES );
		
	mRadius			= mRadius * pow( normPlayCount + 0.5f, 2.0f );
	mOrbitPeriod	= mTrackLength;
	mAxisAngle		= 0.0f;//Rand::randFloat( 5.0f, 30.0f );
	
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

void NodeTrack::drawPlanet( std::vector< gl::Texture*> texs )
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
	
	
	if( mIsSelected ){
		gl::enableAdditiveBlending();
		texs[G_CLOUDS_TYPE]->enableAndBind();
		gl::color( mAtmosphereColor );
		gl::rotate( Vec3f( 0.0f, app::getElapsedSeconds() * -25.0f, mAxisAngle ) );
		gl::drawSphere( Vec3f::zero(), mRadius * 0.385f, mSphereRes );
		
		glDisable( GL_LIGHTING );
		mPlanetTex.enableAndBind();
		gl::drawSphere( Vec3f::zero(), mRadius * 0.395f, mSphereRes );
		glEnable( GL_LIGHTING );
		gl::disableAlphaBlending();
	}
	
	
	gl::popModelView();
}

void NodeTrack::drawRings( gl::Texture *tex )
{
	if( mPlanetTexIndex == G_NUM_PLANET_TYPES - 1 ){
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::rotate( mMatrix );
		gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * 10.0f, mAxisAngle ) );
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.5f ) );
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
	/*
	std::cout << "Index = " << mIndex << std::endl;
	std::cout << "AlbumName = " << mAlbum->getAlbumTitle() << std::endl;
	mPlayer->play( mAlbum, mIndex );
	mIsPlaying = true;
	*/
	
	Node::select();
}

