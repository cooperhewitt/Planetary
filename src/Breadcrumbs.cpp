//
//  Breadcrumbs.cpp
//  Kepler
//
//  Created by Tom Carden on 3/22/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include "Globals.h"
#include "Breadcrumbs.h"
#include "CinderFlurry.h"
#include "BloomGl.h"

using namespace pollen::flurry;

void Breadcrumbs::setup( AppCocoaTouch *app, const Font &font )
{	
	if (mApp != NULL) {
		mApp->unregisterTouchesEnded( mCallbackTouchesEnded );
		mApp->unregisterTouchesEnded( mCallbackTouchesBegan );
        mApp->unregisterOrientationChanged( mCallbackOrientationChanged );
	}
	mCallbackTouchesBegan = app->registerTouchesBegan( this, &Breadcrumbs::touchesBegan );
	mCallbackTouchesEnded = app->registerTouchesEnded( this, &Breadcrumbs::touchesEnded );
    mCallbackOrientationChanged = app->registerOrientationChanged( this, &Breadcrumbs::orientationChanged );
    setInterfaceOrientation( app->getInterfaceOrientation() );
    
	mApp = app;
	mFont = font;
	mHierarchyHasChanged = false;
	prevSelectedIndex = -1;
	
	TextLayout layout;
	layout.setFont( mFont );
	layout.setColor( Color::white() );			
	layout.addLine( "•" );
	bool PREMULT = false;
	mSeparatorTexture = gl::Texture( layout.render( true, PREMULT ) );
}

bool Breadcrumbs::orientationChanged( OrientationEvent event )
{
    if (event.getInterfaceOrientation() != mInterfaceOrientation) {
        setInterfaceOrientation( event.getInterfaceOrientation() );
    }
    return false;
}

void Breadcrumbs::setInterfaceOrientation( const Orientation &orientation )
{
    mInterfaceOrientation = orientation;
    mOrientationMtx = getOrientationMatrix44<float>(orientation);
    mInterfaceSize = app::getWindowSize();
    if (isLandscapeOrientation(orientation)) {
        mInterfaceSize = mInterfaceSize.yx();
    }
}

bool Breadcrumbs::touchesBegan( TouchEvent event )
{
    Vec2f pos = event.getTouches()[0].getPos();
	touchesBeganRectIndex = -1;
	for( int i = 0; i < clickRects.size(); i++ ){
		if (clickRects[i].contains(pos)) {
			touchesBeganRectIndex = i;
		}
	}
	return false;
}

bool Breadcrumbs::touchesEnded( TouchEvent event )
{
	for( int i = 0; i < clickRects.size(); i++ ){
		if (clickRects[i].contains(event.getTouches()[0].getPos())) {
			if( i == touchesBeganRectIndex ){
				prevSelectedIndex = i;
				// !!! EVENT STUFF (notify listeners)
				mCallbacksBreadcrumbSelected.call( BreadcrumbEvent( prevSelectedIndex, mPreviousHierarchy[i] ) );
                Flurry::getInstrumentation()->logEvent("Breadcrumbs Used");

				return true;
			}
		}
	}
	return false;
}

void Breadcrumbs::setHierarchy(vector<string> hierarchy)
{
	mPreviousHierarchy = hierarchy;
	mHierarchyHasChanged = true;
}

const vector<string>& Breadcrumbs::getHierarchy()
{
	return mPreviousHierarchy;
}

void Breadcrumbs::update()
{
	if( mHierarchyHasChanged ){
		mTextures.clear();
		
		for( int i = 0; i < mPreviousHierarchy.size(); i++ ){
			TextLayout layout;
			layout.setFont( mFont );
			layout.setColor( Color::white() );			
			layout.addLine(mPreviousHierarchy[i]);
			bool PREMULT = false;
			mTextures.push_back( gl::Texture( layout.render( true, PREMULT ) ) );
		}
		
		mHierarchyHasChanged = false;
	}
}

void Breadcrumbs::draw( const gl::Texture &uiButtonsTex )
{
    float rectHeight = 26.0f;
    Rectf breadcrumbRect( 0.0f, rectHeight, mInterfaceSize.x, 0.0f );
    float lineY = 27.0f;
    float buttonY	= 5.0f;
        
    gl::pushModelView();
    gl::multModelView( mOrientationMtx );
    
	gl::enableAlphaBlending();
	
	gl::color( Color::white() );
	uiButtonsTex.enableAndBind();
	drawButton( breadcrumbRect, 0.41f, 0.9f, 0.49f, 1.0f );
	uiButtonsTex.disable();

	
	gl::color( ColorA( COLOR_BRIGHT_BLUE, 0.1f ) );
	gl::drawLine( Vec2f( 1.0f, lineY ), Vec2f( breadcrumbRect.x2, lineY ) );
    

	gl::color( Color::white() );
	float buttonX	= 25.0f;
	float xMargin	= 5.0f;
	float yMargin	= 20.0f;
	mHeight = 0;
	clickRects.clear();
	
	for( int i=0; i<mPreviousHierarchy.size(); i++ ){
		if( i == mPreviousHierarchy.size() - 1 ){
			gl::color( Color::white() );
		} else {
			gl::color( COLOR_BRIGHT_BLUE );	
		}
        
		if( i > 0 ){
			gl::draw( mSeparatorTexture, Vec2f( buttonX, buttonY ) );
			buttonX += mSeparatorTexture.getWidth() + xMargin*2;
		}
		/*
         if( i == prevSelectedIndex ){
         gl::color( Color( 0.3f, 0.4f, 1.0f ) );
         }
         */
		
		gl::draw( mTextures[i], Vec2f( buttonX, buttonY ) );			
        
        Vec3f topLeft( buttonX-xMargin, buttonY-yMargin, 0 );
        Vec3f bottomRight( buttonX+mTextures[i].getWidth()+xMargin, buttonY+mTextures[i].getHeight()+yMargin, 0 );
        Rectf clickRect( (mOrientationMtx * topLeft).xy(), (mOrientationMtx * bottomRight).xy() );
		clickRects.push_back( clickRect.canonicalized() );
        
		mHeight = max(mHeight, (float)(mTextures[i].getHeight()));
		buttonX += mTextures[i].getWidth() + xMargin*2;
	}		
    
    gl::popModelView();
    
    //    // draw clickRects in screen space for debuggen
    //    gl::color( ColorA( 1.0f, 1.0f, 0.0f, 0.25f ) );	
    //    for (int i = 0; i < clickRects.size(); i++) {
    //        gl::drawSolidRect( clickRects[i] );
    //    }
    
}