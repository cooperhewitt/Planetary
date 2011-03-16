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

	// TODO: restore this behavior
	//if( mName.size() == 0 ){
	//	mName = "untitled";
	//}
	
	//createNameTexture();
	
    mZoomPer            = 0.0f;
    
	mScreenPos			= Vec2f::zero();
	mEclipsePer			= 1.0f;
	mTransPos			= mPos;

	mStartAngle			= Rand::randFloat( TWO_PI );
	mOrbitAngle			= mStartAngle;
	mOrbitPeriod		= Rand::randFloat( 25.0f, 150.0f );
	mOrbitRadius		= 0.01f;
	mAngularVelocity	= 0.0f;
	mPercentPlayed		= 0.0f;
	mDistFromCamZAxis	= 1000.0f;
	mDistFromCamZAxisPer = 1.0f;
	mPlanetTexIndex		= 0;
    
    mSphereRes          = 12;
    mSphereResInt       = 12;
		
	mIsSelected			= false;
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
	mRadiusDest			= mParentNode->mRadiusDest * 0.02f;
	mRadius				= mRadiusDest;
	mPos				= mParentNode->mPos;
	mPrevPos			= mParentNode->mPos;
	mVel				= mParentNode->mVel;
	mOrbitPeriod		= Rand::randFloat( 75.0f, 150.0f );
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

    if( mIsSelected ){
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
        if( mIsSelected ){
            mSphereRes		-= ( mSphereRes - 16 ) * 0.1f;
            mCamDistAlpha	-= ( mCamDistAlpha - 1.0f ) * 0.1f;
        } else {
            mSphereRes		-= ( mSphereRes - 8 ) * 0.1f;
            mCamDistAlpha	-= ( mCamDistAlpha - 0.0f ) * 0.1f;
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

void Node::drawStar()
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawStar();
	}
}

void Node::drawStarGlow()
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawStarGlow();
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

void Node::drawRings( const gl::Texture &tex )
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawRings( tex );
	}
}

void Node::drawOrbitRing( NodeTrack *playingNode, GLfloat *ringVertsLowRes, GLfloat *ringVertsHighRes )
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawOrbitRing( playingNode, ringVertsLowRes, ringVertsHighRes );
	}
}

void Node::drawName( const CameraPersp &cam, float pinchAlphaOffset )
{	
	if( cam.worldToEyeDepth( mTransPos ) < 0 ){
		if( mIsSelected ){
			gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
		} else {
			gl::color( ColorA( mColor, mZoomPer * pinchAlphaOffset ) );
		}

		if (mNameTex == NULL) {
			createNameTexture();
		}
		
//		Vec2f pos1 = mScreenPos + Vec2f( mSphereScreenRadius * 0.265f, mSphereScreenRadius * -0.265f );
//		Vec2f pos2 = mScreenPos + Vec2f( mSphereScreenRadius * 0.5f, mSphereScreenRadius * -0.5f );
		
		Vec2f pos1 = mScreenPos + Vec2f( mSphereScreenRadius * 0.265f, 0.0f );
		Vec2f pos2 = pos1 + Vec2f( 20.0f, 0.0f );
		
		gl::pushModelView();
		gl::translate( pos2 + Vec2f( 2.0f, -10.0f ) );
		if( mIsSelected ){
			float s = mZoomPer * 0.5f + 1.0f;
			gl::scale( Vec3f( s, s, 1.0f ) );
		}
		gl::draw( mNameTex, Vec2f::zero() );
		gl::popModelView();
		
		gl::color( ColorA( 0.1f, 0.2f, 0.5f, 0.2f * mZoomPer * pinchAlphaOffset ) );
		gl::drawLine( pos1, pos2 );
		
		/*
		if( mIsSelected ){
			gl::color( ColorA( 0.1f, 0.2f, 0.5f, 0.65f * pinchAlphaOffset ) );
			if( mGen == G_TRACK_LEVEL ){
				gl::drawStrokedCircle( mScreenPos, mSphereScreenRadius * 0.375f );
			} else if( mGen == G_ALBUM_LEVEL ){
				gl::drawStrokedCircle( mScreenPos, mSphereScreenRadius * 0.675f );
			}
		}
		*/
	}
	
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawName( cam, pinchAlphaOffset );
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
	if (mNameTex != NULL) {
		
		Vec2f p = mScreenPos + Vec2f( mSphereScreenRadius * 0.6f, mSphereScreenRadius * -0.6f );
		
		Rectf r = Rectf( p.x - 50, p.y - 35, p.x + mNameTex.getWidth() + 30, p.y + mNameTex.getHeight() + -5 );
		
		if( r.contains( pos ) && mIsHighlighted && ! mIsSelected ){
			std::cout << "HIT FOUND" << std::endl;
			nodes.push_back( this );
		}
		
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


