#pragma once
#include "Particle.h"
#include "cinder/Quaternion.h"
#include "Node.h"
#include <list>

class ParticleController {
 public:
	ParticleController();
	void pullToCenter( Node *node );
	void update();
	void draw( Node *node, const ci::Matrix44f &mat );
	void drawScreenspace( Node *node, const ci::Matrix44f &mat, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void addParticles( int amt );
	void removeParticles( int amt );
	
	std::list<Particle>	mParticles;
	int mNumParticles;
};