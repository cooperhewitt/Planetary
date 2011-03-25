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
#include "Globals.h"
#include "Node.h"

using namespace ci;
using namespace std;

Node::Node( Node *parent, int index, const Font &font )
	: mParentNode( parent ), mIndex( index ), mFont( font )
{
	if( mParentNode ){
		initWithParent();
	} else {
		init();
	}

	//createNameTexture();
	
    mZoomPer            = 0.0f;
    
	mScreenPos			= Vec2f::zero();
	mEclipsePer			= 1.0f;
	mTransPos			= mPos;

	mOrbitStartAngle	= Rand::randFloat( TWO_PI );
	mOrbitAngle			= mOrbitStartAngle;
	mOrbitPeriod		= Rand::randFloat( 25.0f, 150.0f );
	mOrbitRadius		= 0.01f;
	mAngularVelocity	= 0.0f;
	mPercentPlayed		= 0.0f;
	mDistFromCamZAxis	= 1000.0f;
	mDistFromCamZAxisPer = 1.0f;
	mPlanetTexIndex		= 0;
    
    mSphereRes          = 12;
    mSphereResInt       = 12;
		
	mHitArea			= Rectf( 0.0f, 0.0f, 10.0f, 10.0f ); //just for init.
	mHighlightStrength	= 0.0f;
	mIsTapped			= false;
	mIsSelected			= false;
    mIsPlaying          = false;
	mIsHighlighted		= false;
}

void Node::init()
{
	mGen				= G_ARTIST_LEVEL;
	mRadiusDest			= 2.0f;
	mRadius				= 2.0f;
	mPos				= Rand::randVec3f();
	mPrevPos			= mPos;
	mVel				= Vec3f::zero();
	mOrbitRadiusDest	= 0.0f;
	mOrbitPeriod		= 0.0f;
}

void Node::initWithParent()
{
	mGen				= mParentNode->mGen + 1;
	mRadiusDest			= mParentNode->mRadiusDest * 0.01f;
	mRadius				= mRadiusDest;
	mPos				= mParentNode->mPos;
	mPrevPos			= mParentNode->mPos;
	mVel				= mParentNode->mVel;
	mOrbitPeriod		= Rand::randFloat( 75.0f, 150.0f );
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
        mZoomPer    = constrain( ( G_ZOOM - mGen ) + 1.0f, 0.0f, 1.0f );
//        mZoomPer    = constrain( 1.0f - abs( G_ZOOM - mGen + 1.0f ), 0.0f, 1.0f ); 
	} else {
        mZoomPer    = constrain( 1.0f - abs( G_ZOOM - mGen + 1.0f ), 0.0f, 1.0f );
    }
	mZoomPer = pow( mZoomPer, 5.0f );
	
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->update( mat );
	}
}

void Node::updateGraphics( const CameraPersp &cam, const Vec3f &bbRight, const Vec3f &bbUp )
{
	mBbRight = bbRight;
	mBbUp    = bbUp;
    
    if( mGen >= G_ALBUM_LEVEL ){
        if( mIsSelected || mIsPlaying ){
            mSphereRes		-= ( mSphereRes - 16 ) * 0.1f;
            mCamDistAlpha	-= ( mCamDistAlpha - 1.0f ) * 0.1f;
        } else {
            mSphereRes		-= ( mSphereRes - 10 ) * 0.1f;
            mCamDistAlpha	-= ( mCamDistAlpha - 0.0f ) * 0.1f;
        }
        
		if( mSphereScreenRadius < 20 ){
			mSphereRes		= 4;
		}
        mSphereResInt       = (int)mSphereRes * 2;
    }
    
	if( mIsHighlighted ){
        mScreenPos              = cam.worldToScreen( mTransPos, app::getWindowWidth(), app::getWindowHeight() );
		mPrevDistFromCamZAxis	= mDistFromCamZAxis;
		mDistFromCamZAxis		= cam.worldToEyeDepth( mTransPos );
		mDistFromCamZAxisPer	= constrain( mDistFromCamZAxis * -0.35f, 0.0f, 1.0f );
		mSphereScreenRadius     = cam.getScreenRadius( mSphere, app::getWindowWidth(), app::getWindowHeight() ) * 0.4f;
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

void Node::drawPlanet( const vector<gl::Texture> &planets )
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawPlanet( planets );
	}
}

void Node::drawClouds( const vector<gl::Texture> &clouds )
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawClouds( clouds );
	}
}

void Node::drawRings( const gl::Texture &tex, GLfloat *planetRingVerts, GLfloat *planetRingTexCoords, float camRingAlpha )
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawRings( tex, planetRingVerts, planetRingTexCoords, camRingAlpha );
	}
}

void Node::drawOrbitRing( float pinchAlphaOffset, GLfloat *ringVertsLowRes, GLfloat *ringVertsHighRes )
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawOrbitRing( pinchAlphaOffset, ringVertsLowRes, ringVertsHighRes );
	}
}

void Node::drawName( const CameraPersp &cam, float pinchAlphaOffset, float angle )
{	
	if( cam.worldToEyeDepth( mTransPos ) < 0 ){
		if( mIsPlaying || mIsSelected ){
			gl::color( ColorA( Color::white(), 1.0f ) );
		} else {
			gl::color( ColorA( COLOR_BRIGHT_BLUE, mZoomPer * pinchAlphaOffset ) );
		}

		if (mNameTex == NULL) {
			createNameTexture();
		}

		
		Vec2f pos1 = mScreenPos + Vec2f( mSphereScreenRadius * 0.275f, mSphereScreenRadius * 0.275f * 0.75f );
        Vec2f offset1( 10.0f, 7.5f );
        offset1.rotate( angle );
		Vec2f pos2 = pos1 + offset1;
        Vec2f offset2( 2.0f, -8.0f );
        offset2.rotate( angle );
		
		gl::pushModelView();
        
		gl::translate( pos2 + offset2 );
        gl::rotate( angle * 180.0f/M_PI );
		if( mIsPlaying ){
			float s = mZoomPer * 0.25f + 1.0f;
			gl::scale( Vec3f( s, s, 1.0f ) );
		}
		gl::draw( mNameTex, Vec2f::zero() );
		gl::popModelView();
		
		glDisable( GL_TEXTURE_2D );
		gl::color( ColorA( COLOR_BLUE, 0.4f * mZoomPer * pinchAlphaOffset ) );
		gl::drawLine( pos1, pos2 );
		
		// draw hit areas
		/*if( !mIsSelected ){
			gl::drawSolidRect( mHitArea );
			gl::drawSolidRect( mSphereHitArea );
		}*/
	}
	
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawName( cam, pinchAlphaOffset, angle );
	}
}

void Node::drawTouchHighlight()
{
	if( mIsHighlighted ){
		if( mIsTapped ){
			gl::color( ColorA( mColor, mHighlightStrength ) );
			Vec2f radius = Vec2f( mRadius * 25.0f, mRadius * 25.0f );
			
			gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
			mHighlightStrength -= ( mHighlightStrength - 0.0f ) * 0.15f;
		}
		
		if( mHighlightStrength < 0.01f ){
			mIsTapped = false;
		}

		for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
			(*nodeIt)->drawTouchHighlight();
		}
	}
}

void Node::checkForSphereIntersect( vector<Node*> &nodes, const Ray &ray, Matrix44f &mat )
{
	mSphere.setCenter( mat.transformPointAffine( mPos ) );

	if( mSphere.intersects( ray ) && mIsHighlighted && ! mIsSelected ){
		std::cout << "HIT FOUND" << std::endl;
		nodes.push_back( this );
	}
	
	vector<Node*>::iterator nodeIt;
	for( nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->checkForSphereIntersect( nodes, ray, mat );
	}
}

void Node::checkForNameTouch( vector<Node*> &nodes, const Vec2f &pos )
{
	bool didTouch = false;
	
	Vec2f p = mScreenPos + Vec2f( mSphereScreenRadius * 0.25f, 0.0f );
	float r = mSphereScreenRadius * 0.5f + 5.0f;
	
	mSphereHitArea	= Rectf( p.x - r, p.y - r, p.x + r, p.y + r );
	
	if( mNameTex != NULL )
		mHitArea		= Rectf( p.x - 15, p.y - 5, p.x + mNameTex.getWidth() + 20, p.y + mNameTex.getHeight() + 10 );
	
	
	if( mIsHighlighted && ! mIsSelected ){
		if( mSphereHitArea.contains( pos ) ) didTouch = true;

		if( !didTouch && mNameTex != NULL ){
			if( mHitArea.contains( pos ) ) didTouch = true;
		}
		
		if( didTouch ) nodes.push_back( this );
	}
	
	if( mIsHighlighted ){
		vector<Node*>::iterator nodeIt;
		for( nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
			(*nodeIt)->checkForNameTouch( nodes, pos );
		}
	}
}

void Node::select()
{
	mIsSelected = true;
}

void Node::deselect()
{
	// TODO: Instead of killing them right away, sentence them to die but only after
	// their gen is 1.0 greater than the current zoom level. 
	mIsSelected = false;
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		delete (*nodeIt);
	}
	mChildNodes.clear();
}


