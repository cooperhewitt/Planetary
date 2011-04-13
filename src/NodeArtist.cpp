/*
 *  NodeArtist.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 1/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "cinder/app/AppBasic.h"
#include "NodeArtist.h"
#include "NodeTrack.h"
#include "NodeAlbum.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "Globals.h"
#import <map>
#import "CinderFlurry.h"

using namespace pollen::flurry;
using namespace ci;
using namespace ci::ipod;
using namespace std;

NodeArtist::NodeArtist( int index, const Font &font )
	: Node( NULL, index, font )
{
	mPosDest		= Rand::randVec3f() * Rand::randFloat( 50.0f, 200.0f );
	mPos			= mPosDest + Rand::randVec3f() * 25.0f;
	
	mIdealCameraDist = mRadius * 2.0f;
	
	mSphere			= Sphere( mPos, 3.65f );
	
	mAge			= 0.0f;
	mBirthPause		= Rand::randFloat( 50.0f );
	
	mRadiusDest		= mRadius * 0.66f;
	mRadius			= 0.0f;
	
	mOrbitRadiusMin	= mRadiusDest * 1.0f;
	mOrbitRadiusMax	= mRadiusDest * 2.5f;
}


void NodeArtist::update( const Matrix44f &mat )
{
    Vec3f prevTransPos  = mTransPos;
    // if mTransPos hasn't been set yet, use a guess:
    // FIXME: set mTransPos correctly in the constructor
    if (prevTransPos.length() < 0.0001) prevTransPos = mat * mPos;    
    
	mPos -= ( mPos - mPosDest ) * 0.1f;
	mAge ++;
	
	if( mAge > mBirthPause ){
		if( G_ZOOM > G_ALPHA_LEVEL + 0.5f && !mIsSelected ){
			mRadius -= ( mRadius - 0.125f ) * 0.1f;
			mRadius += Rand::randFloat( 0.0125f, 0.065f );
		} else {
			mRadius -= ( mRadius - mRadiusDest ) * 0.1f;
		}
	}
	
	Node::update( mat );
    
	mTransVel = mTransPos - prevTransPos;    
}

void NodeArtist::drawEclipseGlow()
{
	if( mIsHighlighted && mDistFromCamZAxisPer > 0.0f ){
        float alpha         = mDistFromCamZAxisPer;
        gl::color( ColorA( mGlowColor, alpha * mEclipseStrength ) );
		Vec2f radius = Vec2f( mRadius, mRadius ) * ( mEclipseStrength + 1.0f ) * 22.0f;
		gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
	}
	
	if( G_IS_IPAD2 && mIsHighlighted ){
        gl::color( ColorA( mGlowColor, mDistFromCamZAxisPer ) );
		Vec2f radius = Vec2f( mRadius, mRadius ) * 7.5f;
		gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
	}
	 
	Node::drawEclipseGlow();
}

void NodeArtist::drawPlanet( const vector<gl::Texture> &planets )
{
	if( mIsSelected ){
		glDisable( GL_LIGHTING );
		glDisable( GL_TEXTURE_2D );
		gl::pushModelView();
		gl::translate( mTransPos );
		gl::color( mGlowColor );
		float radius = mRadius * 0.3f;
		gl::enableAdditiveBlending();
		if( G_ZOOM > G_ARTIST_LEVEL ){
			gl::disableDepthRead();
			gl::drawSphere( Vec3f::zero(), radius, 32 );
			gl::enableDepthRead();
		} else {
			gl::drawSolidCircle( Vec2f::zero(), radius, 64 );
		}
		gl::popModelView();
		glEnable( GL_LIGHTING );
	}
	
	Node::drawPlanet( planets );
}

void NodeArtist::drawClouds( const vector<gl::Texture> &planets, const vector<gl::Texture> &clouds )
{
	Node::drawClouds( planets, clouds );
}

void NodeArtist::select()
{
	if (!mIsSelected) {


		// TODO: switch this to use artist ID
        Flurry::getInstrumentation()->startTimeEvent("Albums loaded");
		vector<ipod::PlaylistRef> albumsBySelectedArtist = getAlbumsWithArtist( mPlaylist->getArtistName() );
		mNumAlbums = albumsBySelectedArtist.size();
		
		int i=0;
        int trackcount = 0;
		for(vector<PlaylistRef>::iterator it = albumsBySelectedArtist.begin(); it != albumsBySelectedArtist.end(); ++it){
			PlaylistRef album	= *it;
			NodeAlbum *newNode = new NodeAlbum( this, i, mFont );
			mChildNodes.push_back( newNode );
            trackcount += album->m_tracks.size();
			newNode->setData( album );
			i++;
		}
        std::map<string, string> params;
        params["Artist"] = mPlaylist->getArtistName();
        params["NumAlbums"] = i_to_string(mChildNodes.size());
        params["NumTracks"] = i_to_string(trackcount);
        Flurry::getInstrumentation()->stopTimeEvent("Albums loaded", params);

		for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
			(*it)->setSphereData( mTotalVertsHiRes, mSphereVertsHiRes, mSphereTexCoordsHiRes, mSphereNormalsHiRes,
								  mTotalVertsLoRes, mSphereVertsLoRes, mSphereTexCoordsLoRes, mSphereNormalsLoRes );
		}
		
		setChildOrbitRadii();
		
	}
	Node::select();
}

void NodeArtist::setChildOrbitRadii()
{
	float orbitOffset = mRadiusDest * 1.5f;
	for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
		NodeAlbum* albumNode = (NodeAlbum*)(*it);
		float amt = math<float>::max( albumNode->mNumTracks * 0.01f, 0.06f );
		orbitOffset += amt;
		(*it)->mOrbitRadiusDest = orbitOffset;
		orbitOffset += amt;
	}
}

void NodeArtist::setData( PlaylistRef playlist )
{
	mPlaylist = playlist;
	
	string name		= getName();
	char c1			= ' ';
	char c2			= ' ';
	if( name.length() >= 3 ){
		c1 = name[1];
		c2 = name[2];
	}
	int c1Int = constrain( int(c1), 32, 127 );
	int c2Int = constrain( int(c2), 32, 127 );
	
	int totalCharAscii = ( c1Int - 32 ) + ( c2Int - 32 );
	float asciiPer = ( (float)totalCharAscii/( ( 95.0f ) * 2.0f ) ) * 93.0f ;
	
	mHue			= sin( asciiPer ) * 0.3f + 0.33f;
	

	mSat			= 1.0f - sin( mHue * M_PI );
	mColor			= Color( CM_HSV, mHue, mSat * 0.5f, 1.0f );
	mGlowColor		= Color( CM_HSV, mHue, mSat * 0.5f + 0.5f, 1.0f );
}

string NodeArtist::getName()
{
	string name = mPlaylist->getArtistName();
	if( name.size() < 1 ) name = "Untitled";
	return name;
}

uint64_t NodeArtist::getId()
{
    return mPlaylist->getArtistId();
}
