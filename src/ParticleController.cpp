#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "ParticleController.h"
#include "Globals.h"

using namespace ci;
using std::list;

ParticleController::ParticleController()
{
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

