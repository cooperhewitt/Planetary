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
	mLifespan       = Rand::randInt( 20, 50 );
	mIsDead			= false;
	
	setup();
}

void Dust::setup()
{
	Vec2f randVec2 = Rand::randVec2f() * Rand::randFloat( 0.5f, 1.5f );
	Vec3f randVec = Vec3f( randVec2, 0.0f );
	mPos		= randVec * 0.1f;
	mPrevPos	= mPos;
	mVel		= Vec3f( -randVec2.y, randVec2.x, 0.0f ) * 0.00005f;
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
