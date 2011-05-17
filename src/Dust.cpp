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
	mLifespan       = 2;//Rand::randInt( 35, 230 );
	mIsDead			= false;
	
	setup( true );
}

void Dust::setup( bool isGalaxyDust )
{
	Vec2f randVec2;
	Vec3f randVec;
	
	if( isGalaxyDust ){
		randVec2	= Rand::randVec2f() * Rand::randFloat( 0.25f, 1.75f );
		randVec		= Vec3f( randVec2.x, 0.0f, randVec2.y );
		mPos		= randVec * 65.0f;
	} else {
		randVec2	= Rand::randVec2f() * Rand::randFloat( 0.5f, 1.75f );
		randVec		= Vec3f( randVec2.x, 0.0f, randVec2.y );
		mPos		= randVec * 0.1f;
	}
	
	
	mPrevPos	= mPos;
	mVel		= Vec3f( -randVec2.y, 0.0f, randVec2.x ) * 0.00005f;
	mDecay		= 0.98f;
	mAge		= 0;
}

void Dust::update( bool isGalaxyDust )
{
	mPrevPos = mPos;
    mPos += mVel;
    mAge ++;
    if( mAge > mLifespan ){
        setup( isGalaxyDust );		
    }
}
