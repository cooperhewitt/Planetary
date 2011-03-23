/*
 *  Breadcrumbs.h
 *  CinderFizz
 *
 *  Created by Tom Carden on 2/15/11.
 *  Copyright 2011 Bloom Studio Inc. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <vector>
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rect.h"
#include "cinder/Text.h"
#include "cinder/Font.h"
#include "cinder/Function.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BreadcrumbEvent : public ci::app::Event {
private:
	std::string mLabel;
	int mLevel;
public:	
	BreadcrumbEvent( int level, std::string label ): ci::app::Event(), mLevel(level), mLabel(label) {
	}
	~BreadcrumbEvent() { }
	int getLevel() {
		return mLevel;
	}
	std::string getLabel() {
		return mLabel;
	}
};

class Breadcrumbs {

public:

	Breadcrumbs()
    {
		mApp = NULL;
		mHeight = 0.0f;
	}
	
	~Breadcrumbs()
    { 
        // TODO: any event cleanup?
    }

	// !!! EVENT STUFF (slightly nicer interface for adding listeners)
	template<typename T>
	CallbackId registerBreadcrumbSelected( T *obj, bool (T::*callback)(BreadcrumbEvent) ){
		return mCallbacksBreadcrumbSelected.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}	
	
	void setup( AppCocoaTouch *app, const Font &font);
	bool touchesBegan( TouchEvent event );
	bool touchesEnded( TouchEvent event );
    bool orientationChanged( OrientationEvent event );
	void setHierarchy(vector<string> hierarchy);
	const vector<string>& getHierarchy();
	void update();
	void draw();
	float getHeight() { return mHeight; }

private:
	
	AppCocoaTouch *mApp;
	Font mFont;
	
	float mHeight; // calculated in draw()
	
	vector<string> mPreviousHierarchy;
	bool mHierarchyHasChanged;
	
	vector<gl::Texture> mTextures;
	gl::Texture mSeparatorTexture;
	vector<Rectf> clickRects;
	int touchesBeganRectIndex;
	int prevSelectedIndex;
    
    DeviceOrientation mDeviceOrientation;
	
	CallbackId mCallbackTouchesBegan;
	CallbackId mCallbackTouchesEnded;
    CallbackId mCallbackOrientationChanged;
	
	//////////////////////
	
	// !!! EVENT STUFF (keep track of listeners)
	CallbackMgr<bool(BreadcrumbEvent)> mCallbacksBreadcrumbSelected;
	
};
<<<<<<< HEAD
=======

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

    // TODO:
    //mDeviceOrientation = app->getOrientation();
    mDeviceOrientation = PORTRAIT_ORIENTATION;
    
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

bool Breadcrumbs::orientationChanged(OrientationEvent event)
{
    // TODO: OrientationEvent helper for this?
    if (UIDeviceOrientationIsValidInterfaceOrientation(event.getOrientation())) {
        mDeviceOrientation = event.getOrientation();
    }
	return false;
}

bool Breadcrumbs::touchesBegan( TouchEvent event )
{
    Vec2f pos = event.getTouches()[0].getPos();
    std::cout << "touchesBegan: " << pos << std::endl;
	touchesBeganRectIndex = -1;
	for( int i = 0; i < clickRects.size(); i++ ){
        std::cout << "testing: " << clickRects[i] << std::endl;
		if (clickRects[i].contains(pos)) {
            std::cout << "YES!" << std::endl;
			touchesBeganRectIndex = i;
		}
	}
	
	std::cout << "touchesBeganRectIndex = " << touchesBeganRectIndex << std::endl;
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

void Breadcrumbs::draw()
{
    float width = app::getWindowWidth();
    float height = app::getWindowHeight();
    float rectHeight = 24.0f;
    Rectf breadcrumbRect( 0.0f, 0.0f, width, rectHeight );
    float lineY = 25.0f;
    float buttonY	= 3.0f;

    Matrix44f orientationMtx;
    
    switch ( mDeviceOrientation )
    {
        case UPSIDE_DOWN_PORTRAIT_ORIENTATION:
            orientationMtx.translate( Vec3f( width, height, 0 ) );            
            orientationMtx.rotate( Vec3f( 0, 0, M_PI ) );
            break;
        case LANDSCAPE_LEFT_ORIENTATION:
            orientationMtx.translate( Vec3f( width, 0, 0 ) );
            orientationMtx.rotate( Vec3f( 0, 0, M_PI/2.0f ) );
            breadcrumbRect.x2 = height;
            break;
        case LANDSCAPE_RIGHT_ORIENTATION:
            orientationMtx.translate( Vec3f( 0, height, 0 ) );
            orientationMtx.rotate( Vec3f( 0, 0, -M_PI/2.0f ) );
            breadcrumbRect.x2 = height;
            break;
        default:
            break;
    }
    
    gl::pushModelView();
    gl::multModelView( orientationMtx );
    
	gl::color( ColorA( Color::black(), 0.75f ) );
	gl::drawSolidRect( breadcrumbRect );
	
	gl::enableAlphaBlending();
	gl::color( ColorA( Color::white(), 0.1f ) );
	gl::drawLine( Vec2f( 1.0f, lineY ), Vec2f( breadcrumbRect.x2, lineY ) );

	gl::enableAlphaBlending(false);
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
			gl::color( COLOR_BLUE );	
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
        Rectf clickRect( (orientationMtx * topLeft).xy(), (orientationMtx * bottomRight).xy() );
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
>>>>>>> 2b173e9b4b4f70ac18b8b13459f583fd85fd1c17
