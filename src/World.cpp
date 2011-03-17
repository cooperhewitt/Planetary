/*
 *  World.cpp
 *  Kepler
 *
 *  Created by Robert Hodgin on 2/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "World.h"
#include "NodeArtist.h"
#include "cinder/gl/gl.h"
#include "cinder/Rect.h"
#include "cinder/Text.h"
#include "cinder/Rand.h"
#include "Globals.h"

using std::stringstream;
using namespace ci;
using namespace ci::app;
using namespace ci::ipod;
using namespace std;

World::World()
{

}

void World::initNodes( Player *player, const Font &font )
{
	float t = App::get()->getElapsedSeconds();
	
	int i=0;
	for(vector<PlaylistRef>::iterator it = mData->mArtists.begin(); it != mData->mArtists.end(); ++it){
		PlaylistRef artist	= *it;
		NodeArtist *newNode = new NodeArtist( NULL, i, font );
		newNode->setData(artist);
		mNodes.push_back( newNode );
	}
	
	cout << (App::get()->getElapsedSeconds() - t) << " seconds to World::initNodes" << endl;
}

void World::initRingVertexArray()
{
	mRingVertsLowRes	= new float[ G_RING_LOW_RES*2 ]; // X,Y
	for( int i=0; i<G_RING_LOW_RES; i++ ){
		float per				= (float)i/(float)(G_RING_LOW_RES-1);
		float angle				= per * TWO_PI;
		mRingVertsLowRes[i*2+0]	= cos( angle );
		mRingVertsLowRes[i*2+1]	= sin( angle );
	}
	
	mRingVertsHighRes	= new float[ G_RING_HIGH_RES*2 ]; // X,Y
	for( int i=0; i<G_RING_HIGH_RES; i++ ){
		float per					= (float)i/(float)(G_RING_HIGH_RES-1);
		float angle					= per * TWO_PI;
		mRingVertsHighRes[i*2+0]	= cos( angle );
		mRingVertsHighRes[i*2+1]	= sin( angle );
	}
}


void World::filterNodes()
{
	deselectAllNodes();
	
	for(vector<int>::iterator it = mData->mFilteredArtists.begin(); it != mData->mFilteredArtists.end(); ++it){
		mNodes[*it]->mIsHighlighted = true;
	}
	
	buildConstellation();
}

void World::deselectAllNodes()
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->mIsHighlighted = false;
		(*it)->mIsSelected = false;
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

void World::update( const Matrix44f &mat )
{
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

void World::buildStarsVertexArray( const Vec3f &bbRight, const Vec3f &bbUp )
{
	mTotalStarVertices	= mData->mArtists.size() * 6;	// 6 = 2 triangles per quad
	mStarVerts			= new float[mTotalStarVertices*3];
	mStarTexCoords		= new float[mTotalStarVertices*2];
	//mStarColors			= new float[mTotalStarVertices*4];
	int vIndex = 0;
	int tIndex = 0;
	//int cIndex = 0;
	
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		Vec3f pos				= (*it)->mPos;
		Vec3f p1				= pos - bbRight - bbUp;
		Vec3f p2				= pos + bbRight - bbUp;
		Vec3f p3				= pos - bbRight + bbUp;
		Vec3f p4				= pos + bbRight + bbUp;

		float radius			= (*it)->mRadius;
		
		float x1				= pos.x - radius;
		float x2				= pos.x + radius;
		float y1				= pos.y - radius;
		float y2				= pos.y + radius;
		float z					= pos.z;
		
		float u1				= 0.0f;
		float u2				= 1.0f;
		float v1				= 0.0f;
		float v2				= 1.0f;
		
		mStarVerts[vIndex++]		= x1;
		mStarVerts[vIndex++]		= y1;
		mStarVerts[vIndex++]		= z;
		mStarTexCoords[tIndex++]	= u1;
		mStarTexCoords[tIndex++]	= v1;
		
		mStarVerts[vIndex++]		= x2;
		mStarVerts[vIndex++]		= y1;
		mStarVerts[vIndex++]		= z;
		mStarTexCoords[tIndex++]	= u2;
		mStarTexCoords[tIndex++]	= v1;
		
		mStarVerts[vIndex++]		= x1;
		mStarVerts[vIndex++]		= y2;
		mStarVerts[vIndex++]		= z;
		mStarTexCoords[tIndex++]	= u1;
		mStarTexCoords[tIndex++]	= v2;
		
		mStarVerts[vIndex++]		= x2;
		mStarVerts[vIndex++]		= y1;
		mStarVerts[vIndex++]		= z;
		mStarTexCoords[tIndex++]	= u2;
		mStarTexCoords[tIndex++]	= v1;
		
		mStarVerts[vIndex++]		= x1;
		mStarVerts[vIndex++]		= y2;
		mStarVerts[vIndex++]		= z;
		mStarTexCoords[tIndex++]	= u1;
		mStarTexCoords[tIndex++]	= v2;
		
		mStarVerts[vIndex++]		= x2;
		mStarVerts[vIndex++]		= y2;
		mStarVerts[vIndex++]		= z;
		mStarTexCoords[tIndex++]	= u2;
		mStarTexCoords[tIndex++]	= v2;
	}
}

void World::drawStarsVertexArray( const Matrix44f &mat )
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer( 3, GL_FLOAT, 0, mStarVerts );
	glTexCoordPointer( 2, GL_FLOAT, 0, mStarTexCoords );

	gl::pushModelView();
	gl::rotate( mat );
	gl::color( ColorA( 1, 1, 1, 1 ) );
	glDrawArrays( GL_TRIANGLES, 0, mTotalStarVertices );
	gl::popModelView();
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
}

void World::drawStars()
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawStar();
	}
}

void World::drawStarGlows()
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawStarGlow();
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

void World::drawRings( const gl::Texture &tex )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawRings( tex );
	}
}

void World::drawNames( const CameraPersp &cam, float pinchAlphaOffset )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		if( (*it)->mIsHighlighted ){
			(*it)->drawName( cam, pinchAlphaOffset );
		}
	}
}

void World::drawOrbitRings( NodeTrack *playingNode )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawOrbitRing( playingNode, mRingVertsLowRes, mRingVertsHighRes );
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
	if( mTotalVertices > 2 ){
		float zoomPer = ( 1.0f - (G_ZOOM-1.0f) ) * 0.2f;
		
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		//glEnableClientState( GL_COLOR_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mTexCoords );
		//glColorPointer( 4, GL_FLOAT, 0, mColors );
		
		gl::pushModelView();
		gl::rotate( mat );
		gl::color( ColorA( 0.12f, 0.25f, 0.85f, zoomPer ) );
		glDrawArrays( GL_LINES, 0, mTotalVertices );
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
		float shortestDist = 1000.0f;
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
	
	mTotalVertices	= mConstellation.size();
	mVerts			= new float[mTotalVertices*3];
	mTexCoords		= new float[mTotalVertices*2];
	mColors			= new float[mTotalVertices*4];
	int vIndex = 0;
	int tIndex = 0;
	//int cIndex = 0;
	int distancesIndex = 0;
	for( int i=0; i<mTotalVertices; i++ ){
		Vec3f pos			= mConstellation[i];
		mVerts[vIndex++]	= pos.x;
		mVerts[vIndex++]	= pos.y;
		mVerts[vIndex++]	= pos.z;
		
		if( i%2 == 0 ){
			mTexCoords[tIndex++]	= 0.0f;
			mTexCoords[tIndex++]	= 0.5f;
		} else {
			mTexCoords[tIndex++]	= distances[distancesIndex] * 0.5f;
			mTexCoords[tIndex++]	= 0.5f;
			distancesIndex ++;
		}
		/*
		ColorA c			= mConstellationColors[i];
		mColors[cIndex++]	= c.r;
		mColors[cIndex++]	= c.g;
		mColors[cIndex++]	= c.b;
		mColors[cIndex++]	= c.a;
		*/
	}
}
