/*
 *  Node.cpp
 *  Kepler
 *
 *  Created by Robert Hodgin on 2/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "BloomGl.h"
#include "Globals.h"
#include "Node.h"

using namespace ci;
using namespace std;

Node::Node( Node *parent, int index, const Font &font, const Surface &surfaces )
	: mParentNode( parent ), mIndex( index ), mFont( font ), mSurfaces( surfaces )
{
    mZoomPer            = 0.0f;
    
	mScreenPos			= Vec2f::zero();
	mEclipsePer			= 1.0f;
	mEclipseStrength	= 0.0f;
	mEclipseAngle		= 0.0f;
	mEclipseDirBasedAlpha = 0.0f;
	mTransPos			= Vec3f::zero();
	mTransVel			= Vec3f::zero();
	
	mOrbitStartAngle	= Rand::randFloat( TWO_PI );
	mOrbitAngle			= mOrbitStartAngle;
	mOrbitPeriod		= Rand::randFloat( 125.0f, 150.0f ); // TODO: move to NodeArtist and make non-random
	mOrbitRadius		= 0.01f;
	mAngularVelocity	= 0.0f;
	mPercentPlayed		= 0.0f;
	mDistFromCamZAxis	= 1000.0f;
	mDistFromCamZAxisPer = 1.0f;
	mPlanetTexIndex		= 0;
		
	mHitArea			= Rectf( 0.0f, 0.0f, 10.0f, 10.0f ); //just for init.
	mHighlightStrength	= 0.0f;
	
	mIsTapped			= false;
	mIsSelected			= false;
    mIsPlaying          = false;
	mIsHighlighted		= false;
	mIsDying			= false;
	mIsDead				= false;
	
	mDeathCount			= 0;
	mDeathThresh		= 100;
	mDeathPer			= 0.0f;
}

void Node::setIsDying( bool isDying )
{
	mIsDying = isDying;
	
	if( mIsDying == false ){
		mIsDead = false;
		mDeathCount = 0;
	}
}

void Node::setSphereData( int totalHiVertices, float *sphereHiVerts, float *sphereHiTexCoords, float *sphereHiNormals, 
						 int totalLoVertices, float *sphereLoVerts, float *sphereLoTexCoords, float *sphereLoNormals )
{
	mTotalVertsHiRes		= totalHiVertices;
	mTotalVertsLoRes		= totalLoVertices;
	mSphereVertsHiRes		= sphereHiVerts;
	mSphereTexCoordsHiRes	= sphereHiTexCoords;
	mSphereNormalsHiRes		= sphereHiNormals;
	mSphereVertsLoRes		= sphereLoVerts;
	mSphereTexCoordsLoRes	= sphereLoTexCoords;
	mSphereNormalsLoRes		= sphereLoNormals;
}

void Node::createNameTexture()
{
	TextLayout layout;
	layout.setFont( mFont );
	layout.setColor( Color( 1.0f, 1.0f, 1.0f ) );
	layout.addLine( getName() );
	Surface8u nameSurface	= Surface8u( layout.render( true, false ) );
	mNameTex				= gl::Texture( nameSurface );
}

void Node::update( const Matrix44f &mat )
{	
	mOrbitRadius -= ( mOrbitRadius - mOrbitRadiusDest ) * 0.1f;
	mMatrix         = mat;
	mTransPos       = mMatrix * mPos;
    
	mSphere.setCenter( mTransPos );

    if( mIsPlaying || mIsSelected ){
        mZoomPer    = constrain( ( G_ZOOM - mGen ) + 2.0f, 0.0f, 1.0f );
	} else {
        mZoomPer    = constrain( 1.0f - abs( G_ZOOM - mGen + 1.0f ), 0.0f, 1.0f );
    }
	mZoomPer = pow( mZoomPer, 4.0f );
	
	
	if( mIsDying ){
		mDeathCount ++;
		if( mDeathCount > mDeathThresh ){
			mIsDead = true;
			mIsSelected = false;
		}
	}
	
	mDeathPer = 1.0f - (float)mDeathCount/(float)mDeathThresh;
	
	
	
	bool clearChildNodes = false;
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		if( (*nodeIt)->mIsDead ){
			clearChildNodes = true;
		}
		(*nodeIt)->update( mat );
	}
	
	if( clearChildNodes ){
		mIsSelected = false;
		for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
			delete (*nodeIt);
		}
		mChildNodes.clear();
	}
}

void Node::updateGraphics( const CameraPersp &cam, const Vec3f &bbRight, const Vec3f &bbUp )
{
	mBbRight = bbRight;
	mBbUp    = bbUp;

    
	if( mIsHighlighted ){
        mScreenPos              = cam.worldToScreen( mTransPos, app::getWindowWidth(), app::getWindowHeight() );
		mPrevDistFromCamZAxis	= mDistFromCamZAxis;
		mDistFromCamZAxis		= cam.worldToEyeDepth( mTransPos );
		mDistFromCamZAxisPer	= constrain( mDistFromCamZAxis * -0.5f, 0.0f, 1.0f ); // REL: -0.35f
		mSphereScreenRadius     = cam.getScreenRadius( mSphere, app::getWindowWidth(), app::getWindowHeight() );
        float r					= max( mSphereScreenRadius, 15.0f );        
        mSphereHitArea			= Rectf( mScreenPos.x - r, mScreenPos.y - r, mScreenPos.x + r, mScreenPos.y + r );        
	}
	
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->updateGraphics( cam, mBbRight, mBbUp );
	}
}

void Node::drawEclipseGlow()
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawEclipseGlow();
	}
}

//void Node::drawPlanet( const vector<gl::Texture> &planets )
//{
//	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
//		(*nodeIt)->drawPlanet( planets );
//	}
//}
//
//void Node::drawClouds( const vector<gl::Texture> &planets, const vector<gl::Texture> &clouds )
//{
//	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
//		(*nodeIt)->drawClouds( planets, clouds );
//	}
//}
//
//void Node::drawAtmosphere( const gl::Texture &tex )
//{
//	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
//		(*nodeIt)->drawAtmosphere( tex );
//	}
//}

void Node::drawRings( const gl::Texture &tex, GLfloat *planetRingVerts, GLfloat *planetRingTexCoords, float camZPos )
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawRings( tex, planetRingVerts, planetRingTexCoords, camZPos );
	}
}

void Node::drawOrbitRing( float pinchAlphaOffset, float camAlpha, const gl::Texture &orbitRingGradient, GLfloat *ringVertsLowRes, GLfloat *ringTexLowRes, GLfloat *ringVertsHighRes, GLfloat *ringTexHighRes )
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawOrbitRing( pinchAlphaOffset, camAlpha, orbitRingGradient, ringVertsLowRes, ringTexLowRes, ringVertsHighRes, ringTexHighRes );
	}
}



void Node::drawName( const CameraPersp &cam, float pinchAlphaPer, float angle )
{	
	if( cam.worldToEyeDepth( mTransPos ) < 0 ){
		float alpha;
		Color c;
		
		if( mIsSelected || ( G_CURRENT_LEVEL < mGen && mIsPlaying ) ){
			c = Color::white();
			
			if( G_CURRENT_LEVEL >= mGen - 2 )
				alpha = mZoomPer * mDeathPer;
			else
				alpha = 0.0f;
			
			
		} else {
			c = COLOR_BRIGHT_BLUE;
			
			if( G_CURRENT_LEVEL >= mGen - 1 )
				alpha = 0.5f * pinchAlphaPer * mZoomPer * mDeathPer;
			else
				alpha = 0.0f;
			
		}
		
		gl::color( ColorA( c, alpha ) );
		
		
		if( alpha > 0 ){
			Vec2f pos1, pos2;
			Vec2f offset0, offset1, offset2;
			
			if (mNameTex == NULL) {
				createNameTexture();
			}

			offset0 = Vec2f( mSphereScreenRadius, mSphereScreenRadius ) * 0.75f;
			offset0.rotate( angle );
			pos1 = mScreenPos + offset0;
			offset1 = Vec2f( 10.0f, 10.0f );
			offset1.rotate( angle );
			pos2 = pos1 + offset1;
			offset2 = Vec2f( 2.0f, -8.0f );
			offset2.rotate( angle );

			Vec2f texCorner = mNameTex.getSize();
			
			gl::pushModelView();
			gl::translate( pos2 + offset2 );
			if (angle != 0) {
				gl::rotate( angle * 180.0f/M_PI );
				texCorner.rotate( angle );
			}
			if( mIsPlaying ){
				float s = mZoomPer * 0.25f + 1.0f;
				gl::scale( Vec3f( s, s, 1.0f ) );
				texCorner *= s;
			}
			
			gl::draw( mNameTex, Vec2f::zero() );
			gl::popModelView();
			
			mHitArea = Rectf( pos2 + offset2, pos2 + offset2 + texCorner);
			mHitArea.canonicalize();        
			inflateRect( mHitArea, 5.0f );
			
			glDisable( GL_TEXTURE_2D );
			
			gl::color( ColorA( COLOR_BRIGHT_BLUE, alpha * 0.5f ) );
			gl::drawLine( pos1, pos2 );
		} else {
			mHitArea = Rectf( -10000.0f, -10000.0f, -9999.0f, -9999.0f );
		}
		
		
		
		/*
		// For viewing node states
		if( mIsHighlighted ){
			gl::color( Color( 1.0f, 0.0f, 0.0f ) );
			gl::drawLine( pos1 + Vec2f( 1.0f, -1.0f ), pos1 + Vec2f( -1.0f, 1.0f ) );
		}
		
		if( mIsSelected ){
			gl::color( Color( 0.0f, 1.0f, 0.0f ) );
			gl::drawLine( pos1 + Vec2f( 3.0f, 1.0f ), pos1 + Vec2f( 1.0f, 3.0f ) );
		}
		
		if( mIsPlaying ){
			gl::color( Color( 0.0f, 0.0f, 1.0f ) );
			gl::drawLine( pos1 + Vec2f( 5.0f, 3.0f ), pos1 + Vec2f( 3.0f, 5.0f ) );
		}
		 */
	}
	

	
	
	
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawName( cam, pinchAlphaPer, angle );
	}
}

void Node::drawTouchHighlight( float zoomAlpha )
{
	if( mIsHighlighted ){
		Vec2f radius = Vec2f( mRadius * 5.0f, mRadius * 5.0f );
		if( mIsTapped ){
			gl::color( ColorA( mColor, mHighlightStrength ) );
			mHighlightStrength -= ( mHighlightStrength - 0.0f ) * 0.1f;
			gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
		}
		
		if( mGen == G_TRACK_LEVEL ){
			float alpha = max( 0.7f - mDistFromCamZAxisPer, 0.0f );
			gl::color( ColorA( COLOR_BRIGHT_BLUE, alpha + mEclipseStrength ) );
			gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
		} else if( mGen == G_ALBUM_LEVEL ){
			float alpha = max( ( mDistFromCamZAxis + 5.0f ) * 0.25f, 0.0f );
			gl::color( ColorA( COLOR_BRIGHT_BLUE, alpha + mEclipseStrength ) );
			gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
		}
		
		if( mHighlightStrength < 0.01f ){
			mIsTapped = false;
		}

		for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
			(*nodeIt)->drawTouchHighlight( zoomAlpha );
		}
	}
}

void Node::checkForNameTouch( vector<Node*> &nodes, const Vec2f &pos )
{
	if( mDistFromCamZAxisPer > 0.0f ){
		if( mSphereHitArea.contains( pos ) || ( mNameTex != NULL && mHitArea.contains( pos ) && G_DRAW_TEXT ) ) {
			nodes.push_back( this );
		}
	}
    vector<Node*>::iterator nodeIt;
    for( nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
        (*nodeIt)->checkForNameTouch( nodes, pos );
    }
}

void Node::select()
{
	mIsSelected = true;
	setIsDying( false );
	
    vector<Node*>::iterator nodeIt;
	for( nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
        (*nodeIt)->setIsDying( false );
    }
}

void Node::deselect()
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->mIsDying = true;
	}
	mIsSelected = false;
	
//	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
//		delete (*nodeIt);
//	}
//	mChildNodes.clear();
}


