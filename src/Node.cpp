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
	
	mSphere				= Sphere( mPos, mRadius * 2.0f );
	mIsSelected			= false;
	mIsHighlighted		= false;
}

void Node::init()
{
	mGen				= 0;
	mRadius				= 2.0f;
	mPos				= Rand::randVec3f() * Rand::randFloat( 50.0f, 300.0f );
	mVel				= Vec3f::zero();
}

void Node::initWithParent()
{
	mGen				= mParentNode->mGen + 1;
	mRadius				= mParentNode->mRadius * 0.05f;
	mPos				= mParentNode->mPos;
	mVel				= mParentNode->mVel;
}

void Node::createNameTexture()
{
	TextLayout layout;
	layout.setFont( mFont );
	layout.setColor( Color( 1.0f, 1.0f, 1.0f ) );
	layout.addLine( mName );
	mNameTex = gl::Texture( layout.render( true, false ) );
}

void Node::update( const Matrix44f &mat, const Vec3f &bbRight, const Vec3f &bbUp )
{	
	mMatrix		= mat;
	mBbRight	= bbRight;
	mBbUp		= bbUp;
	mTransPos	= mMatrix * mPos;
	
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->update( mat, bbRight, bbUp );
	}
}

void Node::draw()
{
	if( mIsSelected ){
		gl::color( Color( 1.0f, 0.0f, 0.0f ) );
	} else {
		gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	}
	gl::pushModelView();
	gl::translate( mTransPos );
	gl::drawBillboard( Vec3f::zero(), Vec2f( mRadius, mRadius ), 0.0f, mBbRight, mBbUp );
	gl::popModelView();
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
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	gl::pushModelView();
	gl::translate( mTransPos );
	mNameTex.enableAndBind();
	gl::drawBillboard( mBbRight * ( mNameTex.getWidth() * 0.075f + mRadius ), Vec2f( mNameTex.getWidth(), mNameTex.getHeight() ) * 0.15f, 0.0f, mBbRight, mBbUp );
	gl::popModelView();
}

void Node::checkForSphereIntersect( Node* &theNode, const Ray &ray, Matrix44f &mat )
{
	if( ! theNode ){
		mSphere.setCenter( mat.transformPointAffine( mPos ) );

		if( mSphere.intersects( ray ) && mIsHighlighted ){
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


