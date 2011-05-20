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
	mGen			= G_ARTIST_LEVEL;
	//mPosDest		= Rand::randVec3f() * Rand::randFloat( 40.0f, 75.0f ); // 40.0f, 200.0f
	Vec2f randVec	= Rand::randVec2f();
	randVec			*= Rand::randFloat( 30.0f, 65.0f );
	mPosDest		= Vec3f( randVec.x, Rand::randFloat( -0.5f, 0.5f ), randVec.y );
	mPos			= mPosDest;// + Rand::randVec3f() * 25.0f;
	mAcc			= Vec3f::zero();
	
	mAge			= 0.0f;
	mBirthPause		= Rand::randFloat( 50.0f );
	
	mOrbitRadiusDest	= 0.0f;
	mOrbitRadiusMin		= mRadiusDest * 1.0f;
	mOrbitRadiusMax		= mRadiusDest * 2.5f;
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
	float asciiPer = ( (float)totalCharAscii/( 190.0f ) ) * 5000.0f ;
	
	mHue			= sin( asciiPer ) * 0.27f + 0.3f;
	
	mSat			= ( 1.0f - sin( ( mHue + 0.15f ) * M_PI ) ) * 0.875f;
	mColor			= Color( CM_HSV, mHue, mSat, 1.0f );
	mGlowColor		= Color( CM_HSV, mHue, mSat + 0.5f, 1.0f );
	
	mRadiusDest		= 1.3f + ( 0.66f - mHue ) * 2.0f;
	mRadius			= 0.0f;
	
	mSphere			= Sphere( mPos, mRadiusDest * 0.165f );
}



void NodeArtist::update( const Matrix44f &mat, const Surface &surfaces )
{	
	mEclipseStrength = 0.0f;
	
	Vec3f prevTransPos  = mTransPos;
    // if mTransPos hasn't been set yet, use a guess:
    // FIXME: set mTransPos correctly in the constructor
    if( prevTransPos.lengthSquared() < 0.0000001 ) prevTransPos = mat * mPos;  
	
	if( mAge < 50.0f ){
		mPosDest += mAcc;
		mAcc *= 0.99f;
	}
	
	if( mAge < 100.0f ){
		mPos -= ( mPos - mPosDest ) * 0.1f;
		mAge ++;
	}
	
	if( mAge > mBirthPause ){
		mRadius -= ( mRadius - mRadiusDest ) * 0.2f;
		
//		if( mIsSelected ){
//			mRadius -= ( mRadius - mRadiusDest ) * 0.2f;
//		} else {
//			if( G_ZOOM > G_ALPHA_LEVEL + 0.5f ){
//				mRadius -= ( mRadius - 0.1f ) * 0.1f;
//				mRadius += Rand::randFloat( 0.0125f );
//			} else {
//				mRadius -= ( mRadius - 0.2f ) * 0.2f;
//			}
//		}
	}
	
	Node::update( mat, surfaces );
    
	mTransVel = mTransPos - prevTransPos;
}

void NodeArtist::drawEclipseGlow()
{
	if( mIsHighlighted && mDistFromCamZAxisPer > 0.0f ){
		/*
        gl::color( ColorA( mGlowColor, mDistFromCamZAxisPer * ( 1.0f - mEclipseStrength ) * 2.0f ) );
		Vec2f radius = Vec2f( mRadius, mRadius ) * 10.0f;
		gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
		 */
		
		float alpha = G_ALPHA_LEVEL - ( G_ZOOM - 1.0f );
		gl::color( ColorA( mGlowColor, mDistFromCamZAxisPer * ( 1.0f - mEclipseStrength ) * alpha ) );
		Vec2f radius = Vec2f( mRadius, mRadius ) * 10.0f;
		gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );

	}
	
	Node::drawEclipseGlow();
}

void NodeArtist::drawPlanet( const vector<gl::Texture> &planets )
{
	if( mIsSelected ){
		glDisable( GL_LIGHTING );
		glDisable( GL_TEXTURE_2D );
/*
		gl::pushModelView();
		gl::translate( mTransPos );
		float radius = mRadius * 0.2875f;
		
		gl::color( mColor );
		gl::enableAlphaBlending();
		gl::drawSolidCircle( Vec2f::zero(), radius, 64 );
		
		
		gl::enableAdditiveBlending();
		gl::pushModelView();
		gl::color( mGlowColor );
		gl::translate( Vec3f::zAxis() * -0.025f );
		gl::drawSolidCircle( Vec2f::zero(), radius * 0.975f, 64 );
		gl::translate( Vec3f::zAxis() * -0.025f );
		gl::drawSolidCircle( Vec2f::zero(), radius * 0.95f, 64 );
 
 */
		gl::popModelView();
		
		gl::popModelView();
		glEnable( GL_LIGHTING );
	}
}

void NodeArtist::drawClouds( const vector<gl::Texture> &clouds )
{
	Node::drawClouds( clouds );
}

void NodeArtist::drawRings( const gl::Texture &tex, GLfloat *planetRingVerts, GLfloat *planetRingTexCoords, float camZPos )
{
	Node::drawRings( tex, planetRingVerts, planetRingTexCoords, camZPos );
}


void NodeArtist::select()
{
	if( !mIsSelected )
	{
		// TODO: switch this to use artist ID
        Flurry::getInstrumentation()->startTimeEvent("Albums loaded");
		vector<ipod::PlaylistRef> albumsBySelectedArtist = getAlbumsWithArtist( mPlaylist->getArtistName() );
		mNumAlbums = albumsBySelectedArtist.size();
		
		if( mChildNodes.size() == 0 ){
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
			
			for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
				(*it)->setSphereData( mTotalVertsHiRes, mSphereVertsHiRes, mSphereTexCoordsHiRes, mSphereNormalsHiRes,
									 mTotalVertsLoRes, mSphereVertsLoRes, mSphereTexCoordsLoRes, mSphereNormalsLoRes );
			}
			
			setChildOrbitRadii();
			
			std::map<string, string> params;
			params["Artist"] = mPlaylist->getArtistName();
			params["NumAlbums"] = i_to_string(mChildNodes.size());
			params["NumTracks"] = i_to_string(trackcount);
			Flurry::getInstrumentation()->stopTimeEvent("Albums loaded", params);
			
			
		} else {
			for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
				(*it)->setIsDying( false );
			}
		}
		
	}
	Node::select();
}

void NodeArtist::setChildOrbitRadii()
{
	float orbitOffset = mRadiusDest;// * 1.0f;
	for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
		NodeAlbum* albumNode = (NodeAlbum*)(*it);
		float amt = math<float>::max( albumNode->mNumTracks * 0.04f, 0.1f );
		orbitOffset += amt;
		(*it)->mOrbitRadiusDest = orbitOffset;
		orbitOffset += amt;
	}
	mIdealCameraDist = orbitOffset * 2.2f;
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
