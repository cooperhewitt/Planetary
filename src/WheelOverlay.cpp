//
//  WheelOverlay.cpp
//  Kepler
//
//  Created by Robert Hodgin on 7/20/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "WheelOverlay.h"
#include "cinder/gl/gl.h"
#include "cinder/Font.h"
#include "cinder/Text.h"
#include "cinder/ImageIo.h"
#include "Globals.h"
#include "BloomGl.h"
#include <sstream>

using std::stringstream;
using namespace ci;
using namespace ci::app;
using namespace std;

WheelOverlay::WheelOverlay()
{
}

WheelOverlay::~WheelOverlay()
{
}

void WheelOverlay::setup()
{
	mRadius = 315.0f;
	mTex			= gl::Texture( loadImage( loadResource( "alphaWheelMask.png" ) ) );
}

void WheelOverlay::update( const Vec2f &interfaceSize )
{
	mRadius = 315.0f;
	if( interfaceSize.x > interfaceSize.y )
		mRadius = 285.0f;
	
	mTotalVertices = 54;
	delete[] mVerts; 
	mVerts = NULL;
	mVerts = new VertexData[mTotalVertices];
	
	
	float W	= interfaceSize.x;
	float H = interfaceSize.y;
	float CW = W/2;
	float CH = H/2;
	float L = ( W - mRadius * 2.0f )/2;
	float T = ( H - mRadius * 2.0f )/2;
	float R = L + mRadius * 2.0f;
	float B = T + mRadius * 2.0f;
	
	if( interfaceSize.x > interfaceSize.y ){
		T -= 12.0f;
		B -= 12.0f;
	}
	
	vector<Vec2i> positions;
	positions.push_back( Vec2i( 0 - CW, 0 - CH ) );
	positions.push_back( Vec2i( L - CW, 0 - CH ) );
	positions.push_back( Vec2i( R - CW, 0 - CH ) );
	positions.push_back( Vec2i( W - CW, 0 - CH ) );
	
	positions.push_back( Vec2i( 0 - CW, T - CH ) );
	positions.push_back( Vec2i( L - CW, T - CH ) );
	positions.push_back( Vec2i( R - CW, T - CH ) );
	positions.push_back( Vec2i( W - CW, T - CH ) );
	
	positions.push_back( Vec2i( 0 - CW, B - CH ) );
	positions.push_back( Vec2i( L - CW, B - CH ) );
	positions.push_back( Vec2i( R - CW, B - CH ) );
	positions.push_back( Vec2i( W - CW, B - CH ) );
	
	positions.push_back( Vec2i( 0 - CW, H - CH ) );
	positions.push_back( Vec2i( L - CW, H - CH ) );
	positions.push_back( Vec2i( R - CW, H - CH ) );
	positions.push_back( Vec2i( W - CW, H - CH ) );
	
	vector<Vec2f> textures;
	textures.push_back( Vec2f( 0, 0 ) );
	textures.push_back( Vec2f( 0, 0 ) );
	textures.push_back( Vec2f( 1, 0 ) );
	textures.push_back( Vec2f( 1, 0 ) );
	
	textures.push_back( Vec2f( 0, 0 ) );
	textures.push_back( Vec2f( 0, 0 ) );
	textures.push_back( Vec2f( 1, 0 ) );
	textures.push_back( Vec2f( 1, 0 ) );
	
	textures.push_back( Vec2f( 0, 1 ) );
	textures.push_back( Vec2f( 0, 1 ) );
	textures.push_back( Vec2f( 1, 1 ) );
	textures.push_back( Vec2f( 1, 1 ) );
	
	textures.push_back( Vec2f( 0, 1 ) );
	textures.push_back( Vec2f( 0, 1 ) );
	textures.push_back( Vec2f( 1, 1 ) );
	textures.push_back( Vec2f( 1, 1 ) );
	
	int indices[54] = { 0, 1, 4,
		1, 5, 4,
		1, 2, 5,
		2, 6, 5,
		2, 3, 6,
		3, 7, 6,
		4, 5, 8, 
		5, 9, 8,
		5, 6, 9, 
		6, 10, 9, 
		6, 7, 10, 
		7, 11, 10,
		8, 9, 12, 
		9, 13, 12, 
		9, 10, 13, 
		10, 14, 13, 
		10, 11, 14, 
		11, 15, 14 };
	
	int vIndex = 0;
	for( int i=0; i<54; i++ ){
		mVerts[vIndex].vertex	= positions[indices[i]];
		mVerts[vIndex].texture	= textures[indices[i]];
		
		vIndex ++;
	}
}

void WheelOverlay::draw()
{	
	mTex.enableAndBind();
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	
	glVertexPointer( 2, GL_FLOAT, sizeof(VertexData), mVerts );
	glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), &mVerts[0].texture );
	
	glDrawArrays( GL_TRIANGLES, 0, mTotalVertices );
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	mTex.disable();
}

