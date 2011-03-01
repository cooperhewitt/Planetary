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

NodeAlbum::NodeAlbum( ipod::Player *player, Node *parent, int index, vector<ci::Font*> fonts, std::string name )
	: Node( player, parent, index, fonts, name )
{
	mIsHighlighted	= true;
	float hue		= Rand::randFloat( 0.0f, 0.5f );
	float sat		= 1.0f - sin( hue * 2.0f * M_PI );
	mColor			= Color( CM_HSV, hue, sat * 0.5f, 1.0f );
	mGlowColor		= Color( CM_HSV, hue + 0.15f, sat, 1.0f );
	
	mSphereRes		= 16;
	//mRadius			*= 0.5f;
}

void NodeAlbum::setData( PlaylistRef album )
{
	mAlbum				= album;
	mHighestPlayCount	= 0;
	mLowestPlayCount	= 10000;
	int i=0;
	for(Playlist::Iter it = mAlbum->begin(); it != mAlbum->end(); ++it){
		float numPlays = (*mAlbum)[i]->getPlayCount();
		if( numPlays < mLowestPlayCount )
			mLowestPlayCount = numPlays;
		
		if( numPlays > mHighestPlayCount )
			mHighestPlayCount = numPlays;
			
		i++;
	}
}

void NodeAlbum::update( const Matrix44f &mat, const Vec3f &bbRight, const Vec3f &bbUp )
{
	if( mIsSelected ) mSphereRes = 32;
	else mSphereRes = 16;
	
	double playbackTime		= app::getElapsedSeconds();
	double percentPlayed	= playbackTime/mOrbitPeriod;
	double orbitAngle		= percentPlayed * TWO_PI + mStartAngle;
	
	mPosPrev	= mPos;
	
	Vec3f oldPosRel = mPosRel;
	mPosRel		= Vec3f( cos( orbitAngle ), sin( orbitAngle ), 0.0f ) * mOrbitRadius;
	mPos		= mParentNode->mPos + mPosRel;
	//mVel		= mPos - mPosPrev;
	mVel		= mPosRel - oldPosRel;
	
	Node::update( mat, bbRight, bbUp );
}

void NodeAlbum::drawStars( float uiScale )
{
	gl::color( mColor );
	gl::pushModelView();
	gl::translate( mTransPos );
	
	Vec2f radius = Vec2f( mRadius, mRadius );
	gl::drawBillboard( Vec3f::zero(), radius * 3.5f, 0.0f, mBbRight, mBbUp );
	gl::popModelView();
	
	Node::drawStars( uiScale );
}

void NodeAlbum::drawGlow()
{
	gl::color( mGlowColor );
	
	float radiusMulti;
	if( mIsSelected ){
		radiusMulti = 10.0f;
	} else {
		radiusMulti = 6.0f;
	}
	
	if( G_CURRENT_LEVEL == 1 )
		radiusMulti *= Rand::randFloat( 0.9f, 1.1f );
	
	gl::pushModelView();
	gl::translate( mTransPos );
	gl::drawBillboard( Vec3f::zero(), Vec2f( mRadius, mRadius ) * radiusMulti, 0.0f, mBbRight, mBbUp );
	gl::popModelView();
	
	Node::drawGlow();
}

void NodeAlbum::drawSphere( vector< gl::Texture*> texs )
{
	Node::drawSphere( texs );
}

void NodeAlbum::drawRings( vector< gl::Texture*> texs )
{
	Node::drawRings( texs );
}

void NodeAlbum::drawChildOrbits()
{	
	gl::pushModelView();
	gl::translate( mTransformedPos );
	gl::rotate( mMatrix );
	for( vector<Node*>::iterator c = mChildNodes.begin(); c != mChildNodes.end(); ++c ){
		float r = (*c)->mOrbitRadius;
		
		gl::color( ColorA( 0.15f, 0.2f, 0.4f, 0.15f ) );
		gl::drawStrokedCircle( Vec2f::zero(), r, 100 );
		
		if( (*c)->mIsPlaying ){
			mOrbitLine.push_back( Vec2f( (*c)->mPosRel.x, (*c)->mPosRel.y ) );
			
			gl::color( ColorA( 0.0f, 0.0f, 1.0f, 1.0f ) );
			gl::draw( mOrbitLine );
		}
	}
	gl::popModelView();
}

void NodeAlbum::select()
{
	int i=0;
	for(Playlist::Iter it = mAlbum->begin(); it != mAlbum->end(); ++it){
		TrackRef track		= *it;
		string name			= track->getTitle();
		std::cout << "trackname = " << name << std::endl;
		NodeTrack *newNode	= new NodeTrack( mPlayer, this, i, mFonts, name );
		mChildNodes.push_back( newNode );
		newNode->setData( track, mAlbum );
		i++;
	}
}

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

