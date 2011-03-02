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

using std::stringstream;
using namespace ci;
using namespace ci::app;
using namespace ci::ipod;
using namespace std;

World::World()
{

}

void World::initNodes( const Font &font )
{
	int i=0;
	for(vector<PlaylistRef>::iterator it = mData->mArtists.begin(); it != mData->mArtists.end(); ++it){
		PlaylistRef artist	= *it;
		string name			= artist->getArtistName();
		NodeArtist *newNode = new NodeArtist( NULL, i, font, name );
		mNodes.push_back( newNode );
	}
}

void World::filterNodes()
{
	deselectAllNodes();
	
	for(vector<int>::iterator it = mData->mFilteredArtists.begin(); it != mData->mFilteredArtists.end(); ++it){
		mNodes[*it]->mIsHighlighted = true;
	}
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

void World::update( const CameraPersp &cam, const Matrix44f &mat, const Vec3f &bbRight, const Vec3f &bbUp )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->update( cam, mat, bbRight, bbUp );
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
