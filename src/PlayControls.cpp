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

PlayControls::~PlayControls()
{
    mApp->unregisterTouchesBegan( cbTouchesBegan );
    if ( cbTouchesEnded ) mApp->unregisterTouchesEnded( cbTouchesEnded );
    if ( cbTouchesMoved ) mApp->unregisterTouchesMoved( cbTouchesMoved );
}

void PlayControls::setup( AppCocoaTouch *app, Orientation orientation, const Font &font, const Font &fontSmall, const gl::Texture &uiButtonsTex, const gl::Texture &uiBigButtonsTex, const gl::Texture &uiSmallButtonsTex )
{
    mApp			= app;
    mButtonsTex		= uiButtonsTex; // only stored for dimming sides of scrolling current track label
    
    cbTouchesBegan	= mApp->registerTouchesBegan( this, &PlayControls::touchesBegan );
    cbTouchesEnded	= 0;
    cbTouchesMoved	= 0;		
    
    mActiveElement = NULL;    
    
    mShowSettings = false;
    
    ////////

	float uw = uiBigButtonsTex.getWidth() / 8.0f; // button tex width
	float v1 = 0.0f;                              // button tex off start y
    float v2 = uiBigButtonsTex.getHeight() * 0.5f;// button tex on start y, off end y
    float v3 = uiBigButtonsTex.getHeight();       // button tex on end y

    mGalaxyButton.setup( GOTO_GALAXY,   // ID
                         uiBigButtonsTex,
						 Area(uw*0.0f,v2,uw*1.0f,v3),  // on texture
						 Area(uw*0.0f,v1,uw*1.0f,v2)); // off texture
	
    mCurrentTrackButton.setup(GOTO_CURRENT_TRACK,   // ID
                              uiBigButtonsTex,
                              Area(uw*1.0f,v2,uw*2.0f,v3),  // on texture
                              Area(uw*1.0f,v1,uw*2.0f,v2)); // off texture

	mShowSettingsButton.setup(SETTINGS, 
                              false, 
                              uiBigButtonsTex,
                              Area(uw*2.0f,v2,uw*3.0f,v3),  // on texture
                              Area(uw*2.0f,v1,uw*3.0f,v2)); // off texture   

    mPreviousTrackButton.setup(PREV_TRACK, 
                               uiBigButtonsTex,
                               Area(uw*3.0f,v2,uw*4.0f,v3),  // on texture
                               Area(uw*3.0f,v1,uw*4.0f,v2)); // off texture

    mPlayPauseButton.setup(PLAY_PAUSE, 
                           false,               // on
                           uiBigButtonsTex,
                           Area(uw*4.0f,v2,uw*5.0f,v3), // offUp   
                           Area(uw*4.0f,v1,uw*5.0f,v2), // offDown
                           Area(uw*5.0f,v2,uw*6.0f,v3), // onUp
                           Area(uw*5.0f,v1,uw*6.0f,v2));// onDown

    mNextTrackButton.setup(NEXT_TRACK, 
                           uiBigButtonsTex,
                           Area(uw*6.0f,v2,uw*7.0f,v3),  // on texture
                           Area(uw*6.0f,v1,uw*7.0f,v2)); // off texture
	
    mAlphaWheelButton.setup(SHOW_WHEEL,      // ID
                            false,           // initial toggle state
                            uiBigButtonsTex,
                            Area(uw*7.0f,v2,uw*8.0f,v3),  // on texture
                            Area(uw*7.0f,v1,uw*8.0f,v2)); // off texture    

	
    mPreviousPlaylistButton.setup(PREV_PLAYLIST, 
                                  uiBigButtonsTex,
                                  Area(uw*3.0f,v2,uw*4.0f,v3),  // on texture
                                  Area(uw*3.0f,v1,uw*4.0f,v2)); // off texture
    
    mNextPlaylistButton.setup(NEXT_PLAYLIST,
                              uiBigButtonsTex,
                              Area(uw*6.0f,v2,uw*7.0f,v3),  // on texture
                              Area(uw*6.0f,v1,uw*7.0f,v2)); // off texture

    
     

    // !!! SMALL BUTTONS !!!
    uw = uiSmallButtonsTex.getWidth() / 10.0f;
	v1 = 0.0f; 
    v2 = uiSmallButtonsTex.getHeight() * 0.5f; 
    v3 = uiSmallButtonsTex.getHeight();
    
    {
        mHelpButton.setup(HELP, 
                          false, 
                          uiSmallButtonsTex,
                          Area(uw*0.0f,v2,uw*1.0f,v3),  // on texture
                          Area(uw*0.0f,v1,uw*1.0f,v2)); // off texture
        
        mGyroButton.setup(USE_GYRO, 
                            false, 
                            uiSmallButtonsTex,
                            Area(uw*1.0f,v2,uw*2.0f,v3),  // on texture
                            Area(uw*1.0f,v1,uw*2.0f,v2)); // off texture
        
        mOrbitsButton.setup(DRAW_RINGS, 
                            false, 
                            uiSmallButtonsTex,
                            Area(uw*2.0f,v2,uw*3.0f,v3),  // on texture
                            Area(uw*2.0f,v1,uw*3.0f,v2)); // off texture

        mLabelsButton.setup(DRAW_TEXT, 
                          false, 
                          uiSmallButtonsTex,
                          Area(uw*3.0f,v2,uw*4.0f,v3),  // on texture
                          Area(uw*3.0f,v1,uw*4.0f,v2)); // off texture

        mDebugButton.setup(DEBUG_FEATURE, 
                          false, 
                          uiSmallButtonsTex,
                          Area(uw*4.0f,v2,uw*5.0f,v3),  // on texture
                          Area(uw*4.0f,v1,uw*5.0f,v2)); // off texture
		
		mFeatureButton.setup(TEST_FEATURE, 
						   false, 
                           uiSmallButtonsTex,
						   Area(uw*5.0f,v2,uw*6.0f,v3),  // on texture
						   Area(uw*5.0f,v1,uw*6.0f,v2)); // off texture
		
		mShuffleButton.setup(SHUFFLE, 
							 false, 
                             uiSmallButtonsTex,
							 Area(uw*6.0f,v2,uw*7.0f,v3),  // on texture
							 Area(uw*6.0f,v1,uw*7.0f,v2)); // off texture
		
		mRepeatButton.setup(REPEAT, 
							false, 
                            uiSmallButtonsTex,
							Area(uw*7.0f,v2,uw*8.0f,v3),  // on texture
							Area(uw*7.0f,v1,uw*8.0f,v2)); // off texture
    }
    
    const float vh = uiButtonsTex.getHeight();
    
    mPlayheadSlider.setup(SLIDER,          // ID
                          uiButtonsTex,
                          Area(uw * 0.0f, vh * 0.9f, uw * 1.0f, vh * 1.0f),  // bg texture
                          Area(uw * 2.0f, vh * 0.9f, uw * 3.0f, vh * 1.0f),  // fg texture
                          Area(uw * 4.0f, vh * 0.7f, uw * 5.0f, vh * 0.9f),  // thumb on texture
                          Area(uw * 4.0f, vh * 0.5f, uw * 5.0f, vh * 0.7f)); // thumb off texture

    /////// no textures please, we're British...
    
    mPlaylistLabel.setup(NO_BUTTON, font, Color::white());    
    
    mTrackInfoLabel.setup(NO_BUTTON, font, BRIGHT_BLUE);
    
    mElapsedTimeLabel.setup(NO_BUTTON, fontSmall, BRIGHT_BLUE);
    
    mRemainingTimeLabel.setup(NO_BUTTON, fontSmall, BRIGHT_BLUE);
        
    ///////
    
    setInterfaceOrientation(orientation);
    
    // make drawable and interactive vectors for ::draw() and ::touchXXX
    updateElements();
}

void PlayControls::update()
{
    // clean up listeners here, because if we remove in touchesEnded then things get crazy
    if (mApp->getActiveTouches().size() == 0 && cbTouchesEnded != 0) {
        mApp->unregisterTouchesEnded( cbTouchesEnded );
        mApp->unregisterTouchesMoved( cbTouchesMoved );
        cbTouchesEnded = 0;
        cbTouchesMoved = 0;
    }		

    // FIXME, have buttons handle this themselves
	// This is just for simple buttons. For toggles, check KeplerApp::update()
    int activeId = mActiveElement ? mActiveElement->getId() : NO_BUTTON;
    
	mGalaxyButton.setDown( activeId == mGalaxyButton.getId() );
	mCurrentTrackButton.setDown( activeId == mCurrentTrackButton.getId() );
    mPreviousTrackButton.setDown( activeId == mPreviousTrackButton.getId() );
    mPlayPauseButton.setDown( activeId == mPlayPauseButton.getId() );
    mNextTrackButton.setDown( activeId == mNextTrackButton.getId() );         
    
}

void PlayControls::setInterfaceOrientation( const Orientation &orientation )
{
    mInterfaceOrientation = orientation;
    
    mOrientationMatrix = getOrientationMatrix44(mInterfaceOrientation, getWindowSize());
    
    mInterfaceSize = getWindowSize();
    
    if ( isLandscapeOrientation(orientation) ) {
        mInterfaceSize = mInterfaceSize.yx();
    }
    
    updateUIRects();
}

void PlayControls::updateUIRects()
{
	const float topBorder	 = 10.0f;
	const float sideBorder	 = 10.0f;
    
    const float bSize		 = 50.0f;
    const float bSizeSmall	 = 40.0f;
	const float buttonGap	 = 1.0f;

	const float timeTexWidth = 55.0f;
    const float sliderHeight = 20.0f;
    const float sliderInset  = bSize * 2.0f + sideBorder + timeTexWidth;
    const float sliderWidth  = isLandscapeOrientation(mInterfaceOrientation) ? 328.0f : 201.0f;
    
    // FLY TO CURRENT TRACK-MOON BUTTON
	float y1 = topBorder;
	float y2 = y1 + bSize;
	float x1 = sideBorder;
	float x2 = x1 + bSize;        
    mGalaxyButton.setRect( x1, y1, x2, y2 );
    
	x1 += bSize + buttonGap;
	x2 = x1 + bSize;
    mCurrentTrackButton.setRect( x1, y1, x2, y2 );

    // NEXT / PLAY-PAUSE / PREVIOUS TRACK BUTTON
    x1 = mInterfaceSize.x - sideBorder - bSize;
	x2 = x1 + bSize;
    mNextTrackButton.setRect( x1, y1, x2, y2 );

    x1 -= bSize + buttonGap;
	x2 = x1 + bSize;
    mPlayPauseButton.setRect( x1, y1, x2, y2 );
    
	x1 -= bSize + buttonGap;
	x2 = x1 + bSize;    
    mPreviousTrackButton.setRect( x1, y1, x2, y2 );

	x1 -= ( bSize + buttonGap ) * 2.0f;
	x2 = x1 + bSize;    
    mShowSettingsButton.setRect( x1, y1, x2, y2 );

	// ALPHA WHEEL BUTTON
	x1 -= bSize + buttonGap;
	x2 = x1 + bSize;
    mAlphaWheelButton.setRect( x1, y1, x2, y2 );
	
// LABEL TOGGLE BUTTON
	y1 += 68.0f;
    y2 = y1 + bSizeSmall;
	x1 += 5.0f + bSizeSmall * 2.0f;
	x2 = x1 + bSizeSmall;
    mLabelsButton.setRect( x1, y1, x2, y2 );
	
// ORBIT RING TOGGLE BUTTON
	x1 -= bSizeSmall;
	x2 = x1 + bSizeSmall;
    mOrbitsButton.setRect( x1, y1, x2, y2 );
	
// GYRO TOGGLE BUTTON
	x1 -= bSizeSmall;
	x2 = x1 + bSizeSmall;
    mGyroButton.setRect( x1, y1, x2, y2 );
	
// DEBUG TOGGLE BUTTON
	x1 -= bSizeSmall;
	x2 = x1 + bSizeSmall;
    mDebugButton.setRect( x1, y1, x2, y2 );
	
// FEATURE TOGGLE BUTTON
	x1 -= bSizeSmall;
	x2 = x1 + bSizeSmall;
    mFeatureButton.setRect( x1, y1, x2, y2 );
	
// HELP TOGGLE BUTTON
    x1 -= bSizeSmall;
	x2 = x1 + bSizeSmall;
    mHelpButton.setRect( x1, y1, x2, y2 );
	
// SHUFFLE TOGGLE BUTTON
	x1 = mInterfaceSize.x - sideBorder - bSizeSmall - 3.0f;
	x2 = x1 + bSizeSmall;    
    mShuffleButton.setRect( x1, y1, x2, y2 );

// REPEAT TOGGLE BUTTON
	x1 -= bSize;
	x2 = x1 + bSizeSmall;
    mRepeatButton.setRect( x1, y1, x2, y2 );
	
	
	
	
// PREVIOUS PLAYLIST BUTTON
	x1 = 10.0f;
	x2 = x1 + bSize;
	y1 = 73.0f;
	y2 = y1 + bSize;
	mPreviousPlaylistButton.setRect( x1, y1, x2, y2 );
	
// NEXT PLAYLIST BUTTON
	x1 = x2 + 200.0f;
	x2 = x1 + bSize;
	mNextPlaylistButton.setRect( x1, y1, x2, y2 );
	
// PLAYLIST LABEL
	x1 -= 200.0f;
	x2 = x1 + 200.0f;
	y1 += 15.0f;
	// FIX ME! vvv
	y2 = y1 + 12.0f;
	mPlaylistLabel.setRect( x1, y1, x2, y2 );
    
    const float bgx1 = sliderInset;
    const float bgx2 = bgx1 + sliderWidth;
    const float bgy1 = 32.0f;
    const float bgy2 = bgy1 + sliderHeight;
    mPlayheadSlider.setRect( bgx1, bgy1, bgx2, bgy2 );
    
    const float ctx1 = bgx1 - 43.0f;
    const float ctx2 = bgx2 + 50.0f;
    const float cty1 = bgy1 - 14.0f;
    const float cty2 = cty1; // NB:- will be overridden in ScrollingLabel::draw()
    mTrackInfoLabel.setRect( ctx1, cty1, ctx2, cty2 );

    // FIXME: bottom right coords are made up... maybe just setPos (and getWidth) for these?
    // at least use font height for calculating y2
    mElapsedTimeLabel.setRect( bgx1-40.0f, bgy1+2.0f, bgx1, bgy1+12.0f );
    mRemainingTimeLabel.setRect( bgx2+18.0f, bgy1+2.0f, bgx2+58.0f, bgy1+12.0f );

}

void PlayControls::setShowSettings(bool visible)
{
    if (mShowSettings != visible) {
        mShowSettings = visible;
        updateElements();
    }    
    mShowSettingsButton.setOn(visible); 
}

void PlayControls::updateElements()
{
    drawableElements.clear();
    drawableElements.push_back(&mGalaxyButton);
	drawableElements.push_back(&mCurrentTrackButton);
    drawableElements.push_back(&mShowSettingsButton);
	drawableElements.push_back(&mAlphaWheelButton);
    drawableElements.push_back(&mPreviousTrackButton);
    drawableElements.push_back(&mPlayPauseButton);
    drawableElements.push_back(&mNextTrackButton);
    if (mShowSettings) {    
        drawableElements.push_back(&mHelpButton);
        drawableElements.push_back(&mOrbitsButton);
        drawableElements.push_back(&mLabelsButton);
        drawableElements.push_back(&mDebugButton);
        drawableElements.push_back(&mFeatureButton);
        drawableElements.push_back(&mGyroButton);
        drawableElements.push_back(&mShuffleButton);
        drawableElements.push_back(&mRepeatButton);
    }
    drawableElements.push_back(&mElapsedTimeLabel);
    drawableElements.push_back(&mTrackInfoLabel);
	drawableElements.push_back(&mPlayheadSlider);
    drawableElements.push_back(&mRemainingTimeLabel);
    if (mShowSettings) {
        drawableElements.push_back(&mPlaylistLabel);	
        drawableElements.push_back(&mPreviousPlaylistButton);
        drawableElements.push_back(&mNextPlaylistButton);
    }    

    interactiveElements.clear();
    interactiveElements.push_back(&mCurrentTrackButton);
    interactiveElements.push_back(&mGalaxyButton);
    interactiveElements.push_back(&mPlayheadSlider);
    interactiveElements.push_back(&mAlphaWheelButton);
    interactiveElements.push_back(&mShowSettingsButton);
    if (mShowSettings) {
        interactiveElements.push_back(&mHelpButton);
        interactiveElements.push_back(&mOrbitsButton);
        interactiveElements.push_back(&mLabelsButton);
        interactiveElements.push_back(&mDebugButton);
        interactiveElements.push_back(&mFeatureButton);
        interactiveElements.push_back(&mGyroButton); // FIXME: only if we're using Gyro
		interactiveElements.push_back(&mShuffleButton);
		interactiveElements.push_back(&mRepeatButton);
    }
    interactiveElements.push_back(&mPreviousTrackButton);
    interactiveElements.push_back(&mPlayPauseButton);
    interactiveElements.push_back(&mNextTrackButton);
    if (mShowSettings) {
        interactiveElements.push_back(&mPreviousPlaylistButton);
        interactiveElements.push_back(&mNextPlaylistButton);
    }    
}

void PlayControls::draw(float y)
{
    float t = app::getElapsedSeconds();
    
    mLastDrawY = y;

    // FIXME: make an mActive bool so we can skip interaction if the panel is hiding
    //mActive = (mInterfaceSize.y - y ) > 60.0f;
    
    gl::pushModelView();
    gl::multModelView( mOrientationMatrix );
    gl::translate( Vec2f(0, y) );
    
	float dragAlphaPer = pow( ( mInterfaceSize.y - y ) / 65.0f, 2.0f );    	
    gl::color( ColorA( 1.0f, 1.0f, 1.0f, dragAlphaPer ) );
    
	gl::enableAlphaBlending();    

    for (int i = 0; i < drawableElements.size(); i++) {
        drawableElements[i]->draw();
    }
	
	gl::color( Color::white() );
	
// TEXT LABEL GRADIENTS
	if( mTrackInfoLabel.isScrollingText() ){
		const float w	 = 15.0f;
		Rectf infoRect   = mTrackInfoLabel.getRect();
		Area aLeft		 = Area( 200.0f, 140.0f, 214.0f, 150.0f ); // references the uiButtons image
		Rectf coverLeft  = Rectf( infoRect.x1, infoRect.y1, infoRect.x1 + w, infoRect.y2 );
		Rectf coverRight = Rectf( infoRect.x2 + 1.0f, infoRect.y1, infoRect.x2 - ( w - 1.0f ), infoRect.y2 );    
		gl::draw( mButtonsTex, aLeft, coverLeft );
		gl::draw( mButtonsTex, aLeft, coverRight );
	}
	    
    gl::popModelView();
    
    gl::disableAlphaBlending();
    
    float t2 = app::getElapsedSeconds() - t;
    console() << "PlayControls::draw() elapsed time = " << t2 << std::endl;
}

void PlayControls::dragPlayheadToPos(Vec2f pos) 
{
    // adjust for orientation
    pos = (mOrientationMatrix.inverted() * Vec3f(pos,0)).xy();
    
    // FIXME: assumes slider is horizontal :)
    Rectf rect = mPlayheadSlider.getRect();
    float playheadPer = (pos.x - rect.x1) / (rect.x2 - rect.x1);
    playheadPer = constrain( playheadPer, 0.0f, 1.0f );
    
    mPlayheadSlider.setValue( playheadPer );
    mCallbacksPlayheadMoved.call( playheadPer );                
}

bool PlayControls::touchesBegan( TouchEvent event )
{
    Rectf transformedBounds = transformRect( Rectf(0, mLastDrawY, mInterfaceSize.x, mInterfaceSize.y), mOrientationMatrix );    
	Rectf otherTransBounds	= transformRect( Rectf( 0, mLastDrawY - 50.0f, mInterfaceSize.x - 170.0f, mInterfaceSize.y - 20.0f ), mOrientationMatrix );

    vector<TouchEvent::Touch> touches = event.getTouches(); // FIXME: test all touches in this touchesBegan batch
    
    if( touches.size() > 0 && ( transformedBounds.contains(touches[0].getPos()) || otherTransBounds.contains(touches[0].getPos()) ) ){
        
        if( cbTouchesEnded == 0 ){
            cbTouchesEnded = mApp->registerTouchesEnded( this, &PlayControls::touchesEnded );
            cbTouchesMoved = mApp->registerTouchesMoved( this, &PlayControls::touchesMoved );			
        }

        mActiveElement = findButtonUnderTouches(touches);
		
        if (mActiveElement && mActiveElement->getId() == SLIDER) {
            mPlayheadSlider.setIsDragging(true);
            dragPlayheadToPos(touches.begin()->getPos());
        }
             
        return true;
    }
    else {
        mActiveElement = NULL;
    }
    
    return false;
}

bool PlayControls::touchesMoved( TouchEvent event )
{
    vector<TouchEvent::Touch> touches = event.getTouches();

    mActiveElement = findButtonUnderTouches(touches);

    if( mPlayheadSlider.isDragging() ){
        if( touches.size() == 1 ){
            dragPlayheadToPos(touches.begin()->getPos());
        }
    }
    
    return false;
}	

bool PlayControls::touchesEnded( TouchEvent event )
{
    vector<TouchEvent::Touch> touches = event.getTouches();
    if( mActiveElement && mActiveElement == findButtonUnderTouches(touches) ){
        mCallbacksButtonPressed.call(ButtonId(mActiveElement->getId()));
    }

    mActiveElement = NULL;
    
    mPlayheadSlider.setIsDragging(false);

    return false;
}

        
UIElement* PlayControls::findButtonUnderTouches(vector<TouchEvent::Touch> touches)
{
    Rectf transformedBounds = transformRect( Rectf(0, mLastDrawY, mInterfaceSize.x, mInterfaceSize.y), mOrientationMatrix );    
	Rectf otherTransBounds	= transformRect( Rectf( 0, mLastDrawY - 50.0f, mInterfaceSize.x - 170.0f, mInterfaceSize.y - 20.0f ), mOrientationMatrix );

    // check for touches and return first one found
    // TODO: accept more than one touch, by ID?
    for (int j = 0; j < touches.size(); j++) {
        TouchEvent::Touch touch = touches[j];
        Vec2f pos = touch.getPos();
        if ( transformedBounds.contains( pos ) || otherTransBounds.contains( pos ) ) {
            for (int i = 0; i < interactiveElements.size(); i++) {
                UIElement *element = interactiveElements[i];
                // offset and transform:
                Rectf rect = transformRect( element->getRect().getOffset( Vec2f(0, mLastDrawY) ), mOrientationMatrix );            
                if (rect.contains(pos)) {
                    return element;
                }
            }		
        }
    }
    
    return NULL;
}

// TODO: move this to an operator in Cinder's Matrix class?
Rectf PlayControls::transformRect( const Rectf &rect, const Matrix44f &matrix )
{
    Vec2f topLeft = (matrix * Vec3f(rect.x1,rect.y1,0)).xy();
    Vec2f bottomRight = (matrix * Vec3f(rect.x2,rect.y2,0)).xy();
    Rectf newRect(topLeft, bottomRight);
    newRect.canonicalize();    
    return newRect;
}
