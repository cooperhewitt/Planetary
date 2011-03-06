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
	}
	
	~Breadcrumbs() {
		
	}

	// !!! EVENT STUFF (slightly nicer interface for adding listeners)
	template<typename T>
	CallbackId registerBreadcrumbSelected( T *obj, bool (T::*callback)(BreadcrumbEvent) ){
		return mCallbacksBreadcrumbSelected.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}	
	
	void setup( AppCocoaTouch *app, const Font &font);
	bool touchesEnded( TouchEvent event );
	void setHierarchy(vector<string> hierarchy);
	const vector<string>& getHierarchy();
	void update();
	void draw( float y );

private:
	
	AppCocoaTouch *mApp;
	Font mFont;
	
	vector<string> mPreviousHierarchy;
	bool mHierarchyHasChanged;
	
	vector<gl::Texture> mTextures;
	gl::Texture mSeparatorTexture;
	vector<Rectf> clickRects;
	int prevSelectedIndex;
	
	CallbackId mCallbackTouchesEnded;
	
	//////////////////////
	
	// !!! EVENT STUFF (keep track of listeners)
	CallbackMgr<bool(BreadcrumbEvent)> mCallbacksBreadcrumbSelected;
	
};

void Breadcrumbs::setup( AppCocoaTouch *app, const Font &font) {
	
	if (mApp != NULL) {
		mApp->unregisterTouchesEnded(mCallbackTouchesEnded);
	}
	mCallbackTouchesEnded = app->registerTouchesEnded(this, &Breadcrumbs::touchesEnded);
	
	mApp = app;
	mFont = font;
	mHierarchyHasChanged = false;
	prevSelectedIndex = -1;
	
	TextLayout layout;
	layout.setFont( mFont );
	layout.setColor( Color::white() );			
	layout.addLine( " > " );
	bool PREMULT = false;
	mSeparatorTexture = gl::Texture( layout.render( true, PREMULT ) );
}

// TODO: trigger only if finger has not been dragged? if you
//		 spin the arcball and happen to release on top of a breadcrumb,
//		 the breadcrumb touchEnded is triggered.
bool Breadcrumbs::touchesEnded( TouchEvent event ) {
	for (int i = 0; i < clickRects.size(); i++) {
		if (clickRects[i].contains(event.getTouches()[0].getPos())) {
			prevSelectedIndex = i;
			// !!! EVENT STUFF (notify listeners)
			mCallbacksBreadcrumbSelected.call( BreadcrumbEvent( prevSelectedIndex, mPreviousHierarchy[i] ) );
			return true;
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

void Breadcrumbs::draw( float y ) {
	gl::enableAlphaBlending(false);		
	gl::color( Color::white() );
	float x			= 25.0f;
	float margin	= 5;
	clickRects.clear();
	
	for( int i=0; i<mPreviousHierarchy.size(); i++ ){
		gl::color( Color( 1.0, 1.0, 1.0 ) );			
		if( i > 0 ){
			gl::draw( mSeparatorTexture, Vec2f( x, y ) );			
			x += mSeparatorTexture.getWidth() + margin*2;				
		}
		if( i == prevSelectedIndex ){
			gl::color( Color( 0.3f, 0.4f, 1.0f ) );
		}
		gl::draw( mTextures[i], Vec2f( x,y ) );			
		clickRects.push_back( Rectf( x-margin, y-margin, x+mTextures[i].getWidth()+margin, y+mTextures[i].getHeight()+margin ) );
		x += mTextures[i].getWidth() + margin*2;
	}		
}
