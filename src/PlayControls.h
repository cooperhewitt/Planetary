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

#include "OrientationHelper.h"

#include "UINode.h"
#include "Buttons.h"
#include "Slider.h"
#include "TextLabel.h"
#include "TimeLabel.h"
#include "TextureRect.h"
#include "ScrollingLabel.h"
#include "CinderIPodPlayer.h"

class PlayControls : public UINode {
public:

	enum ButtonId { NO_BUTTON, 
                    PREV_PLAYLIST, SELECT_PLAYLIST, NEXT_PLAYLIST, 
                    SHOW_WHEEL, GOTO_GALAXY, GOTO_CURRENT_TRACK, SETTINGS, 
                    PREV_TRACK, PLAY_PAUSE, NEXT_TRACK, 
                    SHUFFLE, REPEAT, 
                    HELP, DRAW_RINGS, DRAW_TEXT, USE_GYRO, DEBUG_FEATURE,
                    SLIDER, PARAMSLIDER1, PARAMSLIDER2 };

    PlayControls() {};
    ~PlayControls() {};
    
    void setup( Vec2f interfaceSize, 
                ci::ipod::Player *player,
                const ci::Font &font, 
                const ci::Font &fontSmall, 
                const ci::gl::Texture &buttonsTex, 
                const ci::gl::Texture &bigButtonsTex, 
                const ci::gl::Texture &smallButtonsTex );
    
    void update(float y);
    
    void draw();

    // this one updates the drawable and interactive things too:
    void setShowSettings(bool visible);
    
    // State stuff, passed onto UI classes directly...
    // (not gettable, state lives elsewhere and UI changes are handled with callbacks)
    // (all these things should be called in App::update())
    // TODO: investigate doing this automagically with &references or *pointers?

	void setPlaying(bool playing) { mPlayPauseButton->setOn(playing); }
    void setAlphaWheelVisible(bool visible) { mAlphaWheelButton->setOn(visible); };

    void setHelpVisible(bool visible) {		mHelpButton->setOn(visible); };
	void setDebugVisible(bool visible) {	mDebugButton->setOn(visible); };
	void setGyroVisible(bool visible) {		mGyroButton->setOn(visible); };
    void setOrbitsVisible(bool visible) {	mOrbitsButton->setOn(visible); };
    void setLabelsVisible(bool visible) {	mLabelsButton->setOn(visible); };	
	void setRepeatVisible(bool visible) {	mRepeatButton->setOn(visible); };
	void setShuffleVisible(bool visible) {	mShuffleButton->setOn(visible); };
	
    void setElapsedSeconds(int elapsedTime) { mElapsedTimeLabel->setSeconds(elapsedTime); }
    void setRemainingSeconds(int remainingTime) { mRemainingTimeLabel->setSeconds(remainingTime); }
    void setCurrentTrack(std::string currentTrack) { mTrackInfoLabel->setText(currentTrack); }
    void setPlayheadProgress(float value) { mPlayheadSlider->setValue(value); }
    
    float getPlayheadValue() { return mPlayheadSlider->getValue(); }
	float getParamSlider1Value(){ return mParamSlider1->getValue(); }
	float getParamSlider2Value(){ return mParamSlider2->getValue(); }

    bool playheadIsDragging() { return mPlayheadSlider->isDragging(); }

	// !!! EVENT STUFF (slightly nicer interface for adding listeners)
	template<typename T>
    ci::CallbackId registerButtonPressed( T *obj, bool (T::*callback)(ButtonId) )
	{
		return mCallbacksButtonPressed.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}
	
	template<typename T>
	ci::CallbackId registerPlayheadMoved( T *obj, bool (T::*callback)(float) )
	{
		return mCallbacksPlayheadMoved.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}	
	
    bool addedToScene(); // from UINode
    bool removedFromScene(); // from UINode
    
private:
					  
    // instantiate and set fonts/areas/textures (called once)
    void createChildren( const Font &font, const Font &fontSmall, const gl::Texture &uiButtonsTex, const gl::Texture &uiBigButtonsTex, const gl::Texture &uiSmallButtonsTex );
    
    // add everything (called once)
    void addChildren();
    
    // set positions (can be called repeatedly whenever interfaceSize changes)
    void setInterfaceSize( ci::Vec2f interfaceSize );
    
    float mLastDrawY;
    ci::Vec2f mInterfaceSize; // for detecting orientation change, updating layout
    bool mShowSettings;
        
	// !!! EVENT STUFF (keep track of listeners)
	ci::CallbackMgr<bool(ButtonId)> mCallbacksButtonPressed;
	ci::CallbackMgr<bool(float)> mCallbacksPlayheadMoved;
	    
    // relay events from mUIController
    bool onUINodeTouchMoved( UINodeRef nodeRef );    
    bool onUINodeTouchEnded( UINodeRef nodeRef );

    // for removing events when cleaning up
    ci::CallbackId mCbTouchMoved, mCbTouchEnded;
    
    ///////////// Shared UI resources:
    ci::gl::Texture mButtonsTex;

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

    // buttons for flying...
    SimpleButton *mGalaxyButton;
    SimpleButton *mCurrentTrackButton;

    // current track info and playhead...
    ScrollingLabel *mTrackInfoLabel;
        TextureRect *mCoverLeftTextureRect;
        TextureRect *mCoverRightTextureRect;
    TimeLabel *mElapsedTimeLabel;
    Slider *mPlayheadSlider;
    TimeLabel *mRemainingTimeLabel;    

    ToggleButton *mAlphaWheelButton;
        
    // settings...
    ToggleButton *mShowSettingsButton;
        ToggleButton *mHelpButton;
        ToggleButton *mOrbitsButton;
        ToggleButton *mLabelsButton;
        ToggleButton *mDebugButton;
        ToggleButton *mGyroButton;
		ToggleButton *mShuffleButton;
		ToggleButton *mRepeatButton;
    
 // track skip and play/pause controls
    SimpleButton *mPreviousTrackButton;
    TwoStateButton *mPlayPauseButton;
    SimpleButton *mNextTrackButton;
	
	TextLabel *mParamSlider1Label;
    Slider *mParamSlider1;
	TextLabel *mParamSlider2Label;
    Slider *mParamSlider2;        
};