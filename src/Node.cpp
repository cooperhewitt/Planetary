/*
 *  Node.cpp
 *  Kepler
 *
 *  Created by Robert Hodgin on 2/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "Globals.h"
#include "Node.h"
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

using namespace ci;
using namespace std;
using namespace boost;

Node::Node( Node *parent, int index, const Font &font, const Font &smallFont, const Surface &hiResSurfaces, const Surface &loResSurfaces, const Surface &noAlbumArt )
	: mParentNode( parent ), mIndex( index ), mFont( font ), mSmallFont( smallFont ),
	  mHighResSurfaces( hiResSurfaces ), mLowResSurfaces( loResSurfaces ), mNoAlbumArtSurface( noAlbumArt )
{
    mZoomPer            = 0.0f;
    
	mScreenPos			= Vec2f::zero();
	mEclipseStrength	= 0.0f;
	mEclipseAngle		= 0.0f;
	mEclipseDirBasedAlpha = 0.0f;
	mVel                = Vec3f::zero();
	
	mOrbitStartAngle	= Rand::randFloat( TWO_PI );
	mOrbitAngle			= mOrbitStartAngle;
	mOrbitPeriod		= Rand::randFloat( 225.0f, 250.0f ); // TODO: move to NodeArtist and make non-random
	mOrbitRadius		= 0.01f;
	mAngularVelocity	= 0.0f;
	mPercentPlayed		= 0.0f;
	mDistFromCamZAxis	= 1000.0f;
	mDistFromCamZAxisPer = 1.0f;
	mPlanetTexIndex		= 0;
	mScreenDirToCenter	= Vec2f::zero();
	mScreenDistToCenterPer = 0.0f;
	
	mHitArea			= Rectf( 0.0f, 0.0f, 10.0f, 10.0f ); //just for init.
	mHighlightStrength	= 0.0f;
	
	mGenre				= " ";
	
	mIsTapped			= false;
	mIsSelected			= false;
    mIsPlaying          = false;
	mIsHighlighted		= false;
	mIsDying			= false;
	mIsDead				= false;
	
	mDeathCount			= 0;
	mDeathThresh		= 100;
	mDeathPer			= 0.0f;
}

void Node::setIsDying( bool isDying )
{
	mIsDying = isDying;
	
	if( mIsDying == false ){
		mIsDead = false;
		mDeathCount = 0;
	}
}

void Node::setSphereData( BloomSphere *hiSphere, BloomSphere *mdSphere, BloomSphere *loSphere, BloomSphere *tySphere )
{
    mHiSphere = hiSphere;
    mMdSphere = mdSphere;
    mLoSphere = loSphere;
    mTySphere = tySphere;
}

void Node::createNameTexture()
{
	TextLayout layout;
	
	string name = getName();
	string numberLine1, nameLine1;
	string nameLine2 = "(";
	bool isTwoLines = false;
	if( mGen == G_TRACK_LEVEL ){
		numberLine1 = boost::lexical_cast<string>( getTrackNumber() ) + ". ";
	}
	
	layout.setFont( mSmallFont );
	layout.setColor( Color( 0.5f, 0.5f, 0.5f ) );
	layout.addLine( numberLine1 );
	
	layout.setFont( mFont );
	layout.setColor( Color( 1.0f, 1.0f, 1.0f ) );
		
	if( name.length() > 25 ){
		int counter = 0;
		char_separator<char> sep("(");
		tokenizer< char_separator<char> > tokens(name, sep);
		BOOST_FOREACH(string t, tokens)
		{
			if( counter == 0 ){
				nameLine1 = t;
			} else {
				nameLine2.append( t );
				isTwoLines = true;
			}
			counter ++;
		}
	} else {
		nameLine1 = name;
	}

	layout.append( nameLine1 );
	if( isTwoLines ){
		layout.setFont( mSmallFont );
		layout.setColor( ColorA( 0.0f, 0.0f, 0.0f, 0.0f ) );
		layout.addLine( numberLine1 );
		
		//layout.setFont( mFont );
		layout.setColor( Color( 0.5f, 0.5f, 0.5f ) );
		layout.append( nameLine2 );
	}
	
	/*} else {
		layout.setColor( Color( 1.0f, 1.0f, 1.0f ) );
		layout.setFont( mFont );
		layout.addLine( name );
	}*/
	
	if( mGen == G_ALBUM_LEVEL ){
		layout.setFont( mSmallFont );
		layout.setColor( Color( 0.5f, 0.5f, 0.5f ) );
		
		string yearStr = "";
		int year = getReleaseYear();
		if( year < 0 ){
			yearStr = "Unknown";
		} else if( year < 1900 ){
			yearStr = "Incorrect Data";
		} else {
			yearStr = boost::lexical_cast<string>( getReleaseYear() );	
		}
		layout.addLine( yearStr );
	}
	Surface8u nameSurface	= Surface8u( layout.render( true, false ) );
	mNameTex				= gl::Texture( nameSurface );
}

void Node::update( float param1, float param2 )
{	
	mInvRadius		= ( 1.0f/mRadius ) * 0.5f;
	mClosenessFadeAlpha = constrain( ( mDistFromCamZAxis - mRadius ) * mInvRadius, 0.0f, 1.0f );
	
	mOrbitRadius	-= ( mOrbitRadius - mOrbitRadiusDest ) * 0.1f;
    
	mSphere.setCenter( mPos );

    if( mIsPlaying || mIsSelected ){
        mZoomPer    = constrain( ( G_ZOOM - mGen ) + 2.0f, 0.0f, 1.0f );
	} else {
        mZoomPer    = constrain( 1.0f - abs( G_ZOOM - mGen + 1.0f ), 0.0f, 1.0f );
    }
	mZoomPer = pow( mZoomPer, 4.0f );
	
	
	if( mIsDying ){
		mDeathCount ++;
		if( mDeathCount > mDeathThresh ){
			mIsDead = true;
			mIsSelected = false;
		}
	}
	
	mDeathPer = 1.0f - (float)mDeathCount/(float)mDeathThresh;
	mAge ++;
	
	
	bool clearChildNodes = false;
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		if( (*nodeIt)->mIsDead ){
			clearChildNodes = true;
		}
		(*nodeIt)->update( param1, param2 );
	}
	
	if( clearChildNodes ){
		mIsSelected = false;
		for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
			delete (*nodeIt);
		}
		mChildNodes.clear();
	}
}

void Node::updateGraphics( const CameraPersp &cam, const Vec2f &center, const Vec3f &bbRight, const Vec3f &bbUp, const float &w, const float &h )
{
	mBbRight = bbRight;
	mBbUp    = bbUp;
    
	
	if( mGen >= G_ALBUM_LEVEL || mIsHighlighted ){
		mSphereScreenRadius		= cam.getScreenRadius( mSphere, w, h );
        mScreenPos              = cam.worldToScreen( mPos, w, h );
		mScreenDirToCenter		= mScreenPos - center;
		mScreenDistToCenterPer	= mScreenDirToCenter.length()/500.0f;
		mPrevDistFromCamZAxis	= mDistFromCamZAxis;
		mDistFromCamZAxis		= -cam.worldToEyeDepth( mPos );
		mDistFromCamZAxisPer	= constrain( mDistFromCamZAxis * 0.5f, 0.0f, 1.0f ); // REL: -0.35f
        float r					= max( mSphereScreenRadius, 15.0f );        
        mSphereHitArea			= Rectf( mScreenPos.x - r, mScreenPos.y - r, mScreenPos.x + r, mScreenPos.y + r );        
	}
	
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->updateGraphics( cam, center, mBbRight, mBbUp, w, h );
	}
}

void Node::drawEclipseGlow()
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawEclipseGlow();
	}
}

void Node::drawRings( const gl::Texture &tex, const PlanetRing &planetRing, float camZPos )
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawRings( tex, planetRing, camZPos );
	}
}

void Node::drawOrbitRing( float pinchAlphaOffset, float camAlpha, const OrbitRing &orbitRing )
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawOrbitRing( pinchAlphaOffset, camAlpha, orbitRing );
	}
}

void Node::drawName( const CameraPersp &cam, float pinchAlphaPer, float angle )
{	
	if( mDistFromCamZAxis > 0 ){		
		float alpha;
		Color c;
		
		if( mIsSelected || ( G_CURRENT_LEVEL < mGen && mIsPlaying ) ){
			c = Color::white();
			
			if( G_CURRENT_LEVEL >= mGen - 2 )
				alpha = mZoomPer * mDeathPer;
			else
				alpha = 0.0f;
			
			
		} else {
			c = BRIGHT_BLUE;
			if( G_CURRENT_LEVEL >= mGen - 1 )
				alpha = 0.5f * pinchAlphaPer * mZoomPer * mDeathPer;
			else
				alpha = 0.0f;
		}
		
		ColorA col = ColorA( c, alpha );
		
		
		if( alpha > 0 ){
			Vec2f pos1, pos2;
			Vec2f offset0, offset1, offset2;
			
			if (mNameTex == NULL) {
				createNameTexture();
			}

			offset0 = Vec2f( mSphereScreenRadius, mSphereScreenRadius ) * 0.75f;
			offset0.rotate( angle );
			pos1 = mScreenPos + offset0;
			
			offset1 = Vec2f( 5.0f, 5.0f ) * ( ( G_TRACK_LEVEL + 1.0f ) - mGen );
			offset1.rotate( angle );
			pos2 = pos1 + offset1;
			offset2 = Vec2f( 2.0f, -8.0f );
			offset2.rotate( angle );

			Vec2f texCorner = mNameTex.getSize();
			
			glPushMatrix();
			gl::translate( pos2 + offset2 );
			if (angle != 0) {
				gl::rotate( angle * 180.0f/M_PI );
				texCorner.rotate( angle );
			}
			if( mIsPlaying ){
				float s = (mZoomPer * 0.25f) + 1.0f;
				gl::scale( Vec3f( s, s, 1.0f ) );
				texCorner *= s;
			}
			
		// DRAW DROP SHADOW
			if( mIsPlaying ){ 
				gl::enableAlphaBlending();
				gl::color( ColorA( 0.0f, 0.0f, 0.0f, alpha * 0.35f ) );
				gl::draw( mNameTex, Vec2f( 1.0f, 1.0f ) );
				gl::enableAdditiveBlending();
			}
			
			gl::color( col );
			gl::draw( mNameTex, Vec2f::zero() );
			
			glPopMatrix();
			
			mHitArea = Rectf( pos2 + offset2, pos2 + offset2 + texCorner);
			mHitArea.canonicalize();        
            const float inflate = 5.0f;
            // TODO: add .inflate to ci::Rectf
            mHitArea.x1 -= inflate;
            mHitArea.x2 += inflate;
            mHitArea.y1 -= inflate;
            mHitArea.y2 += inflate;
			
			// TODO: this is a lot of state changes per frame. Switch to drawing
			// all names first, then all lines?
			glDisable( GL_TEXTURE_2D );
			
			gl::color( ColorA( BRIGHT_BLUE, alpha * 0.5f ) );
			gl::drawLine( pos1, pos2 );
		} else {
			mHitArea = Rectf( -10000.0f, -10000.0f, -9999.0f, -9999.0f );
		}
		
		
		
		/*
		// For viewing node states
		if( mIsHighlighted ){
			gl::color( Color( 1.0f, 0.0f, 0.0f ) );
			gl::drawLine( pos1 + Vec2f( 1.0f, -1.0f ), pos1 + Vec2f( -1.0f, 1.0f ) );
		}
		
		if( mIsSelected ){
			gl::color( Color( 0.0f, 1.0f, 0.0f ) );
			gl::drawLine( pos1 + Vec2f( 3.0f, 1.0f ), pos1 + Vec2f( 1.0f, 3.0f ) );
		}
		
		if( mIsPlaying ){
			gl::color( Color( 0.0f, 0.0f, 1.0f ) );
			gl::drawLine( pos1 + Vec2f( 5.0f, 3.0f ), pos1 + Vec2f( 3.0f, 5.0f ) );
		}
		 */
	}
	
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		if( (*nodeIt)->mIsHighlighted ){
            (*nodeIt)->drawName( cam, pinchAlphaPer, angle );
		}
	}
}

void Node::drawTouchHighlight( float zoomAlpha )
{
	if( mIsHighlighted ){
		Vec2f radius = Vec2f( mRadius * 5.0f, mRadius * 5.0f );
		if( mIsTapped ){
			gl::color( ColorA( mColor, mHighlightStrength ) );
			mHighlightStrength -= ( mHighlightStrength - 0.0f ) * 0.1f;
			gl::drawBillboard( mPos, radius, 0.0f, mBbRight, mBbUp );
		}
		
		
		if( G_DRAW_RINGS && mClosenessFadeAlpha > 0.0f ){
			if( mGen == G_TRACK_LEVEL ){
				float alpha = max( ( 0.7f - mDistFromCamZAxisPer ) * mDeathPer, 0.0f );
				gl::color( ColorA( BRIGHT_BLUE, ( alpha + mEclipseStrength * mDeathPer ) * mClosenessFadeAlpha ) );
				gl::drawBillboard( mPos, radius, 0.0f, mBbRight, mBbUp );
			} else if( mGen == G_ALBUM_LEVEL ){
				float alpha = constrain( ( 5.0f - mDistFromCamZAxis ) * 0.2f, 0.0f, 1.0f );
				gl::color( ColorA( BRIGHT_BLUE, ( alpha + mEclipseStrength * mDeathPer ) * mClosenessFadeAlpha ) );
				gl::drawBillboard( mPos, radius, 0.0f, mBbRight, mBbUp );
			}
		}
		
		if( mHighlightStrength < 0.01f ){
			mIsTapped = false;
		}

		for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
			(*nodeIt)->drawTouchHighlight( zoomAlpha );
		}
	}
}

void Node::checkForNameTouch( vector<Node*> &nodes, const Vec2f &pos )
{
	if( mIsHighlighted && mDistFromCamZAxisPer > 0.0f ){
		if( mSphereHitArea.contains( pos ) || ( mNameTex != NULL && mHitArea.contains( pos ) && G_DRAW_TEXT ) ) {
			nodes.push_back( this );
		}
	}
    vector<Node*>::iterator nodeIt;
    for( nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
        (*nodeIt)->checkForNameTouch( nodes, pos );
    }
}

void Node::select()
{
	mIsSelected = true;
	setIsDying( false );
	
    vector<Node*>::iterator nodeIt;
	for( nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
        (*nodeIt)->setIsDying( false );
    }
}

void Node::deselect()
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->setIsDying( true );
	}
	mIsSelected = false;
}


