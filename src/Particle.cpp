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
	
	mIsDead			= false;
	mAngle			= Rand::randFloat( 6.2832f );
	mCosAngle		= (float)cos( mAngle );
	mSinAngle		= (float)sin( mAngle );
	
	setup( bbRight, bbUp );
}

void Particle::setup( const Vec3f &bbRight, const Vec3f &bbUp )
{
	mLifespan       = pow( Rand::randFloat( 0.5f, 1.0f ), 2.0f );
	mLifespan		*= 200.0f;
	
	mPos		= ( bbRight * mCosAngle + bbUp * mSinAngle ) * 0.35f;
	mPrevPos	= mPos;
	mVel		= mPos * Rand::randFloat( 0.0015f, 0.0035f );
	mAcc		= Rand::randVec3f() * 0.01f;
	mAge		= 0;
	mAgePer		= 0.0f;
	mRadius		= Rand::randFloat( 1.0f, 5.0f );
}

void Particle::update( float radius, const Vec3f &bbRight, const Vec3f &bbUp )
{
	mAgePer		= 1.0f - (float)mAge/(float)mLifespan;
	mPos		= ( bbRight * mCosAngle + bbUp * mSinAngle ) * ( radius + mAge * 0.00075f );
	//mPrevPos	= mPos;
   // mPos += mVel;
    mAge ++;
    if( mAge > mLifespan ){
        setup( bbRight, bbUp );		
    }
}

void Particle::draw()
{
    gl::drawLine( mPos, mPrevPos );
}
