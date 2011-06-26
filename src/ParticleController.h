#pragma once
#include "Particle.h"
#include "Dust.h"
#include "Node.h"
#include <list>

class ParticleController {
 public:
    
    struct ParticleVertex {
        ci::Vec3f vertex;
        ci::Vec2f texture;
        ci::Vec4f color;
    };
    
    struct DustVertex {
        ci::Vec3f vertex;
        ci::Vec4f color;
    };
    
	ParticleController();
	void update( const ci::Vec3f &camEye, float radius, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );
	void buildParticleVertexArray( float scaleOffset, ci::Color c, float eclipseStrength );
	void buildDustVertexArray( float scaleOffset, Node *node, float pinchAlphaOffset, float dustAlpha );
	void drawParticleVertexArray( Node *node );
	void drawDustVertexArray( Node *node );
	void addParticles( int amt );
	void removeParticles( int amt );
	void addDusts( int amt );
	
    // TODO: consider dynamic VBOs or VAOs for these arrays
    
	std::list<Particle>	mParticles;
	int mTotalParticleVertices;
    int mPrevTotalParticleVertices; // so we only recreate frames
    ParticleVertex *mParticleVerts;	// TODO: consider POINT_SPRITE stuff for these?
	
	std::list<Dust> mDusts;
	int mTotalDustVertices;
    int mPrevTotalDustVertices; // so we only recreate frames
    DustVertex *mDustVerts;
	
	ci::Vec3f mBbRight;
	ci::Vec3f mBbUp;
	
};