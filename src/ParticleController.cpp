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
    mParticleTexCoords	= NULL;
	mParticleColors		= NULL;
	
	mPrevTotalDustVertices = -1;
    mDustVerts			= NULL;
	mDustColors			= NULL;
	mBbRight			= Vec3f::xAxis();
	mBbUp				= Vec3f::yAxis();
}

void ParticleController::update( float radius, const Vec3f &bbRight, const Vec3f &bbUp )
{
	mBbRight = bbRight;
	mBbUp	 = bbUp;
	
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->update( radius, mBbRight, mBbUp );
	}
	
	bool isGalaxyDust = false;
	if( G_ZOOM < G_ARTIST_LEVEL - 0.5f )
		isGalaxyDust = true;
	
	for( list<Dust>::iterator d = mDusts.begin(); d != mDusts.end(); ++d ){
		d->update( isGalaxyDust );
	}
}


void ParticleController::buildParticleVertexArray()
{
	int vIndex = 0;
	int tIndex = 0;
	Vec3f lookVec = mBbRight.cross( mBbUp ) * 0.025f;
	
	mTotalParticleVertices	= G_NUM_PARTICLES * 6;
	
    if (mTotalParticleVertices != mPrevTotalParticleVertices) {
        if (mParticleVerts != NULL)		delete[] mParticleVerts; 
		if (mParticleTexCoords != NULL) delete[] mParticleTexCoords; 
		
        mParticleVerts			= new float[mTotalParticleVertices*3];
        mParticleTexCoords		= new float[mTotalParticleVertices*2];
		
        mPrevTotalParticleVertices = mTotalParticleVertices;
    }
	
	
	
	float u1				= 0.0f;
	float u2				= 1.0f;
	float v1				= 0.0f;
	float v2				= 1.0f;
	
	// TODO: figure out why we use inverted matrix * billboard vec
	
	for( list<Particle>::iterator it = mParticles.begin(); it != mParticles.end(); ++it ){
		Vec3f pos				= it->mPos + lookVec;
		float radius			= it->mRadius * it->mAgePer;
		
		Vec3f right				= mBbRight * radius;
		Vec3f up				= mBbUp * radius;
		
		
		Vec3f p1				= pos - right - up;
		Vec3f p2				= pos + right - up;
		Vec3f p3				= pos - right + up;
		Vec3f p4				= pos + right + up;
		
		mParticleVerts[vIndex++]		= p1.x;
		mParticleVerts[vIndex++]		= p1.y;
		mParticleVerts[vIndex++]		= p1.z;
		mParticleTexCoords[tIndex++]	= u1;
		mParticleTexCoords[tIndex++]	= v1;
		
		mParticleVerts[vIndex++]		= p2.x;
		mParticleVerts[vIndex++]		= p2.y;
		mParticleVerts[vIndex++]		= p2.z;
		mParticleTexCoords[tIndex++]	= u2;
		mParticleTexCoords[tIndex++]	= v1;
		
		mParticleVerts[vIndex++]		= p3.x;
		mParticleVerts[vIndex++]		= p3.y;
		mParticleVerts[vIndex++]		= p3.z;
		mParticleTexCoords[tIndex++]	= u1;
		mParticleTexCoords[tIndex++]	= v2;
		
		mParticleVerts[vIndex++]		= p2.x;
		mParticleVerts[vIndex++]		= p2.y;
		mParticleVerts[vIndex++]		= p2.z;
		mParticleTexCoords[tIndex++]	= u2;
		mParticleTexCoords[tIndex++]	= v1;
		
		mParticleVerts[vIndex++]		= p3.x;
		mParticleVerts[vIndex++]		= p3.y;
		mParticleVerts[vIndex++]		= p3.z;
		mParticleTexCoords[tIndex++]	= u1;
		mParticleTexCoords[tIndex++]	= v2;
		
		mParticleVerts[vIndex++]		= p4.x;
		mParticleVerts[vIndex++]		= p4.y;
		mParticleVerts[vIndex++]		= p4.z;
		mParticleTexCoords[tIndex++]	= u2;
		mParticleTexCoords[tIndex++]	= v2;
	}
}

void ParticleController::buildDustVertexArray( Node *node, float pinchAlphaPer, float dustAlpha )
{
	mTotalDustVertices	= G_NUM_DUSTS;
	
    if (mTotalDustVertices != mPrevTotalDustVertices) {
        if (mDustVerts != NULL)		delete[] mDustVerts;
		if( mDustColors != NULL)	delete[] mDustColors;
		
        mDustVerts = new float[mTotalDustVertices*3];
		mDustColors = new float[mTotalDustVertices*4];
		
        mPrevTotalDustVertices = mTotalDustVertices;
    }
	
	int vIndex	= 0;
	int cIndex	= 0;
	float alpha;
	Color col;
	
	if( G_ZOOM < G_ARTIST_LEVEL ){
		alpha	= 0.5f * dustAlpha;
		col		= COLOR_BLUE;
	} else {
		alpha	= dustAlpha * pinchAlphaPer;
		if( node )
			col	= node->mColor * 0.1f;
	}
	
	for( list<Dust>::iterator it = mDusts.begin(); it != mDusts.end(); ++it ){
		//Vec3f prev				= it->mPrevPos;
		Vec3f pos				= it->mPos;
		
		mDustVerts[vIndex++]	= pos.x;
		mDustVerts[vIndex++]	= pos.y;
		mDustVerts[vIndex++]	= pos.z;
		
		mDustColors[cIndex++]	= col.r;
		mDustColors[cIndex++]	= col.g;
		mDustColors[cIndex++]	= col.b;
		mDustColors[cIndex++]	= alpha;
	}
}
	

void ParticleController::drawParticleVertexArray( Node *node, const Matrix44f &mat )
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	
	glVertexPointer( 3, GL_FLOAT, 0, mParticleVerts );
	glTexCoordPointer( 2, GL_FLOAT, 0, mParticleTexCoords );
	
	gl::pushModelView();
	if( node ){
		gl::translate( node->mTransPos );
		gl::color( ColorA( node->mGlowColor, 0.5f ) );
	}
	
	gl::rotate( mat );
	glDrawArrays( GL_TRIANGLES, 0, mTotalParticleVertices );
	gl::popModelView();
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

void ParticleController::drawDustVertexArray( Node *node, const Matrix44f &mat )
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glVertexPointer( 3, GL_FLOAT, 0, mDustVerts );
	glColorPointer( 4, GL_FLOAT, 0, mDustColors );
	gl::pushModelView();
	if( node )
		gl::translate( node->mTransPos );
		
	gl::rotate( mat );
	glDrawArrays( GL_POINTS, 0, mTotalDustVertices );
	gl::popModelView();
	
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

