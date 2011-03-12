/*
 *  NodeAlbum.cpp
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
#include "cinder/PolyLine.h"
#include "Globals.h"

using namespace ci;
using namespace ci::ipod;
using namespace std;

NodeAlbum::NodeAlbum( Node *parent, int index, const Font &font, std::string name )
	: Node( parent, index, font, name )
{
	mIsHighlighted	= true;

	float hue		= Rand::randFloat( 0.0f, 0.5f );
	if( hue > 0.2f && hue < 0.4f ){
		hue			= Rand::randFloat( 0.0f, 0.5f );
	}
	float sat		= 1.0f - sin( hue * 2.0f * M_PI );
	mColor			= Color( CM_HSV, hue, sat * 0.2f, 1.0f );
	mGlowColor		= Color( CM_HSV, hue, sat + 0.3f, 1.0f );
	
	float invAlbumPer = 1.0f/(float)mParentNode->mNumAlbums;
	float albumNumPer = (float)mIndex * invAlbumPer;
	
	float minAmt		= mParentNode->mRadius * 0.5f;
	float maxAmt		= mParentNode->mRadius * 2.0f;
	float deltaAmt		= maxAmt - minAmt;
	mOrbitRadiusDest	= minAmt + deltaAmt * albumNumPer + Rand::randFloat( mParentNode->mRadius * 2.0f * invAlbumPer );

	mIdealCameraDist = mRadius * 2.0f;
	
	mSphere			= Sphere( mPos, mRadius * 1.6f );
	
}


void NodeAlbum::setData( PlaylistRef album )
{
	mAlbum				= album;
	mNumTracks			= mAlbum->size();
	mCurrentTrackIndex	= 0;
	mHighestPlayCount	= 0;
	mLowestPlayCount	= 10000;
//	int i=0;
//	for(Playlist::Iter it = mAlbum->begin(); it != mAlbum->end(); ++it){
	for (int i = 0; i < mNumTracks; i++) {
		float numPlays = (*mAlbum)[i]->getPlayCount();
		if( numPlays < mLowestPlayCount )
			mLowestPlayCount = numPlays;
		
		if( numPlays > mHighestPlayCount )
			mHighestPlayCount = numPlays;
			
		i++;
	}
}


void NodeAlbum::update( const Matrix44f &mat )
{
	double playbackTime		= app::getElapsedSeconds();
	double percentPlayed	= playbackTime/mOrbitPeriod;
	double orbitAngle		= percentPlayed * TWO_PI + mStartAngle;
	
	mPosPrev	= mTransPos;
	
	mPosRel		= Vec3f( cos( orbitAngle ), sin( orbitAngle ), 0.0f ) * mOrbitRadius;
	mPos		= mParentNode->mPos + mPosRel;

	Node::update( mat );
	
	mVel		= mTransPos - mPosPrev;	
}

void NodeAlbum::drawStar()
{
	gl::color( mColor );
	gl::drawBillboard( mTransPos, Vec2f( mRadius, mRadius ) * 0.66f, 0.0f, mBbRight, mBbUp );
	
	Node::drawStar();
}

void NodeAlbum::drawStarGlow()
{
	if( mIsHighlighted && mDistFromCamZAxisPer > 0.0f ){
		gl::color( ColorA( mGlowColor, min( mDistFromCamZAxisPer * 300.0f, 1.0f ) ) );
		Vec2f radius = Vec2f( mRadius, mRadius ) * 3.5f;
		if( G_ZOOM == G_TRACK_LEVEL )
			radius *= ( mEclipsePer * 0.25f + 1.0f );
		gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
	}
	
	Node::drawStarGlow();
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

void NodeAlbum::drawPlanet( const Matrix44f &accelMatrix, const vector<gl::Texture> &planets )
{	
	if( mIsSelected ){
		glDisable( GL_LIGHTING );
		glDisable( GL_TEXTURE_2D );
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::color( Color::white() );
		//float amt = mEclipsePer * 0.25f + 0.75f;
		//gl::color( ColorA( mGlowColor.r + amt, mGlowColor.g + amt, mGlowColor.b + amt, 1.0f ) );
		gl::drawSolidCircle( Vec2f::zero(), mRadius * 0.215f, 100 );
		gl::popModelView();
		glEnable( GL_LIGHTING );
	}
		
	Node::drawPlanet( accelMatrix, planets );
}

void NodeAlbum::drawClouds( const Matrix44f &accelMatrix, const vector<gl::Texture> &clouds )
{
	Node::drawClouds( accelMatrix, clouds );
}

void NodeAlbum::drawRings( const gl::Texture &tex )
{
	Node::drawRings( tex );
}

void NodeAlbum::drawAtmosphere()
{
	Node::drawAtmosphere();
}

void NodeAlbum::select()
{
	if( !mIsSelected && mChildNodes.size() == 0 ){
		//int i=0;
		//for( Playlist::Iter it = mAlbum->begin(); it != mAlbum->end(); ++it ){
		for (int i = 0; i < mNumTracks; i++) {
			//TrackRef track		= *it;
			TrackRef track		= (*mAlbum)[i];
			string name			= track->getTitle();
			std::cout << "trackname = " << name << std::endl;
			NodeTrack *newNode	= new NodeTrack( this, i, mFont, name );
			//newNode->setIPodPlayer( mPlayer );
			mChildNodes.push_back( newNode );
			newNode->setData( track, mAlbum );
			i++;
		}
		
		
	}	
	Node::select();
}

/*
void NodeAlbum::selectNextTrack( Node *nodeSelected, string trackName )
{
	for( vector<Node*>::iterator c = mChildNodes.begin(); c != mChildNodes.end(); ++c ){
		if( (*c)->mIsSelected ){
			(*c)->deselect();
		}
		
		if( (*c)->mName == trackName ){
			nodeSelected = (*c);
			nodeSelected->mIsSelected = true;
		}
	}
}
*/
