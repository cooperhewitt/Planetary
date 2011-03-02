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
	createNameTexture();
	
	mTransPos			= mPos;
	mSphere				= Sphere( mPos, mRadius * 2.0f );
	
	mStartAngle			= Rand::randFloat( TWO_PI );
	mOrbitAngle			= mStartAngle;
	mOrbitPeriod		= Rand::randFloat( 25.0f, 150.0f );
	mAngularVelocity	= 0.0f;
	mPercentPlayed		= 0.0f;
	mDistFromCamZAxis	= 1000.0f;
	mDistFromCamZAxisPer = 1.0f;
		
	mIsSelected			= false;
	mIsHighlighted		= false;
}

void Node::init()
{
	mGen				= 0;
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
	mOrbitRadiusDest	= Rand::randFloat( mParentNode->mRadius * 1.0f, mParentNode->mRadius * 2.0f );
	mOrbitPeriod		= Rand::randFloat( 25.0f, 150.0f );
}

void Node::createNameTexture()
{
	TextLayout layout;
	layout.setFont( mFont );
	layout.setColor( Color( 1.0f, 1.0f, 1.0f ) );
	layout.addLine( mName );
	mNameTex = gl::Texture( layout.render( true, false ) );
}

void Node::update( const CameraPersp &cam, const Matrix44f &mat, const Vec3f &bbRight, const Vec3f &bbUp )
{
	mOrbitRadius -= ( mOrbitRadius - mOrbitRadiusDest ) * 0.02f;
	mMatrix		= mat;
	mBbRight	= bbRight;
	mBbUp		= bbUp;
	mTransPos	= mMatrix * mPos;
	
	mDistFromCamZAxis = cam.worldToEyeDepth( mTransPos );
	mDistFromCamZAxisPer = constrain( mDistFromCamZAxis * -0.35f, 0.0f, 1.0f );
	
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->update( cam, mat, bbRight, bbUp );
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

void Node::drawName()
{	
	if( mIsHighlighted ){
		gl::color( Color( 1.0f, 1.0f, 1.0f ) );
		mNameTex.enableAndBind();
		Vec3f pos	= mTransPos + mBbRight * ( mNameTex.getWidth() * 0.075f + mRadius );
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
			gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
		} else {
			gl::color( ColorA( mColor, 0.5f ) );
		}
		
		Vec2f pos = cam.worldToScreen( mTransPos, app::getWindowWidth(), app::getWindowHeight() ) + Vec2f( mRadius * 4.0f, -mNameTex.getHeight() * 0.5f );
		if( G_LOCK_TO_LEFT_SIDE ) pos.x = app::getWindowWidth() - mNameTex.getWidth() - 10.0f;

		gl::draw( mNameTex, pos );
	}
	
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawOrthoName( cam );
	}
}

void Node::drawSphere()
{
	if( mIsHighlighted ){
		gl::color( ColorA( mGlowColor, 0.3f ) );
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::draw( mSphere );
		gl::popModelView();
		
		for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
			(*nodeIt)->drawSphere();
		}
	}
}

void Node::drawOrbitalRings()
{
}

void Node::checkForSphereIntersect( Node* &theNode, const Ray &ray, Matrix44f &mat )
{
	if( ! theNode ){
		mSphere.setCenter( mat.transformPointAffine( mPos ) );

		if( mSphere.intersects( ray ) && mIsHighlighted && ! mIsSelected ){
			theNode			= this;
			
		} else {
			vector<Node*>::iterator nodeIt;
			for( nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
				(*nodeIt)->checkForSphereIntersect( theNode, ray, mat );
			}
		}
	}
}

void Node::select()
{
	mIsSelected = true;
}

void Node::deselect()
{
	mIsSelected = false;
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		delete (*nodeIt);
	}
	mChildNodes.clear();
}


