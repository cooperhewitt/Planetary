/*
 *  PlayControls.h
 *  Kepler
 *
 *  Created by Tom Carden on 3/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "PlayControls.h"
#include "Globals.h"
#include "BloomGl.h"
#include "UIController.h" // for mRoot

using namespace ci;
using namespace ci::app;
using namespace std;

void PlayControls::setup( Vec2f interfaceSize, ipod::Player *player, const Font &font, const Font &fontSmall, const gl::Texture &uiButtonsTex, const gl::Texture &uiBigButtonsTex, const gl::Texture &uiSmallButtonsTex )
{
    mButtonsTex = uiButtonsTex; // only stored for dimming sides of scrolling current track label    
    mShowSettings = false;
    
    // create, add, and position everything...
    createChildren( font, fontSmall, uiButtonsTex, uiBigButtonsTex, uiSmallButtonsTex );
    setInterfaceSize( interfaceSize );
        
    // set initial state...
    setPlaying( player->getPlayState() == ipod::Player::StatePlaying );    
    setShowSettings( G_SHOW_SETTINGS );
    setOrbitsVisible( G_DRAW_RINGS );
    setLabelsVisible( G_DRAW_TEXT );
    setHelpVisible( G_HELP );
    setDebugVisible( G_DEBUG );	    
    setShuffleVisible( player->getShuffleMode() != ipod::Player::ShuffleModeOff );
    setRepeatVisible( player->getRepeatMode() != ipod::Player::RepeatModeNone );    
    if( G_IS_IPAD2 ) {
        setGyroVisible( G_USE_GYRO );
    }
}

void PlayControls::createChildren( const Font &font, const Font &fontSmall, const gl::Texture &uiButtonsTex, const gl::Texture &uiBigButtonsTex, const gl::Texture &uiSmallButtonsTex )
{
	float uw = uiBigButtonsTex.getWidth() / 8.0f; // button tex width
	float v1 = 0.0f;                              // button tex off start y
    float v2 = uiBigButtonsTex.getHeight() * 0.5f;// button tex on start y, off end y
    float v3 = uiBigButtonsTex.getHeight();       // button tex on end y

    mGalaxyButton = new SimpleButton( GOTO_GALAXY,   // ID
                                      uiBigButtonsTex,
                                      Area(uw*0.0f,v2,uw*1.0f,v3),  // on texture
                                      Area(uw*0.0f,v1,uw*1.0f,v2) );// off texture
    
    mCurrentTrackButton = new SimpleButton( GOTO_CURRENT_TRACK,   // ID
                                            uiBigButtonsTex,
                                            Area(uw*1.0f,v2,uw*2.0f,v3),  // on texture
                                            Area(uw*1.0f,v1,uw*2.0f,v2) );// off texture

	mShowSettingsButton = new ToggleButton( SETTINGS, 
                                            false, 
                                            uiBigButtonsTex,
                                            Area(uw*2.0f,v2,uw*3.0f,v3),  // on texture
                                            Area(uw*2.0f,v1,uw*3.0f,v2) );// off texture   

    mPreviousTrackButton = new SimpleButton( PREV_TRACK, 
                                             uiBigButtonsTex,
                                             Area(uw*3.0f,v2,uw*4.0f,v3),  // on texture
                                             Area(uw*3.0f,v1,uw*4.0f,v2) );// off texture
	
    mPlayPauseButton = new TwoStateButton( PLAY_PAUSE, 
                                           false, // initial state is updated in setup()
                                           uiBigButtonsTex,
                                           Area(uw*4.0f,v2,uw*5.0f,v3),  // offDown   
                                           Area(uw*4.0f,v1,uw*5.0f,v2),  // offUp
                                           Area(uw*5.0f,v2,uw*6.0f,v3),  // onDown
                                           Area(uw*5.0f,v1,uw*6.0f,v2) );// onUp

    mNextTrackButton = new SimpleButton( NEXT_TRACK, 
                                         uiBigButtonsTex,
                                         Area(uw*6.0f,v2,uw*7.0f,v3),  // on texture
                                         Area(uw*6.0f,v1,uw*7.0f,v2) );// off texture
	
    mAlphaWheelButton = new ToggleButton( SHOW_WHEEL,      // ID
                                          false,           // initial toggle state
                                          uiBigButtonsTex,
                                          Area(uw*7.0f,v2,uw*8.0f,v3),  // on texture
                                          Area(uw*7.0f,v1,uw*8.0f,v2) );// off texture    

    
     

    // !!! SMALL BUTTONS !!!
    uw = uiSmallButtonsTex.getWidth() / 10.0f;
	v1 = 0.0f; 
    v2 = uiSmallButtonsTex.getHeight() * 0.5f; 
    v3 = uiSmallButtonsTex.getHeight();
    
    {
        mHelpButton = new ToggleButton( HELP, 
                                        false, 
                                        uiSmallButtonsTex,
                                        Area(uw*0.0f,v2,uw*1.0f,v3),  // on texture
                                        Area(uw*0.0f,v1,uw*1.0f,v2)); // off texture
        
		if( G_IS_IPAD2 ){
			mGyroButton = new ToggleButton( USE_GYRO, 
                                            false, 
                                            uiSmallButtonsTex,
                                            Area(uw*1.0f,v2,uw*2.0f,v3),  // on texture
                                            Area(uw*1.0f,v1,uw*2.0f,v2)); // off texture
		}
        
        mOrbitsButton = new ToggleButton( DRAW_RINGS, 
                                          false, 
                                          uiSmallButtonsTex,
                                          Area(uw*2.0f,v2,uw*3.0f,v3),  // on texture
                                          Area(uw*2.0f,v1,uw*3.0f,v2)); // off texture

        mLabelsButton = new ToggleButton( DRAW_TEXT, 
                                          false, 
                                          uiSmallButtonsTex,
                                          Area(uw*3.0f,v2,uw*4.0f,v3),  // on texture
                                          Area(uw*3.0f,v1,uw*4.0f,v2)); // off texture

        mDebugButton = new ToggleButton( DEBUG_FEATURE, 
                                         false, 
                                         uiSmallButtonsTex,
                                         Area(uw*4.0f,v2,uw*5.0f,v3),  // on texture
                                         Area(uw*4.0f,v1,uw*5.0f,v2)); // off texture
		
		mPreviousPlaylistButton = new SimpleButton( PREV_PLAYLIST, 
                                                    uiSmallButtonsTex,
                                                    Area(uw*6.0f,v2,uw*7.0f,v3),  // on texture
                                                    Area(uw*6.0f,v1,uw*7.0f,v2)); // off texture
		
		mNextPlaylistButton = new SimpleButton( NEXT_PLAYLIST,
                                                uiSmallButtonsTex,
                                                Area(uw*7.0f,v2,uw*8.0f,v3),  // on texture
                                                Area(uw*7.0f,v1,uw*8.0f,v2)); // off texture
		
		mShuffleButton = new ToggleButton( SHUFFLE, 
                                           false, 
                                           uiSmallButtonsTex,
                                           Area(uw*8.0f,v2,uw*9.0f,v3),  // on texture
                                           Area(uw*8.0f,v1,uw*9.0f,v2)); // off texture
		
		mRepeatButton = new ToggleButton( REPEAT, 
                                          false, 
                                          uiSmallButtonsTex,
                                          Area(uw*9.0f,v2,uw*10.0f,v3),  // on texture
                                          Area(uw*9.0f,v1,uw*10.0f,v2)); // off texture
    }
    
    const float vh = uiButtonsTex.getHeight();
    
    mPlayheadSlider = new Slider( SLIDER,          // ID
                                  uiButtonsTex,
                                  Area(uw * 0.0f, vh * 0.5f, uw * 1.0f, vh * 0.6f),  // bg texture
                                  Area(uw * 0.0f, vh * 0.6f, uw * 1.0f, vh * 0.7f),  // fg texture
                                  Area(uw * 0.0f, vh * 0.2f, uw * 1.0f, vh * 0.4f),  // thumb on texture
                                  Area(uw * 0.0f, vh * 0.0f, uw * 1.0f, vh * 0.2f)); // thumb off texture

    /////// no textures please, we're British...
    
    mPlaylistLabel = new ScrollingLabel(SELECT_PLAYLIST, font, BRIGHT_BLUE);    
    
    mTrackInfoLabel = new ScrollingLabel(NO_BUTTON, font, BRIGHT_BLUE);
    
    mElapsedTimeLabel = new TimeLabel(NO_BUTTON, fontSmall, BRIGHT_BLUE);
    
    mRemainingTimeLabel = new TimeLabel(NO_BUTTON, fontSmall, BRIGHT_BLUE);
        
    ///////
	// TODO: add initial value
	mParamSlider1 = new Slider( PARAMSLIDER1,          // ID
                                uiButtonsTex,
                                Area(uw * 0.0f, vh * 0.5f, uw * 1.0f, vh * 0.6f),  // bg texture
                                Area(uw * 0.0f, vh * 0.6f, uw * 1.0f, vh * 0.7f),  // fg texture
                                Area(uw * 0.0f, vh * 0.2f, uw * 1.0f, vh * 0.4f),  // thumb on texture
                                Area(uw * 0.0f, vh * 0.0f, uw * 1.0f, vh * 0.2f)); // thumb off texture
	mParamSlider1->setValue( 0.25f );
	mParamSlider1Label = new TextLabel( NO_BUTTON, font, BRIGHT_BLUE );
	mParamSlider1Label->setText( "Scale" );
	
	mParamSlider2 = new Slider( PARAMSLIDER2,          // ID
                                uiButtonsTex,
                                Area(uw * 0.0f, vh * 0.5f, uw * 1.0f, vh * 0.6f),  // bg texture
                                Area(uw * 0.0f, vh * 0.6f, uw * 1.0f, vh * 0.7f),  // fg texture
                                Area(uw * 0.0f, vh * 0.2f, uw * 1.0f, vh * 0.4f),  // thumb on texture
                                Area(uw * 0.0f, vh * 0.0f, uw * 1.0f, vh * 0.2f)); // thumb off texture
	mParamSlider2->setValue( 0.025f );
	mParamSlider2Label = new TextLabel( NO_BUTTON, font, BRIGHT_BLUE );
	mParamSlider2Label->setText( "Speed" );
    
    // scrolling bits
    Area aLeft = Area( 200.0f, 140.0f, 214.0f, 150.0f ); // references the uiButtons image    
    mCoverLeftTextureRect = new TextureRect( mButtonsTex, aLeft );
    // FIXME: I think this texture should be flipped in x? can we mess with aLeft to make an aRight?
    mCoverRightTextureRect = new TextureRect( mButtonsTex, aLeft );    
}    

bool PlayControls::addedToScene()
{
    // now mRoot is valid we can add children
    addChildren(); // FIXME: make it so you can add children even if mRoot is invalid
    // add listeners to relay callbacks...
    std::cout << "registering callbacks in PlayControls" << std::endl;
    mCbTouchMoved = mRoot->registerUINodeTouchMoved( this, &PlayControls::onUINodeTouchMoved );
    mCbTouchEnded = mRoot->registerUINodeTouchEnded( this, &PlayControls::onUINodeTouchEnded );    
    return false;
}

bool PlayControls::removedFromScene()
{
    // remove listeners...
    // FIXME: this should also be done in destructor (?)
    mRoot->unregisterUINodeTouchMoved( mCbTouchMoved );
    mRoot->unregisterUINodeTouchEnded( mCbTouchEnded );    
    return false;
}

void PlayControls::addChildren()
{
    // bit of hack, these are first for batch reasons
    // (we want the little fadey bits to be drawn on top)
    addChild( UINodeRef(mElapsedTimeLabel) );
    addChild( UINodeRef(mTrackInfoLabel) );
    addChild( UINodeRef(mRemainingTimeLabel) );    
    
    addChild( UINodeRef(mGalaxyButton) );
	addChild( UINodeRef(mCurrentTrackButton) );
    addChild( UINodeRef(mShowSettingsButton) );
	addChild( UINodeRef(mAlphaWheelButton) );
    addChild( UINodeRef(mPreviousTrackButton) );
    addChild( UINodeRef(mPlayPauseButton) );
    addChild( UINodeRef(mNextTrackButton) );
	
    // FIXME: hide these if (!mShowSettings) {   
    addChild( UINodeRef(mShuffleButton) );
    addChild( UINodeRef(mRepeatButton) );
    addChild( UINodeRef(mHelpButton) );
    addChild( UINodeRef(mOrbitsButton) );
    addChild( UINodeRef(mLabelsButton) );
    addChild( UINodeRef(mDebugButton) );
    if( G_IS_IPAD2 ) addChild( UINodeRef(mGyroButton) );
    addChild( UINodeRef(mPlaylistLabel) );	
    addChild( UINodeRef(mPreviousPlaylistButton) );
    addChild( UINodeRef(mNextPlaylistButton) );
    //    }
    
	addChild( UINodeRef(mPlayheadSlider) );
	addChild( UINodeRef(mParamSlider1) );
	addChild( UINodeRef(mParamSlider2) );
	addChild( UINodeRef(mParamSlider1Label) );
    addChild( UINodeRef(mParamSlider2Label) );    

    // shaded bits on top of scrolling mTrackInfoLabel
    addChild( UINodeRef(mCoverLeftTextureRect) );
    addChild( UINodeRef(mCoverRightTextureRect) );
}


bool PlayControls::onUINodeTouchMoved( UINodeRef nodeRef )
{
    if ( nodeRef->getId() == mPlayheadSlider->getId() ) {
        mCallbacksPlayheadMoved.call( mPlayheadSlider->getValue() );
    }
    return false;
}

bool PlayControls::onUINodeTouchEnded( UINodeRef nodeRef )
{
    mCallbacksButtonPressed.call(ButtonId(nodeRef->getId()));
    return false;
}

void PlayControls::setPlaylistSelected(const bool &selected) 
{ 
    mPlaylistLabel->setColor(selected ? Color::white() : BRIGHT_BLUE);
}

void PlayControls::setPlaylist(const string &playlist)
{ 
    mPlaylistLabel->setText(playlist); 
}    

void PlayControls::setInterfaceSize( Vec2f interfaceSize )
{
    mInterfaceSize = interfaceSize;
    
	const float topBorder	 = 5.0f;
	const float sideBorder	 = 10.0f;
    
    const float bSize		 = 50.0f;
    const float bSizeSmall	 = 40.0f;
	const float buttonGap	 = 1.0f;

	const float timeTexWidth = 55.0f;
    const float sliderHeight = 20.0f;
    const float sliderInset  = bSize * 2.0f + sideBorder + timeTexWidth;
    const bool  landscape    = interfaceSize.x > interfaceSize.y;
    const float sliderWidth  = landscape ? 328.0f : 201.0f;
    
    // FLY TO CURRENT TRACK-MOON BUTTON
	float y1 = topBorder;
	float y2 = y1 + bSize;
	float x1 = sideBorder;
	float x2 = x1 + bSize;        
    mGalaxyButton->setRect( x1, y1, x2, y2 );
    
	x1 += bSize + buttonGap;
	x2 = x1 + bSize;
    mCurrentTrackButton->setRect( x1, y1, x2, y2 );

    // NEXT / PLAY-PAUSE / PREVIOUS TRACK BUTTON
    x1 = interfaceSize.x - sideBorder - bSize;
	x2 = x1 + bSize;
    mNextTrackButton->setRect( x1, y1, x2, y2 );

    x1 -= bSize + buttonGap;
	x2 = x1 + bSize;
    mPlayPauseButton->setRect( x1, y1, x2, y2 );
    
	x1 -= bSize + buttonGap;
	x2 = x1 + bSize;    
    mPreviousTrackButton->setRect( x1, y1, x2, y2 );

	x1 -= ( bSize + buttonGap ) * 2.0f;
	x2 = x1 + bSize;    
    mShowSettingsButton->setRect( x1, y1, x2, y2 );

	// ALPHA WHEEL BUTTON
	x1 -= bSize + buttonGap;
	x2 = x1 + bSize;
    mAlphaWheelButton->setRect( x1, y1, x2, y2 );
	
	
	
	y1 += 60.0f;
    y2 = y1 + bSizeSmall;
	
// SHUFFLE TOGGLE BUTTON
	x1 = interfaceSize.x - sideBorder - bSizeSmall - 3.0f;
	x2 = x1 + bSizeSmall;    
    mShuffleButton->setRect( x1, y1, x2, y2 );

// REPEAT TOGGLE BUTTON
	x1 -= bSizeSmall;
	x2 = x1 + bSizeSmall;
    mRepeatButton->setRect( x1, y1, x2, y2 );
	
// TEXT LABELS TOGGLE BUTTON
	x1 -= bSizeSmall;
	x2 = x1 + bSizeSmall;
    mLabelsButton->setRect( x1, y1, x2, y2 );
	
// ORBIT RING TOGGLE BUTTON
	x1 -= bSizeSmall;
	x2 = x1 + bSizeSmall;
    mOrbitsButton->setRect( x1, y1, x2, y2 );
	
// GYRO TOGGLE BUTTON
	if( G_IS_IPAD2 ){
		x1 -= bSizeSmall;
		x2 = x1 + bSizeSmall;
		mGyroButton->setRect( x1, y1, x2, y2 );
	}
	
// DEBUG TOGGLE BUTTON
	x1 -= bSizeSmall;
	x2 = x1 + bSizeSmall;
    mDebugButton->setRect( x1, y1, x2, y2 );
	
// HELP TOGGLE BUTTON
    x1 -= bSizeSmall;
	x2 = x1 + bSizeSmall;
    mHelpButton->setRect( x1, y1, x2, y2 );
	
	
	
// PREVIOUS PLAYLIST BUTTON
	x1 = 10.0f;
	x2 = x1 + bSize;
	mPreviousPlaylistButton->setRect( x1, y1, x2, y2 );
	
// NEXT PLAYLIST BUTTON
	x1 = x2;
	x2 = x1 + bSize;
	mNextPlaylistButton->setRect( x1, y1, x2, y2 );
	
// PLAYLIST LABEL
	x1 = x2 + 10.0f;
	x2 = x1 + 300.0f;
	y1 += 12.0f;
	// this 18.0f offset is a hack to make hit-testing work
    // rect is used for layout and for hit-testing, but ScrollingLabel.draw compensates for this
	y2 = y1 + 18.0f;
	mPlaylistLabel->setRect( x1, y1, x2, y2 );
    
    const float bgx1 = sliderInset;
    const float bgx2 = bgx1 + sliderWidth;
    const float bgy1 = 32.0f;
    const float bgy2 = bgy1 + sliderHeight;
    mPlayheadSlider->setRect( bgx1, bgy1, bgx2, bgy2 );
    mParamSlider1->setRect( 60.0f, bgy1 + 80.0f, interfaceSize.x * 0.5f - 20.0f, bgy2 + 80.0f );
    mParamSlider2->setRect( interfaceSize.x * 0.5f + 60.0f, bgy1 + 80.0f, interfaceSize.x - 20.0f, bgy2 + 80.0f );
	mParamSlider1Label->setRect( 20.0f, bgy1 + 80.0f, 70.0f, bgy2 + 80.0f );
	mParamSlider2Label->setRect( interfaceSize.x * 0.5f + 20.0f, bgy1 + 80.0f, interfaceSize.x * 0.5f + 70.0f, bgy2 + 80.0f );
	
    const float ctx1 = bgx1 - 43.0f;
    const float ctx2 = bgx2 + 48.0f;
    const float cty1 = bgy1 - 16.0f;
    const float cty2 = cty1; // NB:- will be overridden in ScrollingLabel::draw()
    mTrackInfoLabel->setRect( ctx1, cty1, ctx2, cty2 );

    // FIXME: bottom right coords are made up... maybe just setPos (and getWidth) for these?
    // at least use font height for calculating y2
    mElapsedTimeLabel->setRect( bgx1-40.0f, bgy1+2.0f, bgx1, bgy1+12.0f );
    mRemainingTimeLabel->setRect( bgx2+18.0f, bgy1+2.0f, bgx2+58.0f, bgy1+12.0f );

}

void PlayControls::setShowSettings(bool visible)
{
    if (mShowSettings != visible) {
        mShowSettings = visible;
        // FIXME: make a container just for settings things and don't draw it when !mShowSettings
    }    
    mShowSettingsButton->setOn(visible); 
}

void PlayControls::update( float y )
{
    if (mLastDrawY != y) {
        Matrix44f transform;
        transform.translate(Vec3f(0,y,0));
        mLastDrawY = y;
        setTransform(transform);
    }
    
    Vec2f interfaceSize = mRoot->getInterfaceSize();
    if ( mInterfaceSize != interfaceSize ) {
        setInterfaceSize( interfaceSize );
    }    

    // FIXME: make an mActive bool so we can skip interaction and drawing if the panel is hiding
    //mActive = (mInterfaceSize.y - y ) > 60.0f;
    
    const float w	 = 15.0f;
	Rectf infoRect   = mTrackInfoLabel->getRect();

	if( mTrackInfoLabel->isScrollingText() ) {
        mCoverLeftTextureRect->setRect( infoRect.x1, infoRect.y1, infoRect.x1 + w, infoRect.y2 );
    } else {
        mCoverLeftTextureRect->setRect( infoRect.x1, infoRect.y1, infoRect.x1, infoRect.y1 ); // zero size (FIXME: visible true/false? or remove from scene?
    }
    mCoverRightTextureRect->setRect( infoRect.x2 + 1.0f, infoRect.y1, infoRect.x2 - ( w - 1.0f ), infoRect.y2 );
    
    // FIXME: need label gradients for playlist label as well - perhaps nest inside scrolling label class?

}

void PlayControls::draw()
{
    // apply this alpha to all children
    // FIXME: is there a more reliable way to do this, does UINode need more inheritable properties?
	const float dragAlphaPer = pow( ( mInterfaceSize.y - mLastDrawY ) / 65.0f, 2.0f );    	
    gl::color( ColorA( 1.0f, 1.0f, 1.0f, dragAlphaPer ) );
    UINode::draw();
}

