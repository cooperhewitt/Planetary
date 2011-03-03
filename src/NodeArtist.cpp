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
	float sat		= 1.0f - sin( hue * 2.0f * M_PI );
	mColor			= Color( CM_HSV, hue, sat * 0.25f, 1.0f );
	mGlowColor		= Color( CM_HSV, hue, sat, 1.0f );
}

void NodeArtist::update( const CameraPersp &cam, const Matrix44f &mat, const Vec3f &bbRight, const Vec3f &bbUp )
{
	mPos -= ( mPos - mPosDest ) * 0.1f;
	
	Node::update( cam, mat, bbRight, bbUp );
}

void NodeArtist::drawStar()
{
	gl::color( mColor );
	gl::drawBillboard( mTransPos, Vec2f( mRadius, mRadius ), 0.0f, mBbRight, mBbUp );

	Node::drawStar();
}

void NodeArtist::drawStarGlow()
{
	if( mIsHighlighted && mDistFromCamZAxisPer > 0.0f ){
		gl::color( ColorA( mGlowColor, mDistFromCamZAxisPer ) );
		Vec2f radius = Vec2f( mRadius, mRadius ) * 4.5f;
		if( mIsSelected ) radius *= 2.5f;
		gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
	}

	Node::drawStarGlow();
}

void NodeArtist::drawOrbitalRings()
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
		
		for( vector<Node*>::iterator c = mChildNodes.begin(); c != mChildNodes.end(); ++c ){
			(*c)->drawOrbitalRings();
		}
	}
}

void NodeArtist::select()
{
	vector<ipod::PlaylistRef> albumsBySelectedArtist = getAlbumsWithArtist( mName );
	
	int i=0;
	for(vector<PlaylistRef>::iterator it = albumsBySelectedArtist.begin(); it != albumsBySelectedArtist.end(); ++it){
		PlaylistRef album	= *it;
		string name			= album->getAlbumTitle();
		NodeAlbum *newNode = new NodeAlbum( this, i, mFont, name );
		
		mChildNodes.push_back( newNode );
		newNode->setData( album );
		i++;
	}
	
	Node::select();
}