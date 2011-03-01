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

void NodeArtist::update( const Matrix44f &mat, const Vec3f &bbRight, const Vec3f &bbUp )
{
	mPos -= ( mPos - mPosDest ) * 0.1f;
	Node::update( mat, bbRight, bbUp );
}

void NodeArtist::drawStar()
{
	gl::color( mColor );
	Vec2f radius = Vec2f( mRadius, mRadius );
	if( mIsSelected ) radius *= 3.0f;
	gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );

	Node::drawStar();
}

void NodeArtist::drawStarGlow()
{
	if( mIsHighlighted ){
		gl::color( mGlowColor );
		Vec2f radius = Vec2f( mRadius, mRadius ) * 7.5f;
		if( mIsSelected ) radius *= 5.0f;
		gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
	}

	Node::drawStarGlow();
}

/*
void NodeArtist::select()
{
	vector<ipod::PlaylistRef> albumsBySelectedArtist = getAlbumsWithArtist( mName );
	
	int i=0;
	for(vector<PlaylistRef>::iterator it = albumsBySelectedArtist.begin(); it != albumsBySelectedArtist.end(); ++it){
		PlaylistRef album	= *it;
		string name			= album->getAlbumTitle();
		NodeAlbum *newNode = new NodeAlbum( mPlayer, this, i, mFonts, name );
		mChildNodes.push_back( newNode );
		newNode->setData( album );
		i++;
	}
}*/