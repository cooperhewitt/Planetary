#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "Node.h"
#include <vector>

class Particle {
 public:
	Particle();
	Particle( int index, ci::Vec3f pos, ci::Vec3f vel );
	void update( Node *node );
	void draw();
    void drawScreenSpace();
	void drawLines();
	void limitSpeed();

	int			mIndex;
	ci::Vec3f	mPos;
	ci::Vec3f	mVel;
	ci::Vec3f	mVelNormal;
	ci::Vec3f	mAcc;

	ci::Color	mColor;
	
	float		mDecay;
	float		mRadius;
	float		mLength;
	float		mMaxSpeed, mMaxSpeedSqrd;
	float		mMinSpeed, mMinSpeedSqrd;
	int			mAge;
	int         mLifespan;
	bool		mIsDead;
};