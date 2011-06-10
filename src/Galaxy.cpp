//
//  Galaxy.cpp
//  Kepler
//
//  Created by Tom Carden on 6/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Galaxy.h"

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
   	mLightMatterBaseRadius = initialCamDist * 0.75f;
	mDarkMatterBaseRadius = initialCamDist * 0.86f; 

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
        
		float radius = mLightMatterBaseRadius * 0.9f;
		gl::pushModelView();
        mGalaxyDome.enableAndBind();
        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glVertexPointer( 3, GL_FLOAT, 0, mDarkMatterVerts );
        glTexCoordPointer( 2, GL_FLOAT, 0, mDarkMatterTexCoords );
        
        gl::scale( Vec3f( radius, radius * 0.69f, radius ) );
        gl::rotate( Vec3f( 0.0f, mElapsedSeconds * -2.0f, 0.0f ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
		
        gl::scale( Vec3f( 1.25, 1.15f, 1.25f ) );
        gl::rotate( Vec3f( 0.0f, mElapsedSeconds * -0.5f, 0.0f ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
        
        glDisableClientState( GL_VERTEX_ARRAY );
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        mGalaxyDome.disable();
		gl::popModelView();
	}
}

void Galaxy::drawSpiralPlanes()
{	
    // GALAXY SPIRAL PLANES
	float alpha = ( 1.25f - mCamGalaxyAlpha ) * mZoomOff;//sqrt(camGalaxyAlpha) * zoomOff;
	if( alpha > 0.01f ){
		mGalaxyTex.enableAndBind();
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mGalaxyVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mGalaxyTexCoords );
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, alpha ) );
		
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
		float radius = mDarkMatterBaseRadius * 0.85f;
		gl::pushModelView();
		mDarkMatterTex.enableAndBind();
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mDarkMatterVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mDarkMatterTexCoords );
		
		gl::rotate( Vec3f( 0.0f, mElapsedSeconds * -2.0f, 0.0f ) );
		gl::scale( Vec3f( radius, radius * 0.5f, radius ) );
        glDrawArrays( GL_TRIANGLES, 0, 6 * mDarkMatterCylinderRes );
		
		gl::rotate( Vec3f( 0.0f, mElapsedSeconds * -0.5f, 0.0f ) );
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
