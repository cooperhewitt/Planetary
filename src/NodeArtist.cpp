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

NodeArtist::NodeArtist( Node *parent, int index, const Font &font, std::string name )
	: Node( parent, index, font, name )
{
	mPosDest		= Rand::randVec3f() * Rand::randFloat( 50.0f, 150.0f );
	
	float hue		= Rand::randFloat( 0.0f, 0.5f );
	if( hue > 0.2f && hue < 0.4f ){
		hue			= Rand::randFloat( 0.0f, 0.5f );
	}
	float sat		= 1.0f - sin( hue * 2.0f * M_PI );
	mColor			= Color( CM_HSV, hue, sat * 0.2f, 1.0f );
	mGlowColor		= Color( CM_HSV, hue, sat + 0.3f, 1.0f );
	mIdealCameraDist = mRadius * 2.0f;
	
	mSphere			= Sphere( mPos, 3.7f );
	mHitSphere		= Sphere( mPos, 10.0f );
}

void NodeArtist::update( const Matrix44f &mat )
{
	mPos -= ( mPos - mPosDest ) * 0.1f;
	if( mIsSelected ) mHitSphere.setRadius( 0.5f );
	else mHitSphere.setRadius( 10.0f );
	
	
	Node::update( mat );
}

void NodeArtist::drawStar()
{
	gl::color( mColor );
	gl::drawBillboard( mTransPos, Vec2f( mRadius, mRadius ) * 0.66f, 0.0f, mBbRight, mBbUp );

	Node::drawStar();
}

void NodeArtist::drawStarGlow()
{
	if( mIsHighlighted && mDistFromCamZAxisPer > 0.0f ){
		gl::color( ColorA( mGlowColor, mDistFromCamZAxisPer ) );
		Vec2f radius = Vec2f( mRadius, mRadius ) * 8.5f;
//		if( mIsSelected ) radius *= 2.5f;
		gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
	}

	Node::drawStarGlow();
}

void NodeArtist::drawOrbitRing()
{
	Node::drawOrbitRing();
}

void NodeArtist::drawPlanet( const Matrix44f &accelMatrix, const vector<gl::Texture> &planets )
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

void NodeArtist::drawClouds( const Matrix44f &accelMatrix, const vector<gl::Texture> &clouds )
{
	Node::drawClouds( accelMatrix, clouds );
}

void NodeArtist::drawRings( const gl::Texture &tex )
{
	Node::drawRings( tex );
}

void NodeArtist::drawAtmosphere()
{
	Node::drawAtmosphere();
}

void NodeArtist::select()
{
	if (!mIsSelected) {
		vector<ipod::PlaylistRef> albumsBySelectedArtist = getAlbumsWithArtist( mName );
		mNumAlbums = albumsBySelectedArtist.size();
		
		int i=0;
		for(vector<PlaylistRef>::iterator it = albumsBySelectedArtist.begin(); it != albumsBySelectedArtist.end(); ++it){
			PlaylistRef album	= *it;
			string name			= album->getAlbumTitle();
			NodeAlbum *newNode = new NodeAlbum( this, i, mFont, name );
			mChildNodes.push_back( newNode );
			newNode->setData( album );
			i++;
		}
		
	}
	Node::select();
}