//
//  WheelOverlay.cpp
//  Kepler
//
//  Created by Robert Hodgin on 7/20/11.
//  Copyright 2011 Bloom Studio, Inc.. All rights reserved.
//

#include "WheelOverlay.h"

#include <sstream>

#include "cinder/gl/gl.h"
#include "cinder/Font.h"
#include "cinder/Text.h"
#include "cinder/ImageIo.h"

#include "Globals.h"
#include "BloomGl.h"
#include "BloomScene.h"


using std::stringstream;
using namespace ci;
using namespace ci::app;
using namespace std;

void WheelOverlay::setup( const gl::Texture &tex )
{
    mWheelScale	= 2.25f;
	mShowWheel  = false;    
	mRadius     = -1.0f; // updated in update :)
    mVerts      = NULL;
    mTex = tex;
}

void WheelOverlay::update()
{
    float prevWheelScale = mWheelScale;
    
	if( getShowWheel() ){
		mWheelScale -= ( mWheelScale - 1.0f ) * 0.2f;
	} else {
		mWheelScale -= ( mWheelScale - 2.25f ) * 0.2f;	
	}    
        
    const Vec2f interfaceSize = getRoot()->getInterfaceSize();
    
    if (mInterfaceSize != interfaceSize || mWheelScale != prevWheelScale) {
        
        mInterfaceSize = interfaceSize;
        mInterfaceCenter = mInterfaceSize * 0.5f;        

        Matrix44f mat;
        mat.translate( Vec3f(mInterfaceCenter, 0) );
        
        if ( mInterfaceSize.x > mInterfaceSize.y ) {
            // adjust for control panel in landscape
            float amount = (mInterfaceSize.x - mInterfaceSize.y) / (1024-768);            
            mat.translate( Vec3f(0, -15.0f * amount, 0) );
        }

        mat.scale( Vec3f( mWheelScale, mWheelScale, 1.0f ) );
        setTransform(mat);        
    }    
    
    float prevRadius = mRadius;

	mRadius = 315.0f;
	if( interfaceSize.x > interfaceSize.y ) {
        // adjust for control panel in landscape
        float amount = (interfaceSize.x - interfaceSize.y) / (1024-768);        
		mRadius -= 30.0f * amount;
    }
    
    if (mVerts == NULL || mRadius != prevRadius) {
        updateVerts();
    }
}

void WheelOverlay::updateVerts()
{	
	mTotalVertices = 6;
	delete[] mVerts; 
	mVerts = NULL;
	mVerts = new VertexData[mTotalVertices];
	
	float W	= 1280; // sqrt(1024 * 1024 + 768 * 768) (diagonal)
	float H = W;    // squared off for orientation animation
	float CW = W/2;
	float CH = H/2;
		
	vector<Vec2i> positions;
	positions.push_back( Vec2i( 0 - CW, 0 - CH ) );
	positions.push_back( Vec2i( W - CW, 0 - CH ) );
	positions.push_back( Vec2i( 0 - CW, H - CH ) );
	positions.push_back( Vec2i( W - CW, H - CH ) );
	
	vector<Vec2f> textures;
	textures.push_back( Vec2f( 0, 0 ) );
	textures.push_back( Vec2f( 1, 0 ) );
	textures.push_back( Vec2f( 0, 1 ) );
	textures.push_back( Vec2f( 1, 1 ) );
	
	int indices[6] = { 0, 1, 2, 1, 3, 2 };
	
	int vIndex = 0;
	for( int i=0; i<6; i++ ){
		mVerts[vIndex].vertex	= positions[indices[i]];
		mVerts[vIndex].texture	= textures[indices[i]];
		
		vIndex ++;
	}
}

void WheelOverlay::draw()
{	
    if ( mWheelScale < 2.24f ) {    
        
        gl::color( ColorA( 1.0f, 1.0f, 1.0f, constrain(2.0f - mWheelScale, 0.0f, 1.0f) ) );
        
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
}

void WheelOverlay::setShowWheel( bool b )
{
    mShowWheel = b; 
    mCallbacksWheelToggled.call( b );
}

bool WheelOverlay::hitTest( const Vec2f &globalPoint )
{
    return globalToLocal( globalPoint ).distance( Vec2f::zero() ) > mRadius;
}
