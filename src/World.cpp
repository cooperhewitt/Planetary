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
	int i=0;
	for(vector<PlaylistRef>::iterator it = mData->mArtists.begin(); it != mData->mArtists.end(); ++it){
		PlaylistRef artist	= *it;
		string name			= artist->getArtistName();
		NodeArtist *newNode = new NodeArtist( NULL, i, font, name );
		newNode->setIPodPlayer( player );
		mNodes.push_back( newNode );
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

void World::checkForSphereIntersect( Node* &theNode, const Ray &ray, Matrix44f &mat )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		if( (*it)->mIsHighlighted ){
			(*it)->checkForSphereIntersect( theNode, ray, mat );
		}
	}
}

void World::update( const Matrix44f &mat, const Vec3f &bbRight, const Vec3f &bbUp )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->update( mat, bbRight, bbUp );
	}
}

void World::updateGraphics( const CameraPersp &cam )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->updateGraphics( cam );
	}
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

void World::drawNames()
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawName();
	}
}

void World::drawOrthoNames( const CameraPersp &cam )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		if( (*it)->mIsHighlighted ){
			(*it)->drawOrthoName( cam );
		}
	}
}

void World::drawSpheres()
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawSphere();
	}
}

void World::drawOrbitalRings()
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawOrbitalRings();
	}
}

void World::drawPlanets( const Matrix44f &accelMatrix, std::vector< gl::Texture*> texs )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawPlanet( accelMatrix, texs );
	}
}

void World::drawRings( gl::Texture *tex )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawRings( tex );
	}
}

void World::drawConstellation( const Matrix44f &mat )
{
	if( mTotalVertices > 1 ){
		float zoomPer = ( 1.0f - (G_ZOOM-1.0f) ) * 0.3f;
		gl::pushModelView();
		gl::rotate( mat );
		gl::color( ColorA( 0.5f, 0.6f, 1.0f, zoomPer ) );
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		//glEnableClientState( GL_COLOR_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mTexCoords );
		//glColorPointer( 4, GL_FLOAT, 0, mColors );
		glDrawArrays( GL_LINES, 0, mTotalVertices );
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );		
		//glDisableClientState( GL_COLOR_ARRAY );
		gl::popModelView();
	}
}


void World::buildConstellation()
{
	mConstellation.clear();
	//mConstellationColors.clear();
	
	// CREATE DATA FOR CONSTELLATION
	vector<float> distances;
	for( vector<int>::iterator it1 = mData->mFilteredArtists.begin(); it1 != mData->mFilteredArtists.end(); ++it1 ){
		Node *child1 = mNodes[(*it1)];
		float shortestDist = 1000.0f;
		Node *nearestChild;
		
		vector<int>::iterator it2 = it1;
		for( ++it2; it2 != mData->mFilteredArtists.end(); ++it2 ) {
			Node *child2 = mNodes[(*it2)];
			
			Vec3f dirBetweenChildren = child1->mPos - child2->mPos;
			float distBetweenChildren = dirBetweenChildren.length();
			if( distBetweenChildren < shortestDist ){
				shortestDist = distBetweenChildren;
				nearestChild = child2;
			}
		}
		
		distances.push_back( shortestDist * 0.35f );
		mConstellation.push_back( child1->mPos );
		mConstellation.push_back( nearestChild->mPos );
		
		//mConstellationColors.push_back( ColorA( child1->mGlowColor, 0.3f ) );
		//mConstellationColors.push_back( ColorA( nearestChild->mGlowColor, 0.3f ) );
	}
	
	mTotalVertices	= mConstellation.size();
	mVerts			= new float[mTotalVertices*3];
	mTexCoords		= new float[mTotalVertices*2];
	//mColors			= new float[mTotalVertices*4];
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
			mTexCoords[tIndex++]	= distances[distancesIndex];
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
