#pragma once
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "Node.h"
#include <vector>

class Particle {
 public:
	Particle();
	Particle( int index, ci::Vec3f pos, ci::Vec3f vel );
	void setup();
	void pullToCenter( Node *trackNode );
	void update();
	void draw();
	void drawScreenspace( const ci::Vec3f &sUp, const ci::Vec3f &sRight );
	
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