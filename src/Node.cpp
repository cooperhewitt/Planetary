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
#include "BloomGl.h"
#include "Globals.h"
#include "Node.h"

using namespace ci;
using namespace std;

Node::Node( Node *parent, int index, const Font &font )
	: mParentNode( parent ), mIndex( index ), mFont( font )
{
	if( mParentNode ){
		initWithParent();
	} else {
		init();
	}

	//createNameTexture();
	
    mZoomPer            = 0.0f;
    
	mScreenPos			= Vec2f::zero();
	mEclipsePer			= 1.0f;
	mEclipseStrength	= 0.0f;
	mEclipseAngle		= 0.0f;
	mEclipseDirBasedAlpha = 0.0f;
	mTransPos			= Vec3f::zero();

	mOrbitStartAngle	= Rand::randFloat( TWO_PI );
	mOrbitAngle			= mOrbitStartAngle;
	mOrbitPeriod		= Rand::randFloat( 125.0f, 150.0f ); // TODO: move to NodeArtist and make non-random
	mOrbitRadius		= 0.01f;
	mAngularVelocity	= 0.0f;
	mPercentPlayed		= 0.0f;
	mCamDistAlpha		= 0.0f;
	mDistFromCamZAxis	= 1000.0f;
	mDistFromCamZAxisPer = 1.0f;
	mPlanetTexIndex		= 0;
    
    mSphereRes          = 12;
    mSphereResInt       = 12;
		
	mHitArea			= Rectf( 0.0f, 0.0f, 10.0f, 10.0f ); //just for init.
	mHighlightStrength	= 0.0f;
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

void Node::init()
{
	mGen				= G_ARTIST_LEVEL;
	mPos				= Rand::randVec3f();
	mAcc				= Vec3f::zero();
	mTransVel			= Vec3f::zero();
	mOrbitRadiusDest	= 0.0f;
	mOrbitPeriod		= 0.0f;
}

void Node::initWithParent()
{
	mGen				= mParentNode->mGen + 1;
	mPos				= mParentNode->mPos;
	mTransVel			= Vec3f::zero();
	mOrbitPeriod		= 0.0f;//Rand::randFloat( 35.0f, 50.0f );
}

void Node::setIsDying( bool isDying )
{
	mIsDying = isDying;
	
	if( mIsDying = false ){
		mIsDead = false;
		mDeathCount = 0;
	}
}

void Node::setSphereData( int totalHiVertices, float *sphereHiVerts, float *sphereHiTexCoords, float *sphereHiNormals, 
						 int totalLoVertices, float *sphereLoVerts, float *sphereLoTexCoords, float *sphereLoNormals )
{
	mTotalVertsHiRes		= totalHiVertices;
	mTotalVertsLoRes		= totalLoVertices;
	mSphereVertsHiRes		= sphereHiVerts;
	mSphereTexCoordsHiRes	= sphereHiTexCoords;
	mSphereNormalsHiRes		= sphereHiNormals;
	mSphereVertsLoRes		= sphereLoVerts;
	mSphereTexCoordsLoRes	= sphereLoTexCoords;
	mSphereNormalsLoRes		= sphereLoNormals;
}

void Node::createNameTexture()
{
	TextLayout layout;
	layout.setFont( mFont );
	layout.setColor( Color( 1.0f, 1.0f, 1.0f ) );
	layout.addLine( getName() );
	Surface8u nameSurface	= Surface8u( layout.render( true, false ) );
	mNameTex				= gl::Texture( nameSurface );
}

void Node::update( const Matrix44f &mat )
{	
	mOrbitRadius -= ( mOrbitRadius - mOrbitRadiusDest ) * 0.1f;
	mMatrix         = mat;
	mTransPos       = mMatrix * mPos;
    
	mSphere.setCenter( mTransPos );

    if( mIsPlaying || mIsSelected ){
        mZoomPer    = constrain( ( G_ZOOM - mGen ) + 1.0f, 0.0f, 1.0f );
//        mZoomPer    = constrain( 1.0f - abs( G_ZOOM - mGen + 1.0f ), 0.0f, 1.0f ); 
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
	
	
	
	bool clearChildNodes = false;
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		if( (*nodeIt)->mIsDead ){
			clearChildNodes = true;
		}
		(*nodeIt)->update( mat );
	}
	
	if( clearChildNodes ){
		mIsSelected = false;
		for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
			delete (*nodeIt);
		}
		mChildNodes.clear();
	}
}

void Node::updateGraphics( const CameraPersp &cam, const Vec3f &bbRight, const Vec3f &bbUp )
{
	mBbRight = bbRight;
	mBbUp    = bbUp;
    
    if( mGen >= G_ALBUM_LEVEL ){
        if( mIsSelected || mIsPlaying ){
            mSphereRes		-= ( mSphereRes - 16 ) * 0.1f;
            mCamDistAlpha	-= ( mCamDistAlpha - 1.0f ) * 0.1f;
        } else {
            mSphereRes		-= ( mSphereRes - 10 ) * 0.1f;
            mCamDistAlpha	-= ( mCamDistAlpha - 0.0f ) * 0.1f;
        }
        
		if( mSphereScreenRadius < 20 ){
			mSphereRes		= 4;
		}
        mSphereResInt       = (int)mSphereRes * 2;
    }
    
	if( mIsHighlighted ){
        mScreenPos              = cam.worldToScreen( mTransPos, app::getWindowWidth(), app::getWindowHeight() );
		mPrevDistFromCamZAxis	= mDistFromCamZAxis;
		mDistFromCamZAxis		= cam.worldToEyeDepth( mTransPos );
		mDistFromCamZAxisPer	= constrain( mDistFromCamZAxis * -0.5f, 0.0f, 1.0f ); // REL: -0.35f
		mSphereScreenRadius     = cam.getScreenRadius( mSphere, app::getWindowWidth(), app::getWindowHeight() ) * 0.4f;
        //Vec2f p = mScreenPos + Vec2f( mSphereScreenRadius * 0.25f, 0.0f );
		Vec2f p = mScreenPos;
        float r = mSphereScreenRadius * 0.5f + 5.0f;        
        mSphereHitArea	= Rectf( p.x - r, p.y - r, p.x + r, p.y + r );        
	}
	
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->updateGraphics( cam, mBbRight, mBbUp );
	}
}

void Node::drawEclipseGlow()
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawEclipseGlow();
	}
}

//void Node::drawPlanet( const vector<gl::Texture> &planets )
//{
//	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
//		(*nodeIt)->drawPlanet( planets );
//	}
//}
//
//void Node::drawClouds( const vector<gl::Texture> &planets, const vector<gl::Texture> &clouds )
//{
//	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
//		(*nodeIt)->drawClouds( planets, clouds );
//	}
//}
//
//void Node::drawAtmosphere( const gl::Texture &tex )
//{
//	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
//		(*nodeIt)->drawAtmosphere( tex );
//	}
//}

void Node::drawRings( const gl::Texture &tex, GLfloat *planetRingVerts, GLfloat *planetRingTexCoords, float camZPos )
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawRings( tex, planetRingVerts, planetRingTexCoords, camZPos );
	}
}

void Node::drawOrbitRing( float pinchAlphaOffset, GLfloat *ringVertsLowRes, GLfloat *ringVertsHighRes )
{
	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->drawOrbitRing( pinchAlphaOffset, ringVertsLowRes, ringVertsHighRes );
	}
}



void Node::drawName( const CameraPersp &cam, float pinchAlphaPer, float angle )
{	
	if( cam.worldToEyeDepth( mTransPos ) < 0 ){
		Vec2f pos1, pos2;
		Vec2f offset0, offset1, offset2;
		float screenRadNew = mSphereScreenRadius * 0.25f;
		
		if( mIsSelected || ( G_ZOOM < mGen && mIsPlaying ) ){
			float alpha = 1.0f;
			if( G_ZOOM < mGen - 1 )
				alpha = constrain( ( G_ZOOM - mGen ) + 2.0f, 0.0f, 1.0f );
			else if( G_ZOOM < mGen )
				alpha = pinchAlphaPer;
			
			gl::color( ColorA( Color::white(), alpha * mDeathPer ) );
		} else {
			gl::color( ColorA( COLOR_BRIGHT_BLUE, 0.45f * mZoomPer * mDeathPer ) );
		}

		if (mNameTex == NULL) {
			createNameTexture();
		}

        offset0 = Vec2f( screenRadNew, screenRadNew );
        offset0.rotate( angle );
		pos1 = mScreenPos + offset0;
        offset1 = Vec2f( 10.0f, 10.0f );
        offset1.rotate( angle );
		pos2 = pos1 + offset1;
        offset2 = Vec2f( 2.0f, -8.0f );
        offset2.rotate( angle );

        Vec2f texCorner = mNameTex.getSize();
		
		gl::pushModelView();
		gl::translate( pos2 + offset2 );
        if (angle != 0) {
            gl::rotate( angle * 180.0f/M_PI );
            texCorner.rotate( angle );
        }
		if( mIsPlaying ){
			float s = mZoomPer * 0.25f + 1.0f;
			gl::scale( Vec3f( s, s, 1.0f ) );
            texCorner *= s;
		}
		gl::draw( mNameTex, Vec2f::zero() );
		gl::popModelView();
        
        mHitArea = Rectf( pos2 + offset2, pos2 + offset2 + texCorner);
        mHitArea.canonicalize();        
        inflateRect( mHitArea, 5.0f );
		
		glDisable( GL_TEXTURE_2D );
		
		
		gl::color( ColorA( COLOR_BLUE, 0.4f * mZoomPer * mDeathPer ) );
		gl::drawLine( pos1, pos2 );
		
		
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
		(*nodeIt)->drawName( cam, pinchAlphaPer, angle );
	}
}

void Node::drawTouchHighlight()
{
	if( mIsHighlighted ){
		if( mIsTapped ){
			gl::color( ColorA( mColor, mHighlightStrength ) );
			Vec2f radius = Vec2f( mRadius * 25.0f, mRadius * 25.0f );
			gl::drawBillboard( mTransPos, radius, 0.0f, mBbRight, mBbUp );
			mHighlightStrength -= ( mHighlightStrength - 0.0f ) * 0.15f;
		}
		
		if( mHighlightStrength < 0.01f ){
			mIsTapped = false;
		}

		for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
			(*nodeIt)->drawTouchHighlight();
		}
	}
}

void Node::checkForSphereIntersect( vector<Node*> &nodes, const Ray &ray, Matrix44f &mat )
{
	mSphere.setCenter( mat.transformPointAffine( mPos ) );

	if( mSphere.intersects( ray ) && mIsHighlighted && ! mIsSelected ){
		std::cout << "HIT FOUND" << std::endl;
		nodes.push_back( this );
	}
	
	vector<Node*>::iterator nodeIt;
	for( nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
		(*nodeIt)->checkForSphereIntersect( nodes, ray, mat );
	}
}

void Node::checkForNameTouch( vector<Node*> &nodes, const Vec2f &pos )
{
    if( mSphereHitArea.contains( pos ) || ( mNameTex != NULL && mHitArea.contains( pos ) && G_DRAW_TEXT ) ) {
        nodes.push_back( this );
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
		(*nodeIt)->mIsDying = true;
	}
	mIsSelected = false;
	
//	for( vector<Node*>::iterator nodeIt = mChildNodes.begin(); nodeIt != mChildNodes.end(); ++nodeIt ){
//		delete (*nodeIt);
//	}
//	mChildNodes.clear();
}


