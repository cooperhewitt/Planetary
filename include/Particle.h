#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "Node.h"
#include <vector>

class Particle {
 public:
	Particle();
	Particle( int index, ci::Vec3f pos, ci::Vec3f vel, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void setup( const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void update( float radius, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void draw();
	
	int			mIndex;
	ci::Vec3f	mPos, mPrevPos;
	ci::Vec3f	mVel;
	ci::Vec3f	mAcc;

	float		mAngle;
	float		mCosAngle, mSinAngle;
	ci::Color	mColor;
	float		mRadius, mRadiusDest;
	float		mDecay;
	int			mAge;
	float         mLifespan;
	float		mAgePer;
	bool		mIsDead;
};