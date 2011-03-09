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
	mVel			= mParent->mVel;
	mAcc			= Vec3f::zero();
	mAxis			= Rand::randVec3f() * 0.0042f;
	mIndex			= index;
	
	mSpeed			= 0.1f;
	mOrbitRadius	= Rand::randFloat( 0.1f, 0.2f );
	mRadius			= 0.2f;
}

void Orbiter::update( const Vec3f &pos )
{	

}


void Orbiter::draw( const Matrix44f &mat, const Vec3f &pos, const Vec3f &bbRight, const Vec3f &bbUp )
{
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	gl::drawBillboard( mAxis, Vec2f( mRadius, mRadius ) * 0.002f, 0.0f, bbRight, bbUp );
}


