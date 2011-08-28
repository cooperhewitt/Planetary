#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "ParticleController.h"
#include "Globals.h"

using namespace ci;
using std::list;

ParticleController::ParticleController()
{
	mPrevTotalParticleVertices = -1;
    mParticleVerts		= NULL;
	
	mPrevTotalDustVertices = -1;
    mDustVerts			= NULL;

	mBbRight			= Vec3f::xAxis();
	mBbUp				= Vec3f::yAxis();
}

void ParticleController::update( const Vec3f &camEye, float radius, const Vec3f &bbRight, const Vec3f &bbUp )
{
	mBbRight = bbRight;
	mBbUp	 = bbUp;
	
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->update( radius, mBbRight, mBbUp );
	}
	
	bool isGalaxyDust = false;
	if( G_ZOOM < G_ARTIST_LEVEL - 0.5f ) { // pass in as arguments to avoid global dependencies
		isGalaxyDust = true;
    }
	
	for( list<Dust>::iterator d = mDusts.begin(); d != mDusts.end(); ++d ){
		d->update( camEye );
	}
}


void ParticleController::buildParticleVertexArray( float scaleOffset, Color c, float eclipseStrength )
{
//	Vec3f lookVec = mBbRight.cross( mBbUp ) * 0.025f;
	
	mTotalParticleVertices = mParticles.size() * 6;
	
    if (mTotalParticleVertices != mPrevTotalParticleVertices) {
        if( mParticleVerts != NULL ) {
            delete[] mParticleVerts; 
        }
        mParticleVerts = new ParticleVertex[mTotalParticleVertices];
        mPrevTotalParticleVertices = mTotalParticleVertices;
    }
	
	const float u1   = 0.0f;
	const float u2   = 1.0f;
	const float v1   = 0.0f;
	const float v2   = 1.0f;
    
	int vIndex = 0;
	
	for( list<Particle>::iterator it = mParticles.begin(); it != mParticles.end(); ++it ){
        
		Vec3f pos				= it->mPos;// + lookVec;
		float radius			= it->mRadius * ( 1.0f - it->mAgePer ) * scaleOffset;// * eclipseStrength;// * sin( it->mAgePer * M_PI );
		float alpha				= constrain(it->mAgePer * eclipseStrength, 0.0f, 1.0f);
		
//		Vec3f right				= mBbRight * radius * it->mQuat;
//		Vec3f up				= mBbUp * radius * it->mQuat;

		Vec3f right				= Vec3f::yAxis() * radius * it->mQuat;
		Vec3f up				= Vec3f::xAxis() * radius * it->mQuat;
		
		Vec3f p1				= pos - right - up;
		Vec3f p2				= pos + right - up;
		Vec3f p3				= pos - right + up;
		Vec3f p4				= pos + right + up;
        
        Vec4f col(c.r, c.g, c.b, alpha);
        
		mParticleVerts[vIndex].vertex  = p1;
		mParticleVerts[vIndex].texture = Vec2f(u1,v1);
		mParticleVerts[vIndex].color   = col;
        vIndex++;		

        mParticleVerts[vIndex].vertex  = p2;
		mParticleVerts[vIndex].texture = Vec2f(u2,v1);
		mParticleVerts[vIndex].color   = col;
        vIndex++;		

		mParticleVerts[vIndex].vertex  = p3;
		mParticleVerts[vIndex].texture = Vec2f(u1,v2);
		mParticleVerts[vIndex].color   = col;
        vIndex++;		

        mParticleVerts[vIndex].vertex  = p2;
		mParticleVerts[vIndex].texture = Vec2f(u2,v1);
		mParticleVerts[vIndex].color   = col;
        vIndex++;		

		mParticleVerts[vIndex].vertex  = p3;
		mParticleVerts[vIndex].texture = Vec2f(u1,v2);
		mParticleVerts[vIndex].color   = col;
        vIndex++;		

		mParticleVerts[vIndex].vertex  = p4;
		mParticleVerts[vIndex].texture = Vec2f(u2,v2);
		mParticleVerts[vIndex].color   = col;
        vIndex++;		
	}
}

void ParticleController::buildDustVertexArray( float scaleOffset, Node *node, float pinchAlphaPer, float dustAlpha )
{
	mTotalDustVertices = mDusts.size();
	
    if (mTotalDustVertices != mPrevTotalDustVertices) {
        if (mDustVerts != NULL) {
            delete[] mDustVerts;
        }
        mDustVerts = new DustVertex[mTotalDustVertices];
        mPrevTotalDustVertices = mTotalDustVertices;
    }
	
	int vIndex	= 0;
	float alpha = dustAlpha * pinchAlphaPer;
	Color col;
	
	if (node) {
		col	= node->mGlowColor;
    }

	for( list<Dust>::iterator it = mDusts.begin(); it != mDusts.end(); ++it ){        
		mDustVerts[vIndex].vertex = it->mPos; //0.7f
		mDustVerts[vIndex].color = Vec4f(col.r, col.g, col.b, alpha * it->mAgePer);
        vIndex++;
	}
}
	

void ParticleController::drawParticleVertexArray( Node *node, float multi )
{
	// PARTICLES
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	
	glVertexPointer( 3, GL_FLOAT, sizeof(ParticleVertex), mParticleVerts );
	glTexCoordPointer( 2, GL_FLOAT, sizeof(ParticleVertex), &mParticleVerts[0].texture );
	glColorPointer( 4, GL_FLOAT, sizeof(ParticleVertex), &mParticleVerts[0].color );	
	
	glPushMatrix();
	if( node ){
		gl::translate( node->mPos );
		float radius = node->mRadius * multi + 0.007f;
		gl::scale( Vec3f( radius, radius, radius ) );
	}
	glDrawArrays( GL_TRIANGLES, 0, mTotalParticleVertices );
	glPopMatrix();
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
}

void ParticleController::drawDustVertexArray( Node *node, float multi )
{
	// DUST
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glVertexPointer( 3, GL_FLOAT, sizeof(DustVertex), mDustVerts );
	glColorPointer( 4, GL_FLOAT, sizeof(DustVertex), &mDustVerts[0].color );

	glPushMatrix();
	if( node ) {
		gl::translate( node->mPos );
		float radius = node->mRadius * multi + 0.007f;
		gl::scale( Vec3f( radius, radius, radius ) );
    }
	glDrawArrays( GL_POINTS, 0, mTotalDustVertices );
	glPopMatrix();
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
}

void ParticleController::addDusts( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		Vec3f pos = Rand::randVec3f() * Rand::randFloat( 100.0f, 200.0f );
		Vec3f vel = Rand::randVec3f();
		
		mDusts.push_back( Dust( i, pos, vel ) );
	}
}

void ParticleController::addParticles( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		Vec3f pos = Rand::randVec3f() * Rand::randFloat( 100.0f, 200.0f );
		Vec3f vel = Rand::randVec3f();
		
		mParticles.push_back( Particle( i, pos, vel, mBbRight, mBbUp ) );
	}
}

void ParticleController::removeParticles( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		mParticles.pop_back();
	}
}

