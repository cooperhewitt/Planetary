#include "Dust.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;
using std::vector;

Dust::Dust()
{
}

Dust::Dust( int index, Vec3f pos, Vec3f vel )
{
	mIndex			= index;
	mColor			= ColorA( 1.0f, 1.0f, 1.0f, 1.0f );
	
	mLifespan       = Rand::randInt( 25, 50 );
	mIsDead			= false;
	
	setup();
}

void Dust::setup()
{
	Vec3f randVec = Rand::randVec3f();
	mPos		= randVec * 0.5f;
	mPrevPos	= mPos;
	mVel		= randVec * 0.04f;
	mAcc		= Rand::randVec3f() * 0.01f;
	mDecay		= 0.98f;
	mAge		= 0;
	mAgePer		= 0.0f;
	mRadius		= Rand::randFloat( 0.025f, 0.25f );
}

void Dust::update()
{
	mAgePer = 1.0f - (float)mAge/(float)mLifespan;
	//mVel += mAcc;
	mPrevPos = mPos;
    mPos += mVel;// * mAgePer;// * velScale;
	// mVel *= mDecay;
	// mDecay -= ( mDecay - 0.99f ) * 0.1f;
	
    mAge ++;
    if( mAge > mLifespan ){
        setup();		
    }
}
