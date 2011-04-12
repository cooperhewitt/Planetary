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
	mOrientationMatrix = getOrientationMatrix44<float>( orientation );
    
    Vec2f interfaceSize = getWindowSize();
	
    if ( isLandscapeOrientation( orientation ) ) {
        interfaceSize = interfaceSize.yx(); // swizzle it!
    }
    
	float panelWidth		= mHelpPanelTex.getWidth();
	float border			= ( interfaceSize.x - panelWidth ) * 0.5f;
	
    mPanelRect.x1 = border;
    mPanelRect.x2 = border + panelWidth;
	mCloseRect		= Rectf( mPanelRect.x2 - 20.0f, mPanelRect.y1 - 20.0f, mPanelRect.x2 + 20.0f, mPanelRect.y1 + 20.0f );
	
    // cancel interactions
    mIsCloseTouched   = false;
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
	mCloseRect				= Rectf( mPanelRect.x2 - 35.0f, mPanelRect.y1, mPanelRect.x2, mPanelRect.y1 + 35.0f );

    // just do orientation stuff in here:
	setInterfaceOrientation( mApp->getInterfaceOrientation() );
}
 
bool HelpLayer::touchesBegan( TouchEvent event )
{
	Vec2f touchPos = event.getTouches().begin()->getPos();

    // find where mPanelTabRect is being drawn in screen space (i.e. touchPos space)
    Rectf closeRect = transformRect( mCloseRect, mOrientationMatrix);

    mIsCloseTouched = closeRect.contains( touchPos );
		
	return mIsCloseTouched;
}

bool HelpLayer::touchesMoved( TouchEvent event )
{
	Vec2f touchPos = event.getTouches().begin()->getPos();
    
	if( mIsCloseTouched ){
        // find where mPanelTabRect is being drawn in screen space (i.e. touchPos space)
		Rectf closeRect = transformRect( mCloseRect, mOrientationMatrix);
        
        // apply the touch pos and offset in screen space
        Vec2f newPos = touchPos;
        closeRect.offset(newPos - closeRect.getUpperLeft());

        // pull the screen-space rect back into mPanelTabRect space
        Rectf tabRect = transformRect( closeRect, mOrientationMatrix.inverted() );
	}

	return mIsCloseTouched;
}

bool HelpLayer::touchesEnded( TouchEvent event )
{
	if( mIsCloseTouched ){
        mCallbacksHelpButtonPressed.call( this );
		mIsCloseTouched = false;
    }
    
	return false;
}

void HelpLayer::update()
{

}

void HelpLayer::draw( const gl::Texture &uiButtonsTex )
{	
    gl::pushModelView();
    gl::multModelView( mOrientationMatrix );
    
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
