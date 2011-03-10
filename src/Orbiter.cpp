/*
 *  NodeTrack.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "cinder/app/AppBasic.h"
#include "Orbiter.h"
#include "cinder/Text.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "Globals.h"
#include "cinder/Quaternion.h"


using namespace ci;
using namespace ci::ipod;
using namespace std;

Orbiter::Orbiter( NodeTrack *parent, int index )
{	
	mParent			= parent;
	
	mPos			= mParent->mPos;
	mTailLength		= 5;
	for( int i=0; i<mTailLength; i++ ){
		mTailPos.push_back( mPos );
	}
	mVel			= mParent->mVel;
	mAcc			= Vec3f::zero();
	mAxis			= Rand::randVec3f() * 0.0001f;
	mIndex			= index;
	
	mSpeed			= 0.1f;
	mOrbitRadius	= Rand::randFloat( 0.1f, 0.2f );
	mRadius			= 0.0001f;
}

void Orbiter::update( const Matrix44f &mat, const Vec3f &pos )
{	
	Vec3f transAxis = mat * mAxis;
	float per = 0.0f;
	for( int i=mTailLength-1; i>0; i-- ){
		per = 1.0f - (float)i/(float)(mTailLength-1);
		mTailPos[i] = mTailPos[i-1] + mParent->mVel + transAxis * per;
	}
	mPos = pos + transAxis;
	mTailPos[0] = mPos;
}


void Orbiter::draw( const Matrix44f &mat, const Vec3f &pos, const Vec3f &bbRight, const Vec3f &bbUp )
{
	float per = 0.0f;
	int i = 0;
	for( vector<Vec3f>::iterator it = mTailPos.begin(); it != mTailPos.end(); ++it ){
		per = 1.0f - (float)i/(float)(mTailLength-1);
		//gl::drawBillboard( *it, Vec2f( mRadius, mRadius ) * 0.002f, 0.0f, bbRight, bbUp );
		gl::pushModelView();
		gl::translate( *it );
		gl::rotate( mat );
		gl::drawSphere( Vec3f::zero(), mRadius * per );
		gl::popModelView();
		i ++;
	}
	
}


