//
//  Galaxy.cpp
//  Kepler
//
//  Created by Tom Carden on 6/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Galaxy.h"

using namespace ci;

void Galaxy::setup(float initialCamDist, ci::gl::Texture galaxyDome, ci::gl::Texture galaxyTex, ci::gl::Texture darkMatterTex, ci::gl::Texture starGlowTex)
{
	mDarkMatterCylinderRes = 48;    
    initGalaxyVertexArray();
    initDarkMatterVertexArray();    
   	mLightMatterBaseRadius = initialCamDist * 0.75f;
	mDarkMatterBaseRadius = initialCamDist * 0.86f; 

    mGalaxyDome = galaxyDome;
    mGalaxyTex = galaxyTex;
    mDarkMatterTex = darkMatterTex;
    mStarGlowTex = starGlowTex;
}

void Galaxy::drawLightMatter(float invAlpha, Color color /* BRIGHT_BLUE */, float elapsedSeconds)
{
	gl::enableAdditiveBlending();
	
    // LIGHTMATTER
	if( invAlpha > 0.01f ){
		gl::color( ColorA( color, invAlpha * 2.0f ) );
        
		float radius = mLightMatterBaseRadius * 0.9f;
		gl::pushModelView();
        mGalaxyDome.enableAndBind();
        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glVertexPointer( 3, GL_FLOAT, 0, mDarkMatterVerts );
        glTexCoordPointer( 2, GL_FLOAT, 0, mDarkMatterTexCoords );
        
        gl::scale( Vec3f( radius, radius * 0.69f, radius ) );
        gl::rotate( Vec3f( 0.0f, elapsedSeconds * -2.0f, 0.0f ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
		
        gl::scale( Vec3f( 1.25, 1.15f, 1.25f ) );
        gl::rotate( Vec3f( 0.0f, elapsedSeconds * -0.5f, 0.0f ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
        
        glDisableClientState( GL_VERTEX_ARRAY );
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        mGalaxyDome.disable();
		gl::popModelView();
	}
}

void Galaxy::drawSpiralPlanes(float camGalaxyAlpha, float zoomOff, float elapsedSeconds)
{	
    // GALAXY SPIRAL PLANES
	float alpha = ( 1.25f - camGalaxyAlpha ) * zoomOff;//sqrt(camGalaxyAlpha) * zoomOff;
	if( alpha > 0.01f ){
		mGalaxyTex.enableAndBind();
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mGalaxyVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mGalaxyTexCoords );
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, alpha ) );
		
		gl::translate( Vec3f( 0.0f, 2.5f, 0.0f ) );
		gl::rotate( Vec3f( 0.0f, elapsedSeconds * -4.0f, 0.0f ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 );
		
		gl::translate( Vec3f( 0.0f, -5.0f, 0.0f ) );
		gl::rotate( Vec3f( 0.0f, elapsedSeconds * -2.0f, 0.0f ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 );
		
		gl::translate( Vec3f( 0.0f, 2.5f, 0.0f ) );
		gl::scale( Vec3f( 0.5f, 0.5f, 0.5f ) );
		gl::rotate( Vec3f( 0.0f, elapsedSeconds * -15.0f, 0.0f ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 );
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		mGalaxyTex.disable();
	}
}

void Galaxy::drawCenter(float invAlpha, Color color /* BLUE */, float elapsedSeconds, Vec3f bbRight, Vec3f bbUp)
{
    // CENTER OF GALAXY
	if( invAlpha > 0.01f ){
		mStarGlowTex.enableAndBind();
		gl::color( ColorA( color, invAlpha ) );
		gl::drawBillboard( Vec3f::zero(), Vec2f( 300.0f, 300.0f ), elapsedSeconds * 10.0f, bbRight, bbUp );
		gl::color( ColorA( color, invAlpha * 1.5f ) );
		gl::drawBillboard( Vec3f::zero(), Vec2f( 200.0f, 200.0f ), -elapsedSeconds * 7.0f, bbRight, bbUp );
		mStarGlowTex.disable();
	}
}

void Galaxy::drawDarkMatter(float invAlpha, float camGalaxyAlpha, float zoomOff, float elapsedSeconds)
{
    // DARKMATTER //////////////////////////////////////////////////////////////////////////////////////////
	if( invAlpha > 0.01f ){
		glEnable( GL_CULL_FACE ); 
		glCullFace( GL_FRONT ); 
		//float alpha = pow( 1.0f - camGalaxyAlpha, 8.0f ) * zoomOff;
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, invAlpha ) );
		float radius = mDarkMatterBaseRadius * 0.85f;
		gl::pushModelView();
		mDarkMatterTex.enableAndBind();
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mDarkMatterVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mDarkMatterTexCoords );
		
		gl::rotate( Vec3f( 0.0f, elapsedSeconds * -2.0f, 0.0f ) );
		gl::scale( Vec3f( radius, radius * 0.5f, radius ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
		
		gl::rotate( Vec3f( 0.0f, elapsedSeconds * -0.5f, 0.0f ) );
		gl::scale( Vec3f( 1.3f, 1.3f, 1.3f ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
        
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		mDarkMatterTex.disable();
		gl::popModelView();
		glDisable( GL_CULL_FACE ); 
	}
}

void Galaxy::initGalaxyVertexArray()
{
	std::cout << "initializing Galaxy Vertex Array" << std::endl;
	mGalaxyVerts		= new float[18];
	mGalaxyTexCoords	= new float[12];
	int i	= 0;
	int t	= 0;
	float w	= 400.0f;
	Vec3f corner;
	
	
	corner			= Vec3f( -w, 0.0f, -w );
	mGalaxyVerts[i++]			= corner.x;
	mGalaxyVerts[i++]			= corner.y;
	mGalaxyVerts[i++]			= corner.z;
	mGalaxyTexCoords[t++]		= 0.0f;
	mGalaxyTexCoords[t++]		= 0.0f;
	
	corner			= Vec3f( w, 0.0f, -w );
	mGalaxyVerts[i++]			= corner.x;
	mGalaxyVerts[i++]			= corner.y;
	mGalaxyVerts[i++]			= corner.z;
	mGalaxyTexCoords[t++]		= 1.0f;
	mGalaxyTexCoords[t++]		= 0.0f;
	
	corner			= Vec3f( w, 0.0f, w );	
	mGalaxyVerts[i++]			= corner.x;
	mGalaxyVerts[i++]			= corner.y;
	mGalaxyVerts[i++]			= corner.z;
	mGalaxyTexCoords[t++]		= 1.0f;
	mGalaxyTexCoords[t++]		= 1.0f;
	
	corner			= Vec3f( -w, 0.0f, -w );
	mGalaxyVerts[i++]			= corner.x;
	mGalaxyVerts[i++]			= corner.y;
	mGalaxyVerts[i++]			= corner.z;
	mGalaxyTexCoords[t++]		= 0.0f;
	mGalaxyTexCoords[t++]		= 0.0f;
	
	corner			= Vec3f( w, 0.0f, w );	
	mGalaxyVerts[i++]			= corner.x;
	mGalaxyVerts[i++]			= corner.y;
	mGalaxyVerts[i++]			= corner.z;
	mGalaxyTexCoords[t++]		= 1.0f;
	mGalaxyTexCoords[t++]		= 1.0f;
	
	corner			= Vec3f( -w, 0.0f, w );	
	mGalaxyVerts[i++]			= corner.x;
	mGalaxyVerts[i++]			= corner.y;
	mGalaxyVerts[i++]			= corner.z;
	mGalaxyTexCoords[t++]		= 0.0f;
	mGalaxyTexCoords[t++]		= 1.0f;
}


void Galaxy::initDarkMatterVertexArray()
{
	std::cout << "initializing Dark Matter Vertex Array" << std::endl;
	mDarkMatterVerts		= new float[ mDarkMatterCylinderRes * 6 * 3 ]; // cylinderRes * two-triangles * 3d
	mDarkMatterTexCoords	= new float[ mDarkMatterCylinderRes * 6 * 2 ]; // cylinderRes * two-triangles * 2d
	
    const float TWO_PI = 2.0f * M_PI;
    
	int i	= 0;
	int t	= 0;
	
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
		
		mDarkMatterVerts[i++]		= v1.x;
		mDarkMatterVerts[i++]		= v1.y;
		mDarkMatterVerts[i++]		= v1.z;
		mDarkMatterTexCoords[t++]	= per1;
		mDarkMatterTexCoords[t++]	= 0.0f;
		
		mDarkMatterVerts[i++]		= v2.x;
		mDarkMatterVerts[i++]		= v2.y;
		mDarkMatterVerts[i++]		= v2.z;
		mDarkMatterTexCoords[t++]	= per2;
		mDarkMatterTexCoords[t++]	= 0.0f;
		
		mDarkMatterVerts[i++]		= v3.x;
		mDarkMatterVerts[i++]		= v3.y;
		mDarkMatterVerts[i++]		= v3.z;
		mDarkMatterTexCoords[t++]	= per1;
		mDarkMatterTexCoords[t++]	= 1.0f;
		
		mDarkMatterVerts[i++]		= v2.x;
		mDarkMatterVerts[i++]		= v2.y;
		mDarkMatterVerts[i++]		= v2.z;
		mDarkMatterTexCoords[t++]	= per2;
		mDarkMatterTexCoords[t++]	= 0.0f;
		
		mDarkMatterVerts[i++]		= v4.x;
		mDarkMatterVerts[i++]		= v4.y;
		mDarkMatterVerts[i++]		= v4.z;
		mDarkMatterTexCoords[t++]	= per2;
		mDarkMatterTexCoords[t++]	= 1.0f;
		
		mDarkMatterVerts[i++]		= v3.x;
		mDarkMatterVerts[i++]		= v3.y;
		mDarkMatterVerts[i++]		= v3.z;
		mDarkMatterTexCoords[t++]	= per1;
		mDarkMatterTexCoords[t++]	= 1.0f;
	}
}
