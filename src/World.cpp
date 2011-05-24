/*
 *  World.cpp
 *  Kepler
 *
 *  Created by Robert Hodgin on 2/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include <deque>
#include "World.h"
#include "NodeArtist.h"
#include "cinder/gl/gl.h"
#include "cinder/Rect.h"
#include "cinder/Text.h"
#include "cinder/Rand.h"
#include "Globals.h"
#include "BloomGl.h"

using std::stringstream;
using namespace ci;
using namespace ci::app;
using namespace ci::ipod;
using namespace std;

World::World()
{
    mPrevTotalStarVertices	= -1;
    mStarVerts				= NULL;
    mStarTexCoords			= NULL;
	mStarColors				= NULL;
	
	mPrevTotalConstellationVertices = -1;
	mConstellationVerts			= NULL;
	mConstellationTexCoords		= NULL;
    
}

void World::setup( Data *data )
{
    mData = data;

	if( !mIsInitialized ){
		// VERTEX ARRAY SPHERE
		if( G_IS_IPAD2 ){
			buildSphereVertexArray( 32, &mNumSphereHiResVerts, mSphereHiResVerts, mSphereHiResTexCoords, mSphereHiResNormals );
			buildSphereVertexArray( 20, &mNumSphereMdResVerts, mSphereMdResVerts, mSphereMdResTexCoords, mSphereMdResNormals );
			buildSphereVertexArray( 16, &mNumSphereLoResVerts, mSphereLoResVerts, mSphereLoResTexCoords, mSphereLoResNormals );
			buildSphereVertexArray( 10, &mNumSphereTyResVerts, mSphereTyResVerts, mSphereTyResTexCoords, mSphereTyResNormals );
		} else {
			buildSphereVertexArray( 32, &mNumSphereHiResVerts, mSphereHiResVerts, mSphereHiResTexCoords, mSphereHiResNormals );
			buildSphereVertexArray( 26, &mNumSphereMdResVerts, mSphereMdResVerts, mSphereMdResTexCoords, mSphereMdResNormals );
			buildSphereVertexArray( 12, &mNumSphereLoResVerts, mSphereLoResVerts, mSphereLoResTexCoords, mSphereLoResNormals );
			buildSphereVertexArray( 8,  &mNumSphereTyResVerts, mSphereTyResVerts, mSphereTyResTexCoords, mSphereTyResNormals );
		}
	}
	
	
	mAge			= 0;
	mEndRepulseAge	= 50;//200
	mIsRepulsing	= true;
	mIsInitialized	= false;
}


void World::buildSphereVertexArray( int segments, int *numVerts, float* &sphereVerts, float* &sphereTexCoords, float* &sphereNormals )
{	
    if (sphereVerts != NULL) delete[] sphereVerts;
    if (sphereNormals != NULL) delete[] sphereNormals;
    if (sphereTexCoords != NULL) delete[] sphereTexCoords;
    
	*numVerts			= segments * (segments/2) * 2 * 3;
	sphereVerts			= new float[ *numVerts * 3 ];
	sphereNormals		= new float[ *numVerts * 3 ];
	sphereTexCoords		= new float[ *numVerts * 2 ];
	vector<Vec2f> texCoords;
	vector<Triangle> triangles;
	
	for( int j = 0; j < segments / 2; j++ ) {
		float theta1 = j * TWO_PI / segments - ( M_PI_2 );
		float cosTheta1 = cos( theta1 );
		float sinTheta1 = sin( theta1 );
		
		float theta2 = (j + 1) * TWO_PI / segments - ( M_PI_2 );
		float cosTheta2 = cos( theta2 );
		float sinTheta2 = sin( theta2 );
		
		Vec3f oldv1, oldv2, newv1, newv2;
		Vec2f oldt1, oldt2, newt1, newt2;
		
		for( int i = 0; i <= segments; i++ ) {
			oldv1			= newv1;
			oldv2			= newv2;
			
			oldt1			= newt1;
			oldt2			= newt2;
			
			float theta3	= i * TWO_PI / segments;
			float cosTheta3 = cos( theta3 );
			float sinTheta3 = sin( theta3 );
			
			float invI		= i / (float)segments;
			float u			= 0.999f - invI;
			float v1		= 0.999f - 2 * j / (float)segments;
			float v2		= 0.999f - 2 * (j+1) / (float)segments;
			
			newt1			= Vec2f( u, v1 );
			newt2			= Vec2f( u, v2 );
			
			newv1			= Vec3f( cosTheta1 * cosTheta3, sinTheta1, cosTheta1 * sinTheta3 );			
			newv2			= Vec3f( cosTheta2 * cosTheta3, sinTheta2, cosTheta2 * sinTheta3 );
			
			if( i > 0 ){
				triangles.push_back( Triangle( oldv1, oldv2, newv1 ) );
				triangles.push_back( Triangle( oldv2, newv2, newv1 ) );
				
				texCoords.push_back( oldt1 );
				texCoords.push_back( oldt2 );
				texCoords.push_back( newt1 );
				
				texCoords.push_back( oldt2 );
				texCoords.push_back( newt2 );
				texCoords.push_back( newt1 );
			}
		}
	}
	
	
	int index = 0;
	int nIndex = 0;
	for( int i=0; i<triangles.size(); i++ ){
		Triangle t = triangles[i];
		sphereVerts[index++]		= t.p1.x;
		sphereVerts[index++]		= t.p1.y;
		sphereVerts[index++]		= t.p1.z;
		
		sphereVerts[index++]		= t.p2.x;
		sphereVerts[index++]		= t.p2.y;
		sphereVerts[index++]		= t.p2.z;
		
		sphereVerts[index++]		= t.p3.x;
		sphereVerts[index++]		= t.p3.y;
		sphereVerts[index++]		= t.p3.z;
		
		sphereNormals[nIndex++]	= t.p1.x;
		sphereNormals[nIndex++]	= t.p1.y;
		sphereNormals[nIndex++]	= t.p1.z;
		
		sphereNormals[nIndex++]	= t.p2.x;
		sphereNormals[nIndex++]	= t.p2.y;
		sphereNormals[nIndex++]	= t.p2.z;
		
		sphereNormals[nIndex++]	= t.p3.x;
		sphereNormals[nIndex++]	= t.p3.y;
		sphereNormals[nIndex++]	= t.p3.z;
	}
	
	int tIndex = 0;
	for( int i=0; i<texCoords.size(); i++ ){
		sphereTexCoords[tIndex++]	= texCoords[i].x;
		sphereTexCoords[tIndex++]	= texCoords[i].y;
	}
}

void World::initNodes( Player *player, const Font &font, const Font &smallFont, const Surface &surfaces )
{
	float t = App::get()->getElapsedSeconds();

	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
        Node *node = *it;
        delete node;
    }
	mNodes.clear();
    
	int i=0;
	for(vector<PlaylistRef>::iterator it = mData->mArtists.begin(); it != mData->mArtists.end(); ++it){
		PlaylistRef artist	= *it;
		NodeArtist *newNode = new NodeArtist( i++, font, smallFont, surfaces );
		newNode->setData(artist);
		mNodes.push_back( newNode );
	}
	
	cout << (App::get()->getElapsedSeconds() - t) << " seconds to World::initNodes" << endl;

    initVertexArrays();
	
	mIsInitialized = true;
}

void World::initVertexArrays()
{
	buildOrbitRingsVertexArray();
	buildPlanetRingsVertexArray();
    initNodeSphereData( mNumSphereHiResVerts, mSphereHiResVerts, mSphereHiResTexCoords, mSphereHiResNormals,
					    mNumSphereMdResVerts, mSphereMdResVerts, mSphereMdResTexCoords, mSphereMdResNormals,
					    mNumSphereLoResVerts, mSphereLoResVerts, mSphereLoResTexCoords, mSphereLoResNormals,
					    mNumSphereTyResVerts, mSphereTyResVerts, mSphereTyResTexCoords, mSphereTyResNormals ); 
}

void World::initNodeSphereData( int totalHiVertices, float *sphereHiVerts, float *sphereHiTexCoords, float *sphereHiNormals,
							    int totalMdVertices, float *sphereMdVerts, float *sphereMdTexCoords, float *sphereMdNormals,
							    int totalLoVertices, float *sphereLoVerts, float *sphereLoTexCoords, float *sphereLoNormals,
							    int totalTyVertices, float *sphereTyVerts, float *sphereTyTexCoords, float *sphereTyNormals )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->setSphereData( totalHiVertices, sphereHiVerts, sphereHiTexCoords, sphereHiNormals,
							  totalMdVertices, sphereMdVerts, sphereMdTexCoords, sphereMdNormals,
							  totalLoVertices, sphereLoVerts, sphereLoTexCoords, sphereLoNormals,
							  totalTyVertices, sphereTyVerts, sphereTyTexCoords, sphereTyNormals );
	}
}

void World::filterNodes()
{
	deselectAllNodes();
	
	for(vector<int>::iterator it = mData->mFilteredArtists.begin(); it != mData->mFilteredArtists.end(); ++it){
		mNodes[*it]->mIsHighlighted = true;
	}
	
	if( mData->mFilteredArtists.size() > 1 ){
		buildConstellation();
	}
}

void World::deselectAllNodes()
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->mIsHighlighted = false;
		(*it)->mIsSelected = false;
	}
}

void World::setIsPlaying( uint64_t artistId, uint64_t albumId, uint64_t trackId )
{
	mPlayingTrackNode = NULL;
	
    // TODO: proper iterators I suppose?
    for (int i = 0; i < mNodes.size(); i++) {
        Node* artistNode = mNodes[i];
        artistNode->mIsPlaying = artistNode->getId() == artistId;
        for (int j = 0; j < artistNode->mChildNodes.size(); j++) {					
            Node* albumNode = artistNode->mChildNodes[j];
            albumNode->mIsPlaying = albumNode->getId() == albumId;
            for (int k = 0; k < albumNode->mChildNodes.size(); k++) {
                // FIXME: what's the proper C++ way to do this cast?
                Node *trackNode = albumNode->mChildNodes[k];
                trackNode->mIsPlaying = trackNode->getId() == trackId;
				if( trackNode->mIsPlaying && !trackNode->mIsDying ){
					mPlayingTrackNode = (NodeTrack*)trackNode;
				}
            }            
        }
    }
}

void World::checkForNameTouch( vector<Node*> &nodes, const Vec2f &pos )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		if( (*it)->mIsHighlighted ){
			(*it)->checkForNameTouch( nodes, pos );
		}
	}
}

void World::buildStarsVertexArray( const Vec3f &bbRight, const Vec3f &bbUp, float zoomAlpha )
{
	mTotalStarVertices	= mData->mArtists.size() * 6;	// 6 = 2 triangles per quad

    if (mTotalStarVertices != mPrevTotalStarVertices) {
        if (mStarVerts != NULL)		delete[] mStarVerts; 
		if (mStarTexCoords != NULL) delete[] mStarTexCoords; 
		if (mStarColors != NULL)	delete[] mStarColors;
		
        mStarVerts			= new float[mTotalStarVertices*3];
        mStarTexCoords		= new float[mTotalStarVertices*2];
        mStarColors			= new float[mTotalStarVertices*4];
		
        mPrevTotalStarVertices = mTotalStarVertices;
    }
	
	int vIndex	= 0;
	int tIndex	= 0;
	int cIndex	= 0;
	
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		Vec3f pos	= (*it)->mPos;
		float r		= (*it)->mRadius * 0.4f;
		if( !(*it)->mIsHighlighted )
			r		= r - zoomAlpha * 2.0f;
		
		ColorA col	= ColorA( (*it)->mColor, 1.0f );
		
		Vec3f right	= bbRight * r;
		Vec3f up	= bbUp * r;
		
		Vec3f p1	= pos - right - up;
		Vec3f p2	= pos + right - up;
		Vec3f p3	= pos - right + up;
		Vec3f p4	= pos + right + up;
		
		mStarVerts[vIndex++]		= p1.x;
		mStarVerts[vIndex++]		= p1.y;
		mStarVerts[vIndex++]		= p1.z;
		mStarTexCoords[tIndex++]	= 0.0f;
		mStarTexCoords[tIndex++]	= 0.0f;
		mStarColors[cIndex++]		= col.r;
		mStarColors[cIndex++]		= col.g;
		mStarColors[cIndex++]		= col.b;
		mStarColors[cIndex++]		= col.a;
		
		mStarVerts[vIndex++]		= p2.x;
		mStarVerts[vIndex++]		= p2.y;
		mStarVerts[vIndex++]		= p2.z;
		mStarTexCoords[tIndex++]	= 1.0f;
		mStarTexCoords[tIndex++]	= 0.0f;
		mStarColors[cIndex++]		= col.r;
		mStarColors[cIndex++]		= col.g;
		mStarColors[cIndex++]		= col.b;
		mStarColors[cIndex++]		= col.a;
		
		mStarVerts[vIndex++]		= p3.x;
		mStarVerts[vIndex++]		= p3.y;
		mStarVerts[vIndex++]		= p3.z;
		mStarTexCoords[tIndex++]	= 0.0f;
		mStarTexCoords[tIndex++]	= 1.0f;
		mStarColors[cIndex++]		= col.r;
		mStarColors[cIndex++]		= col.g;
		mStarColors[cIndex++]		= col.b;
		mStarColors[cIndex++]		= col.a;
		
		mStarVerts[vIndex++]		= p2.x;
		mStarVerts[vIndex++]		= p2.y;
		mStarVerts[vIndex++]		= p2.z;
		mStarTexCoords[tIndex++]	= 1.0f;
		mStarTexCoords[tIndex++]	= 0.0f;
		mStarColors[cIndex++]		= col.r;
		mStarColors[cIndex++]		= col.g;
		mStarColors[cIndex++]		= col.b;
		mStarColors[cIndex++]		= col.a;
		
		mStarVerts[vIndex++]		= p3.x;
		mStarVerts[vIndex++]		= p3.y;
		mStarVerts[vIndex++]		= p3.z;
		mStarTexCoords[tIndex++]	= 0.0f;
		mStarTexCoords[tIndex++]	= 1.0f;
		mStarColors[cIndex++]		= col.r;
		mStarColors[cIndex++]		= col.g;
		mStarColors[cIndex++]		= col.b;
		mStarColors[cIndex++]		= col.a;
		
		mStarVerts[vIndex++]		= p4.x;
		mStarVerts[vIndex++]		= p4.y;
		mStarVerts[vIndex++]		= p4.z;
		mStarTexCoords[tIndex++]	= 1.0f;
		mStarTexCoords[tIndex++]	= 1.0f;
		mStarColors[cIndex++]		= col.r;
		mStarColors[cIndex++]		= col.g;
		mStarColors[cIndex++]		= col.b;
		mStarColors[cIndex++]		= col.a;
	}
}

void World::drawStarsVertexArray( const Matrix44f &mat )
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	
	glVertexPointer( 3, GL_FLOAT, 0, mStarVerts );
	glTexCoordPointer( 2, GL_FLOAT, 0, mStarTexCoords );
	glColorPointer( 4, GL_FLOAT, 0, mStarColors );
	
	gl::pushModelView();
	gl::rotate( mat );
	glDrawArrays( GL_TRIANGLES, 0, mTotalStarVertices );
	gl::popModelView();
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
}

void World::buildStarGlowsVertexArray( const Vec3f &bbRight, const Vec3f &bbUp, float zoomAlpha )
{
	mTotalStarGlowVertices	= mData->mFilteredArtists.size() * 6;	// 6 = 2 triangles per quad
	
    if (mTotalStarGlowVertices != mPrevTotalStarGlowVertices) {
        if (mStarGlowVerts != NULL)		delete[] mStarGlowVerts; 
		if (mStarGlowTexCoords != NULL) delete[] mStarGlowTexCoords; 
		if (mStarGlowColors != NULL)	delete[] mStarGlowColors;
		
        mStarGlowVerts			= new float[mTotalStarGlowVertices*3];
        mStarGlowTexCoords		= new float[mTotalStarGlowVertices*2];
        mStarGlowColors			= new float[mTotalStarGlowVertices*4];
		
        mPrevTotalStarGlowVertices = mTotalStarGlowVertices;
    }
	
	int vIndex = 0;
	int tIndex = 0;
	int cIndex = 0;
	
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		if( (*it)->mIsHighlighted ){
			Vec3f pos			= (*it)->mPos;
			float r				= (*it)->mRadius * ( (*it)->mEclipseStrength * 2.0f + 1.75f ); // HERE IS WHERE YOU CAN MAKE THE GLOW HUGER/BIGGER/AWESOMER
			//if( !(*it)->mIsSelected )
			//	r				-= zoomAlpha;
			
			float alpha			= (*it)->mDistFromCamZAxisPer * ( 1.0f - (*it)->mEclipseStrength );
			if( !(*it)->mIsSelected && !(*it)->mIsPlaying )
				alpha			= 1.0f - zoomAlpha;
			
			ColorA col			= ColorA( (*it)->mGlowColor, alpha );
			
			Vec3f right			= bbRight * r;
			Vec3f up			= bbUp * r;
			
			Vec3f p1			= pos - right - up;
			Vec3f p2			= pos + right - up;
			Vec3f p3			= pos - right + up;
			Vec3f p4			= pos + right + up;
			
			mStarGlowVerts[vIndex++]		= p1.x;
			mStarGlowVerts[vIndex++]		= p1.y;
			mStarGlowVerts[vIndex++]		= p1.z;
			mStarGlowTexCoords[tIndex++]	= 0.0f;
			mStarGlowTexCoords[tIndex++]	= 0.0f;
			mStarGlowColors[cIndex++]		= col.r;
			mStarGlowColors[cIndex++]		= col.g;
			mStarGlowColors[cIndex++]		= col.b;
			mStarGlowColors[cIndex++]		= col.a;
			
			mStarGlowVerts[vIndex++]		= p2.x;
			mStarGlowVerts[vIndex++]		= p2.y;
			mStarGlowVerts[vIndex++]		= p2.z;
			mStarGlowTexCoords[tIndex++]	= 1.0f;
			mStarGlowTexCoords[tIndex++]	= 0.0f;
			mStarGlowColors[cIndex++]		= col.r;
			mStarGlowColors[cIndex++]		= col.g;
			mStarGlowColors[cIndex++]		= col.b;
			mStarGlowColors[cIndex++]		= col.a;
			
			mStarGlowVerts[vIndex++]		= p3.x;
			mStarGlowVerts[vIndex++]		= p3.y;
			mStarGlowVerts[vIndex++]		= p3.z;
			mStarGlowTexCoords[tIndex++]	= 0.0f;
			mStarGlowTexCoords[tIndex++]	= 1.0f;
			mStarGlowColors[cIndex++]		= col.r;
			mStarGlowColors[cIndex++]		= col.g;
			mStarGlowColors[cIndex++]		= col.b;
			mStarGlowColors[cIndex++]		= col.a;
			
			mStarGlowVerts[vIndex++]		= p2.x;
			mStarGlowVerts[vIndex++]		= p2.y;
			mStarGlowVerts[vIndex++]		= p2.z;
			mStarGlowTexCoords[tIndex++]	= 1.0f;
			mStarGlowTexCoords[tIndex++]	= 0.0f;
			mStarGlowColors[cIndex++]		= col.r;
			mStarGlowColors[cIndex++]		= col.g;
			mStarGlowColors[cIndex++]		= col.b;
			mStarGlowColors[cIndex++]		= col.a;
			
			mStarGlowVerts[vIndex++]		= p3.x;
			mStarGlowVerts[vIndex++]		= p3.y;
			mStarGlowVerts[vIndex++]		= p3.z;
			mStarGlowTexCoords[tIndex++]	= 0.0f;
			mStarGlowTexCoords[tIndex++]	= 1.0f;
			mStarGlowColors[cIndex++]		= col.r;
			mStarGlowColors[cIndex++]		= col.g;
			mStarGlowColors[cIndex++]		= col.b;
			mStarGlowColors[cIndex++]		= col.a;
			
			mStarGlowVerts[vIndex++]		= p4.x;
			mStarGlowVerts[vIndex++]		= p4.y;
			mStarGlowVerts[vIndex++]		= p4.z;
			mStarGlowTexCoords[tIndex++]	= 1.0f;
			mStarGlowTexCoords[tIndex++]	= 1.0f;
			mStarGlowColors[cIndex++]		= col.r;
			mStarGlowColors[cIndex++]		= col.g;
			mStarGlowColors[cIndex++]		= col.b;
			mStarGlowColors[cIndex++]		= col.a;
		}
	}
}

void World::buildPlanetRingsVertexArray()
{
    if (mPlanetRingVerts != NULL) delete[] mPlanetRingVerts;
    if (mPlanetRingTexCoords != NULL) delete[] mPlanetRingTexCoords;
    
	mPlanetRingVerts		= new float[18];
	mPlanetRingTexCoords	= new float[12];
	int i = 0;
	int t = 0;
	Vec3f corner;
	float w	= 1.0f;
	
	corner			= Vec3f( -w, 0.0f, -w );
	mPlanetRingVerts[i++]			= corner.x;
	mPlanetRingVerts[i++]			= corner.y;
	mPlanetRingVerts[i++]			= corner.z;
	mPlanetRingTexCoords[t++]		= 0.0f;
	mPlanetRingTexCoords[t++]		= 0.0f;
	
	corner			= Vec3f( w, 0.0f, -w );
	mPlanetRingVerts[i++]			= corner.x;
	mPlanetRingVerts[i++]			= corner.y;
	mPlanetRingVerts[i++]			= corner.z;
	mPlanetRingTexCoords[t++]		= 1.0f;
	mPlanetRingTexCoords[t++]		= 0.0f;
	
	corner			= Vec3f( w, 0.0f, w );	
	mPlanetRingVerts[i++]			= corner.x;
	mPlanetRingVerts[i++]			= corner.y;
	mPlanetRingVerts[i++]			= corner.z;
	mPlanetRingTexCoords[t++]		= 1.0f;
	mPlanetRingTexCoords[t++]		= 1.0f;
	
	corner			= Vec3f( -w, 0.0f, -w );
	mPlanetRingVerts[i++]			= corner.x;
	mPlanetRingVerts[i++]			= corner.y;
	mPlanetRingVerts[i++]			= corner.z;
	mPlanetRingTexCoords[t++]		= 0.0f;
	mPlanetRingTexCoords[t++]		= 0.0f;
	
	corner			= Vec3f( w, 0.0f, w );	
	mPlanetRingVerts[i++]			= corner.x;
	mPlanetRingVerts[i++]			= corner.y;
	mPlanetRingVerts[i++]			= corner.z;
	mPlanetRingTexCoords[t++]		= 1.0f;
	mPlanetRingTexCoords[t++]		= 1.0f;
	
	corner			= Vec3f( -w, 0.0f, w );	
	mPlanetRingVerts[i++]			= corner.x;
	mPlanetRingVerts[i++]			= corner.y;
	mPlanetRingVerts[i++]			= corner.z;
	mPlanetRingTexCoords[t++]		= 0.0f;
	mPlanetRingTexCoords[t++]		= 1.0f;
}


void World::buildOrbitRingsVertexArray()
{
	if( mRingVertsLowRes	!= NULL ) delete[] mRingVertsLowRes;
	if( mRingTexLowRes		!= NULL ) delete[] mRingTexLowRes;
	if( mRingVertsHighRes	!= NULL ) delete[] mRingVertsHighRes;
	if( mRingTexHighRes		!= NULL ) delete[] mRingTexHighRes;
	
	mRingVertsLowRes	= new float[ G_RING_LOW_RES*2 ];	// X,Y
	mRingTexLowRes		= new float[ G_RING_LOW_RES*2 ];	// U,V
	mRingVertsHighRes	= new float[ G_RING_HIGH_RES*2 ];	// X,Y
	mRingTexHighRes		= new float[ G_RING_HIGH_RES*2 ];	// U,V
	
	Color c				= BRIGHT_BLUE;
	
	for( int i=0; i<G_RING_LOW_RES; i++ ){
		float per				 = (float)i/(float)(G_RING_LOW_RES-1);
		float angle				 = per * TWO_PI;
		mRingVertsLowRes[i*2+0]	 = cos( angle );
		mRingVertsLowRes[i*2+1]	 = sin( angle );
		mRingTexLowRes[i*2+0] = per;
		mRingTexLowRes[i*2+1] = 0.5f;
	}
	
	for( int i=0; i<G_RING_HIGH_RES; i++ ){
		float per				 = (float)i/(float)(G_RING_HIGH_RES-1);
		float angle				 = per * TWO_PI;
		mRingVertsHighRes[i*2+0] = cos( angle );
		mRingVertsHighRes[i*2+1] = sin( angle );
		mRingTexHighRes[i*2+0] = per;
		mRingTexHighRes[i*2+1] = 0.5f;
	}
}

void World::update( const Matrix44f &mat )
{
	if( mIsInitialized ){
		mAge ++;
	
	
		if( mAge == mEndRepulseAge ){
			mIsRepulsing = false;
		}
		
		if( mAge == mEndRepulseAge + 100 ){
			buildConstellation();
		}
		
		if( mIsRepulsing ){
			repulseNodes();
		}
		
		for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
			(*it)->update( mat );
		}
	}
}

void World::repulseNodes()
{
	for( vector<Node*>::iterator p1 = mNodes.begin(); p1 != mNodes.end(); ++p1 ){
		
		vector<Node*>::iterator p2 = p1;
		for( ++p2; p2 != mNodes.end(); ++p2 ) {
			Vec3f dir = (*p1)->mPosDest - (*p2)->mPosDest;
			
			float thresh = 50.0f;
			if( dir.x > -thresh && dir.x < thresh && dir.y > -thresh && dir.y < thresh && dir.z > -thresh && dir.z < thresh ){
				float distSqrd = dir.lengthSquared();
				
				if( distSqrd > 0.0f ){
					float F = 1.0f/distSqrd;
					dir = F * dir.normalized() * 0.75f;
					dir.y *= 0.7f;
					
					// acceleration = force / mass
					(*p1)->mAcc += dir;
					(*p2)->mAcc -= dir;
				}
			}
		}
	}
}



void World::updateGraphics( const CameraPersp &cam, const Vec3f &bbRight, const Vec3f &bbUp )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->updateGraphics( cam, bbRight, bbUp );
	}
}


void World::drawStarGlowsVertexArray( const Matrix44f &mat )
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	
	glVertexPointer( 3, GL_FLOAT, 0, mStarGlowVerts );
	glTexCoordPointer( 2, GL_FLOAT, 0, mStarGlowTexCoords );
	glColorPointer( 4, GL_FLOAT, 0, mStarGlowColors );
	
	gl::pushModelView();
	gl::rotate( mat );
	glDrawArrays( GL_TRIANGLES, 0, mTotalStarGlowVertices );
	gl::popModelView();
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
}

void World::drawEclipseGlows()
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawEclipseGlow();
	}
}

void World::drawPlanets( const vector<gl::Texture> &planets )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawPlanet( planets );
	}
}

void World::drawClouds( const vector<gl::Texture> &clouds )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawClouds( clouds );
	}
}

void World::drawRings( const gl::Texture &tex, float camZPos )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawRings( tex, mPlanetRingVerts, mPlanetRingTexCoords, camZPos );
	}
}

void World::drawNames( const CameraPersp &cam, float pinchAlphaOffset, float angle )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		if( (*it)->mIsHighlighted ){
			(*it)->drawName( cam, pinchAlphaOffset, angle );
		}
	}
}

void World::drawOrbitRings( float pinchAlphaOffset, float camAlpha, const gl::Texture &orbitRingGradient )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawOrbitRing( pinchAlphaOffset, camAlpha, orbitRingGradient, mRingVertsLowRes, mRingTexLowRes, mRingVertsHighRes, mRingTexHighRes );
	}
}

void World::drawTouchHighlights( float zoomAlpha )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawTouchHighlight( zoomAlpha );
	}
}

void World::drawConstellation( const Matrix44f &mat )
{
	if( mTotalConstellationVertices > 2 ){
		float zoomPer = ( 1.0f - (G_ZOOM-1.0f) ) * 0.4f;
		
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		//glEnableClientState( GL_COLOR_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mConstellationVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mConstellationTexCoords );
		//glColorPointer( 4, GL_FLOAT, 0, mColors );
		
		gl::pushModelView();
		gl::rotate( mat );
		gl::color( ColorA( 0.12f, 0.25f, 0.85f, zoomPer ) );
		glDrawArrays( GL_LINES, 0, mTotalConstellationVertices );
		gl::popModelView();
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
		//glDisableClientState( GL_COLOR_ARRAY );
	}
}


void World::buildConstellation()
{
	mConstellation.clear();
	//mConstellationColors.clear();
	
	

	// CREATE DATA FOR CONSTELLATION
	vector<float> distances;	// used for tex coords of the dotted line
	for( vector<int>::iterator it1 = mData->mFilteredArtists.begin(); it1 != mData->mFilteredArtists.end(); ++it1 ){
		Node *child1 = mNodes[*it1];
		float shortestDist = 5000.0f;
		Node *nearestChild;
		
		vector<int>::iterator it2 = it1;
		for( ++it2; it2 != mData->mFilteredArtists.end(); ++it2 ) {
			Node *child2 = mNodes[*it2];
			
			Vec3f dirBetweenChildren = child1->mPosDest - child2->mPosDest;
			float distBetweenChildren = dirBetweenChildren.length();
			if( distBetweenChildren < shortestDist ){
				shortestDist = distBetweenChildren;
				nearestChild = child2;
			}
		}
		
		distances.push_back( shortestDist );
		mConstellation.push_back( child1->mPosDest );
		mConstellation.push_back( nearestChild->mPosDest );
		
		//mConstellationColors.push_back( ColorA( child1->mGlowColor, 0.15f ) );
		//mConstellationColors.push_back( ColorA( nearestChild->mGlowColor, 0.15f ) );
	}
	
	
	/*
	// CONSTELLATION IN ALPHABETICAL ORDER
	Node *child1;
	Node *child2;
	int index = 0;
	vector<float> distances;	// used for tex coords of the dotted line
	for( vector<int>::iterator it = mData->mFilteredArtists.begin(); it != mData->mFilteredArtists.end(); ++it ){
		if( index > 0 ){
			child2 = mNodes[*it];
				
			Vec3f dirBetweenChildren = child1->mPosDest - child2->mPosDest;
			float distBetweenChildren = dirBetweenChildren.length();
			
			distances.push_back( distBetweenChildren );
			mConstellation.push_back( child1->mPosDest );
			mConstellation.push_back( child2->mPosDest );
		}
		child1 = mNodes[*it];
		index ++;
	}
	*/



	mTotalConstellationVertices	= mConstellation.size();
	if (mTotalConstellationVertices != mPrevTotalConstellationVertices) {
		if (mConstellationVerts != NULL) delete[] mConstellationVerts; 
		if (mConstellationTexCoords != NULL) delete[] mConstellationTexCoords; 
		
		mConstellationVerts			= new float[mTotalConstellationVertices*3];
		mConstellationTexCoords		= new float[mTotalConstellationVertices*2];
		mPrevTotalConstellationVertices = mTotalConstellationVertices;
	}
	
	
	
	
	int vIndex = 0;
	int tIndex = 0;
	int distancesIndex = 0;
	for( int i=0; i<mTotalConstellationVertices; i++ ){
		Vec3f pos = mConstellation[i];
		mConstellationVerts[vIndex++]	= pos.x;
		mConstellationVerts[vIndex++]	= pos.y;
		mConstellationVerts[vIndex++]	= pos.z;
		
		if( i%2 == 0 ){
			mConstellationTexCoords[tIndex++]	= 0.0f;
			mConstellationTexCoords[tIndex++]	= 0.5f;
		} else {
			mConstellationTexCoords[tIndex++]	= distances[distancesIndex] * 0.4f;
			mConstellationTexCoords[tIndex++]	= 0.5f;
			distancesIndex ++;
		}
	}
}

std::vector<Node*> World::getDepthSortedNodes(int fromGen, int toGen)
{
    std::vector<Node*> sortedNodes;

    if (mNodes.size() > 0) {
        std::deque<Node*> queue;
        
        // initialize queue with all artist nodes
        queue.insert(queue.begin(), mNodes.begin(), mNodes.end());

        while (queue.size() > 0) {
            Node* node = queue.front();
            // remove
            queue.pop_front();
            // collect this node if it's valid
            if (node->mGen >= fromGen && node->mGen <= toGen) {
                sortedNodes.push_back(node);
            }
            // add all node's children to the queue
            queue.insert(queue.end(), node->mChildNodes.begin(), node->mChildNodes.end());
        }
        
        sort(sortedNodes.begin(), sortedNodes.end(), nodeSortFunc);
    }
    
    return sortedNodes;
}

bool nodeSortFunc(Node* a, Node* b) {
    return a->mTransPos.z > b->mTransPos.z;
}

