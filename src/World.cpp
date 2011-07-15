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
#include "NodeAlbum.h"
#include "NodeTrack.h"
#include "cinder/gl/gl.h"
#include "cinder/Rect.h"
#include "cinder/Text.h"
#include "cinder/Rand.h"
#include "Globals.h"
#include "BloomSphere.h"
#include "Filter.h"

using std::stringstream;
using namespace ci;
using namespace ci::app;
using namespace ci::ipod;
using namespace std;

void World::setup()
{
	if( !mSpheresInitialized ){
		// VERTEX ARRAY SPHERE      
		if ( G_IS_IPAD2 ) {
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
        mSpheresInitialized = true;
	}
	
	mAge			= 0;
	mEndRepulseAge	= 30;//200
	mIsRepulsing	= true;
	mIsInitialized	= false;
}

void World::initNodes( const vector<PlaylistRef> &artists, const Font &font, const Font &smallFont, const Surface &highResSurfaces, const Surface &lowResSurfaces, const Surface &noAlbumArt )
{
	float t = App::get()->getElapsedSeconds();

	for( vector<NodeArtist*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
        NodeArtist* node = *it;
        delete node;
    }
	mNodes.clear();
    mNodesById.clear();
    
	int i=0;
	for(vector<PlaylistRef>::const_iterator it = artists.begin(); it != artists.end(); ++it){
        PlaylistRef artistPlaylist = *it;
		NodeArtist *newNode = new NodeArtist( i++, font, smallFont, highResSurfaces, lowResSurfaces, noAlbumArt );
		newNode->setData( artistPlaylist );
        newNode->setSphereData( &mHiSphere, &mMdSphere, &mLoSphere, &mTySphere );
		mNodes.push_back( newNode );
        mNodesById[artistPlaylist->getArtistId()] = newNode;
	}

    mOrbitRing.setup();
    mPlanetRing.setup();
	
	cout << (App::get()->getElapsedSeconds() - t) << " seconds to World::initNodes" << endl;
    
	mIsInitialized = true;
}

void World::setFilter(FilterRef filterRef)
{
    mFilterRef = filterRef;
    
    // deselect all nodes first
	for( vector<NodeArtist*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->mIsHighlighted = false;
		(*it)->mIsSelected = false; // FIXME: should really be deselect, but how to guarantee useful nodes aren't deleted?
	}
	
    mFilteredNodes.clear();
    
	for(vector<NodeArtist*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it){
        if ( mFilterRef->testArtist( (*it)->getPlaylist() ) ) {
            (*it)->mIsHighlighted = true;
            mFilteredNodes.push_back(*it);
        }
	}
	
	if( mFilteredNodes.size() > 1 ){
		mConstellation.setup( mFilteredNodes );
	}
}

void World::updateIsPlaying( uint64_t artistId, uint64_t albumId, uint64_t trackId )
{
	mPlayingTrackNode = NULL;
	
    // TODO: proper iterators I suppose?
    for (int i = 0; i < mNodes.size(); i++) {
        NodeArtist* artistNode = mNodes[i];
        artistNode->mIsPlaying = artistNode->getId() == artistId;
        for (int j = 0; j < artistNode->mChildNodes.size(); j++) {					
            Node* albumNode = artistNode->mChildNodes[j];
            albumNode->mIsPlaying = albumNode->getId() == albumId;
            for (int k = 0; k < albumNode->mChildNodes.size(); k++) {
                Node *trackNode = albumNode->mChildNodes[k];
                bool wasPlaying = trackNode->mIsPlaying;
                trackNode->mIsPlaying = trackNode->getId() == trackId;
				if( trackNode->mIsPlaying && !trackNode->isDying() ){
					mPlayingTrackNode = (NodeTrack*)trackNode;
                    if (!wasPlaying) {
                        ((NodeTrack*)trackNode)->setStartAngle();
                    }
				}
            }            
        }
    }
}

void World::selectHierarchy( uint64_t artistId, uint64_t albumId, uint64_t trackId )
{
    // TODO: proper iterators I suppose?    
    for (int i = 0; i < mNodes.size(); i++) {
        NodeArtist* artistNode = mNodes[i];
        if (artistNode->getId() == artistId) {
            artistNode->select();
            for (int j = 0; j < artistNode->mChildNodes.size(); j++) {					
                Node* albumNode = artistNode->mChildNodes[j];
                if (albumNode->getId() == albumId) {
                    albumNode->select();
                    for (int k = 0; k < albumNode->mChildNodes.size(); k++) {
                        Node *trackNode = albumNode->mChildNodes[k];
                        if (trackNode->getId() == trackId) {
                            trackNode->select();
                        }
                        else {
                            trackNode->deselect();
                        }
                    }                                
                }
                else {
                    albumNode->deselect();
                }
            }            
        }
        else {
            artistNode->deselect();
        }
    }
}

NodeTrack* World::getTrackNodeById( uint64_t artistId, uint64_t albumId, uint64_t trackId )
{
    // NB:- artist and album must be selected, otherwise track node won't exist
    // TODO: proper iterators I suppose?        
    for (int i = 0; i < mNodes.size(); i++) {
        NodeArtist* artistNode = mNodes[i];
        if (artistNode->getId() == artistId) {
            for (int j = 0; j < artistNode->mChildNodes.size(); j++) {					
                Node* albumNode = artistNode->mChildNodes[j];
                if (albumNode->getId() == albumId) {
                    for (int k = 0; k < albumNode->mChildNodes.size(); k++) {
                        Node *trackNode = albumNode->mChildNodes[k];
                        if (trackNode->getId() == trackId) {
                            return (NodeTrack*)trackNode;
                        }
                    }            
                    break;
                }
            }
            break;
        }
    }    
    return NULL;
}

void World::updateAgainstCurrentFilter()
{
    if (mFilterRef) {
        // TODO: proper iterators I suppose?
        for (int i = 0; i < mNodes.size(); i++) {
            NodeArtist* artistNode = mNodes[i];
            artistNode->mIsHighlighted = mFilterRef->testArtist(artistNode->getPlaylist());
			
            for (int j = 0; j < artistNode->mChildNodes.size(); j++) {					
                // FIXME: static cast?
                NodeAlbum* albumNode = (NodeAlbum*)(artistNode->mChildNodes[j]);
                albumNode->mIsHighlighted = mFilterRef->testAlbum(albumNode->getPlaylist());
				
                for (int k = 0; k < albumNode->mChildNodes.size(); k++) {
                    // FIXME: static cast?
                    NodeTrack *trackNode = (NodeTrack*)(albumNode->mChildNodes[k]);
                    trackNode->mIsHighlighted = mFilterRef->testTrack(trackNode->mTrack);
                }            
            }
        }        
    }
}

void World::checkForNameTouch( vector<Node*> &nodes, const Vec2f &pos )
{
    for( vector<NodeArtist*>::iterator it = mNodes.begin(); it != mNodes.end(); it++) {
        if( (*it)->mIsHighlighted ) {
            (*it)->checkForNameTouch( nodes, pos );
        }
    }
}

void World::updateGraphics( const CameraPersp &cam, const Vec2f &center, const Vec3f &bbRight, const Vec3f &bbUp, const float &zoomAlpha )
{
    const float w = app::getWindowWidth();
    const float h = app::getWindowHeight();
    
	for( vector<NodeArtist*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->updateGraphics( cam, center, bbRight, bbUp, w, h );
	}
    
    if (mIsInitialized) {
        mStars.setup(mNodes, bbRight, bbUp, zoomAlpha * 0.3f);
        mStarGlows.setup(mFilteredNodes, bbRight, bbUp, zoomAlpha);
    }
}

void World::update( float param1, float param2 )
{
	if( mIsInitialized ){
		mAge ++;
	
		if( mAge == mEndRepulseAge ){
			mIsRepulsing = false;
		}
		
		if( mAge == mEndRepulseAge + 100 ){
			mConstellation.setup( mFilteredNodes );
		}
		
		if( mIsRepulsing ){
			repulseNodes();
		}

        // reset mPlayingTrackNode before Node::update() has a chance to delete it
        // in future: this is why we should be using shared pointers :(
        const Node *ptn = mPlayingTrackNode; // temp for shorter next line
        const bool playingTrackNodeWillDieNextUpdate = ptn && (ptn->mDeathCount == ptn->mDeathThresh);
        if (playingTrackNodeWillDieNextUpdate) {
            mPlayingTrackNode = NULL;
        }   
		
		for( vector<NodeArtist*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
			(*it)->update( param1, param2 );
		}        
	}
}

void World::repulseNodes()
{
	for( vector<NodeArtist*>::iterator p1 = mNodes.begin(); p1 != mNodes.end(); ++p1 ){
		
		vector<NodeArtist*>::iterator p2 = p1;
		for( ++p2; p2 != mNodes.end(); ++p2 ) {
			Vec3f dir = (*p1)->mPosDest - (*p2)->mPosDest;
			
			float thresh = 10.0f;
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

void World::drawRings( const gl::Texture &tex, float camZPos )
{
	for( vector<NodeArtist*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawRings( tex, mPlanetRing, camZPos );
	}
}

void World::drawNames( const CameraPersp &cam, float pinchAlphaOffset, float angle )
{
    // FIXME: consider splitting Node::drawName into drawNameShadow and drawName and using
    // a single bloom::gl::begin/endBatch to reduce the number of state switches
    // needs to extend bloom::gl batching to support storing the current color as well as texture
	for( vector<NodeArtist*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		if( (*it)->mIsHighlighted ){
			(*it)->drawName( cam, pinchAlphaOffset, angle );
		}
	}
}

// assumes texture is already bound
void World::drawOrbitRings( float pinchAlphaOffset, float camAlpha )
{
	for( vector<NodeArtist*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawOrbitRing( pinchAlphaOffset, camAlpha, mOrbitRing );
	}
}

void World::drawTouchHighlights( float zoomAlpha )
{
	for( vector<NodeArtist*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->drawTouchHighlight( zoomAlpha );
	}
}

void World::drawConstellation()
{
    const float alpha = ( 1.0f - (G_ZOOM-1.0f) ) * 0.4f;
    mConstellation.draw( alpha );
}

void World::drawHitAreas()
{    
    // HIT AREA VISUALIZER
    for (int i = 0; i < mNodes.size(); i++) {
        NodeArtist* artistNode = mNodes[i];
        if (artistNode->mIsHighlighted) {
            gl::color(ColorA(0.0f,0.0f,1.0f,0.25f));
            if( artistNode->mDistFromCamZAxisPer > 0.0f ){
                if( G_DRAW_TEXT && artistNode->mIsHighlighted ) gl::drawSolidRect(artistNode->mHitArea);
                gl::drawSolidRect(artistNode->mSphereHitArea);       
            }
            for (int j = 0; j < artistNode->mChildNodes.size(); j++) {					
                Node* albumNode = artistNode->mChildNodes[j];
                if (albumNode->mIsHighlighted) {
                    gl::color(ColorA(0.0f,1.0f,0.0f,0.25f));
                    if( G_DRAW_TEXT ) gl::drawSolidRect(albumNode->mHitArea);
                    gl::drawSolidRect(albumNode->mSphereHitArea);
                    
                    for (int k = 0; k < albumNode->mChildNodes.size(); k++) {
                        Node *trackNode = albumNode->mChildNodes[k];
                        if (trackNode->mIsHighlighted) {
                            gl::color(ColorA(1.0f,0.0f,0.0f,0.25f));
                            if( G_DRAW_TEXT ) gl::drawSolidRect(trackNode->mHitArea);
                            gl::drawSolidRect(trackNode->mSphereHitArea);
                        }
                    }            
                }
            }
        }
    }
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

