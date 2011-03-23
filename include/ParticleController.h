#pragma once
#include "Particle.h"
#include "Dust.h"
#include "Node.h"
#include <list>

class ParticleController {
 public:
	ParticleController();
	void update();
	void buildParticleVertexArray( const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void buildDustVertexArray( Node *node, float pinchAlphaOffset );
	void drawParticleVertexArray( Node *node, const ci::Matrix44f &mat );
	void drawDustVertexArray( Node *node, const ci::Matrix44f &mat );
	void addParticles( int amt );
	void removeParticles( int amt );
	void addDusts( int amt );
	
	std::list<Particle>	mParticles;
	int mNumParticles;
	int mTotalParticleVertices;
    int mPrevTotalParticleVertices; // so we only recreate frames
	GLfloat *mParticleVerts;
	GLfloat *mParticleTexCoords;
	GLfloat *mParticleColors;
	
	
	std::list<Dust> mDusts;
	int mNumDusts;
	int mTotalDustVertices;
    int mPrevTotalDustVertices; // so we only recreate frames
	GLfloat *mDustVerts;
	GLfloat *mDustColors;
	
	
};