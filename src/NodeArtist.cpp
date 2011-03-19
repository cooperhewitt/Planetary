/*
 *  NodeArtist.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "cinder/app/AppBasic.h"
#include "NodeArtist.h"
#include "NodeAlbum.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "Globals.h"

using namespace ci;
using namespace ci::ipod;
using namespace std;

NodeArtist::NodeArtist( Node *parent, int index, const Font &font )
	: Node( parent, index, font )
{
	mPosDest		= Rand::randVec3f() * Rand::randFloat( 50.0f, 200.0f );
	mPos			= mPosDest + Rand::randVec3f() * 25.0f;
	
	
	mHue			= Rand::randFloat( 0.0f, 0.66f );
	if( mHue > 0.2f && mHue < 0.35f ){
		mHue		= Rand::randFloat( 0.0f, 0.66f );
	}
	mSat			= 1.0f - sin( mHue * 1.3f * M_PI );
	mColor			= Color( CM_HSV, mHue, mSat * 0.2f, 1.0f );
	mGlowColor		= Color( CM_HSV, mHue, mSat + 0.3f, 1.0f );
	mDepthDiskColor = Color( CM_HSV, mHue, mSat, 1.0f );
	mIdealCameraDist = mRadius * 2.0f;
	
	mSphere			= Sphere( mPos, 3.65f );
	
	mAge			= 0.0f;
	mBirthPause		= Rand::randFloat( 50.0f );
	
	mRadiusDest		= mRadius * 0.66f;
	mRadius			= 0.0f;
	
	mOrbitRadiusMin	= mRadiusDest * 1.0f;
	mOrbitRadiusMax	= mRadiusDest * 2.5f;
}

void NodeArtist::update( const Matrix44f &mat )
{
	mPos -= ( mPos - mPosDest ) * 0.1f;
	mAge ++;
	
	if( mAge > mBirthPause ){
		if( G_ZOOM > G_ALPHA_LEVEL + 0.5f && !mIsSelected ){
			mRadius -= ( mRadius - 0.25f ) * 0.1f;
			mRadius += Rand::randFloat( 0.0125f, 0.065f );
		} else {
			mRadius -= ( mRadius - mRadiusDest ) * 0.1f;
		}
	}
	
	Node::update( mat );
}


void NodeArtist::drawStar()
{
	float radius    = mRadius;
	if( G_ZOOM > G_ALPHA_LEVEL + 0.5f && !mIsSelected ){
		mRadius -= ( mRadius - 0.25f ) * 0.1f;
		mRadius += Rand::randFloat( 0.0125f, 0.065f );
		gl::color( ColorA( mColor, 0.7f ) );
	} else {
		mRadius -= ( mRadius - mRadiusDest ) * 0.1f;
		gl::color( mColor );
	}
	
	
	if( mIsHighlighted ) radius += math<float>::max( G_ARTIST_LEVEL - G_ZOOM, 0.0f );
	gl::drawBillboard( mTransPos, Vec2f( radius, radius ), 0.0f, mBbRight, mBbUp );
	
	Node::drawStar();
}

void NodeArtist::drawStarGlow()
{
	if( mIsHighlighted && mDistFromCamZAxisPer > 0.0f ){
        float zoomOffset    = math<float>::max( G_ARTIST_LEVEL - G_ZOOM, 0.0f );
		
        float alpha         = mDistFromCamZAxisPer;
        gl::color( ColorA( mGlowColor, alpha ) );

		// if in alpha view, make highlighted artists flicker
        float flickerAmt = ( 8.5f + zoomOffset * Rand::randFloat( 12.0f, 15.0f ) );
		Vec2f radius = Vec2f( mRadius, mRadius ) * flickerAmt;
        
		gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
	}

	Node::drawStarGlow();
}

void NodeArtist::drawPlanet( const vector<gl::Texture> &planets )
{
	if( mIsSelected ){
		glDisable( GL_LIGHTING );
		glDisable( GL_TEXTURE_2D );
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::color( mDepthDiskColor );
		float radius = mRadius * 0.3f;
		gl::enableAdditiveBlending();
		gl::drawSolidCircle( Vec2f::zero(), radius, 64 );
		gl::popModelView();
		glEnable( GL_LIGHTING );
	}
	
	Node::drawPlanet( planets );
}

void NodeArtist::drawClouds( const vector<gl::Texture> &planets )
{
	Node::drawClouds( planets );
}

void NodeArtist::select()
{
	if (!mIsSelected) {
		// TODO: switch this to use artist ID
		vector<ipod::PlaylistRef> albumsBySelectedArtist = getAlbumsWithArtist( mPlaylist->getArtistName() );
		mNumAlbums = albumsBySelectedArtist.size();
		
		int i=0;
		for(vector<PlaylistRef>::iterator it = albumsBySelectedArtist.begin(); it != albumsBySelectedArtist.end(); ++it){
			PlaylistRef album	= *it;
			NodeAlbum *newNode = new NodeAlbum( this, i, mFont );
			mChildNodes.push_back( newNode );
			newNode->setData( album );
			i++;
		}
		
		for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
			(*it)->setSphereData( mTotalVertsHiRes, mSphereVertsHiRes, mSphereTexCoordsHiRes, mSphereNormalsHiRes,
								  mTotalVertsLoRes, mSphereVertsLoRes, mSphereTexCoordsLoRes, mSphereNormalsLoRes );
		}
		
		setChildOrbitRadii();
		
	}
	Node::select();
}

void NodeArtist::setChildOrbitRadii()
{
	float orbitRadius = mOrbitRadiusMin;
	float orbitOffset;
	for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
		orbitOffset = (*it)->mRadius * 5.0f;
		orbitRadius += orbitOffset;
		(*it)->mOrbitRadiusDest = orbitRadius;
		orbitRadius += orbitOffset;
	}
}

void NodeArtist::setData( PlaylistRef playlist )
{
	mPlaylist = playlist;
}

string NodeArtist::getName()
{
	string name = mPlaylist->getArtistName();
	if( name.size() < 1 ) name = "Untitled";
	return name;
}
