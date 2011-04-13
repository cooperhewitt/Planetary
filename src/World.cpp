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

    // VERTEX ARRAY SPHERE
	initSphereVertexArray( 32, &mNumSphereHiResVerts, mSphereHiResVerts, mSphereHiResTexCoords, mSphereHiResNormals );
	initSphereVertexArray( 16, &mNumSphereLoResVerts, mSphereLoResVerts, mSphereLoResTexCoords, mSphereLoResNormals );
	
	mAge = 0;
	mEndRepulseAge = 250;//200
	mIsRepulsing = true;
}


void World::initSphereVertexArray( int segments, int *numVerts, float* &sphereVerts, float* &sphereTexCoords, float* &sphereNormals )
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
				triangles.push_back( Triangle( oldv2, newv1, newv2 ) );
				
				texCoords.push_back( oldt1 );
				texCoords.push_back( oldt2 );
				texCoords.push_back( newt1 );
				
				texCoords.push_back( oldt2 );
				texCoords.push_back( newt1 );
				texCoords.push_back( newt2 );
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

void World::initNodes( Player *player, const Font &font )
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
		NodeArtist *newNode = new NodeArtist( i++, font );
		newNode->setData(artist);
		mNodes.push_back( newNode );
	}
	
	cout << (App::get()->getElapsedSeconds() - t) << " seconds to World::initNodes" << endl;

    initVertexArrays();
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
					dir = F * dir.normalized() * 3.5f;
					
					// acceleration = force / mass
					(*p1)->mAcc += dir;
					(*p2)->mAcc -= dir;
				}
			}
		}
	}
}


void World::initVertexArrays()
{
    if( mRingVertsLowRes != NULL ) delete[] mRingVertsLowRes;
	//if( mRingColorsLowRes != NULL ) delete[] mRingColorsLowRes;
    
	mRingVertsLowRes	= new float[ G_RING_LOW_RES*2 ]; // X,Y
	//mRingColorsLowRes	= new float[ G_RING_LOW_RES*4 ];
	for( int i=0; i<G_RING_LOW_RES; i++ ){
		float per				= (float)i/(float)(G_RING_LOW_RES-1);
		float angle				= per * TWO_PI;
		mRingVertsLowRes[i*2+0]	= cos( angle );
		mRingVertsLowRes[i*2+1]	= sin( angle );
		/*
		Color c	= lerp( COLOR_BRIGHT_BLUE, COLOR_BLUE, per );
		mRingColorsLowRes[i*4+0] = c.r;
		mRingColorsLowRes[i*4+1] = c.g;
		mRingColorsLowRes[i*4+2] = c.b;
		mRingColorsLowRes[i*4+3] = 1.0f - per;
		*/
	}

    if( mRingVertsHighRes != NULL ) delete[] mRingVertsHighRes;
	//if( mRingColorsHighRes != NULL ) delete[] mRingColorsHighRes;
	
	mRingVertsHighRes	= new float[ G_RING_HIGH_RES*2 ]; // X,Y
	//mRingColorsHighRes	= new float[ G_RING_HIGH_RES*4 ];
	for( int i=0; i<G_RING_HIGH_RES; i++ ){
		float per					= (float)i/(float)(G_RING_HIGH_RES-1);
		float angle					= per * TWO_PI;
		mRingVertsHighRes[i*2+0]	= cos( angle );
		mRingVertsHighRes[i*2+1]	= sin( angle );
		/*
		Color c	= lerp( COLOR_BRIGHT_BLUE, COLOR_BLUE, per );
		mRingColorsHighRes[i*4+0]	= c.r;
		mRingColorsHighRes[i*4+1]	= c.g;
		mRingColorsHighRes[i*4+2]	= c.b;
		mRingColorsHighRes[i*4+3]	= 1.0f - per;
		*/
	}
	
	buildPlanetRingsVertexArray();
    
    initNodeSphereData( mNumSphereHiResVerts, mSphereHiResVerts, mSphereHiResTexCoords, mSphereHiResNormals,
                              mNumSphereLoResVerts, mSphereLoResVerts, mSphereLoResTexCoords, mSphereLoResNormals ); 
}

void World::initNodeSphereData( int totalHiVertices, float *sphereHiVerts, float *sphereHiTexCoords, float *sphereHiNormals, 
							   int totalLoVertices, float *sphereLoVerts, float *sphereLoTexCoords, float *sphereLoNormals )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->setSphereData( totalHiVertices, sphereHiVerts, sphereHiTexCoords, sphereHiNormals, totalLoVertices, sphereLoVerts, sphereLoTexCoords, sphereLoNormals );
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
				if( trackNode->mIsPlaying ){
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

void World::checkForSphereIntersect( vector<Node*> &nodes, const Ray &ray, Matrix44f &mat )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		if( (*it)->mIsHighlighted ){
			(*it)->checkForSphereIntersect( nodes, ray, mat );
		}
	}
}

void World::buildStarsVertexArray( const Vec3f &bbRight, const Vec3f &bbUp )
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
	
	int vIndex = 0;
	int tIndex = 0;
	int cIndex = 0;
	
	float u1				= 0.0f;
	float u2				= 1.0f;
	float v1				= 0.0f;
	float v2				= 1.0f;
	
	// TODO: figure out why we use inverted matrix * billboard vec
	
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		Vec3f pos				= (*it)->mPos;
		float radius			= (*it)->mRadius * 0.5f;
		if( (*it)->mIsHighlighted ) radius += math<float>::max( G_ARTIST_LEVEL - G_ZOOM, 0.0f );
		
		ColorA col				= ColorA( (*it)->mColor, 1.0f );
		
		Vec3f right				= bbRight * radius;
		Vec3f up				= bbUp * radius;
		
		Vec3f p1				= pos - right - up;
		Vec3f p2				= pos + right - up;
		Vec3f p3				= pos - right + up;
		Vec3f p4				= pos + right + up;
		
		mStarVerts[vIndex++]		= p1.x;
		mStarVerts[vIndex++]		= p1.y;
		mStarVerts[vIndex++]		= p1.z;
		mStarTexCoords[tIndex++]	= u1;
		mStarTexCoords[tIndex++]	= v1;
		mStarColors[cIndex++]		= col.r;
		mStarColors[cIndex++]		= col.g;
		mStarColors[cIndex++]		= col.b;
		mStarColors[cIndex++]		= col.a;
		
		mStarVerts[vIndex++]		= p2.x;
		mStarVerts[vIndex++]		= p2.y;
		mStarVerts[vIndex++]		= p2.z;
		mStarTexCoords[tIndex++]	= u2;
		mStarTexCoords[tIndex++]	= v1;
		mStarColors[cIndex++]		= col.r;
		mStarColors[cIndex++]		= col.g;
		mStarColors[cIndex++]		= col.b;
		mStarColors[cIndex++]		= col.a;
		
		mStarVerts[vIndex++]		= p3.x;
		mStarVerts[vIndex++]		= p3.y;
		mStarVerts[vIndex++]		= p3.z;
		mStarTexCoords[tIndex++]	= u1;
		mStarTexCoords[tIndex++]	= v2;
		mStarColors[cIndex++]		= col.r;
		mStarColors[cIndex++]		= col.g;
		mStarColors[cIndex++]		= col.b;
		mStarColors[cIndex++]		= col.a;
		
		mStarVerts[vIndex++]		= p2.x;
		mStarVerts[vIndex++]		= p2.y;
		mStarVerts[vIndex++]		= p2.z;
		mStarTexCoords[tIndex++]	= u2;
		mStarTexCoords[tIndex++]	= v1;
		mStarColors[cIndex++]		= col.r;
		mStarColors[cIndex++]		= col.g;
		mStarColors[cIndex++]		= col.b;
		mStarColors[cIndex++]		= col.a;
		
		mStarVerts[vIndex++]		= p3.x;
		mStarVerts[vIndex++]		= p3.y;
		mStarVerts[vIndex++]		= p3.z;
		mStarTexCoords[tIndex++]	= u1;
		mStarTexCoords[tIndex++]	= v2;
		mStarColors[cIndex++]		= col.r;
		mStarColors[cIndex++]		= col.g;
		mStarColors[cIndex++]		= col.b;
		mStarColors[cIndex++]		= col.a;
		
		mStarVerts[vIndex++]		= p4.x;
		mStarVerts[vIndex++]		= p4.y;
		mStarVerts[vIndex++]		= p4.z;
		mStarTexCoords[tIndex++]	= u2;
		mStarTexCoords[tIndex++]	= v2;
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

void World::buildStarGlowsVertexArray( const Vec3f &bbRight, const Vec3f &bbUp )
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
	
	float u1				= 0.0f;
	float u2				= 1.0f;
	float v1				= 0.0f;
	float v2				= 1.0f;
	float zoomOffset		= math<float>::max( G_ARTIST_LEVEL - G_ZOOM, 0.0f );
	
	// TODO: figure out why we use inverted matrix * billboard vec
	
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		if( (*it)->mIsHighlighted ){
			Vec3f pos				= (*it)->mPos;
			float flickerAmt		= ( 8.5f + zoomOffset * Rand::randFloat( 12.0f, 15.0f ) );
			float radius			= (*it)->mRadius * 0.5f * flickerAmt;
			
			float glowAlpha			= 1.0f;
			
			if( !(*it)->mIsSelected && !(*it)->mIsPlaying ){
				glowAlpha = zoomOffset;
			}
			   
			   
			ColorA col				= ColorA( (*it)->mGlowColor, (*it)->mDistFromCamZAxisPer * glowAlpha * 0.5f );
			
			Vec3f right				= bbRight * radius;
			Vec3f up				= bbUp * radius;
			
			Vec3f p1				= pos - right - up;
			Vec3f p2				= pos + right - up;
			Vec3f p3				= pos - right + up;
			Vec3f p4				= pos + right + up;
			
			mStarGlowVerts[vIndex++]		= p1.x;
			mStarGlowVerts[vIndex++]		= p1.y;
			mStarGlowVerts[vIndex++]		= p1.z;
			mStarGlowTexCoords[tIndex++]	= u1;
			mStarGlowTexCoords[tIndex++]	= v1;
			mStarGlowColors[cIndex++]		= col.r;
			mStarGlowColors[cIndex++]		= col.g;
			mStarGlowColors[cIndex++]		= col.b;
			mStarGlowColors[cIndex++]		= col.a;
			
			mStarGlowVerts[vIndex++]		= p2.x;
			mStarGlowVerts[vIndex++]		= p2.y;
			mStarGlowVerts[vIndex++]		= p2.z;
			mStarGlowTexCoords[tIndex++]	= u2;
			mStarGlowTexCoords[tIndex++]	= v1;
			mStarGlowColors[cIndex++]		= col.r;
			mStarGlowColors[cIndex++]		= col.g;
			mStarGlowColors[cIndex++]		= col.b;
			mStarGlowColors[cIndex++]		= col.a;
			
			mStarGlowVerts[vIndex++]		= p3.x;
			mStarGlowVerts[vIndex++]		= p3.y;
			mStarGlowVerts[vIndex++]		= p3.z;
			mStarGlowTexCoords[tIndex++]	= u1;
			mStarGlowTexCoords[tIndex++]	= v2;
			mStarGlowColors[cIndex++]		= col.r;
			mStarGlowColors[cIndex++]		= col.g;
			mStarGlowColors[cIndex++]		= col.b;
			mStarGlowColors[cIndex++]		= col.a;
			
			mStarGlowVerts[vIndex++]		= p2.x;
			mStarGlowVerts[vIndex++]		= p2.y;
			mStarGlowVerts[vIndex++]		= p2.z;
			mStarGlowTexCoords[tIndex++]	= u2;
			mStarGlowTexCoords[tIndex++]	= v1;
			mStarGlowColors[cIndex++]		= col.r;
			mStarGlowColors[cIndex++]		= col.g;
			mStarGlowColors[cIndex++]		= col.b;
			mStarGlowColors[cIndex++]		= col.a;
			
			mStarGlowVerts[vIndex++]		= p3.x;
			mStarGlowVerts[vIndex++]		= p3.y;
			mStarGlowVerts[vIndex++]		= p3.z;
			mStarGlowTexCoords[tIndex++]	= u1;
			mStarGlowTexCoords[tIndex++]	= v2;
			mStarGlowColors[cIndex++]		= col.r;
			mStarGlowColors[cIndex++]		= col.g;
			mStarGlowColors[cIndex++]		= col.b;
			mStarGlowColors[cIndex++]		= col.a;
			
			mStarGlowVerts[vIndex++]		= p4.x;
			mStarGlowVerts[vIndex++]		= p4.y;
			mStarGlowVerts[vIndex++]		= p4.z;
			mStarGlowTexCoords[tIndex++]	= u2;
			mStarGlowTexCoords[tIndex++]	= v2;
			mStarGlowColors[cIndex++]		= col.r;
			mStarGlowColors[cIndex++]		= col.g;
			mStarGlowColors[cIndex++]		= col.b;
			mStarGlowColors[cIndex++]		= col.a;
		}
	}
}


void World::update( const Matrix44f &mat )
{
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

void World::drawClouds( const vector<gl::Texture> &planets, const vector<gl::Texture> &clouds )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawClouds( planets, clouds );
	}
}

void World::drawRings( const gl::Texture &tex, float camRingAlpha )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawRings( tex, mPlanetRingVerts, mPlanetRingTexCoords, camRingAlpha );
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

void World::drawOrbitRings( float pinchAlphaOffset )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawOrbitRing( pinchAlphaOffset, mRingVertsLowRes, mRingVertsHighRes );
	}
}

void World::drawTouchHighlights()
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawTouchHighlight();
	}
}

void World::drawConstellation( const Matrix44f &mat )
{
	if( mTotalConstellationVertices > 2 ){
		float zoomPer = ( 1.0f - (G_ZOOM-1.0f) ) * 0.2f;
		
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
    return a->mTransPos.z < b->mTransPos.z;
}

