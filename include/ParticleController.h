#pragma once
#include "Particle.h"
#include "cinder/Quaternion.h"
#include "Node.h"
#include <list>

class ParticleController {
 public:
	ParticleController();
	void update( Node *node );
	void draw();
	void drawLines();
	void addParticles( int amt );
	void removeParticles( int amt );
	
	std::list<Particle>	mParticles;
	int mNumParticles;
};