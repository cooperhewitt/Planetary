/*
 *  NodeTrack.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "NodeTrack.h"
#include "cinder/Vector.h"

class NodeTrack;

class Orbiter
{
  public:
	Orbiter( NodeTrack *parent, int index );
	void update( const ci::Vec3f &pos );
	void draw( const ci::Matrix44f &mat, const ci::Vec3f &pos, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	
  private:
	NodeTrack *mParent;
	ci::Vec3f mPos;
	ci::Vec3f mVel;
	ci::Vec3f mAcc;
	ci::Vec3f mAxis;
	
	float mSpeed;
	float mOrbitRadius;
	float mRadius;
	float mIndex;
};