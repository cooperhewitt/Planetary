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

void PlayControls::setup( AppCocoaTouch *app, Orientation orientation, const Font &font, const Font &fontSmall, const gl::Texture &uiButtonsTex, const gl::Texture &uiBigButtonsTex, const gl::Texture &uiSmallButtonsTex )
{
    mApp			= app;
    mFont			= font;
    mButtonsTex		= uiButtonsTex;
	mBigButtonsTex	= uiBigButtonsTex;
	mSmallButtonsTex= uiSmallButtonsTex;
    
    // TODO: unregister these in destructor!
    cbTouchesBegan	= mApp->registerTouchesBegan( this, &PlayControls::touchesBegan );
    cbTouchesEnded	= 0;
    cbTouchesMoved	= 0;		
    
    mActiveElement = NULL;    
    
    mShowSettings = false;
    
    ////////

	float uw = 1.0f/8.0f;	// button tex width
	float v1 = 0.0f;		// button tex off start y
    float v2 = 0.5f;		// button tex on start y, off end y
    float v3 = 1.0f;		// button tex on end y

    mGalaxyButton.setup( GOTO_GALAXY,   // ID
						 Rectf(uw*0.0f,v2,uw*1.0f,v3),  // on texture
						 Rectf(uw*0.0f,v1,uw*1.0f,v2)); // off texture
	
    mCurrentTrackButton.setup(GOTO_CURRENT_TRACK,   // ID
                              Rectf(uw*1.0f,v2,uw*2.0f,v3),  // on texture
                              Rectf(uw*1.0f,v1,uw*2.0f,v2)); // off texture

	mShowSettingsButton.setup(SETTINGS, 
                              false, 
                              Rectf(uw*2.0f,v2,uw*3.0f,v3),  // on texture
                              Rectf(uw*2.0f,v1,uw*3.0f,v2)); // off texture   

    mPreviousTrackButton.setup(PREV_TRACK, 
                               Rectf(uw*3.0f,v2,uw*4.0f,v3),  // on texture
                               Rectf(uw*3.0f,v1,uw*4.0f,v2)); // off texture

    mPlayPauseButton.setup(PLAY_PAUSE, 
                           false,               // on
                           Rectf(uw*4.0f,v2,uw*5.0f,v3), // offUp   
                           Rectf(uw*4.0f,v1,uw*5.0f,v2), // offDown
                           Rectf(uw*5.0f,v2,uw*6.0f,v3), // onUp
                           Rectf(uw*5.0f,v1,uw*6.0f,v2));// onDown

    mNextTrackButton.setup(NEXT_TRACK, 
                           Rectf(uw*6.0f,v2,uw*7.0f,v3),  // on texture
                           Rectf(uw*6.0f,v1,uw*7.0f,v2)); // off texture
	
    mAlphaWheelButton.setup(SHOW_WHEEL,      // ID
                            false,           // initial toggle state
                            Rectf(uw*7.0f,v2,uw*8.0f,v3),  // on texture
                            Rectf(uw*7.0f,v1,uw*8.0f,v2)); // off texture    

	
    mPreviousPlaylistButton.setup(PREV_PLAYLIST, 
                                  Rectf(uw*3.0f,v2,uw*4.0f,v3),  // on texture
                                  Rectf(uw*3.0f,v1,uw*4.0f,v2)); // off texture
    
    mNextPlaylistButton.setup(NEXT_PLAYLIST,
                              Rectf(uw*6.0f,v2,uw*7.0f,v3),  // on texture
                              Rectf(uw*6.0f,v1,uw*7.0f,v2)); // off texture

    
     

    // !!! SMALL BUTTONS !!!
    uw = 1.0/10.0f;
	v1 = 0.0f; 
    v2 = 0.5f; 
    v3 = 1.0f;
    
    {
        mHelpButton.setup(HELP, 
                          false, 
                          Rectf(uw*0.0f,v2,uw*1.0f,v3),  // on texture
                          Rectf(uw*0.0f,v1,uw*1.0f,v2)); // off texture
        
        mGyroButton.setup(USE_GYRO, 
                            false, 
                            Rectf(uw*1.0f,v2,uw*2.0f,v3),  // on texture
                            Rectf(uw*1.0f,v1,uw*2.0f,v2)); // off texture
        
        mOrbitsButton.setup(DRAW_RINGS, 
                            false, 
                            Rectf(uw*2.0f,v2,uw*3.0f,v3),  // on texture
                            Rectf(uw*2.0f,v1,uw*3.0f,v2)); // off texture

        mLabelsButton.setup(DRAW_TEXT, 
                          false, 
                          Rectf(uw*3.0f,v2,uw*4.0f,v3),  // on texture
                          Rectf(uw*3.0f,v1,uw*4.0f,v2)); // off texture

        mDebugButton.setup(DEBUG_FEATURE, 
                          false, 
                          Rectf(uw*4.0f,v2,uw*5.0f,v3),  // on texture
                          Rectf(uw*4.0f,v1,uw*5.0f,v2)); // off texture
		
		mFeatureButton.setup(TEST_FEATURE, 
						   false, 
						   Rectf(uw*5.0f,v2,uw*6.0f,v3),  // on texture
						   Rectf(uw*5.0f,v1,uw*6.0f,v2)); // off texture
		
		mShuffleButton.setup(SHUFFLE, 
							 false, 
							 Rectf(uw*6.0f,v2,uw*7.0f,v3),  // on texture
							 Rectf(uw*6.0f,v1,uw*7.0f,v2)); // off texture
		
		mRepeatButton.setup(REPEAT, 
							false, 
							Rectf(uw*7.0f,v2,uw*8.0f,v3),  // on texture
							Rectf(uw*7.0f,v1,uw*8.0f,v2)); // off texture
    }
    
    mPlayheadSlider.setup(SLIDER,          // ID
                          Rectf(uw * 0.0f, 0.9f, uw * 1.0f, 1.0f),  // bg texture
                          Rectf(uw * 2.0f, 0.9f, uw * 3.0f, 1.0f),  // fg texture
                          Rectf(uw * 4.0f, 0.7f, uw * 5.0f, 0.9f),  // thumb on texture
                          Rectf(uw * 4.0f, 0.5f, uw * 5.0f, 0.7f)); // thumb off texture

    /////// no textures please, we're British...
    
    mPlaylistLabel.setup(font, Color::white());    
    
    mTrackInfoLabel.setup(font, BRIGHT_BLUE );
    
    mElapsedTimeLabel.setup(fontSmall, BRIGHT_BLUE);
    
    mRemainingTimeLabel.setup(fontSmall, BRIGHT_BLUE);
        
    ///////
    
    setInterfaceOrientation(orientation);
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
	mGalaxyButton.setDown( mActiveElement->getId() == mGalaxyButton.getId() );
	mCurrentTrackButton.setDown( mActiveElement->getId() == mCurrentTrackButton.getId() );
    mPreviousTrackButton.setDown( mActiveElement->getId() == mPreviousTrackButton.getId() );
    mPlayPauseButton.setDown( mActiveElement->getId() == mPlayPauseButton.getId() );
    mNextTrackButton.setDown( mActiveElement->getId() == mNextTrackButton.getId() );         
    
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

void PlayControls::draw(float y)
{
    mLastDrawY = y;

    // FIXME: make an mActive bool so we can skip interaction if the panel is hiding
    //mActive = (mInterfaceSize.y - y ) > 60.0f;
    
    gl::pushModelView();
    gl::multModelView( mOrientationMatrix );
    gl::translate( Vec2f(0, y) );
    
	float dragAlphaPer = pow( ( mInterfaceSize.y - y ) / 65.0f, 2.0f );    	
    gl::color( ColorA( 1.0f, 1.0f, 1.0f, dragAlphaPer ) );
    
    
	gl::enableAlphaBlending();    

	
// BIG BUTTONS
	mBigButtonsTex.enableAndBind();
    mGalaxyButton.draw();
	mCurrentTrackButton.draw();
    mShowSettingsButton.draw();
	mAlphaWheelButton.draw();
	
// PREV PLAY NEXT	
    mPreviousTrackButton.draw();
    mPlayPauseButton.draw();
    mNextTrackButton.draw();

	
// SETTINGS SMALL BUTTONS
	mSmallButtonsTex.enableAndBind();
	mHelpButton.draw();
	mOrbitsButton.draw();
	mLabelsButton.draw();
	mDebugButton.draw();
	mFeatureButton.draw();
	mGyroButton.draw();
	mShuffleButton.draw();
	mRepeatButton.draw();
	
// SLIDER
	mButtonsTex.enableAndBind();
	mPlayheadSlider.draw();

    mButtonsTex.unbind();

	
// TEXT
    mElapsedTimeLabel.draw();
    mRemainingTimeLabel.draw();
    mTrackInfoLabel.draw();
	mPlaylistLabel.draw();
	
	
	gl::color( Color::white() );
	
	
// TEXT LABEL GRADIENTS
	if( mTrackInfoLabel.isScrollingText() ){
		float w			= 15.0f;
		Rectf infoRect   = mTrackInfoLabel.getRect();
		Area aLeft		 = Area( 200.0f, 140.0f, 214.0f, 150.0f ); // references the uiButtons image
		Rectf coverLeft  = Rectf( infoRect.x1, infoRect.y1, infoRect.x1 + w, infoRect.y2 );
		Rectf coverRight = Rectf( infoRect.x2 + 1.0f, infoRect.y1, infoRect.x2 - ( w - 1.0f ), infoRect.y2 );    
		gl::draw( mButtonsTex, aLeft, coverLeft );
		gl::draw( mButtonsTex, aLeft, coverRight );
	}
	

// PLAYLIST BUTTONS
	mBigButtonsTex.enableAndBind();
    mPreviousPlaylistButton.draw();
    mNextPlaylistButton.draw();
	mBigButtonsTex.unbind();
    
    
    gl::popModelView();
    
    gl::disableAlphaBlending();
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

    // TODO: should we cache this and only update it if mShowSettings changes?
    vector<UIElement*> elements;
    elements.push_back(&mCurrentTrackButton);
    elements.push_back(&mGalaxyButton);
    elements.push_back(&mPlayheadSlider);
    elements.push_back(&mAlphaWheelButton);
    elements.push_back(&mShowSettingsButton);
    if (mShowSettings) {
        elements.push_back(&mHelpButton);
        elements.push_back(&mOrbitsButton);
        elements.push_back(&mLabelsButton);
        elements.push_back(&mDebugButton);
        elements.push_back(&mFeatureButton);
        elements.push_back(&mGyroButton); // FIXME: only if we're using Gyro
		elements.push_back(&mShuffleButton);
		elements.push_back(&mRepeatButton);
    }
    elements.push_back(&mPreviousTrackButton);
    elements.push_back(&mPlayPauseButton);
    elements.push_back(&mNextTrackButton);
    elements.push_back(&mPreviousPlaylistButton);
    elements.push_back(&mNextPlaylistButton);

    // check for touches and return first one found
    // TODO: accept more than one touch, by ID?
    for (int j = 0; j < touches.size(); j++) {
        TouchEvent::Touch touch = touches[j];
        Vec2f pos = touch.getPos();
        if ( transformedBounds.contains( pos ) || otherTransBounds.contains( pos ) ) {
            for (int i = 0; i < elements.size(); i++) {
                UIElement *element = elements[i];
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
