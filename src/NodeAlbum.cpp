/*
 *  NodeAlbum.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "cinder/app/AppBasic.h"
#include "NodeArtist.h"
#include "NodeAlbum.h"
#include "NodeTrack.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/PolyLine.h"
#include "Globals.h"

using namespace ci;
using namespace ci::ipod;
using namespace std;

NodeAlbum::NodeAlbum( Node *parent, int index, const Font &font )
	: Node( parent, index, font )
{
	mIsHighlighted		= true;
	mHasAlbumArt		= false;
	mHasCreatedAlbumArt = false;
	mIdealCameraDist	= mRadius * 8.0f;
	mEclipseStrength	= 0.0f;
}


void NodeAlbum::setData( PlaylistRef album )
{
// ALBUM INFORMATION
	mAlbum				= album;
	mNumTracks			= mAlbum->size();
	mHighestPlayCount	= 0;
	mLowestPlayCount	= 10000;
	for( int i = 0; i < mNumTracks; i++ ){
		float numPlays = (*mAlbum)[i]->getPlayCount();
		if( numPlays < mLowestPlayCount )
			mLowestPlayCount = numPlays;
		if( numPlays > mHighestPlayCount )
			mHighestPlayCount = numPlays;
	}
	
	
	
// ORBIT RADIUS	
	// FIXME: bad c++?
	float numAlbums		= ((NodeArtist*)mParentNode)->mNumAlbums + 2.0f;
	
	float invAlbumPer	= 1.0f/(float)numAlbums;
	float albumNumPer	= (float)mIndex * invAlbumPer;
	
	float minAmt		= mParentNode->mOrbitRadiusMin;
	float maxAmt		= mParentNode->mOrbitRadiusMax;
	float deltaAmt		= maxAmt - minAmt;
	mOrbitRadiusDest	= minAmt + deltaAmt * albumNumPer;// + Rand::randFloat( maxAmt * invAlbumPer * 0.35f );
	
	
// COLORS
	float hue			= Rand::randFloat( 0.15f, 0.75f );
	float sat			= Rand::randFloat( 0.15f, 0.25f );
	float val			= Rand::randFloat( 0.85f, 1.00f );
	mColor				= Color( CM_HSV, hue, sat, val );
	mGlowColor			= mParentNode->mGlowColor;
	mEclipseColor       = mColor;


// PHYSICAL PROPERTIES
	mRadius				*= 0.85f;
	mSphere				= Sphere( mPos, mRadius * 7.5f );
	mAxialTilt			= Rand::randFloat( 5.0f );
    mAxialVel			= Rand::randFloat( 10.0f, 45.0f );
	mHasRings			= false;
	if( mNumTracks > 2 ) mHasRings = true;
	
	
// CHILD ORBIT RADIUS CONSTRAINTS
	mOrbitRadiusMin		= mRadius * 3.0f;
	mOrbitRadiusMax		= mRadius * 8.5f;
	

// TEXTURE IDs
    mPlanetTexIndex		= 3 * G_NUM_PLANET_TYPE_OPTIONS + Rand::randInt( 6 );
	std::cout << "Album Planet Texture Index = " << mPlanetTexIndex << std::endl;
	//( mIndex + 6 )%( G_NUM_PLANET_TYPES * G_NUM_PLANET_TYPE_OPTIONS );//(int)( normPlayCount * ( G_NUM_PLANET_TYPES - 1 ) );
	mCloudTexIndex		= Rand::randInt( G_NUM_CLOUD_TYPES );
}


void NodeAlbum::update( const Matrix44f &mat )
{
	if( !mHasCreatedAlbumArt && mChildNodes.size() > 0 ){
		Surface albumArt	= ((NodeTrack*)mChildNodes[0])->mTrack->getArtwork( Vec2i( 256, 256 ) );
		if( albumArt ){
			int x				= Rand::randInt( 127 );
			int y				= Rand::randInt( 64 );
			Area a				= Area( x, y, x+1, y+64 );
			Surface crop		= albumArt.clone( a );
			mAlbumArt			= gl::Texture( crop );
			mHasAlbumArt		= true;
		}
		
		mHasCreatedAlbumArt = true;
	}
	
	
	double playbackTime		= app::getElapsedSeconds();
	double percentPlayed	= playbackTime/mOrbitPeriod;
	double orbitAngle		= percentPlayed * TWO_PI + mOrbitStartAngle;
    
	mPrevPos	= mTransPos;
	
	mRelPos		= Vec3f( cos( orbitAngle ), sin( orbitAngle ), 0.0f ) * mOrbitRadius;
	mPos		= mParentNode->mPos + mRelPos;
    
    float eclipseDist = 1.0f;
    if( mParentNode->mDistFromCamZAxisPer > 0.0f ){
        float dist			= mScreenPos.distance( mParentNode->mScreenPos );
        eclipseDist			= constrain( dist/200.0f, 0.0f, 1.0f );
		if( G_ZOOM == G_ALBUM_LEVEL ){
			mEclipseStrength	= math<float>::max( 500.0f - abs( mSphereScreenRadius - mParentNode->mSphereScreenRadius ), 0.0f ) / 500.0f; 
			mEclipseStrength	= pow( mEclipseStrength, 5.0f );
		}
    }
	mEclipseColor = ( mColor + Color::white() ) * 0.5f * eclipseDist;
    
	Node::update( mat );
	
	mVel		= mTransPos - mPrevPos;	
}

void NodeAlbum::drawEclipseGlow()
{
	if( mIsSelected && mDistFromCamZAxisPer > 0.0f ){
        gl::color( ColorA( mParentNode->mGlowColor, mEclipseStrength ) );
		Vec2f radius = Vec2f( mRadius, mRadius ) * ( mEclipseStrength + 1.0f ) * 3.25f;
		gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
	}
	
	Node::drawEclipseGlow();
}

void NodeAlbum::drawOrbitRing( float pinchAlphaOffset, GLfloat *ringVertsLowRes, GLfloat *ringVertsHighRes )
{		
	float newPinchAlphaOffset = pinchAlphaOffset;
	if( G_ZOOM < G_ALBUM_LEVEL - 0.5f ){
		newPinchAlphaOffset = pinchAlphaOffset;
	} else {
		newPinchAlphaOffset = 1.0f;
	}
	
	
	if( mIsPlaying ){
		gl::color( ColorA( COLOR_BRIGHT_BLUE, 0.45f * newPinchAlphaOffset ) );
	} else {
		gl::color( ColorA( COLOR_BLUE, 0.2f * newPinchAlphaOffset ) );
	}
	
	gl::pushModelView();
	gl::translate( mParentNode->mTransPos );
	gl::scale( Vec3f( mOrbitRadius, mOrbitRadius, mOrbitRadius ) );
	gl::rotate( mMatrix );
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, ringVertsHighRes );
	glDrawArrays( GL_LINE_STRIP, 0, G_RING_HIGH_RES );
	glDisableClientState( GL_VERTEX_ARRAY );
	gl::popModelView();
	
	if( G_ZOOM < G_ALBUM_LEVEL - 0.5f ){
		newPinchAlphaOffset = pinchAlphaOffset;
	} else if( G_ZOOM < G_TRACK_LEVEL - 0.5f ){
		newPinchAlphaOffset = pinchAlphaOffset;
	} else {
		newPinchAlphaOffset = 1.0f;
	}
	
	if( G_ZOOM < G_TRACK_LEVEL - 0.5f ){
		newPinchAlphaOffset = pinchAlphaOffset;
	} else {
		newPinchAlphaOffset = 1.0f;
	}
	
	Node::drawOrbitRing( newPinchAlphaOffset, ringVertsLowRes, ringVertsHighRes );
}


void NodeAlbum::drawPlanet( const vector<gl::Texture> &planets )
{	
	glEnable( GL_RESCALE_NORMAL );
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	int numVerts;
	if( mIsSelected ){
		glVertexPointer( 3, GL_FLOAT, 0, mSphereVertsHiRes );
		glTexCoordPointer( 2, GL_FLOAT, 0, mSphereTexCoordsHiRes );
		glNormalPointer( GL_FLOAT, 0, mSphereNormalsHiRes );
		numVerts = mTotalVertsHiRes;
	} else {
		glVertexPointer( 3, GL_FLOAT, 0, mSphereVertsLoRes );
		glTexCoordPointer( 2, GL_FLOAT, 0, mSphereTexCoordsLoRes );
		glNormalPointer( GL_FLOAT, 0, mSphereNormalsLoRes );
		numVerts = mTotalVertsLoRes;
	}
	
	gl::disableAlphaBlending();
	
    gl::pushModelView();
	gl::translate( mTransPos );
	gl::scale( Vec3f( mRadius, mRadius, mRadius ) );
	gl::rotate( mMatrix );
	gl::rotate( Vec3f( 0.0f, 0.0f, mAxialTilt ) );
	gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * mAxialVel, 0.0f ) );
	gl::color( mEclipseColor );
	if( mHasAlbumArt ){
		mAlbumArt.enableAndBind();
	} else {
		planets[mPlanetTexIndex].enableAndBind();
	}
	
	glDrawArrays( GL_TRIANGLES, 0, numVerts );
	gl::popModelView();
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );	
	
	Node::drawPlanet( planets );
}


void NodeAlbum::drawClouds( const vector<gl::Texture> &clouds )
{
    if( mCamDistAlpha > 0.05f ){
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glEnableClientState( GL_NORMAL_ARRAY );
		int numVerts;
		if( mIsSelected ){
			glVertexPointer( 3, GL_FLOAT, 0, mSphereVertsHiRes );
			glTexCoordPointer( 2, GL_FLOAT, 0, mSphereTexCoordsHiRes );
			glNormalPointer( GL_FLOAT, 0, mSphereNormalsHiRes );
			numVerts = mTotalVertsHiRes;
		} else {
			glVertexPointer( 3, GL_FLOAT, 0, mSphereVertsLoRes );
			glTexCoordPointer( 2, GL_FLOAT, 0, mSphereTexCoordsLoRes );
			glNormalPointer( GL_FLOAT, 0, mSphereNormalsLoRes );
			numVerts = mTotalVertsLoRes;
		}
		
		gl::disableAlphaBlending();
		
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::pushModelView();
		float radius = mRadius + 0.00025f;
		gl::scale( Vec3f( radius, radius, radius ) );
		glEnable( GL_RESCALE_NORMAL );
		gl::rotate( mMatrix );
		gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * mAxialVel * 0.75f, mAxialTilt ) );
// SHADOW CLOUDS
		glDisable( GL_LIGHTING );
		gl::disableAlphaBlending();
		gl::enableAlphaBlending();
		gl::color( ColorA( 0.0f, 0.0f, 0.0f, mCamDistAlpha * 0.5f ) );
		clouds[mCloudTexIndex].enableAndBind();
		glDrawArrays( GL_TRIANGLES, 0, numVerts );
		gl::popModelView();
		glEnable( GL_LIGHTING );		
// LIT CLOUDS
		gl::pushModelView();
		radius = mRadius + 0.0005f;
		gl::scale( Vec3f( radius, radius, radius ) );
		glEnable( GL_RESCALE_NORMAL );
		gl::rotate( mMatrix );
		gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * mAxialVel * 0.75f, mAxialTilt ) );
		gl::enableAdditiveBlending();
		gl::color( ColorA( mEclipseColor, mCamDistAlpha ) );
		glDrawArrays( GL_TRIANGLES, 0, numVerts );
		gl::popModelView();
		gl::popModelView();
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glDisableClientState( GL_NORMAL_ARRAY );
	}
    
	Node::drawClouds( clouds );
}


void NodeAlbum::drawRings( const gl::Texture &tex, GLfloat *planetRingVerts, GLfloat *planetRingTexCoords, float camRingAlpha )
{
	if( mHasRings && G_ZOOM > G_ARTIST_LEVEL && mIsPlaying ){
		gl::pushModelView();
		gl::translate( mTransPos );
		float c = mRadius * 9.0f;
		gl::scale( Vec3f( c, c, c ) );
		gl::rotate( mMatrix );
		gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * mAxialVel * 0.2f, 0.0f ) );
		gl::color( ColorA( mColor, mZoomPer * camRingAlpha * 50.0f ) );
		tex.enableAndBind();
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, planetRingVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, planetRingTexCoords );
		
		gl::enableAdditiveBlending();
		glDrawArrays( GL_TRIANGLES, 0, 6 );
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		tex.disable();
		gl::popModelView();
	}
}


void NodeAlbum::select( )
{
	if( !mIsSelected && mChildNodes.size() == 0 ){
		for (int i = 0; i < mNumTracks; i++) {
			TrackRef track		= (*mAlbum)[i];
			string name			= track->getTitle();
			NodeTrack *newNode	= new NodeTrack( this, i, mFont );
			mChildNodes.push_back( newNode );
			newNode->setData( track, mAlbum );
		}
		
		for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
			(*it)->setSphereData( mTotalVertsHiRes, mSphereVertsHiRes, mSphereTexCoordsHiRes, mSphereNormalsHiRes,
								  mTotalVertsLoRes, mSphereVertsLoRes, mSphereTexCoordsLoRes, mSphereNormalsLoRes );
		}
		
		setChildOrbitRadii();
	}	
	Node::select();
}

void NodeAlbum::setChildOrbitRadii()
{
	float orbitRadius = mOrbitRadiusMin;
	float orbitOffset;
	for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
		orbitOffset = (*it)->mRadius * 3.0f;
		orbitRadius += orbitOffset;
		(*it)->mOrbitRadiusDest = orbitRadius;
		orbitRadius += orbitOffset;
	}
}

string NodeAlbum::getName()
{
	string name = mAlbum->getAlbumTitle();
	if( name.size() < 1 ) name = "Untitled";
	return name;
}

uint64_t NodeAlbum::getId()
{
    return mAlbum->getAlbumId();
}
