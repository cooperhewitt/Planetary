#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "Node.h"
#include <vector>

class Dust {
public:
	Dust();
	Dust( int index, ci::Vec3f pos, ci::Vec3f vel );
	void setup( const ci::Vec3f &camEye );
	void update( const ci::Vec3f &camEye );
	
	int			mIndex;
	ci::Vec3f	mPos, mPrevPos;
	ci::Vec3f	mVel;
	ci::Vec3f	mAcc;
	
	ci::Color	mColor;
	float		mRadius;
	
	float		mDecay;
	int			mAge;
	int         mLifespan;
	float		mAgePer;
	bool		mIsDead;
};