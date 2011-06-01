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
	mLifespan       = Rand::randInt( 50, 100 );
	mIsDead			= false;
	
	setup( Vec3f( 0.0f, 0.0f, 0.0f ) );
}

void Dust::setup( const Vec3f &camEye )
{
	Vec3f randVec  = Rand::randVec3f();
	
	mPos		= randVec * 0.4f;
	
	mPrevPos	= mPos;
	mVel		= randVec * Rand::randFloat( 0.0005f, 0.00275f );
	mDecay		= 0.98f;
	mAge		= 0;
}

void Dust::update( const Vec3f &camEye )
{
	mPrevPos = mPos;
    mPos += mVel;
    mAge ++;
	mAgePer = sin( mAge/(float)mLifespan * M_PI );
    if( mAge > mLifespan ){
        setup( camEye );		
    }
}
