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
#include <deque>
#import "CinderFlurry.h"

using namespace pollen::flurry;
using namespace ci;
using namespace ci::ipod;
using namespace std;

NodeArtist::NodeArtist( int index, const Font &font, const Font &smallFont, const Surface &hiResSurfaces, const Surface &loResSurfaces, const Surface &noAlbumArt )
	: Node( NULL, index, font, smallFont, hiResSurfaces, loResSurfaces, noAlbumArt )
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
	mOrbitRadiusMin		= mRadiusInit * 1.0f;
	mOrbitRadiusMax		= mRadiusInit * 2.5f;
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
	
	mHue			= sin( asciiPer ) * 0.4f + 0.4f; // Range of 0.0(red) to 0.8(deep purple)
	
	mSat			= ( 1.0f - sin( ( mHue + 0.15f ) * M_PI ) ) * 0.75f;
	mColor			= Color( CM_HSV, mHue, mSat, 1.0f );
	mGlowColor		= Color( CM_HSV, mHue, min( mSat + 0.2f, 1.0f ), 1.0f );
	
	mRadiusInit		= 1.25f + ( 0.66f - mHue );
	mRadiusDest		= mRadiusInit;
	mRadius			= 0.0f;
	
	mSphere			= Sphere( mPos, mRadiusInit * 0.175f );
	
	mAxialVel		= Rand::randFloat( 12.0f, 25.0f );
	mAxialRot			= Vec3f( 0.0f, Rand::randFloat( 150.0f ), 0.0f );
}



void NodeArtist::update( float param1, float param2 )
{	
//	float hue		= mHue + Rand::randFloat( 0.05f );
//	mSat			= ( 1.0f - sin( ( hue + 0.15f ) * M_PI ) ) * 0.875f;
//	mColor			= Color( CM_HSV, hue, mSat + 0.2f, 1.0f );
//	mGlowColor		= Color( CM_HSV, hue, mSat + 0.5f, 1.0f );
	
	
	mAxialRot.y += mAxialVel * ( param2 * 3.0f );
	mEclipseStrength = 0.0f;
	
	Vec3f prevPos  = mPos;
	
	if( mAge < 50.0f ){
		mPosDest += mAcc;
		mAcc *= 0.99f;
	}
	
	if( mAge < 100.0f ){
		mPos -= ( mPos - mPosDest ) * 0.1f;
	}
	
	if( mAge > mBirthPause ){
		if( mIsSelected )
			mRadiusDest		= mRadiusInit * param1;
			mRadius			-= ( mRadius - mRadiusDest ) * 0.2f;
			mSphere			= Sphere( mPos, mRadius * 0.175f );
		
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
	
	Node::update( param1, param2 );
    
	mVel = mPos - prevPos;
	mEclipseStrength = constrain( mEclipseStrength, 0.0f, 1.0f );
}

// NOT BEING USED AT THE MOMENT
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
		gl::drawBillboard( mPos, radius, 0.0f, mBbRight, mBbUp );

	}
	
	Node::drawEclipseGlow();
}

void NodeArtist::drawPlanet( const gl::Texture &tex )
{
	if( ( mIsSelected || mIsPlaying ) ){// && ( mHue < 0.2f || mHue > 0.7f ) ){	// Only needed for red/orange stars
        // FIXME: move rotation calculation to something called from main app's update()
		mAxialRot = Vec3f( 0.0f, app::getElapsedSeconds() * mAxialVel * 0.75f, mAxialTilt );
		
		gl::pushModelView();
		gl::translate( mPos );
		gl::scale( Vec3f( mRadius, mRadius, mRadius ) * mDeathPer * 0.16f );
		gl::rotate( mAxialRot );
		gl::color( ColorA( ( mColor + Color::white() ) * 0.5f, 1.0f ) );

		tex.enableAndBind();		
		glDisable( GL_LIGHTING );
        
		if( mSphereScreenRadius < 600.0f ){
			if( mSphereScreenRadius > 75.0f ){
                mHiSphere->draw();
			} else if( mSphereScreenRadius > 35.0f ){
                mMdSphere->draw();
			} else if( mSphereScreenRadius > 10.0f ){
                mLoSphere->draw();
			} else {
                mTySphere->draw();
			}
		} else {
            mLoSphere->draw();
		}
        
		glEnable( GL_LIGHTING );        
		gl::popModelView();		
	}
}

void NodeArtist::drawAtmosphere( const Vec2f &center, const gl::Texture &tex, const gl::Texture &directionalTex, float pinchAlphaPer )
{
	if( mIsHighlighted ){
		Vec2f dir		= mScreenPos - center;
		float dirLength = dir.length()/500.0f;
		float angle		= atan2( dir.y, dir.x );
		float stretch	= dirLength * mRadius * 0.1f;
		float alpha = ( 1.0f - dirLength * 0.75f );
		alpha *= mDeathPer;
		
//		gl::color( ColorA( ( mGlowColor + BRIGHT_BLUE ) * 0.5f, alpha * ( 0.2f - mHue ) * 15.0f ) );
		gl::color( ColorA( ( mColor + Color::white() ), alpha ) );
		
		float radiusOffset = ( ( mSphereScreenRadius/300.0f ) ) * 0.1f;
		Vec2f radius = Vec2f( mRadius, mRadius ) * ( 2.46f + radiusOffset ) * 0.16f;
		//Vec2f radius = Vec2f( mRadius, mRadius ) * 2.46f;
		
		tex.enableAndBind();
		Vec3f posOffset = Vec3f( cos(angle), sin(angle), 0.0f ) * stretch * 0.1f;
		gl::drawBillboard( mPos - posOffset, radius, -toDegrees( angle ), mBbRight, mBbUp );
		tex.disable();
	}
	//}
}

void NodeArtist::drawExtraGlow( const gl::Texture &tex )
{
	if( mIsHighlighted ){
		Vec2f dir		= mScreenPos - app::getWindowCenter();
		float dirLength = dir.length()/800.0f;
		float angle		= atan2( dir.y, dir.x );
		float stretch	= dirLength * mRadius * 0.1f;
		float alpha = ( 1.0f - dirLength ) * sin( mEclipseStrength * M_PI_2 + M_PI_2 );
		alpha *= mDeathPer;
		
		//		gl::color( ColorA( ( mGlowColor + BRIGHT_BLUE ) * 0.5f, alpha * ( 0.2f - mHue ) * 15.0f ) );

		Vec2f radius = Vec2f( mRadius, mRadius ) * 9.5f;
		//Vec2f radius = Vec2f( mRadius, mRadius ) * 2.46f;
		
		tex.enableAndBind();
		Vec3f posOffset = Vec3f( cos(angle), sin(angle), 0.0f ) * stretch * 0.1f;
		
		gl::color( ColorA( mGlowColor, alpha * 0.65f ) );
		gl::drawBillboard( mPos - posOffset, radius, -toDegrees( angle ), mBbRight, mBbUp );
		
	// SMALLER INNER GLOW
		//gl::color( ColorA( mGlowColor, sin( mEclipseStrength * M_PI ) * 0.5f + 0.2f ) );
		gl::color( ColorA( mGlowColor, sin( ( mEclipseStrength * 0.75f + 0.25f ) * M_PI ) * sin( mEclipseStrength * 1.0f + 0.4f ) ) );
		gl::drawBillboard( mPos - posOffset, radius * sin( ( mEclipseStrength * 0.75f + 0.25f ) * M_PI ) * sin( mEclipseStrength * 1.5f + 0.2f ), -toDegrees( angle ), mBbRight, mBbUp );
		tex.disable();
	}
	//}
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
				NodeAlbum *newNode = new NodeAlbum( this, i, mFont, mSmallFont, mHighResSurfaces, mLowResSurfaces, mNoAlbumArtSurface );
				mChildNodes.push_back( newNode );
				trackcount += album->m_tracks.size();
				newNode->setData( album );
				i++;
			}
			
			for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
				(*it)->setSphereData( mHiSphere, mMdSphere, mLoSphere, mTySphere );
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

bool yearSortFunc( Node* a, Node* b ){
	return a->getReleaseYear() < b->getReleaseYear();
}

void NodeArtist::setChildOrbitRadii()
{
	std::vector<Node*> sortedNodes;
	if( mChildNodes.size() > 0 ){
		sort( mChildNodes.begin(), mChildNodes.end(), yearSortFunc );

		float orbitOffset = 1.25f;
		for( vector<Node*>::iterator it = mChildNodes.begin(); it != mChildNodes.end(); ++it ){
			NodeAlbum* albumNode = (NodeAlbum*)(*it);
			float amt = math<float>::max( albumNode->mNumTracks * 0.025f, 0.05f );
			orbitOffset += amt;
			(*it)->mOrbitRadiusDest = orbitOffset;
			orbitOffset += amt;
		}
		mIdealCameraDist = orbitOffset * 2.5f;
	}
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
