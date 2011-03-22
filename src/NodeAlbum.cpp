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
	mIsHighlighted	= true;
	mHasRings		= false;
	if( Rand::randFloat() < 0.2f ) mHasRings = true;
	
	

	mIdealCameraDist	= mRadius * 8.0f;
    mPlanetTexIndex		= 0;
	mEclipseStrength	= 0.0f;
}


void NodeAlbum::setData( PlaylistRef album )
{
	mAlbum				= album;
	mNumTracks			= mAlbum->size();
	mHighestPlayCount	= 0;
	mLowestPlayCount	= 10000;
	
	
	
	
	// FIXME: bad c++?
	float numAlbums		= ((NodeArtist*)mParentNode)->mNumAlbums + 2.0f;
	
	float invAlbumPer	= 1.0f/(float)numAlbums;
	float albumNumPer	= (float)mIndex * invAlbumPer;
	
	float minAmt		= mParentNode->mOrbitRadiusMin;
	float maxAmt		= mParentNode->mOrbitRadiusMax;
	float deltaAmt		= maxAmt - minAmt;
	mOrbitRadiusDest	= minAmt + deltaAmt * albumNumPer;// + Rand::randFloat( maxAmt * invAlbumPer * 0.35f );
	
	
	
	
	
	float hue			= Rand::randFloat( 0.15f, 0.75f );
	float sat			= Rand::randFloat( 0.15f, 0.25f );
	float val			= Rand::randFloat( 0.85f, 1.00f );
	mColor				= Color( CM_HSV, hue, sat, val );
	mEclipseColor       = mColor;

	mRadius				*= 0.85f;
	mSphere				= Sphere( mPos, mRadius * 7.5f );
	mMass				= ( pow( mNumTracks, 3.0f ) * ( M_PI * 4.0f ) ) * 0.3333334f;
	
	// for the children
	mOrbitRadiusMin		= mRadius * 2.0f;
	mOrbitRadiusMax		= mRadius * 7.5f;
	if( mHasRings ){
		mOrbitRadiusMin = mRadius * 3.0f;
		mOrbitRadiusMax = mRadius * 8.5f;
	}
	
	mAxialTilt			= Rand::randFloat( 5.0f, 30.0f );
    mAxialVel			= Rand::randFloat( 10.0f, 45.0f );
    
    mPlanetTexIndex		= ( mIndex + 6 )%( G_NUM_PLANET_TYPES * G_NUM_PLANET_TYPE_OPTIONS );//(int)( normPlayCount * ( G_NUM_PLANET_TYPES - 1 ) );
	mCloudTexIndex		= Rand::randInt( G_NUM_CLOUD_TYPES );

	
	//if( mPlayCount > 5 ){
	//	mHasClouds = true;
	//}
	
	
	for (int i = 0; i < mNumTracks; i++) {
		float numPlays = (*mAlbum)[i]->getPlayCount();
		if( numPlays < mLowestPlayCount )
			mLowestPlayCount = numPlays;
		if( numPlays > mHighestPlayCount )
			mHighestPlayCount = numPlays;
	}
}


void NodeAlbum::update( const Matrix44f &mat )
{
	double playbackTime		= app::getElapsedSeconds();
	double percentPlayed	= playbackTime/mOrbitPeriod;
	double orbitAngle		= percentPlayed * TWO_PI + mStartAngle;
    
	mPrevPos	= mTransPos;
	
	mRelPos		= Vec3f( cos( orbitAngle ), sin( orbitAngle ), 0.0f ) * mOrbitRadius;
	mPos		= mParentNode->mPos + mRelPos;
    
    float eclipseDist = 1.0f;
    if( mParentNode->mDistFromCamZAxisPer > 0.0f ){
        float dist			= mScreenPos.distance( mParentNode->mScreenPos );
        eclipseDist			= constrain( dist/200.0f, 0.0f, 1.0f );
		if( G_ZOOM == G_ALBUM_LEVEL ){
			mEclipseStrength	= math<float>::max( 50.0f - abs( mSphereScreenRadius - mParentNode->mSphereScreenRadius ), 0.0f ) / 50.0f; 
			mEclipseStrength	= pow( mEclipseStrength, 2.0f );
		}
		
		/*
		if( mIsSelected ){
			std::cout << "========================================================" << std::endl;
			std::cout << "Sun Screen Radius = " << mParentNode->mSphereScreenRadius << std::endl;
			std::cout << "Planet Screen Radius = " << mSphereScreenRadius << std::endl;
			std::cout << "mEclipseStrength = " << mEclipseStrength << std::endl;
		}
		 */
    }
	mEclipseColor = ( mColor + Color::white() ) * 0.5f * eclipseDist;
    
	Node::update( mat );
	
	mVel		= mTransPos - mPrevPos;	
}

void NodeAlbum::drawEclipseGlow()
{
	if( mIsSelected && mDistFromCamZAxisPer > 0.0f ){
        gl::color( ColorA( mParentNode->mGlowColor, mEclipseStrength * 3.0f ) );
		Vec2f radius = Vec2f( mRadius, mRadius ) * 3.25f;
		gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
	}
	
	Node::drawEclipseGlow();
}

void NodeAlbum::drawOrbitRing( GLfloat *ringVertsLowRes, GLfloat *ringVertsHighRes )
{
	if( mIsPlaying ){
		gl::color( ColorA( 0.2f, 0.3f, 0.7f, 0.45f ) );
	} else {
		gl::color( ColorA( 0.15f, 0.2f, 0.4f, 0.2f ) );
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
	
	Node::drawOrbitRing( ringVertsLowRes, ringVertsHighRes );
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
	planets[mPlanetTexIndex].enableAndBind();
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



void NodeAlbum::drawRings( const gl::Texture &tex, GLfloat *planetRingVerts, GLfloat *planetRingTexCoords )
{
	if( mHasRings ){
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::scale( Vec3f( mRadius * 3.0f, mRadius * 3.0f, mRadius * 3.0f ) );
		gl::rotate( mMatrix );
		gl::rotate( Vec3f( 90.0f, app::getElapsedSeconds() * mAxialVel * 0.75f, mAxialTilt ) );
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
		tex.enableAndBind();
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, planetRingVerts );
		glTexCoordPointer( 2, GL_FLOAT, 0, planetRingTexCoords );
		
		gl::disableAlphaBlending();
		gl::enableAlphaBlending();
		glDrawArrays( GL_TRIANGLES, 0, 6 );
		
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
