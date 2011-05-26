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
#include "cinder/Utilities.h"

#include "Orientation.h"
#include "OrientationEvent.h"

#include "Buttons.h"
#include "Slider.h"
#include "TimeLabel.h"
#include "ScrollingLabel.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PlayControls {
public:

	enum ButtonId { NO_BUTTON, PREV_PLAYLIST, NEXT_PLAYLIST, SHOW_WHEEL
                    GOTO_GALAXY, GOTO_CURRENT_TRACK, SETTINGS, PREV_TRACK, 
                    PLAY_PAUSE, NEXT_TRACK, SLIDER, HELP, DRAW_RINGS, 
                    DRAW_TEXT, USE_GYRO, DEBUG_FEATURE };
	
	void setup( AppCocoaTouch *app, Orientation orientation, Font font, gl::Texture texture );
	void update();
    void draw(float y);

	bool touchesBegan( TouchEvent event );
	bool touchesMoved( TouchEvent event );	
	bool touchesEnded( TouchEvent event );
    void setInterfaceOrientation( const Orientation &orientation);
	
    // State stuff, passed onto UI classes directly...
    // (not gettable, state lives elsewhere and UI changes are handled with callbacks)
    // (all these things should be called in App::update())
    // TODO: investigate doing this automagically with &references or *pointers?
	void setPlaying(bool playing) { mPlayPauseButton.setOn(playing); }
    void setAlphaWheelVisible(bool visible) { mAlphaWheelButton.setOn(visible); };
    void setOrbitsVisible(bool visible) { mOrbitsButton.setOn(visible); };
    void setLabelsVisible(bool visible) { mLabelsButton.setOn(visible); };
    void setHelpVisible(bool visible) { mHelpButton.setOn(visible); };
    void setElapsedSeconds(int elapsedTime) { mElapsedTimeLabel.setSeconds(elapsedTime); }
    void setRemainingSeconds(int remainingTime) { mRemainingTimeLabel.setSeconds(remainingTime); }
    void setCurrentTrack(string currentTrack) { mTrackInfoLabel.setText(currentTrack); }
    void setLastTrackChangeTime(float time) { mTrackInfoLabel.setLastTrackChangeTime(time); }
    void setPlayheadProgress(float value) { mPlayheadSlider.setValue(value); }

	// !!! EVENT STUFF (slightly nicer interface for adding listeners)
	template<typename T>
	CallbackId registerButtonPressed( T *obj, bool (T::*callback)(ButtonId) )
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

	CallbackId cbTouchesBegan, cbTouchesMoved, cbTouchesEnded, cbOrientationChanged;
			
	ButtonId findButtonUnderTouches(vector<TouchEvent::Touch> touches);
    Rectf transformRect( const Rectf &rect, const Matrix44f &matrix );
    void updateUIRects();
    void dragPlayheadToPos(Vec2f pos);
    
    ButtonId mLastTouchedType;
    float mLastDrawY;
    
    Orientation mInterfaceOrientation;
    Matrix44f   mOrientationMatrix;
    Vec2f       mInterfaceSize;
    
	// !!! EVENT STUFF (keep track of listeners)
	CallbackMgr<bool(ButtonId)> mCallbacksButtonPressed;
	CallbackMgr<bool(float)> mCallbacksPlayheadMoved;
	
    ///////////// Shared UI resources:
    Font mFont;
    gl::Texture mTexture;

    // FIXME: replace with a label
	gl::Texture mPlaylistNameTex;
    
    ///////////// UI Classes:
    
    SimpleButton mCurrentTrackButton;

    ScrollingLabel mTrackInfoLabel;
    TimeLabel mElapsedTimeLabel;
    Slider mPlayheadSlider;
    TimeLabel mRemainingTimeLabel;    

    ToggleButton mAlphaWheelButton;
    
    ToggleButton mHelpButton;
    ToggleButton mOrbitsButton;
    ToggleButton mLabelsButton;

    SimpleButton mPreviousTrackButton;
    TwoStateButton mPlayPauseButton;
    SimpleButton mNextTrackButton;
        
};