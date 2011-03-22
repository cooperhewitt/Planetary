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
	
	mLifespan       = Rand::randInt( 5, 20 );
	mIsDead			= false;
	
	setup();
}

void Dust::setup()
{
	Vec3f randVec = Rand::randVec3f();
	mPos		= randVec * 0.42f;
	mPrevPos	= mPos;
	mVel		= Rand::randVec3f() * 0.002f;
	mDecay		= 0.98f;
	mAge		= 0;
}

void Dust::update()
{
	mPrevPos = mPos;
    mPos += mVel;
    mAge ++;
    if( mAge > mLifespan ){
        setup();		
    }
}
