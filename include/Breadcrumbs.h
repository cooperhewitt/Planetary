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
#include "Globals.h"

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
	void draw( const ci::gl::Texture &bgTex );
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
