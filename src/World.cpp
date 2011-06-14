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
				if( trackNode->mIsPlaying && !trackNode->mIsDying ){
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

void World::buildStarsVertexArray( const ci::Vec3f &bbRight, const ci::Vec3f &bbUp, float zoomAlpha )
{
    mStars.setup(mNodes, bbRight, bbUp, zoomAlpha);
}

void World::drawStarsVertexArray()
{
    mStars.draw();
}

void World::buildStarGlowsVertexArray( const Vec3f &bbRight, const Vec3f &bbUp, float zoomAlpha )
{
    mStarGlows.setup(mNodes, mData->mFilteredArtists.size(), bbRight, bbUp, zoomAlpha);
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



void World::updateGraphics( const CameraPersp &cam, const Vec3f &bbRight, const Vec3f &bbUp )
{
	for( vector<Node*>::iterator it = mNodes.begin(); it != mNodes.end(); ++it ){
		(*it)->updateGraphics( cam, bbRight, bbUp );
	}
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

void World::drawConstellation()
{
	if( mTotalConstellationVertices > 2 ){
        
        // FIXME: pass this in as a function argument (to remove dependency on global)
		float zoomPer = ( 1.0f - (G_ZOOM-1.0f) ) * 0.4f;
		
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		//glEnableClientState( GL_COLOR_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, mConstellationVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, mConstellationTexCoords );
		//glColorPointer( 4, GL_FLOAT, 0, mColors );
		
		gl::color( ColorA( 0.12f, 0.25f, 0.85f, zoomPer ) );
		glDrawArrays( GL_LINES, 0, mTotalConstellationVertices );
		
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
			mConstellationTexCoords[tIndex++]	= distances[distancesIndex];// * 0.4f;
			mConstellationTexCoords[tIndex++]	= 0.5f;
			distancesIndex ++;
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

