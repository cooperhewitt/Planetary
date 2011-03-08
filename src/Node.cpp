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

Node::Node( Node *parent, int index, const Font &font, std::string name )
	: mParentNode( parent ), mIndex( index ), mFont( font ), mName( name )
{
	if( mParentNode ){
		initWithParent();
	} else {
		init();
	}
	
	if( mName.size() == 0 ){
		mName = "untitled";
	}
	
	
	createNameTexture();
	
	mTransPos			= mPos;
	mCamZVel			= 0.0f;
	mStartAngle			= Rand::randFloat( TWO_PI );
	mOrbitAngle			= mStartAngle;
	mOrbitPeriod		= Rand::randFloat( 25.0f, 150.0f );
	mOrbitRadius		= 0.01f;
	mAngularVelocity	= 0.0f;
	mPercentPlayed		= 0.0f;
	mDistFromCamZAxis	= 1000.0f;
	mDistFromCamZAxisPer = 1.0f;
	mPlanetTexIndex		= 0;
		
	mIsSelected			= false;
	mIsHighlighted		= false;
}

void Node::init()
{
	mGen				= G_ARTIST_LEVEL;
	mRadius				= 2.0f;
	mPos				= Rand::randVec3f();
	mPosPrev			= mPos;
	mVel				= Vec3f::zero();
	mOrbitRadiusDest	= 0.0f;
	mOrbitPeriod		= 0.0f;
}

void Node::initWithParent()
{
	mGen				= mParentNode->mGen + 1;
	mRadius				= mParentNode->mRadius * 0.05f;
	mPos				= mParentNode->mPos;
	mPosPrev			= mParentNode->mPos;
	mVel				= mParentNode->mVel;
	mOrbitPeriod		= Rand::randFloat( 25.0f, 150.0f );
}

void Node::setIPodPlayer( ci::ipod::Player *player )
{
	mPlayer = player;
}

void Node::createNameTexture()
{
	TextLayout layout;
	layout.setFont( mFont );
	layout.setColor( Color( 1.0f, 1.0f, 1.0f ) );
	layout.addLine( mName );
	Surface8u nameSurface	= Surface8u( layout.render( true, false ) );
	mNameTex				= gl::Texture( nameSurface );

	if( mGen == G_TRACK_LEVEL ){
		Surface8u planetSurface = Surface( 512, 256, true, SurfaceChannelOrder::RGBA );
		Vec2i offset = Vec2i( 60, 128 - mNameTex.getHeight() * 0.5f );
		planetSurface.copyFrom( nameSurface, nameSurface.getBounds(), offset );
	
		mPlanetTex = gl::Texture( planetSurface );
	}
}

void Node::update( const Matrix44f &mat, const Vec3f &bbRight, const Vec3f &bbUp )
{
	mOrbitRadius -= ( mOrbitRadius - mOrbitRadiusDest ) * 0.02f;
	mMatrix		= mat;
	mBbRight	= bbRight;
	mBbUp		= bbUp;
	mTransPos	= mMatrix * mPos;
	mSphere.setCenter( mTransPos );
	mHitSphere.setCenter( mTransPos );
		
	mZoomPer	= constrain( 1.0f - abs( G_ZOOM-mGen+1.0f ), 0.0f, 0.75f );
	mZoomPer = pow( mZoomPer, 3.0f );
	if( mGen == G_TRACK_LEVEL && mIsSelected ){
		mZoomPer = 1.0f - mZoomPer;
	} else if( mIsSelected ){
		mZoomPer = 1.0f;
	}

	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->update( mat, bbRight, bbUp );
	}
}

void Node::updateGraphics( const CameraPersp &cam )
{
	if( mIsHighlighted ){
		mPrevDistFromCamZAxis	= mDistFromCamZAxis;
		mDistFromCamZAxis		= cam.worldToEyeDepth( mTransPos );
		mCamZVel				= mDistFromCamZAxis - mPrevDistFromCamZAxis;
		mDistFromCamZAxisPer = constrain( mDistFromCamZAxis * -0.35f, 0.0f, 1.0f );
		mSphereScreenRadius = cam.getScreenRadius( mSphere, app::getWindowWidth(), app::getWindowHeight() ) * 0.4f;
	}
	
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->updateGraphics( cam );
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


// DEPRECATED
void Node::drawName()
{	
	
	if( mIsHighlighted ){
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, mZoomPer ) );
		mNameTex.enableAndBind();
		Vec3f pos	= mTransPos + mBbRight * ( mNameTex.getWidth() * 0.075f );
		Vec2f size	= Vec2f( mNameTex.getWidth(), mNameTex.getHeight() );
		gl::drawBillboard( pos, size, 0.0f, mBbRight, mBbUp );
		for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
			(*nodeIt)->drawName();
		}
	}
}

void Node::drawOrthoName( const CameraPersp &cam )
{	
	if( cam.worldToEyeDepth( mTransPos ) < 0 ){
		if( mIsSelected ){
			gl::color( ColorA( 1.0f, 1.0f, 1.0f, mZoomPer ) );
		} else {
			gl::color( ColorA( mColor, mZoomPer ) );
		}

		
		Vec2f offset = Vec2f( mSphereScreenRadius * 0.4f, -mNameTex.getHeight() * 0.5f );
		Vec2f pos = cam.worldToScreen( mTransPos, app::getWindowWidth(), app::getWindowHeight() ) + offset;

		gl::draw( mNameTex, pos );
	}
	
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawOrthoName( cam );
	}
}

void Node::drawSphere()
{
	if( mIsHighlighted ){
		gl::color( ColorA( mGlowColor, 0.05f ) );
		gl::draw( mHitSphere, 16 );
		
		for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
			(*nodeIt)->drawSphere();
		}
	}
}

void Node::drawOrbitalRings()
{
}

void Node::drawPlanet( Matrix44f accelMatrix, std::vector< gl::Texture*> texs )
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawPlanet( accelMatrix, texs );
	}
}

void Node::drawRings( gl::Texture *tex )
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawRings( tex );
	}
}

void Node::drawAtmosphere()
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawAtmosphere();
	}
}

void Node::checkForSphereIntersect( vector<Node*> &nodes, const Ray &ray, Matrix44f &mat )
{
	mHitSphere.setCenter( mat.transformPointAffine( mPos ) );

	if( mHitSphere.intersects( ray ) && mIsHighlighted && ! mIsSelected ){
		std::cout << "HIT FOUND" << std::endl;
		nodes.push_back( this );
		
	}
	
	vector<Node*>::iterator nodeIt;
	for( nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->checkForSphereIntersect( nodes, ray, mat );
	}
}

/*
void Node::checkForSphereIntersect( Node* &theNode, const Ray &ray, Matrix44f &mat )
{
	if( ! theNode ){
		mHitSphere.setCenter( mat.transformPointAffine( mPos ) );

		if( mHitSphere.intersects( ray ) && mIsHighlighted && ! mIsSelected ){
			theNode			= this;
			
		} else {
			vector<Node*>::iterator nodeIt;
			for( nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
				(*nodeIt)->checkForSphereIntersect( theNode, ray, mat );
			}
		}
	}
}
*/

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


