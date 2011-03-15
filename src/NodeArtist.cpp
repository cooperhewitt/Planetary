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
	mPosDest		= Rand::randVec3f() * Rand::randFloat( 50.0f, 150.0f );
	mPos			= mPosDest + Rand::randVec3f() * 25.0f;
	
	
	float hue		= Rand::randFloat( 0.0f, 0.5f );
	if( hue > 0.2f && hue < 0.4f ){
		hue			= Rand::randFloat( 0.0f, 0.5f );
	}
	float sat		= 1.0f - sin( hue * 2.0f * M_PI );
	mColor			= Color( CM_HSV, hue, sat * 0.2f, 1.0f );
	mGlowColor		= Color( CM_HSV, hue, sat + 0.3f, 1.0f );
	mIdealCameraDist = mRadius * 2.0f;
	
	mSphere			= Sphere( mPos, 3.65f );
	
	mAge			= 0.0f;
	mBirthPause		= Rand::randFloat( 50.0f );
	
	mRadiusDest		= mRadius * 0.66f;
	mRadius			= 0.0f;
}

void NodeArtist::update( const Matrix44f &mat )
{
	mPos -= ( mPos - mPosDest ) * 0.1f;
	mAge ++;
	
	Node::update( mat );
}

void NodeArtist::drawStar()
{
	if( mAge > mBirthPause ){
		mRadius -= ( mRadius - mRadiusDest ) * 0.1f;
		gl::color( mColor );
		gl::drawBillboard( mTransPos, Vec2f( mRadius, mRadius ), 0.0f, mBbRight, mBbUp );
	}
	
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
		Vec2f radius = Vec2f( mRadiusDest, mRadiusDest ) * flickerAmt;
        
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
		//float amt = mEclipsePer * 0.25f + 0.75f;
		//gl::color( ColorA( mGlowColor.r + amt, mGlowColor.g + amt, mGlowColor.b + amt, 1.0f ) );
		gl::color( Color::white() );
		float radius = mRadius * 0.33f;
		gl::enableAlphaBlending();
		gl::drawSolidCircle( Vec2f::zero(), radius, 100 );
		gl::popModelView();
		glEnable( GL_LIGHTING );
	}
	
	Node::drawPlanet( planets );
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
		
	}
	Node::select();
}

void NodeArtist::setData( PlaylistRef playlist )
{
	mPlaylist = playlist;
}

string NodeArtist::getName()
{
	return mPlaylist->getArtistName();
}
