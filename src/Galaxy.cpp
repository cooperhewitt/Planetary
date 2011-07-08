//
//  Galaxy.cpp
//  Kepler
//
//  Created by Tom Carden on 6/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Galaxy.h"
#include "Globals.h"

using namespace ci;

void Galaxy::update(const Vec3f &eye, const float &fadeInAlphaToArtist, const float &elapsedSeconds, const Vec3f &bbRight, const Vec3f &bbUp)
{
	// For doing galaxy-axis fades
	mZoomOff = 1.0f - fadeInAlphaToArtist;//constrain( ( G_ARTIST_LEVEL - G_ZOOM ), 0.0f, 1.0f );
	mCamGalaxyAlpha = constrain( abs( eye.y ) * 0.004f, 0.0f, 1.0f );
	mInvAlpha = pow( 1.0f - mCamGalaxyAlpha, 2.5f ) * mZoomOff;    
    mElapsedSeconds = elapsedSeconds;
    mBbRight = bbRight;
    mBbUp = bbUp;
}

void Galaxy::setup(float initialCamDist, ci::Color lightMatterColor, ci::Color centerColor,
ci::gl::Texture galaxyDome, ci::gl::Texture galaxyTex, ci::gl::Texture darkMatterTex, ci::gl::Texture starGlowTex)
{
	mDarkMatterCylinderRes = 48;    
    initGalaxyVertexArray();
    initDarkMatterVertexArray();    
   	mLightMatterBaseRadius = initialCamDist * 0.675f;
	mDarkMatterBaseRadius = initialCamDist * 0.75f; 

    mLightMatterColor = lightMatterColor;
    mCenterColor = centerColor;
    
    mGalaxyDome = galaxyDome;
    mGalaxyTex = galaxyTex;
    mDarkMatterTex = darkMatterTex;
    mStarGlowTex = starGlowTex;
}

void Galaxy::drawLightMatter()
{
	gl::enableAdditiveBlending();
	
    // LIGHTMATTER
	if( mInvAlpha > 0.01f ){
		gl::color( ColorA( mLightMatterColor, mInvAlpha * 2.0f ) );
        
		const float radius = mLightMatterBaseRadius * 0.9f;
		glPushMatrix();
        mGalaxyDome.enableAndBind();

        glBindBuffer(GL_ARRAY_BUFFER, mDarkMatterVBO);
        glVertexPointer( 3, GL_FLOAT, sizeof(VertexData), 0 ); // last arg becomes an offset instead of an address
        glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), (void*)sizeof(Vec3f) ); // NB:- change if type of VertexData.vertex changes
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Leave no VBO bound.                

        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        
        gl::scale( Vec3f( radius, radius * 0.69f, radius ) );
        gl::rotate( Vec3f( 0.0f, mElapsedSeconds * -2.0f, 0.0f ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
		
        gl::scale( Vec3f( 1.25, 1.15f, 1.25f ) );
        gl::rotate( Vec3f( 0.0f, mElapsedSeconds * -0.5f, 0.0f ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
        
        glDisableClientState( GL_VERTEX_ARRAY );
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        mGalaxyDome.disable();
		glPopMatrix();
	}
}

void Galaxy::drawSpiralPlanes()
{	
    // GALAXY SPIRAL PLANES
	const float alpha = ( 1.25f - mCamGalaxyAlpha ) * mZoomOff;//sqrt(camGalaxyAlpha) * zoomOff;
	if( alpha > 0.01f ){

        gl::color( ColorA( 1.0f, 1.0f, 1.0f, alpha ) );

        glBindBuffer(GL_ARRAY_BUFFER, mGalaxyVBO);
        glVertexPointer( 3, GL_FLOAT, sizeof(VertexData), 0 ); // last arg becomes an offset instead of an address
        glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), (void*)sizeof(Vec3f) ); // NB:- change if type of VertexData.vertex changes
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Leave no VBO bound.                
        
        glPushMatrix();
		mGalaxyTex.enableAndBind();
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		
		gl::translate( Vec3f( 0.0f, 2.5f, 0.0f ) );
		gl::rotate( Vec3f( 0.0f, mElapsedSeconds * -4.0f, 0.0f ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 );
		
		gl::translate( Vec3f( 0.0f, -5.0f, 0.0f ) );
		gl::rotate( Vec3f( 0.0f, mElapsedSeconds * -2.0f, 0.0f ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 );
		
		gl::translate( Vec3f( 0.0f, 2.5f, 0.0f ) );
		gl::scale( Vec3f( 0.5f, 0.5f, 0.5f ) );
		gl::rotate( Vec3f( 0.0f, mElapsedSeconds * -15.0f, 0.0f ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 );
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		mGalaxyTex.disable();        
        glPopMatrix();
	}
}

void Galaxy::drawCenter()
{
    // CENTER OF GALAXY
	if( mInvAlpha > 0.01f ){
		mStarGlowTex.enableAndBind();
		gl::color( ColorA( mCenterColor, mInvAlpha ) );
		gl::drawBillboard( Vec3f::zero(), Vec2f( 300.0f, 300.0f ), mElapsedSeconds * 10.0f, mBbRight, mBbUp );
		gl::color( ColorA( mCenterColor, mInvAlpha * 1.5f ) );
		gl::drawBillboard( Vec3f::zero(), Vec2f( 200.0f, 200.0f ), -mElapsedSeconds * 7.0f, mBbRight, mBbUp );
		mStarGlowTex.disable();
	}
}

void Galaxy::drawDarkMatter()
{
    // DARKMATTER //////////////////////////////////////////////////////////////////////////////////////////
	if( mInvAlpha > 0.01f ){
		glEnable( GL_CULL_FACE ); 
		glCullFace( GL_FRONT ); 
		
        //float alpha = pow( 1.0f - camGalaxyAlpha, 8.0f ) * zoomOff;
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, mInvAlpha ) );
		const float radius = mDarkMatterBaseRadius * 0.85f;
        
		glPushMatrix();
		mDarkMatterTex.enableAndBind();
        
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );

        glBindBuffer(GL_ARRAY_BUFFER, mDarkMatterVBO);
        glVertexPointer( 3, GL_FLOAT, sizeof(VertexData), 0 ); // last arg becomes an offset instead of an address
        glTexCoordPointer( 2, GL_FLOAT, sizeof(VertexData), (void*)sizeof(Vec3f) ); // NB:- change if type of VertexData.vertex changes
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Leave no VBO bound.                
        
		gl::rotate( Vec3f( 0.0f, mElapsedSeconds * -2.0f, 0.0f ) );
		gl::scale( Vec3f( radius, radius * 0.5f, radius ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
		
		gl::rotate( Vec3f( 0.0f, mElapsedSeconds * -0.5f, 0.0f ) );
		gl::scale( Vec3f( 1.3f, 1.3f, 1.3f ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
        
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		mDarkMatterTex.disable();
		glPopMatrix();
		glDisable( GL_CULL_FACE ); 
	}
}

void Galaxy::initGalaxyVertexArray()
{
	std::cout << "initializing Galaxy Vertex Array" << std::endl;
    
	VertexData *galaxyVerts = new VertexData[6];

	const float w = 200.0f;

    int vert = 0;

	galaxyVerts[vert].vertex  = Vec3f( -w, 0.0f, -w );
    galaxyVerts[vert].texture = Vec2f::zero();
    vert++;
	
	galaxyVerts[vert].vertex  = Vec3f( w, 0.0f, -w );
    galaxyVerts[vert].texture = Vec2f(1.0f, 0.0f);
    vert++;

    galaxyVerts[vert].vertex  = Vec3f( w, 0.0f, w );
    galaxyVerts[vert].texture = Vec2f(1.0f, 1.0f);
    vert++;

    galaxyVerts[vert].vertex  = Vec3f( -w, 0.0f, -w );
    galaxyVerts[vert].texture = Vec2f(0.0f, 0.0f);
    vert++;

    galaxyVerts[vert].vertex  = Vec3f( w, 0.0f, w );
    galaxyVerts[vert].texture = Vec2f(1.0f, 1.0f);
    vert++;

    galaxyVerts[vert].vertex  = Vec3f( -w, 0.0f, w );
    galaxyVerts[vert].texture = Vec2f(0.0f, 1.0f);
    vert++;
    
    glGenBuffers(1, &mGalaxyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mGalaxyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vert, galaxyVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Leave no VBO bound.        
    
    delete[] galaxyVerts;
}


void Galaxy::initDarkMatterVertexArray()
{
	std::cout << "initializing Dark Matter Vertex Array" << std::endl;
	VertexData *darkMatterVerts = new VertexData[ mDarkMatterCylinderRes * 6 ]; // cylinderRes * two-triangles

    const float TWO_PI = 2.0f * M_PI;
    
	int vert = 0;
	
	for( int x=0; x<mDarkMatterCylinderRes; x++ ){
        
		float per1		= (float)x/(float)mDarkMatterCylinderRes;
		float per2		= (float)(x+1)/(float)mDarkMatterCylinderRes;
		float angle1	= per1 * TWO_PI;
		float angle2	= per2 * TWO_PI;
		
		float sa1 = sin( angle1 );
		float ca1 = cos( angle1 );
		float sa2 = sin( angle2 );
		float ca2 = cos( angle2 );
		
		float h = 0.5f;
		Vec3f v1 = Vec3f( ca1, -h, sa1 );
		Vec3f v2 = Vec3f( ca2, -h, sa2 );
		Vec3f v3 = Vec3f( ca1,  h, sa1 );
		Vec3f v4 = Vec3f( ca2,  h, sa2 );
		
        const float texRepeat = G_USE_COMPRESSED ? 2.0f : 1.0f;
        
		darkMatterVerts[vert].vertex = v1;
		darkMatterVerts[vert].texture = Vec2f(per1 * texRepeat, 0.0f);
		vert++;

        darkMatterVerts[vert].vertex = v2;
		darkMatterVerts[vert].texture = Vec2f(per2 * texRepeat, 0.0f);
		vert++;

        darkMatterVerts[vert].vertex = v3;
		darkMatterVerts[vert].texture = Vec2f(per1 * texRepeat, 1.0f);
		vert++;

        darkMatterVerts[vert].vertex = v2;
		darkMatterVerts[vert].texture = Vec2f(per2 * texRepeat, 0.0f);
		vert++;

        darkMatterVerts[vert].vertex = v4;
		darkMatterVerts[vert].texture = Vec2f(per2 * texRepeat, 1.0f);
		vert++;

        darkMatterVerts[vert].vertex = v3;
		darkMatterVerts[vert].texture = Vec2f(per1 * texRepeat, 1.0f);
		vert++;
	}
    
    glGenBuffers(1, &mDarkMatterVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mDarkMatterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vert, darkMatterVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Leave no VBO bound.        
    
    delete[] darkMatterVerts;
}
