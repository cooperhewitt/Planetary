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
#include "BloomSphere.h"

using std::stringstream;
using namespace ci;
using namespace ci::app;
using namespace ci::ipod;
using namespace std;

World::World()
{
	mPrevTotalConstellationVertices = -1;
	mConstellationVerts			= NULL;
	mConstellationTexCoords		= NULL;
}

void World::setup( Data *data )
{
    mData = data;

    // FIXME: this check for mIsInitialized looks wrong (it's set to false below)
    // ... when is it set to true? ... in anycase, these should only be set once :)
	if( !mIsInitialized ){
		// VERTEX ARRAY SPHERE
		if( G_IS_IPAD2 ){
            mHiSphere.setup(32);
            mMdSphere.setup(20);
            mLoSphere.setup(16);
            mTySphere.setup(10);
		} else {
            mHiSphere.setup(32);
            mMdSphere.setup(16);
            mLoSphere.setup(12);
            mTySphere.setup(8);
		}
	}
	
	mAge			= 0;
	mEndRepulseAge	= 50;//200
	mIsRepulsing	= true;
	mIsInitialized	= false;
}

void World::initNodes( Player *player, const Font &font, const Font &smallFont, const Surface &highResSurfaces, const Surface &lowResSurfaces, const Surface &noAlbumArt )
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
		NodeArtist *newNode = new NodeArtist( i++, font, smallFont, highResSurfaces, lowResSurfaces, noAlbumArt );
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
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->setSphereData( &mHiSphere, &mMdSphere, &mLoSphere, &mTySphere );
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
				if( trackNode->mIsPlaying && !trackNode->isDying() ){
					mPlayingTrackNode = (NodeTrack*)trackNode;
				}
            }            
        }
    }
}

Node* World::getPlayingTrackNode( ci::ipod::TrackRef playingTrack, Node* albumNode )
{
    if (albumNode != NULL) {
        uint64_t trackId = playingTrack->getItemId();
        for (int k = 0; k < albumNode->mChildNodes.size(); k++) {
            Node *trackNode = albumNode->mChildNodes[k];
            if (trackNode->getId() == trackId) {
                return trackNode;
            }
        }
    }
    return NULL;
}

Node* World::getPlayingAlbumNode( ci::ipod::TrackRef playingTrack, Node* artistNode )
{
    if (artistNode != NULL) {
        uint64_t albumId = playingTrack->getAlbumId();
        for (int j = 0; j < artistNode->mChildNodes.size(); j++) {					
            Node* albumNode = artistNode->mChildNodes[j];
            if (albumNode->getId() == albumId) {
                return albumNode;
            }
        }
    }
    return NULL;
}

Node* World::getPlayingArtistNode( ci::ipod::TrackRef playingTrack )
{
    uint64_t artistId = playingTrack->getArtistId();    
    for (int i = 0; i < mNodes.size(); i++) {
        Node* artistNode = mNodes[i];
        if (artistNode->getId() == artistId) {
            return artistNode;
        }
    }    
    return NULL;
}

void World::checkForNameTouch( vector<Node*> &nodes, const Vec2f &pos )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		if( (*it)->mIsHighlighted ){
			(*it)->checkForNameTouch( nodes, pos );
		}
	}
}

void World::updateGraphics( const CameraPersp &cam, const Vec3f &bbRight, const Vec3f &bbUp )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->updateGraphics( cam, bbRight, bbUp );
	}
}

void World::buildStarsVertexArray( const ci::Vec3f &bbRight, const ci::Vec3f &bbUp, float zoomAlpha )
{
    mStars.setup(mNodes, bbRight, bbUp, zoomAlpha);
}

void World::buildStarGlowsVertexArray( const Vec3f &bbRight, const Vec3f &bbUp, float zoomAlpha )
{
    mStarGlows.setup(mNodes, mData->mFilteredArtists.size(), bbRight, bbUp, zoomAlpha);
}

void World::buildPlanetRingsVertexArray()
{
    mPlanetRing.setup();
}


void World::buildOrbitRingsVertexArray()
{
    mOrbitRing.setup();
}

void World::update( float param1, float param2 )
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
			(*it)->update( param1, param2 );
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

void World::drawStarsVertexArray()
{
    mStars.draw();
}

void World::drawStarGlowsVertexArray()
{
    mStarGlows.draw();
}

void World::drawEclipseGlows()
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawEclipseGlow();
	}
}

void World::drawPlanets( const gl::Texture &tex )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawPlanet( tex );
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
		(*it)->drawRings( tex, mPlanetRing, camZPos );
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
		(*it)->drawOrbitRing( pinchAlphaOffset, camAlpha, orbitRingGradient, mOrbitRing );
	}
}

void World::drawTouchHighlights( float zoomAlpha )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawTouchHighlight( zoomAlpha );
	}
}

void World::drawConstellation()
{
    mConstellation.draw();
}


void World::buildConstellation()
{
    mConstellation.setup( mNodes, mData->mFilteredArtists );
}


vector<Node*> World::getUnsortedNodes( int fromGen, int toGen )
{
    vector<Node*> unsortedNodes;
	
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
				unsortedNodes.push_back(node);
            }
            // add all node's children to the queue
            queue.insert(queue.end(), node->mChildNodes.begin(), node->mChildNodes.end());
        }
    }
    
    return unsortedNodes;
}


vector<Node*> World::sortNodes( vector<Node*> nodes )
{
    if( nodes.size() > 0 ){
        sort(nodes.begin(), nodes.end(), nodeSortFunc);
    }
    
    return nodes;
}

bool nodeSortFunc(Node* a, Node* b) {
    return a->mDistFromCamZAxis > b->mDistFromCamZAxis;
}

