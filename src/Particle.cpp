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
	mRadius			= Rand::randFloat( 0.2f, 0.3f );
	if( Rand::randFloat() < 0.01f ){
		mLifespan	*= Rand::randFloat( 100.0f, 150.0f );
		mRadius		*= 2.0f;
	} else {
		mLifespan	*= Rand::randFloat( 100.0f, 400.0f );
	}
	
	mPos		= Rand::randVec3f() * 0.95f;
	mVel		= mPos * ( Rand::randFloat( 0.0075f, 0.02f ) * ( mRadius/5.0f ) );
	mAcc		= Rand::randVec3f() * 0.01f;
	mAge		= 0;
	mAgePer		= 0.0f;
	mQuat		= Quatf( mPos, Rand::randFloat( M_PI * 2.0f ) );
}

void Particle::update( float radius, const Vec3f &bbRight, const Vec3f &bbUp )
{
	mAgePer		= 1.0f - sqrt( (float)mAge/(float)mLifespan );

//	mPos		= ( bbRight * mCosAngle + bbUp * mSinAngle ) * ( radius + mAge * 0.00015f );
	//mPrevPos	= mPos;
   // mPos += mVel;
    mAge ++;
    if( mAge > mLifespan ){
        setup( bbRight, bbUp );		
    }
}

