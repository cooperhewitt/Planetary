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
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Font.h"
#include "cinder/app/TouchEvent.h"
#include "BreadcrumbEvent.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Breadcrumbs {

public:

	Breadcrumbs() {
		mApp = NULL;
		mHeight = 0.0f;
	}
	
	~Breadcrumbs() {
		
	}

	// !!! EVENT STUFF (slightly nicer interface for adding listeners)
	template<typename T>
	CallbackId registerBreadcrumbSelected( T *obj, bool (T::*callback)(BreadcrumbEvent) ){
		return mCallbacksBreadcrumbSelected.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}	
	
	void setup( AppCocoaTouch *app, const Font &font);
	bool touchesBegan( TouchEvent event );
	bool touchesEnded( TouchEvent event );
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
	
	CallbackId mCallbackTouchesBegan;
	CallbackId mCallbackTouchesEnded;
	
	//////////////////////
	
	// !!! EVENT STUFF (keep track of listeners)
	CallbackMgr<bool(BreadcrumbEvent)> mCallbacksBreadcrumbSelected;
	
};

void Breadcrumbs::setup( AppCocoaTouch *app, const Font &font ){
	
	if (mApp != NULL) {
		mApp->unregisterTouchesEnded( mCallbackTouchesEnded );
		mApp->unregisterTouchesEnded( mCallbackTouchesBegan );
	}
	mCallbackTouchesBegan = app->registerTouchesBegan( this, &Breadcrumbs::touchesBegan );
	mCallbackTouchesEnded = app->registerTouchesEnded( this, &Breadcrumbs::touchesEnded );
		
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

bool Breadcrumbs::touchesBegan( TouchEvent event ) {
	touchesBeganRectIndex = -1;
	for( int i = 0; i < clickRects.size(); i++ ){
		if (clickRects[i].contains(event.getTouches()[0].getPos())) {
			touchesBeganRectIndex = i;
		}
	}
	
	std::cout << "touchesBeganRectIndex = " << touchesBeganRectIndex << std::endl;
	return false;
}

bool Breadcrumbs::touchesEnded( TouchEvent event ) {
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

void Breadcrumbs::setHierarchy(vector<string> hierarchy) {
	mPreviousHierarchy = hierarchy;
	mHierarchyHasChanged = true;
}

const vector<string>& Breadcrumbs::getHierarchy() {
	return mPreviousHierarchy;
}

void Breadcrumbs::update() {
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
	gl::color( ColorA( 0.0f, 0.0f, 0.0f, 0.75f ) );
	gl::drawSolidRect( Rectf( 0.0f, 0.0f, app::getWindowWidth(), 24.0f ) );
	
	gl::enableAlphaBlending();
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.1f ) );
	gl::drawLine( Vec2f( 1.0f, 25.0f ), Vec2f( getWindowWidth(), 25.0f ) );

	gl::enableAlphaBlending(false);
	gl::color( Color::white() );
	float x			= 25.0f;
	float y			= 2.0f;
	float xMargin	= 5.0f;
	float yMargin	= 20.0f;
	mHeight = 0;
	clickRects.clear();
	
	for( int i=0; i<mPreviousHierarchy.size(); i++ ){
		if( i == mPreviousHierarchy.size() - 1 ){
			gl::color( Color( 1.0f, 1.0f, 1.0f ) );
		} else {
			gl::color( Color( 24/255.0f, 167/255.0f, 240/255.0f ) );	
		}
				
		if( i > 0 ){
			gl::draw( mSeparatorTexture, Vec2f( x, y ) );			
			x += mSeparatorTexture.getWidth() + xMargin*2;				
		}
		/*
		if( i == prevSelectedIndex ){
			gl::color( Color( 0.3f, 0.4f, 1.0f ) );
		}
		*/
		
		gl::draw( mTextures[i], Vec2f( x,y ) );			
		clickRects.push_back( Rectf( x-xMargin, y-yMargin, x+mTextures[i].getWidth()+xMargin, y+mTextures[i].getHeight()+yMargin ) );
		mHeight = max(mHeight, (float)(mTextures[i].getHeight()));
		x += mTextures[i].getWidth() + xMargin*2;
	}		

}
