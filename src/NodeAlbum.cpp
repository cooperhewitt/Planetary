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
	float sat		= 1.0f - sin( hue * 2.0f * M_PI );
	mColor			= Color( CM_HSV, hue, sat * 0.5f, 1.0f );
	mGlowColor		= Color( CM_HSV, hue + 0.15f, sat, 1.0f );
	
	mSphere.setRadius( mSphere.getRadius() * 5.0f );
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


void NodeAlbum::update( const CameraPersp &cam, const Matrix44f &mat, const Vec3f &bbRight, const Vec3f &bbUp )
{
	double playbackTime		= app::getElapsedSeconds();
	double percentPlayed	= playbackTime/mOrbitPeriod;
	double orbitAngle		= percentPlayed * TWO_PI + mStartAngle;
	
	mPosPrev	= mPos;
	
	Vec3f oldPosRel = mPosRel;
	mPosRel		= Vec3f( cos( orbitAngle ), sin( orbitAngle ), 0.0f ) * mOrbitRadius;
	mPos		= mParentNode->mPos + mPosRel;
	//mVel		= mPos - mPosPrev;
	mVel		= mPosRel - oldPosRel;
	
	Node::update( cam, mat, bbRight, bbUp );
}

void NodeAlbum::drawStar()
{
	gl::color( mColor );
	gl::drawBillboard( mTransPos, Vec2f( mRadius, mRadius ), 0.0f, mBbRight, mBbUp );
	
	Node::drawStar();
}

void NodeAlbum::drawStarGlow()
{
	if( mIsHighlighted ){
		gl::color( mGlowColor );
		Vec2f radius = Vec2f( mRadius, mRadius ) * 3.5f;
		gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
	}
	
	Node::drawStarGlow();
}

void NodeAlbum::drawOrbitalRings()
{
	if( mIsSelected ){
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::rotate( mMatrix );
		for( vector<Node*>::iterator c = mChildNodes.begin(); c != mChildNodes.end(); ++c ){
			float r = (*c)->mOrbitRadius;
			
			gl::color( ColorA( 0.15f, 0.2f, 0.4f, 0.15f ) );
			gl::drawStrokedCircle( Vec2f::zero(), r, 150 );
		}
		gl::popModelView();
	}
}


void NodeAlbum::select()
{
	int i=0;
	for( Playlist::Iter it = mAlbum->begin(); it != mAlbum->end(); ++it ){
		TrackRef track		= *it;
		string name			= track->getTitle();
		std::cout << "trackname = " << name << std::endl;
		NodeTrack *newNode	= new NodeTrack( this, i, mFont, name );
		mChildNodes.push_back( newNode );
		newNode->setData( track, mAlbum );
		i++;
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
