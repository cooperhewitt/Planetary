/*
 *  AlphaWheel.cpp
 *  Kepler
 *
 *  Created by Tom Carden on 3/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "AlphaWheel.h"
#include "cinder/gl/gl.h"
#include "cinder/Font.h"
#include "cinder/Text.h"
#include "cinder/ImageIo.h"
#include "Globals.h"
#include <sstream>

using std::stringstream;
using namespace ci;
using namespace ci::app;
using namespace std;

AlphaWheel::AlphaWheel()
{
}

AlphaWheel::~AlphaWheel()
{
	mApp->unregisterTouchesBegan( mCbTouchesBegan );
	mApp->unregisterTouchesMoved( mCbTouchesMoved );
	mApp->unregisterTouchesEnded( mCbTouchesEnded );
	mApp->unregisterOrientationChanged( mCbOrientationChanged );
}


void AlphaWheel::setup( AppCocoaTouch *app )
{
	mApp = app;
	
	mCbTouchesBegan = mApp->registerTouchesBegan( this, &AlphaWheel::touchesBegan );
	mCbTouchesMoved = mApp->registerTouchesMoved( this, &AlphaWheel::touchesMoved );
	mCbTouchesEnded = mApp->registerTouchesEnded( this, &AlphaWheel::touchesEnded );
    mCbOrientationChanged = mApp->registerOrientationChanged( this, &AlphaWheel::orientationChanged );
	
	// Textures
	mWheelTex		= gl::Texture( loadImage( loadResource( "alphaWheel.png" ) ) );
	mWheelMaskTex	= gl::Texture( loadImage( loadResource( "alphaWheelMask.png" ) ) );
	mBlurRectTex	= gl::Texture( loadImage( loadResource( "blurRect.png" ) ) );
	
	mAlphaString	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ#";
	mAlphaIndex		= 0;
	mAlphaChar		= ' ';
	mPrevAlphaChar	= ' ';
	mShowWheel		= false;
	mWheelScale		= 1.0f;	

    // just do orientation stuff in here:
    setInterfaceOrientation(mApp->getInterfaceOrientation());
}

void AlphaWheel::initAlphaTextures( const Font &font )
{
	for( int i=0; i<mAlphaString.length(); i++ ){
		TextLayout layout;	
		layout.setFont( font );
		layout.setColor( ColorA( 1.0f, 1.0f, 1.0f, 0.3f ) );
		stringstream s;
		s.str("");
		s << mAlphaString[i];
		layout.addCenteredLine( s.str() );
		mAlphaTextures.push_back( gl::Texture( layout.render( true, false ) ) );
	}
}

bool AlphaWheel::orientationChanged( OrientationEvent event )
{
    if (mInterfaceOrientation != event.getInterfaceOrientation()) {
        setInterfaceOrientation(event.getInterfaceOrientation()); 
    }
    return false;
}

void AlphaWheel::setInterfaceOrientation( const Orientation &orientation )
{
    mInterfaceOrientation = orientation;
    
    mOrientationMatrix = getOrientationMatrix44<float>( mInterfaceOrientation );
    
    Vec2f interfaceSize = getWindowSize();
    
    if ( isLandscapeOrientation( mInterfaceOrientation ) ) {
        interfaceSize = interfaceSize.yx(); // swizzle it!
    }
    
    mInterfaceCenter = interfaceSize * 0.5f;
}

bool AlphaWheel::touchesBegan( TouchEvent event )
{
	vector<TouchEvent::Touch> touches = AppCocoaTouch::get()->getActiveTouches();

	if (touches.size() == 1) {
		mTouchPos = touches.begin()->getPos();
		return selectWheelItem( mTouchPos, false );
	}
	
	return false;
}

bool AlphaWheel::touchesMoved( TouchEvent event )
{
	vector<TouchEvent::Touch> touches = AppCocoaTouch::get()->getActiveTouches();
	
	if (touches.size() == 1) {
		mTouchPos = touches.begin()->getPos();
		return selectWheelItem( mTouchPos, false );
	}	
	
	return false;
}

bool AlphaWheel::touchesEnded( TouchEvent event )
{	
	vector<TouchEvent::Touch> touches = AppCocoaTouch::get()->getActiveTouches();
	
	if (touches.size() == 0) {
		return selectWheelItem( mTouchPos, true );
	}
	
	return false;
}


bool AlphaWheel::selectWheelItem( const Vec2f &pos, bool closeWheel )
{
	float timeSincePinchEnded = getElapsedSeconds() - mTimePinchEnded;
	if( mShowWheel && timeSincePinchEnded > 0.5f ){ 
        Vec2f dir = (mOrientationMatrix.inverted() * Vec3f(mTouchPos,0)).xy() - mInterfaceCenter;
		float distToCenter = dir.length();
		if( distToCenter > 225 && distToCenter < 300 ){
			float touchAngle	= atan2( dir.y, dir.x ) + M_PI;				// RANGE 0 -> TWO_PI
			float anglePer		= ( touchAngle + 0.11365f + M_PI*1.5f )/TWO_PI;
			mAlphaIndex			= (int)( anglePer * 27 )%27;
			mPrevAlphaChar		= mAlphaChar;
			mAlphaChar			= mAlphaString.at( mAlphaIndex % mAlphaString.size() );
			if( mPrevAlphaChar != mAlphaChar || closeWheel ){
				mCallbacksAlphaCharSelected.call( this );
			}
		}
		if( closeWheel ){
			mShowWheel = false;
			mCallbacksWheelClosed.call( this );
		}		
        return distToCenter > 225 && distToCenter < 300;
	}
    return false;
}

void AlphaWheel::setTimePinchEnded( float timePinchEnded )
{
	mTimePinchEnded = timePinchEnded;	
}

void AlphaWheel::update( float fov )
{
	//mWheelScale = ( ( 130.0f - fov ) / 30.0f );
	if( getShowWheel() ){
		mWheelScale -= ( mWheelScale - 0.0f ) * 0.2f;
	} else {
		mWheelScale -= ( mWheelScale - 1.15f ) * 0.2f;	
	}	
}

void AlphaWheel::draw( GLfloat *verts, GLfloat *texCoords, GLfloat *colors )
{
	if( mWheelScale < 0.95f ){
		gl::pushModelView();
        gl::multModelView( mOrientationMatrix );
        
		gl::translate( mInterfaceCenter );
		gl::scale( Vec3f( mWheelScale + 1.0f, mWheelScale + 1.0f, 1.0f ) );


        drawWheelMask();
        drawWheelData( verts, texCoords, colors );
		drawWheel();
		
		if( mAlphaChar != ' ' )
			drawAlphaChar();
        
        gl::popModelView();
	}
}

void AlphaWheel::drawWheelMask()
{
    float wMask = mWheelMaskTex.getWidth() * 0.5f;
    float hMask = mWheelMaskTex.getHeight() * 0.5f;

    float c = 1.0f - mWheelScale;
    gl::color( ColorA( c, c, c, c ) );
    
    mWheelMaskTex.enableAndBind();
    gl::drawSolidRect( Rectf( -wMask, -hMask, wMask, hMask ) );
    mWheelMaskTex.disable();    
	
    if ( isLandscapeOrientation(mInterfaceOrientation) ) {
        Vec2f interfaceSize = getWindowSize().yx(); // SWIZ!
        gl::color( Color::black() );
        // left bar, relative to center:
        gl::drawSolidRect( Rectf( -interfaceSize.x/2, -interfaceSize.y/2, -wMask, hMask ) );
        // right bar, relative to center:
        gl::drawSolidRect( Rectf( wMask, -interfaceSize.y/2, interfaceSize.x/2, hMask ) );
    } else {
		Vec2f interfaceSize = getWindowSize().xy();
		gl::color( Color::black() );
        // top bar, relative to center:
        gl::drawSolidRect( Rectf( -interfaceSize.x/2, -interfaceSize.y/2, interfaceSize.x/2, -hMask ) );
        // bottom bar, relative to center:
        gl::drawSolidRect( Rectf( -interfaceSize.x/2, hMask, interfaceSize.x/2, interfaceSize.y/2 ) );
	}
}

void AlphaWheel::drawWheel()
{
	float wWheel = mWheelTex.getWidth() * 0.5f;
	float hWheel = mWheelTex.getWidth() * 0.5f;
	
    float c = 1.0f - mWheelScale;
    gl::color( ColorA( c, c, c, c ) );

	mWheelTex.enableAndBind();
    gl::drawSolidRect( Rectf( -wWheel, -hWheel, wWheel, hWheel ) );
    mWheelTex.disable(); 
}

void AlphaWheel::drawWheelData( GLfloat *verts, GLfloat *texCoords, GLfloat *colors )
{
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f - mWheelScale ) );
	gl::pushModelView();
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, verts );
	glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
	glColorPointer( 4, GL_FLOAT, 0, colors );
	gl::enableAlphaBlending();
	mBlurRectTex.enableAndBind();
	glDrawArrays( GL_TRIANGLES, 0, 27*6 );
	mBlurRectTex.disable();
	gl::enableAlphaBlending();
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	gl::popModelView();
}

void AlphaWheel::drawAlphaChar()
{
	//std::cout << "AlphaWheel::drawAlphaChar mAlphaIndex = " << mAlphaIndex << std::endl;
	float w = mAlphaTextures[mAlphaIndex].getWidth() * 0.5f;
	float h = mAlphaTextures[mAlphaIndex].getHeight() * 0.5f;
	
	gl::color( ColorA( COLOR_BLUE, 1.0f - mWheelScale ) );
	
	gl::enableAdditiveBlending();
    mAlphaTextures[mAlphaIndex].enableAndBind();
	gl::drawSolidRect( Rectf( -w, -h, w, h ) );
	mAlphaTextures[mAlphaIndex].disable();
	gl::enableAlphaBlending();
}
