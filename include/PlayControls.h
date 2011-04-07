/*
 *  PlayControls.h
 *  Kepler
 *
 *  Created by Tom Carden on 3/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <sstream>
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"

using namespace ci;
using namespace ci::app;
using namespace std;

template <class T>
inline std::string to_string( const T& t )
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

class PlayControls {
public:

	enum PlayButton { NO_BUTTON, CURRENT_TRACK, SHOW_WHEEL, PREVIOUS_TRACK, PLAY_PAUSE, NEXT_TRACK, SLIDER };//, HELP, DRAW_RINGS, DRAW_TEXT };
	enum ButtonTexId { TEX_BUTTONS, TEX_SLIDER_BUTTON, TEX_HELP, TEX_DRAW_RINGS, TEX_DRAW_TEXT };	
	
	void setup( AppCocoaTouch *app, bool initialPlayState );
    	
	void update();

	bool touchesBegan( TouchEvent event );
	bool touchesMoved( TouchEvent event );	
	bool touchesEnded( TouchEvent event );
    bool orientationChanged( OrientationEvent event );
	
	void setPlaying(bool playing) { mIsPlaying = playing; }
	bool isPlaying() { return mIsPlaying; }
	
	void draw( const vector<gl::Texture> &texs, const Font &font, float y, float currentTime, float totalTime, bool isDrawingRings, bool isDrawingText );
	void drawButton( const ci::Rectf &rect, float u1, float u2, float v1, float v2 );
	
	// !!! EVENT STUFF (slightly nicer interface for adding listeners)
	template<typename T>
	CallbackId registerButtonPressed( T *obj, bool (T::*callback)(PlayButton) )
	{
		return mCallbacksButtonPressed.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}
	
	template<typename T>
	CallbackId registerPlayheadMoved( T *obj, bool (T::*callback)(float) )
	{
		return mCallbacksPlayheadMoved.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}	
	
private:
					  
	AppCocoaTouch *mApp;
	
	// updated by draw() so that we can test in touchesEnded
	vector<Rectf> touchRects;
	vector<PlayButton> touchTypes;
	PlayButton lastTouchedType;

    Rectf lastDrawnBoundsRect;
    
	bool mIsPlaying;
	int mMinutes, mMinutesTotal, mMinutesLeft;
	int mSeconds, mSecondsTotal, mSecondsLeft;
	int mPrevSeconds;
	
	bool mIsDraggingPlayhead;
	bool mIsDrawingRings, mIsDrawingStars, mIsDrawingPlanets;
	gl::Texture mCurrentTimeTex;
	gl::Texture mRemainingTimeTex;
	
	CallbackId cbTouchesBegan, cbTouchesMoved, cbTouchesEnded, cbOrientationChanged;
			
	PlayButton findButtonUnderTouches(vector<TouchEvent::Touch> touches);
    Rectf transformRect( const Rectf &rect, const Matrix44f &matrix );
    
    DeviceOrientation mDeviceOrientation;
    Matrix44f         mOrientationMtx;
    Vec2f             mInterfaceSize;
    
	// !!! EVENT STUFF (keep track of listeners)
	CallbackMgr<bool(PlayButton)> mCallbacksButtonPressed;
	CallbackMgr<bool(float)> mCallbacksPlayheadMoved;
	
};