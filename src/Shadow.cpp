//
//  Shadow.cpp
//  Kepler
//
//  Created by Tom Carden on 6/25/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#include "cinder/CinderMath.h"
#include "Shadow.h"
#include "Node.h"
#include "Globals.h"

using namespace ci;

Shadow::Shadow()
{
	mShadowVerts		= NULL;
	mShadowTexCoords	= NULL;    
}

Shadow::~Shadow()
{
    if( mShadowVerts != NULL )		delete[] mShadowVerts;
    if( mShadowTexCoords != NULL )  delete[] mShadowTexCoords;
}

void Shadow::setup( Node* node, Node* mParentNode, float camAlpha )
{
	Vec3f P0, P1, P2, P4;
	Vec3f P3a, P3b;
	Vec3f P5a, P5b, P6a, P6b;
	Vec3f outerTanADir, outerTanBDir, innerTanADir, innerTanBDir;
	
	float r0, r1, r0Inner, rTotal;
	float d, dMid, dMidSqrd;
	
	// Positions	
	P0				= mParentNode->mPos;
	P1				= node->mPos;
	P4				= ( P0 + P1 )*0.5f;
	
	// Radii
	r0				= mParentNode->mRadius * 0.175f;
	r1				= node->mRadius * 1.05f;
	rTotal			= r0 + r1;
	r0Inner			= abs( r0 - r1 );
	
	d				= P0.distance( P1 );
	dMid			= d * 0.5f;
	dMidSqrd		= dMid * dMid;
	
	float newRTotal		= r0Inner + dMid;
	float newRDelta		= abs( dMid - r0Inner );
	
	if( dMid > newRTotal ){
		// std::cout << "not intersecting" << std::endl;
	} else if( dMid < newRDelta ){
		// std::cout << "contained" << std::endl;
	} else if( dMid == 0 ){
		// std::cout << "concentric" << std::endl;
	} else {
		float a = ( dMidSqrd - r0Inner * r0Inner + dMidSqrd ) / d;
		P2 = P4 + a * ( ( P0 - P4 ) / dMid );
		
		float h = sqrt( dMidSqrd - a * a ) * 0.5f;
		
		Vec3f p = ( P1 - P0 )/dMid;
		
		P3a = P2 + h * Vec3f( -p.z, p.y, p.x );
		P3b = P2 - h * Vec3f( -p.z, p.y, p.x );
		
		
		Vec3f P3aDirNorm = P3a - P0;
		P3aDirNorm.normalize();
		
		Vec3f P3bDirNorm = P3b - P0;
		P3bDirNorm.normalize();
		
		P5a = P3a + P3aDirNorm * r1;
		P5b = P3b + P3bDirNorm * r1;
		P6a = P1 + P3aDirNorm * r1; 
		P6b = P1 + P3bDirNorm * r1;
		
		float amt = r0 * 3.0f;
		outerTanADir = ( P6a - P5a ) * amt;
		outerTanBDir = ( P6b - P5b ) * amt;
		innerTanADir = ( P6a - P5b ) * amt;
		innerTanBDir = ( P6b - P5a ) * amt;
		
		Vec3f P7a = P6a + outerTanBDir;
		Vec3f P7b = P6b + outerTanADir;
        
		float distOfShadow = math<float>::max( 1.0f - r0, 0.01f );
		P7a = P6a + ( P7a - P6a ).normalized() * distOfShadow;
		P7b = P6b + ( P7b - P6b ).normalized() * distOfShadow;
		
        // move draw() call back into NodeAlbum/NodeTrack, clear verts if there's no shadow
		glEnable( GL_TEXTURE_2D );
        
        buildVerts( P6a, P6b, P7a, P7b );
        
		float alpha = 0.2f * camAlpha * node->mDeathPer;
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, alpha ) );
        
        draw();
	}
    
     if( G_DEBUG ){
         glDisable( GL_TEXTURE_2D );
         
         gl::enableAlphaBlending();
         gl::color( ColorA( node->mGlowColor, 0.4f ) );
         gl::drawLine( P0, P1 );
         
         glPushMatrix();
         gl::translate( P0 );
//         gl::rotate( mMatrix );
         gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
         gl::drawStrokedCircle( Vec2f::zero(), r0, 50 );
         glPopMatrix();
         
         glPushMatrix();
         gl::translate( P0 );
//         gl::rotate( mMatrix );
         gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
         gl::drawStrokedCircle( Vec2f::zero(), r0Inner, 50 );
         glPopMatrix();
         
         glPushMatrix();
         gl::translate( P1 );
//         gl::rotate( mMatrix );
         gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
         gl::drawStrokedCircle( Vec2f::zero(), r1, 25 );
         glPopMatrix();
         
         glPushMatrix();
         gl::translate( P2 );
//         gl::rotate( mMatrix );
         gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
         gl::drawStrokedCircle( Vec2f::zero(), 0.01f, 16 );
         glPopMatrix();
         
         
         
         glPushMatrix();
         gl::translate( P3a );
         //gl::rotate( mMatrix );
         //gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
         gl::drawStrokedCircle( Vec2f::zero(), 0.01f, 16 );
         glPopMatrix();
         
         glPushMatrix();
         gl::translate( P3b );
         //gl::rotate( mMatrix );
         //gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
         gl::drawStrokedCircle( Vec2f::zero(), 0.01f, 16 );
         glPopMatrix();
         
         glPushMatrix();
         gl::translate( P5a );
         gl::drawStrokedCircle( Vec2f::zero(), 0.01f, 16 );
         glPopMatrix();
         
         glPushMatrix();
         gl::translate( P5b );
         gl::drawStrokedCircle( Vec2f::zero(), 0.01f, 16 );
         glPopMatrix();
         
         glPushMatrix();
         gl::translate( P6a );
         gl::drawStrokedCircle( Vec2f::zero(), 0.01f, 16 );
         glPopMatrix();
         
         glPushMatrix();
         gl::translate( P6b );
         gl::drawStrokedCircle( Vec2f::zero(), 0.01f, 16 );
         glPopMatrix();
         
         
//         gl::drawLine( P6a, ( P6a + mMatrix * outerTanBDir ) );
//         gl::drawLine( P6b, ( P6b + mMatrix * outerTanBDir ) );
//         gl::drawLine( P6a, ( P6a + mMatrix * innerTanBDir ) );
//         gl::drawLine( P6b, ( P6b + mMatrix * innerTanBDir ) );
         gl::drawLine( P6a, ( P6a + outerTanBDir ) );
         gl::drawLine( P6b, ( P6b + outerTanBDir ) );
         gl::drawLine( P6a, ( P6a + innerTanBDir ) );
         gl::drawLine( P6b, ( P6b + innerTanBDir ) );
         
         gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.4f ) );	
         glPushMatrix();
         gl::translate( P4 );
//         gl::rotate( mMatrix );
         gl::rotate( Vec3f( 90.0f, 0.0f, 0.0f ) );
         gl::drawStrokedCircle( Vec2f::zero(), dMid, 50 );
         glPopMatrix();
         
         glEnable( GL_TEXTURE_2D );
     }
}

void Shadow::buildVerts( Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4 )
{
    if( mShadowVerts != NULL )		delete[] mShadowVerts;
    if( mShadowTexCoords != NULL )  delete[] mShadowTexCoords;
    
	int numVerts		= 12;			// dont forget to change the vert count in draw vvv
	mShadowVerts		= new float[ numVerts * 3 ]; // x, y
	mShadowTexCoords	= new float[ numVerts * 2 ]; // u, v
	int i = 0;
	int t = 0;
	
	Vec3f v1 = ( p1 + p2 ) * 0.5f;	// midpoint between base vertices
	Vec3f v2 = ( p3 + p4 ) * 0.5f;	// midpoint between end vertices
	
//    std::cout << v1 << " " 
//              << v2 << " " 
//              << p1 << " " 
//              << p2 << " " 
//              << p3 << " " 
//              << p4 << std::endl;
    
	mShadowVerts[i++]	= p1.x;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p1.y;		mShadowTexCoords[t++]	= 0.2f;
	mShadowVerts[i++]	= p1.z;
	mShadowVerts[i++]	= v2.x;		mShadowTexCoords[t++]	= 0.5f;
	mShadowVerts[i++]	= v2.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= v2.z;
	mShadowVerts[i++]	= p3.x;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p3.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= p3.z;
    
	// umbra 
	mShadowVerts[i++]	= p1.x;		mShadowTexCoords[t++]	= 0.5f;
	mShadowVerts[i++]	= p1.y;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p1.z;
	mShadowVerts[i++]	= v1.x;		mShadowTexCoords[t++]	= 0.75f;
	mShadowVerts[i++]	= v1.y;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= v1.z;
	mShadowVerts[i++]	= v2.x;		mShadowTexCoords[t++]	= 0.75f;
	mShadowVerts[i++]	= v2.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= v2.z;
	
	// umbra 
	mShadowVerts[i++]	= v1.x;		mShadowTexCoords[t++]	= 0.75f;
	mShadowVerts[i++]	= v1.y;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= v1.z;
	mShadowVerts[i++]	= p2.x;		mShadowTexCoords[t++]	= 0.5f;
	mShadowVerts[i++]	= p2.y;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p2.z;
	mShadowVerts[i++]	= v2.x;		mShadowTexCoords[t++]	= 0.75f;
	mShadowVerts[i++]	= v2.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= v2.z;
	
	mShadowVerts[i++]	= p2.x;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p2.y;		mShadowTexCoords[t++]	= 0.2f;
	mShadowVerts[i++]	= p2.z;
	mShadowVerts[i++]	= p4.x;		mShadowTexCoords[t++]	= 0.0f;
	mShadowVerts[i++]	= p4.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= p4.z;
	mShadowVerts[i++]	= v2.x;		mShadowTexCoords[t++]	= 0.5f;
	mShadowVerts[i++]	= v2.y;		mShadowTexCoords[t++]	= 1.0f;
	mShadowVerts[i++]	= v2.z;	
	
}

void Shadow::draw()
{
    glVertexPointer( 3, GL_FLOAT, 0, mShadowVerts );
    glTexCoordPointer( 2, GL_FLOAT, 0, mShadowTexCoords );
    
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glDrawArrays( GL_TRIANGLES, 0, 12 ); // dont forget to change the vert count in buildVerts ^^^
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );    
}
