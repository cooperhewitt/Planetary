/*
 *  UiLayer.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "HelpLayer.h"
#include "CinderFlurry.h"
#include "BloomGl.h"
#include "Globals.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

using namespace pollen::flurry;
using namespace ci;
using namespace ci::app;
using namespace std;

HelpLayer::HelpLayer()
{
}

HelpLayer::~HelpLayer()
{
	mApp->unregisterTouchesBegan( mCbTouchesBegan );
	mApp->unregisterTouchesMoved( mCbTouchesMoved );
	mApp->unregisterTouchesEnded( mCbTouchesEnded );
    mApp->unregisterOrientationChanged( mCbOrientationChanged );
}

void HelpLayer::setInterfaceOrientation( const ci::app::Orientation &orientation )
{
	mInterfaceOrientation = orientation;
	mOrientationMtx = getOrientationMatrix44<float>( orientation );
    
    Vec2f interfaceSize = getWindowSize();
	
    if ( isLandscapeOrientation( orientation ) ) {
        interfaceSize = interfaceSize.yx(); // swizzle it!
    }
    
	float panelWidth		= mHelpPanelTex.getWidth();
	float border			= ( interfaceSize.x - panelWidth ) * 0.5f;
	
    mPanelRect.x1	= border;
    mPanelRect.x2	= border + panelWidth;
	mCloseRect		= Rectf( mPanelRect.x2 - 40.0f, mPanelRect.y1, mPanelRect.x2, mPanelRect.y1 + 40.0f );
	
    // cancel interactions
    mIsCloseTouched = false;
}

bool HelpLayer::orientationChanged( OrientationEvent event )
{
	setInterfaceOrientation( event.getInterfaceOrientation() );
	return false;
}

// TODO: move this to an operator in Cinder's Matrix class?
Rectf HelpLayer::transformRect( const Rectf &rect, const Matrix44f &matrix )
{
    Vec2f topLeft = (matrix * Vec3f(rect.x1,rect.y1,0)).xy();
    Vec2f bottomRight = (matrix * Vec3f(rect.x2,rect.y2,0)).xy();
    Rectf newRect(topLeft, bottomRight);
    newRect.canonicalize();    
    return newRect;
}

void HelpLayer::setup( AppCocoaTouch *app )
{
	mApp = app;
	
	mCbTouchesBegan       = mApp->registerTouchesBegan( this, &HelpLayer::touchesBegan );
	mCbTouchesMoved       = mApp->registerTouchesMoved( this, &HelpLayer::touchesMoved );
	mCbTouchesEnded       = mApp->registerTouchesEnded( this, &HelpLayer::touchesEnded );
    mCbOrientationChanged = mApp->registerOrientationChanged( this, &HelpLayer::orientationChanged );

	mIsCloseTouched			= false;

	mHelpPanelTex			= loadImage( loadResource( "helpPanel.png" ) );
	
	// PANEL AND TAB
	float panelWidth		= mHelpPanelTex.getWidth();
	float panelHeight		= mHelpPanelTex.getHeight();
	float border			= ( getWindowWidth() - panelWidth ) * 0.5f;
	
	mPanelRect				= Rectf( border, border * 2.0f, border + panelWidth, border * 2.0f + panelHeight );
	// REMEMBER, ACTUALLY SET IN setInterfaceOrientation() ^^ THATTA WAY
	mCloseRect				= Rectf( mPanelRect.x2 - 35.0f, mPanelRect.y1, mPanelRect.x2, mPanelRect.y1 + 35.0f );

    // just do orientation stuff in here:
	setInterfaceOrientation( mApp->getInterfaceOrientation() );
}
 
bool HelpLayer::touchesBegan( TouchEvent event )
{
	if( G_HELP ){
		Vec2f touchPos = event.getTouches().begin()->getPos();

		// find where mPanelTabRect is being drawn in screen space (i.e. touchPos space)
		Rectf closeRect = transformRect( mCloseRect, mOrientationMtx );
		Rectf panelRect	= transformRect( mPanelRect, mOrientationMtx );
		
		mIsCloseTouched = closeRect.contains( touchPos );
		if( !mIsCloseTouched ) mIsCloseTouched = !panelRect.contains( touchPos );
			
		return mIsCloseTouched;
	}
	return false;
}

bool HelpLayer::touchesMoved( TouchEvent event )
{
	if( G_HELP ){
		Vec2f touchPos = event.getTouches().begin()->getPos();
		
		if( mIsCloseTouched ){
			// find where mPanelTabRect is being drawn in screen space (i.e. touchPos space)
			Rectf closeRect = transformRect( mCloseRect, mOrientationMtx);
			
			// apply the touch pos and offset in screen space
			Vec2f newPos = touchPos;
			closeRect.offset(newPos - closeRect.getUpperLeft());

			// pull the screen-space rect back into mPanelTabRect space
			// TODO: IS THIS A RELIC? SAFE TO DELETE?
			// Rectf tabRect = transformRect( closeRect, mOrientationMtx.inverted() );
		}

		return mIsCloseTouched;
	}

	return false;
}

bool HelpLayer::touchesEnded( TouchEvent event )
{
	if( G_HELP ){
		vector<TouchEvent::Touch> touches = event.getTouches();
		
		Vec2f pos = touches.begin()->getPos();
		pos = (mOrientationMtx.inverted() * Vec3f(pos,0)).xy();
		
		if( mIsCloseTouched ){
			mCallbacksHelpButtonPressed.call( this );
			mIsCloseTouched = false;
		} else {
			Rectf mailButton( mPanelRect.x1 + 147.0f, mPanelRect.y1 + 226.0f, mPanelRect.x1 + 246.0f, mPanelRect.y1 + 253.0f );
			Rectf cinderButton( mPanelRect.x1 + 133.0f, mPanelRect.y1 + 315.0f, mPanelRect.x1 + 265.0f, mPanelRect.y1 + 336.0f );
			
			if( mailButton.contains( pos ) ){
				Url mailtoLink( "mailto:planetary@bloom.io?subject=Planetary feedback" );
				launchWebBrowser( mailtoLink );
			} else if( cinderButton.contains( pos ) ){
				Url cinderWebsite( "http://libcinder.org" );
				launchWebBrowser( cinderWebsite );
			}
		}
    
	}
	return false;
}

void HelpLayer::update()
{

}

void HelpLayer::draw( const gl::Texture &uiButtonsTex )
{	
    gl::pushModelView();
    gl::multModelView( mOrientationMtx );
    
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	gl::draw( mHelpPanelTex, mPanelRect );
	
	float u1 = 0.4f;
	float u2 = 0.5f;
	float v1 = 0.5f;
	float v2 = 0.7f;

	uiButtonsTex.enableAndBind();
	drawButton( mCloseRect, u1, v1, u2, v2 );
    uiButtonsTex.disable();
        
    gl::popModelView();    
}
