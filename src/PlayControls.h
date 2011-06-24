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
#include "TextLabel.h"
#include "TimeLabel.h"
#include "ScrollingLabel.h"
#include "CinderIPodPlayer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PlayControls {
public:

	enum ButtonId { NO_BUTTON, 
                    PREV_PLAYLIST, SELECT_PLAYLIST, NEXT_PLAYLIST, 
                    SHOW_WHEEL, GOTO_GALAXY, GOTO_CURRENT_TRACK, SETTINGS, 
                    PREV_TRACK, PLAY_PAUSE, NEXT_TRACK, 
                    SHUFFLE, REPEAT, 
                    HELP, DRAW_RINGS, DRAW_TEXT, USE_GYRO, DEBUG_FEATURE,
                    SLIDER, PARAMSLIDER1, PARAMSLIDER2 };

    PlayControls() {}
    ~PlayControls();
    
    void setup( AppCocoaTouch *app, 
                Orientation orientation, 
			    ipod::Player *player,
                const ci::Font &font, 
                const ci::Font &fontSmall, 
                const ci::gl::Texture &buttonsTex, 
                const ci::gl::Texture &bigButtonsTex, 
                const ci::gl::Texture &smallButtonsTex );
    
	void update();
    void draw(float y);

	bool touchesBegan( TouchEvent event );
	bool touchesMoved( TouchEvent event );	
	bool touchesEnded( TouchEvent event );
    void setInterfaceOrientation( const Orientation &orientation);

    // this one updates the drawable and interactive things too:
    void setShowSettings(bool visible);
    
    // State stuff, passed onto UI classes directly...
    // (not gettable, state lives elsewhere and UI changes are handled with callbacks)
    // (all these things should be called in App::update())
    // TODO: investigate doing this automagically with &references or *pointers?

	void setPlaying(bool playing) { mPlayPauseButton.setOn(playing); }
    void setAlphaWheelVisible(bool visible) { mAlphaWheelButton.setOn(visible); };

    void setHelpVisible(bool visible) {		mHelpButton.setOn(visible); };
	void setDebugVisible(bool visible) {	mDebugButton.setOn(visible); };
	void setGyroVisible(bool visible) {		mGyroButton.setOn(visible); };
    void setOrbitsVisible(bool visible) {	mOrbitsButton.setOn(visible); };
    void setLabelsVisible(bool visible) {	mLabelsButton.setOn(visible); };	
	void setRepeatVisible(bool visible) {	mRepeatButton.setOn(visible); };
	void setShuffleVisible(bool visible) {	mShuffleButton.setOn(visible); };
	
    void setElapsedSeconds(int elapsedTime) { mElapsedTimeLabel.setSeconds(elapsedTime); }
    void setRemainingSeconds(int remainingTime) { mRemainingTimeLabel.setSeconds(remainingTime); }
    void setCurrentTrack(string currentTrack) { mTrackInfoLabel.setText(currentTrack); }
    void setLastTrackChangeTime(float time) { mTrackInfoLabel.setLastTrackChangeTime(time); }
    void setPlayheadProgress(float value) { mPlayheadSlider.setValue(value); }
    
    void setPlaylist(const string &playlist);
	void setPlaylistSelected(const bool &selected);
    
    float getPlayheadValue() { return mPlayheadSlider.getValue(); }
	float getParamSlider1Value(){ return mParamSlider1.getValue(); }
	float getParamSlider2Value(){ return mParamSlider2.getValue(); }

    bool playheadIsDragging() { return mPlayheadSlider.isDragging(); }

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
	
private:
					  
	AppCocoaTouch *mApp;
	CallbackId cbTouchesBegan, cbTouchesMoved, cbTouchesEnded, cbOrientationChanged;

    // Interaction stuff...
    vector<UIElement*> drawableElements;
    vector<UIElement*> interactiveElements;    
    UIElement* mActiveElement;
	UIElement* findButtonUnderTouches(vector<TouchEvent::Touch> touches);

    // TODO: can we move this to slider class?
	void dragSliderToPos( Slider *slider, Vec2f pos );
    
    // layout happens here
    void updateUIRects();
    // toggled by showsettings:
    void updateElements();
    
    float mLastDrawY;
    bool mShowSettings;
        
    // Orientation bits:
    Orientation mInterfaceOrientation;
    Matrix44f   mOrientationMatrix;
    Vec2f       mInterfaceSize;
    Rectf transformRect( const Rectf &rect, const Matrix44f &matrix ); // TODO: Cinder Matrix/Rect method?
    
	// !!! EVENT STUFF (keep track of listeners)
	CallbackMgr<bool(ButtonId)> mCallbacksButtonPressed;
	CallbackMgr<bool(float)> mCallbacksPlayheadMoved;
	
    ///////////// Shared UI resources:
    gl::Texture mButtonsTex;

    ///////////// UI Classes:
    
    // Remember:
    //
    //   -- SimpleButton is just a trigger (down when touched, up when not)
    //   -- ToggleButton is for things that are sticky, on or off
    //   -- TwoStateButton is really just for Play/Pause, but who knows
    //
    // And furthermore thusly:
    //
    //   -- TextLabel is for simple static labels
    //   -- ScrollingLabel will flutter back and forth if you tell it the lastSomethingOrOtherTime
    //   -- TimeLabel will format a number of seconds as mm:ss
    //

    //
    // To make these useful, be sure to:
    //  * add the hit areas and ids in findButtonUnderTouches
    //  * add a call to Button::draw() or Label::draw() in PlayControls::draw()
    //  * add a call to Button::setup() or Label::setup() in PlayControls::setup()
    //  * set all the positions in updateUIRects()
    //
    // If your UI thing requires state (on/off, text, numThings, etc) then
    // add a setter in the public section of PlayControls and call it from
    // KeplerApp::update() with all the others. Leave globals out of this 
    // class if you can.
    //
    // Likewise, deal with actions that are triggered by buttons by looking 
    // for the button ID in KeplerApp::onPlayControlsButtonPressed()
    //
    
    // We'll make this better later.

    // buttons for flying...
    SimpleButton mGalaxyButton;
    SimpleButton mCurrentTrackButton;

    // current track info and playhead...
    ScrollingLabel mTrackInfoLabel;
    TimeLabel mElapsedTimeLabel;
    Slider mPlayheadSlider;
    TimeLabel mRemainingTimeLabel;    

    ToggleButton mAlphaWheelButton;

    // playlist controls
    SimpleButton mPreviousPlaylistButton;
    ScrollingLabel mPlaylistLabel;
    SimpleButton mNextPlaylistButton;
        
    // settings...
    ToggleButton mShowSettingsButton;
        ToggleButton mHelpButton;
        ToggleButton mOrbitsButton;
        ToggleButton mLabelsButton;
        ToggleButton mDebugButton;
        ToggleButton mGyroButton;
		ToggleButton mShuffleButton;
		ToggleButton mRepeatButton;
    
 // track skip and play/pause controls
    SimpleButton mPreviousTrackButton;
    TwoStateButton mPlayPauseButton;
    SimpleButton mNextTrackButton;
	
	TextLabel mParamSlider1Label;
    Slider mParamSlider1;
	TextLabel mParamSlider2Label;
    Slider mParamSlider2;        
};