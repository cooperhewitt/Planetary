#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;
using std::vector;

Particle::Particle()
{
}

Particle::Particle( int index, Vec3f pos, Vec3f vel, const Vec3f &bbRight, const Vec3f &bbUp )
{
	mIndex			= index;
	mColor			= ColorA( 1.0f, 1.0f, 1.0f, 1.0f );
	
	mLifespan       = pow( Rand::randFloat(), 2.0f );
	mLifespan		*= 100.0f;
	mIsDead			= false;
	mAngle			= Rand::randFloat( 6.2832f );
	mCosAngle		= (float)cos( mAngle );
	mSinAngle		= (float)sin( mAngle );
	
	setup( bbRight, bbUp );
}

void Particle::setup( const Vec3f &bbRight, const Vec3f &bbUp )
{
	mPos		= ( bbRight * mCosAngle + bbUp * mSinAngle ) * 0.375f;
	mPrevPos	= mPos;
	mVel		= mPos * Rand::randFloat( 0.0025f, 0.005f );
	mAcc		= Rand::randVec3f() * 0.01f;
	mAge		= 0;
	mAgePer		= 0.0f;
	mRadius		= 1.0f;//Rand::randFloat( 0.025f, 0.1f );
}

void Particle::update( float radius, const Vec3f &bbRight, const Vec3f &bbUp )
{
	mAgePer		= 1.0f - (float)mAge/(float)mLifespan;
	mPos		= ( bbRight * mCosAngle + bbUp * mSinAngle ) * ( radius + mAge * 0.0015f );
	mPrevPos	= mPos;
    mPos += mVel;
    mAge ++;
    if( mAge > mLifespan ){
        setup( bbRight, bbUp );		
    }
}

void Particle::draw()
{
    gl::drawLine( mPos, mPrevPos );
}
