#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "ParticleController.h"
#include "Globals.h"

using namespace ci;
using std::list;

ParticleController::ParticleController()
{
	mPrevTotalVertices = -1;
    mVerts		= NULL;
    mTexCoords	= NULL;
	mColors		= NULL;
}



void ParticleController::pullToCenter( Node *node )
{
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->pullToCenter( node );
	}
}

void ParticleController::update()
{
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ){
		if( p->mIsDead ){
			p = mParticles.erase( p );
		} else {
			p->update();
			++p;
		}
	}
}


void ParticleController::buildVertexArray( const Vec3f &bbRight, const Vec3f &bbUp )
{
	mTotalVertices	= G_NUM_PARTICLES * 6;	// 6 = 2 triangles per quad
	
    if (mTotalVertices != mPrevTotalVertices) {
        if (mVerts != NULL)		delete[] mVerts; 
		if (mTexCoords != NULL) delete[] mTexCoords; 
		
        mVerts			= new float[mTotalVertices*3];
        mTexCoords		= new float[mTotalVertices*2];
		
        mPrevTotalVertices = mTotalVertices;
    }
	
	int vIndex = 0;
	int tIndex = 0;
	
	float u1				= 0.0f;
	float u2				= 1.0f;
	float v1				= 0.0f;
	float v2				= 1.0f;
	
	// TODO: figure out why we use inverted matrix * billboard vec
	
	for( list<Particle>::iterator it = mParticles.begin(); it != mParticles.end(); ++it ){
		Vec3f pos				= it->mPos;
		float radius			= it->mRadius * 0.5f * sin( it->mAgePer * M_PI );
		
		Vec3f right				= bbRight * radius;
		Vec3f up				= bbUp * radius;
		
		Vec3f p1				= pos - right - up;
		Vec3f p2				= pos + right - up;
		Vec3f p3				= pos - right + up;
		Vec3f p4				= pos + right + up;
		
		mVerts[vIndex++]		= p1.x;
		mVerts[vIndex++]		= p1.y;
		mVerts[vIndex++]		= p1.z;
		mTexCoords[tIndex++]	= u1;
		mTexCoords[tIndex++]	= v1;
		
		mVerts[vIndex++]		= p2.x;
		mVerts[vIndex++]		= p2.y;
		mVerts[vIndex++]		= p2.z;
		mTexCoords[tIndex++]	= u2;
		mTexCoords[tIndex++]	= v1;
		
		mVerts[vIndex++]		= p3.x;
		mVerts[vIndex++]		= p3.y;
		mVerts[vIndex++]		= p3.z;
		mTexCoords[tIndex++]	= u1;
		mTexCoords[tIndex++]	= v2;
		
		mVerts[vIndex++]		= p2.x;
		mVerts[vIndex++]		= p2.y;
		mVerts[vIndex++]		= p2.z;
		mTexCoords[tIndex++]	= u2;
		mTexCoords[tIndex++]	= v1;
		
		mVerts[vIndex++]		= p3.x;
		mVerts[vIndex++]		= p3.y;
		mVerts[vIndex++]		= p3.z;
		mTexCoords[tIndex++]	= u1;
		mTexCoords[tIndex++]	= v2;
		
		mVerts[vIndex++]		= p4.x;
		mVerts[vIndex++]		= p4.y;
		mVerts[vIndex++]		= p4.z;
		mTexCoords[tIndex++]	= u2;
		mTexCoords[tIndex++]	= v2;
	}
}

void ParticleController::drawVertexArray( Node *node, const Matrix44f &mat )
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	
	glVertexPointer( 3, GL_FLOAT, 0, mVerts );
	glTexCoordPointer( 2, GL_FLOAT, 0, mTexCoords );
	
	gl::pushModelView();
	if( node ){
		gl::translate( node->mTransPos );
		gl::color( ColorA( node->mGlowColor, 0.35f ) );
	}
	
	gl::rotate( mat );
	glDrawArrays( GL_TRIANGLES, 0, mTotalVertices );
	gl::popModelView();
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}


void ParticleController::draw( Node *node, const Matrix44f &mat )
{	
	gl::pushModelView();
	if( node ){
		gl::color( ColorA( node->mGlowColor, 0.075f ) ); // 0.2f
		gl::translate( node->mTransPos );
		gl::rotate( mat );
	}
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->draw();
	}
	gl::popModelView();
}

void ParticleController::drawScreenspace( Node *node, const Matrix44f &mat, const Vec3f &bbRight, const Vec3f &bbUp )
{	
	gl::pushModelView();
	if( node ){
		gl::color( ColorA( node->mGlowColor, 0.35f ) ); // 0.2f
		gl::translate( node->mTransPos );
	}
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->drawScreenspace( mat, bbRight, bbUp );
	}
	gl::popModelView();
}

void ParticleController::addParticles( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		Vec3f pos = Rand::randVec3f() * Rand::randFloat( 100.0f, 200.0f );
		Vec3f vel = Rand::randVec3f();
		
		mParticles.push_back( Particle( i, pos, vel ) );
	}
}

void ParticleController::removeParticles( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		mParticles.pop_back();
	}
}

