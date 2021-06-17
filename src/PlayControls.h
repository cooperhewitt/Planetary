/*
 *  PlayControls.h
 *  Kepler
 *
 *  Created by Tom Carden on 3/7/11.
 *  Copyright 2013 Smithsonian Institution. All rights reserved.
 *
 */

#pragma once

#include <sstream>

#include "cinder/app/AppCocoaTouch.h"
#include "cinder/gl/Texture.h"
#include "cinder/Font.h"
#include "cinder/Text.h"
#include "cinder/Utilities.h"

#include "BloomNode.h"
#include "Buttons.h"
#include "Slider.h"
#include "TextLabel.h"
#include "TimeLabel.h"
#include "TextureRect.h"
#include "ScrollingLabel.h"
#include "CinderIPodPlayer.h"

class PlayControls;
typedef std::shared_ptr<PlayControls> PlayControlsRef;

class PlayControls : public BloomNode {
public:

	enum ButtonId { NO_BUTTON = 1000, 
                    SHOW_PLAYLIST_FILTER, SHOW_ALPHA_FILTER,
                    GOTO_GALAXY, GOTO_CURRENT_TRACK, SETTINGS, 
                    PREV_TRACK, PLAY_PAUSE, NEXT_TRACK, 
                    SLIDER,
                    LAST_BUTTON };

    PlayControls(): mOpacity(1.0f) {};
    ~PlayControls() {};
    
    void setup( Vec2f interfaceSize, 
                ci::ipod::Player *player,
                const ci::Font &font, 
                const ci::Font &fontSmall, 
                const ci::gl::Texture &buttonsTex );
    
    void update();
        
    // used in UiLayer layout...
    float getHeight();
    
    void setOpacity( float opacity ) { mOpacity = opacity; }
    
    void setPlaylistButtonVisible( bool visible );
    
    // State stuff, passed onto UI classes directly...
    // (not gettable, state lives elsewhere and UI changes are handled with callbacks)
    // (all these things should be called in App::update())
    // TODO: investigate doing this automagically with &references or *pointers?

    void setShowSettingsOn(bool on) { mShowSettingsButton->setOn(on); }
	void setPlayingOn(bool on) { mPlayPauseButton->setOn(on); }
    void setAlphaOn(bool on) { mAlphaButton->setOn(on); };
    void setPlaylistOn(bool on) { mPlaylistButton->setOn(on); };

    void setElapsedSeconds(int elapsedTime) { mElapsedTimeLabel->setSeconds(elapsedTime); }
    void setRemainingSeconds(int remainingTime) { mRemainingTimeLabel->setSeconds(remainingTime); }
    void setCurrentTrack(std::string currentTrack) { mTrackInfoLabel->setText(currentTrack); }
    void enablePlayerControls( bool enable = true );            
    void disablePlayerControls() { enablePlayerControls(false); }
        
    void setPlayheadValue(float value) { mPlayheadSlider->setValue(value); }
    float getPlayheadValue() { return mPlayheadSlider->getValue(); }
    bool isPlayheadDragging() { return mPlayheadSlider->isDragging(); }
    void cancelPlayheadDrag() { mPlayheadSlider->setIsDragging(false); }

    bool addedToScene(); // from BloomNode
    
    // override so we can batch geometry
    virtual void deepDraw();
    
private:
					  
    // instantiate and set fonts/areas/textures (called once)
    void createChildren( const Font &font, const Font &fontSmall, const gl::Texture &uiButtonsTex );
    
    // add everything (called once)
    void addChildren();
    
    // set positions (can be called repeatedly whenever interfaceSize changes)
    void setInterfaceSize( ci::Vec2f interfaceSize );
    
    ci::Vec2f mInterfaceSize; // for detecting orientation change, updating layout

    float mOpacity;

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

    // filters
    ToggleButton *mAlphaButton;
    ToggleButton *mPlaylistButton;
        
    // settings
    ToggleButton *mShowSettingsButton;
    
    // track skip and play/pause controls
    SimpleButton *mPreviousTrackButton;
    TwoStateButton *mPlayPauseButton;
    SimpleButton *mNextTrackButton;
};