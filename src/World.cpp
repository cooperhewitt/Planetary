/*
 *  World.cpp
 *  Kepler
 *
 *  Created by Robert Hodgin on 2/25/11.
 *  Copyright 2013 Smithsonian Institution. All rights reserved.
 *
 */

#include <boost/foreach.hpp>
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
        mOrbitRing.setup();
        mPlanetRing.setup();
        mSpheresInitialized = true;
	}
	
	mAge			= 0;
	mEndRepulseAge	= 30;//200
	mIsRepulsing	= true;
	mIsInitialized	= false;
}

void World::initNodes( const vector<PlaylistRef> &artists, const Font &font, const Font &smallFont, const Surface &highResSurfaces, const Surface &lowResSurfaces, const Surface &noAlbumArt )
{
//	float t = App::get()->getElapsedSeconds();

	for( vector<NodeArtist*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
        NodeArtist* node = *it;
        delete node;
    }
	mNodes.clear();
    mNodesById.clear();
    
	int i=0;
    BOOST_FOREACH(PlaylistRef artistPlaylist, artists) {
		NodeArtist *newNode = new NodeArtist( i++, font, smallFont, highResSurfaces, lowResSurfaces, noAlbumArt );
		newNode->setData( artistPlaylist );
        newNode->setSphereData( &mHiSphere, &mMdSphere, &mLoSphere, &mTySphere );
		mNodes.push_back( newNode );
        mNodesById[newNode->getId()] = newNode;
	}

//	cout << (App::get()->getElapsedSeconds() - t) << " seconds to World::initNodes" << endl;
    
	mIsInitialized = true;
}

void World::setFilter(FilterRef filterRef)
{    
    mFilterRef = filterRef;
    
    mFilteredNodes.clear();
    
    BOOST_FOREACH(NodeArtist* nodeArtist, mNodes) {
        if ( mFilterRef->testArtist( nodeArtist->getPlaylist() ) ) {
            nodeArtist->mIsHighlighted = true;
            mFilteredNodes.push_back(nodeArtist);
        }
        else {
            nodeArtist->mIsHighlighted = false;
        }
	}
	
	if( mFilteredNodes.size() > 1 ){
		if( mAge >= mEndRepulseAge + 150 ){        
            mConstellation.setup( mFilteredNodes );
        }
	}    
}

void World::updateIsPlaying( uint64_t artistId, uint64_t albumId, uint64_t trackId )
{
	mPlayingTrackNode = NULL;
	
    BOOST_FOREACH(NodeArtist* artistNode, mNodes) {        
        artistNode->mIsPlaying = artistNode->getId() == artistId;
        BOOST_FOREACH(Node* albumNode, artistNode->mChildNodes) {        
            albumNode->mIsPlaying = albumNode->getId() == albumId;
            BOOST_FOREACH(Node *trackNode, albumNode->mChildNodes) {        
                bool wasPlaying = trackNode->mIsPlaying;
                trackNode->mIsPlaying = trackNode->getId() == trackId;
				if( trackNode->mIsPlaying && !trackNode->isDying() ){
					mPlayingTrackNode = static_cast<NodeTrack*>(trackNode);
                    if (!wasPlaying) {
                        mPlayingTrackNode->setStartAngle();
                    }
				}
            }            
        }
    }
}

void World::selectHierarchy( uint64_t artistId, uint64_t albumId, uint64_t trackId )
{
    BOOST_FOREACH(NodeArtist* artistNode, mNodes) {        
        if (artistNode->getId() == artistId) {
            artistNode->select();
            BOOST_FOREACH(Node* albumNode, artistNode->mChildNodes) {        
                if (albumNode->getId() == albumId) {
                    albumNode->select();
                    BOOST_FOREACH(Node *trackNode, albumNode->mChildNodes) {        
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
    NodeArtist* artistNode = getArtistNodeById(artistId);
    if (artistNode->getId() == artistId) {
        BOOST_FOREACH(Node* albumNode, artistNode->mChildNodes) {        
            if (albumNode->getId() == albumId) {
                BOOST_FOREACH(Node *trackNode, albumNode->mChildNodes) {        
                    if (trackNode->getId() == trackId) {
                        return static_cast<NodeTrack*>(trackNode);
                    }
                }            
                break;
            }
        }
    }
    return NULL;
}

NodeAlbum* World::getAlbumNodeById( uint64_t artistId, uint64_t albumId )
{
    // NB:- artist and album must be selected already
    NodeArtist* artistNode = getArtistNodeById(artistId);
    if (artistNode->getId() == artistId) {
        // TODO: add a map childrenById to Node?
        BOOST_FOREACH(Node* albumNode, artistNode->mChildNodes) {        
            if (albumNode->getId() == albumId) {
                return static_cast<NodeAlbum*>(albumNode);
            }
        }
    }
    return NULL;
}

// check albums and artists (that may have been selected since the filter was set) and unhighlight them if needed
void World::updateAgainstCurrentFilter()
{
    if (mFilterRef) {
        BOOST_FOREACH(NodeArtist* artistNode, mFilteredNodes) {
            BOOST_FOREACH(Node* n1, artistNode->mChildNodes) {            
                NodeAlbum* albumNode = static_cast<NodeAlbum*>(n1);                
                albumNode->mIsHighlighted = mFilterRef->testAlbum(albumNode->getPlaylist());
                BOOST_FOREACH(Node* n2, albumNode->mChildNodes) {            
                    NodeTrack *trackNode = static_cast<NodeTrack*>(n2);
                    trackNode->mIsHighlighted = albumNode->mIsHighlighted && mFilterRef->testTrack(trackNode->mTrack);
                }
            }
        }
    }
}

NodeTrack* World::selectPlayingHierarchy( uint64_t artistId, uint64_t albumId, uint64_t trackId )
{
    mPlayingTrackNode = NULL;
    
    BOOST_FOREACH(NodeArtist* artistNode, mNodes) {        
        artistNode->mIsPlaying = artistNode->getId() == artistId;
        if (artistNode->mIsPlaying) {
            artistNode->select();
        }
        else {
            artistNode->deselect();
        }
        BOOST_FOREACH(Node* albumNode, artistNode->mChildNodes) {        
            albumNode->mIsPlaying = albumNode->getId() == albumId;
            if (albumNode->mIsPlaying) {
                albumNode->select();
            }
            else {
                albumNode->deselect();
            }
            BOOST_FOREACH(Node* trackNode, albumNode->mChildNodes) {        
                bool wasPlaying = trackNode->mIsPlaying;
                trackNode->mIsPlaying = trackNode->getId() == trackId;
                if( trackNode->mIsPlaying && !trackNode->isDying() ){
                    mPlayingTrackNode = static_cast<NodeTrack*>(trackNode);
                    if (!wasPlaying) {
                        mPlayingTrackNode->setStartAngle();
                    }
                }
                if (trackNode->mIsPlaying) {
                    trackNode->select();
                }
                else {
                    trackNode->deselect();
                }                            
            }            
        }
    }    
    
    return mPlayingTrackNode;
}

void World::checkForNameTouch( vector<Node*> &nodes, const Vec2f &pos )
{
    BOOST_FOREACH(NodeArtist* artistNode, mFilteredNodes) {        
        artistNode->checkForNameTouch( nodes, pos );
    }
}

void World::updateGraphics( const CameraPersp &cam, const Vec2f &center, const Vec3f &bbRight, const Vec3f &bbUp, const float &zoomAlpha )
{
    const float w = app::getWindowWidth();
    const float h = app::getWindowHeight();
    
    BOOST_FOREACH(NodeArtist* artistNode, mNodes) {        
		artistNode->updateGraphics( cam, center, bbRight, bbUp, w, h );
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
		
		if( mAge == mEndRepulseAge + 150 ){
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
		
        BOOST_FOREACH(NodeArtist* artistNode, mNodes) {
			artistNode->update( param1, param2 );
		}        
	}
}

void World::repulseNodes()
{
	for( vector<NodeArtist*>::iterator p1 = mNodes.begin(); p1 != mNodes.end(); ++p1 ){
		
		vector<NodeArtist*>::iterator p2 = p1;
		for( ++p2; p2 != mNodes.end(); ++p2 ) {
			Vec3f dir = (*p1)->mPosDest - (*p2)->mPosDest;
			
			float thresh = 20.0f;
			if( dir.x > -thresh && dir.x < thresh && dir.y > -thresh && dir.y < thresh && dir.z > -thresh && dir.z < thresh ){
				float distSqrd = dir.lengthSquared();
				
				if( distSqrd > 0.0f ){
					float F = constrain( 1.0f/distSqrd, 0.0f, 1.0f );
					dir = F * dir.normalized() * 0.75f;
					dir.y *= 0.5f;
					
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
    BOOST_FOREACH(NodeArtist* artistNode, mNodes) {        
		artistNode->drawRings( tex, mPlanetRing, camZPos );
	}
}

void World::drawNames( const CameraPersp &cam, float pinchAlphaOffset, float angle )
{
    // FIXME: consider splitting Node::drawName into drawNameShadow and drawName and using
    // a single bloom::gl::begin/endBatch to reduce the number of state switches
    // needs to extend bloom::gl batching to support storing the current color as well as texture
    BOOST_FOREACH(NodeArtist* artistNode, mFilteredNodes) {    
        artistNode->drawName( cam, pinchAlphaOffset, angle );
	}
}

// assumes texture is already bound
void World::drawOrbitRings( float pinchAlphaOffset, float camAlpha, float fadeInAlphaToArtist, float fadeInArtistToAlbum )
{
    BOOST_FOREACH(NodeArtist* artistNode, mNodes) {
		artistNode->drawOrbitRing( pinchAlphaOffset, camAlpha, mOrbitRing, fadeInAlphaToArtist, fadeInArtistToAlbum );
	}
}

void World::drawTouchHighlights( float zoomAlpha )
{
    BOOST_FOREACH(NodeArtist* artistNode, mNodes) {
		artistNode->drawTouchHighlight( zoomAlpha );
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

