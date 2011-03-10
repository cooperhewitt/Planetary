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
	
	
	
	mOrbitRadiusDest = Rand::randFloat( mParentNode->mRadius * 0.5f, mParentNode->mRadius * 1.5f );
	mIdealCameraDist = mRadius * 2.0f;
	
	mSphere			= Sphere( mPos, mRadius * 1.8f );
	mHitSphere		= Sphere( mPos, 0.2f );
}


void NodeAlbum::setData( PlaylistRef album )
{
	mAlbum				= album;
	mCurrentTrackIndex	= 0;
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
	double playbackTime		= app::getElapsedSeconds();
	double percentPlayed	= playbackTime/mOrbitPeriod;
	double orbitAngle		= percentPlayed * TWO_PI + mStartAngle;
	
	mPosPrev	= mTransPos;
	
	mPosRel		= Vec3f( cos( orbitAngle ), sin( orbitAngle ), 0.0f ) * mOrbitRadius;
	mPos		= mParentNode->mPos + mPosRel;

	if( mIsSelected ) mHitSphere.setRadius( 0.02f );
	else mHitSphere.setRadius( 0.2f );

	Node::update( mat, bbRight, bbUp );
	
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
		gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
	}
	
	Node::drawStarGlow();
}

void NodeAlbum::drawOrbitRing()
{
	gl::pushModelView();
	gl::translate( mParentNode->mTransPos );
	gl::rotate( mMatrix );
	
	if( mIsSelected )   gl::color( ColorA( 0.15f, 0.2f, 0.4f, 0.5f ) );
	else				gl::color( ColorA( 0.15f, 0.2f, 0.4f, 0.15f ) );
	
	gl::drawStrokedCircle( Vec2f::zero(), mOrbitRadius, 150 );

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
		int i=0;
		for( Playlist::Iter it = mAlbum->begin(); it != mAlbum->end(); ++it ){
			TrackRef track		= *it;
			string name			= track->getTitle();
			std::cout << "trackname = " << name << std::endl;
			NodeTrack *newNode	= new NodeTrack( this, i, mAlbum->size(), mFont, name );
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
