#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;
using std::vector;

Particle::Particle()
{
}

Particle::Particle( int index, Vec3f pos, Vec3f vel )
{
	mIndex			= index;
	mPos			= pos;
	mVel			= vel;
	mVelNormal		= mVel;
	mAcc			= Vec3f::zero();
	
	mMaxSpeed		= Rand::randFloat( 2.5f, 4.0f );
	mMaxSpeedSqrd	= mMaxSpeed * mMaxSpeed;
	mMinSpeed		= Rand::randFloat( 1.0f, 1.5f );
	mMinSpeedSqrd	= mMinSpeed * mMinSpeed;
	mColor			= ColorA( 1.0f, 1.0f, 1.0f, 1.0f );
		
	mDecay			= 0.0f;
	mRadius			= 1.0f;
	mLength			= 5.0f;
	
	mAge			= 0;
	mLifespan       = Rand::randInt( 5, 20 );
	mIsDead			= false;
}


void Particle::update( Node *node )
{
    mPos += mVel;
   // mVel *= mDecay;
   // mDecay -= ( mDecay - 0.99f ) * 0.1f;

    mAge ++;
    if( mAge > mLifespan && node ){
        mAge = 0;
        mVel = node->mVel;
        mPos = node->mTransPos + mVel + Rand::randVec3f() * Rand::randFloat( 0.001f, 0.002f );
    }
}

void Particle::limitSpeed()
{
	float maxSpeed = mMaxSpeed;// + mCrowdFactor;
	float maxSpeedSqrd = maxSpeed * maxSpeed;
	
	float vLengthSqrd = mVel.lengthSquared();
	if( vLengthSqrd > maxSpeedSqrd ){
		mVel = mVelNormal * maxSpeed;
		
	}/* else if( vLengthSqrd < mMinSpeedSqrd ){
		mVel = mVelNormal * mMinSpeed;
	}*/
}

void Particle::draw()
{
    gl::drawLine( mPos, mPos - mVel );
}

void Particle::drawScreenSpace()
{
	gl::drawSolidRect( Rectf( mPos.x - 10.01f, mPos.y - 10.01f, mPos.x + 10.01f, mPos.y + 10.01f ) );
}

void Particle::drawLines()
{
    gl::color( Color( CM_HSV, Rand::randFloat(), 0.5f, 1.0f ) );
    gl::drawLine( mPos, mPos - mVel * 3.0f );
}

