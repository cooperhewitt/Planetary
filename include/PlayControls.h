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
#include "cinder/Font.h"
#include "cinder/Text.h"
#include "Orientation.h"
#include "OrientationEvent.h"
#include "AlphaWheel.h"

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

	enum PlayButton { NO_BUTTON, PREVIOUS_PLAYLIST, NEXT_PLAYLIST, GALAXY, CURRENT_TRACK, SETTINGS, PREVIOUS_TRACK, 
					  PLAY_PAUSE, NEXT_TRACK, SLIDER, HELP, DRAW_RINGS, DRAW_TEXT, USE_GYRO, DEBUG_FEATURE };
	enum ButtonTexId { TEX_BUTTONS, TEX_SLIDER_BUTTON, TEX_HELP, TEX_DRAW_RINGS, TEX_DRAW_TEXT, TEX_USE_GYRO, TEX_DEBUG_FEATURE };	
	
	void setup( AppCocoaTouch *app, bool initialPlayState, const ci::app::Orientation &orientation );
    	
	void update();

	bool touchesBegan( TouchEvent event );
	bool touchesMoved( TouchEvent event );	
	bool touchesEnded( TouchEvent event );
    void setInterfaceOrientation( const Orientation &orientation);
	
	void setPlaying(bool playing) { mIsPlaying = playing; }
	bool isPlaying() { return mIsPlaying; }
	
	void draw( const Orientation &orientation, const ci::gl::Texture &uiButtonsTex, const ci::gl::Texture &currentTrackTex, AlphaWheel *alphaWheel, const Font &font, float y, float currentTime, float totalTime, float secsSinceTrackChange );
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
	
	
	void createPlaylistTexture( string playlistName, const Font &font );
	
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
	
	gl::Texture mCurrentTimeTex;
	gl::Texture mRemainingTimeTex;
	
	gl::Texture mPlaylistNameTex;
	
	CallbackId cbTouchesBegan, cbTouchesMoved, cbTouchesEnded, cbOrientationChanged;
			
	PlayButton findButtonUnderTouches(vector<TouchEvent::Touch> touches);
    Rectf transformRect( const Rectf &rect, const Matrix44f &matrix );
    
    Orientation mInterfaceOrientation;
    Matrix44f   mOrientationMatrix;
    Vec2f       mInterfaceSize;
    
	// !!! EVENT STUFF (keep track of listeners)
	CallbackMgr<bool(PlayButton)> mCallbacksButtonPressed;
	CallbackMgr<bool(float)> mCallbacksPlayheadMoved;
	
};