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

void PlayControls::setup( AppCocoaTouch *app, Orientation orientation, Font font, gl::Texture texture )
{
    mApp			= app;
    mFont           = font;
    mTexture        = texture;
    
    // TODO: unregister these in destructor!
    cbTouchesBegan	= mApp->registerTouchesBegan( this, &PlayControls::touchesBegan );
    cbTouchesEnded	= 0;
    cbTouchesMoved	= 0;		
    
    mLastTouchedType = NO_BUTTON;    
    
    ////////

	float uw = 1.0f/8.0f; // button tex width
	float v1 = 0.0f;      // button tex off start y
    float v2 = 0.25f;     // button tex on start y, off end y
    float v3 = 0.5f;      // button tex on end y
    
    mCurrentTrackButton.setup(CURRENT_TRACK,   // ID
                              Rectf(uw*0.0f,v2,uw*1.0f,v3),  // on texture
                              Rectf(uw*0.0f,v1,uw*1.0f,v2)); // off texture

    mAlphaWheelButton.setup(SHOW_WHEEL,      // ID
                            false,           // initial toggle state
                            Rectf(uw*1.0f,v2,uw*2.0f,v3),  // on texture
                            Rectf(uw*1.0f,v1,uw*2.0f,v2)); // off texture    

    mPreviousTrackButton.setup(PREVIOUS_TRACK, 
                               Rectf(uw*2.0f,v2,uw*3.0f,v3),  // on texture
                               Rectf(uw*2.0f,v1,uw*3.0f,v2)); // off texture

    mPlayPauseButton.setup(PLAY_PAUSE, 
                           false,               // on
                           Rectf(uw*3.0f,v2,uw*4.0f,v3), // offUp   
                           Rectf(uw*3.0f,v1,uw*4.0f,v2), // offDown
                           Rectf(uw*4.0f,v2,uw*5.0f,v3), // onUp
                           Rectf(uw*4.0f,v1,uw*5.0f,v2));// onDown

    mNextTrackButton.setup(NEXT_TRACK, 
                           Rectf(uw*5.0f,v2,uw*6.0f,v3),  // on texture
                           Rectf(uw*5.0f,v1,uw*6.0f,v2)); // off texture
    
    // !!! SMALL BUTTONS !!!
    uw = 1.0/10.0f;
	v1 = 0.5f; 
    v2 = 0.7f; 
    v3 = 0.9f;
    
    mHelpButton.setup(HELP, 
                      false, 
                      Rectf(uw*0.0f,v2,uw*1.0f,v3),  // on texture
                      Rectf(uw*0.0f,v1,uw*1.0f,v2)); // off texture
    
    mOrbitsButton.setup(DRAW_RINGS, 
                        false, 
                        Rectf(uw*1.0f,v2,uw*2.0f,v3),  // on texture
                        Rectf(uw*1.0f,v1,uw*2.0f,v2)); // off texture
    
    mLabelsButton.setup(DRAW_TEXT, 
                        false, 
                        Rectf(uw*2.0f,v2,uw*3.0f,v3),  // on texture
                        Rectf(uw*2.0f,v1,uw*3.0f,v2)); // off texture

    mPlayheadSlider.setup(SLIDER,          // ID
                          Rectf(uw * 0.0f, 0.9f, uw * 1.0f, 1.0f),  // bg texture
                          Rectf(uw * 2.0f, 0.9f, uw * 3.0f, 1.0f),  // fg texture
                          Rectf(uw * 3.0f, 0.7f, uw * 4.0f, 0.9f),  // thumb on texture
                          Rectf(uw * 3.0f, 0.5f, uw * 4.0f, 0.7f)); // thumb off texture

    /////// no textures please, we're British...
    
    mTrackInfoLabel.setup(font, Color::white());
    
    mElapsedTimeLabel.setup(font, COLOR_BRIGHT_BLUE);
    
    mRemainingTimeLabel.setup(font, COLOR_BRIGHT_BLUE);
        
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
    mCurrentTrackButton.setDown(mLastTouchedType == mCurrentTrackButton.getId());
    //        mAlphaWheelButton.setDown(mLastTouchedType == mAlphaWheelButton.getId());
    //        mHelpButton.setDown(mLastTouchedType == mHelpButton.getId());
    //        mOrbitsButton.setDown(mLastTouchedType == mOrbitsButton.getId());
    //        mLabelsButton.setDown(mLastTouchedType == mLabelsButton.getId());
    mPreviousTrackButton.setDown(mLastTouchedType == mPreviousTrackButton.getId());
    mPlayPauseButton.setDown(mLastTouchedType == mPlayPauseButton.getId());
    mNextTrackButton.setDown(mLastTouchedType == mNextTrackButton.getId());         
    
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
	float topBorder = 7.0f;
	float sideBorder = 10.0f;
    
    float bSize = 50.0f;
    float bSizeSmall = 40.0f;
	float buttonGap	= 1.0f;

	float timeTexWidth	= 55.0f;
    float sliderHeight = 20.0f;
    float sliderInset = bSize + timeTexWidth;
    float sliderWidth = isLandscapeOrientation(mInterfaceOrientation) ? 328.0f : 201.0f;
    
    // FLY TO CURRENT TRACK-MOON BUTTON
	float y1 = topBorder;
	float y2 = y1 + bSize;
	float x1 = sideBorder;
	float x2 = x1 + bSize;        
    mCurrentTrackButton.setRect(Rectf(x1,y1,x2,y2));

    // ALPHA WHEEL BUTTON
    x1 = mInterfaceSize.x / 2 - bSize / 2;
    x2 = x1 + bSize;
    mAlphaWheelButton.setRect(Rectf(x1,y1,x2,y2));

    // NEXT / PLAY-PAUSE / PREVIOUS TRACK BUTTON
    x1 = mInterfaceSize.x - sideBorder - bSize;
	x2 = x1 + bSize;
    mNextTrackButton.setRect(Rectf(x1,y1,x2,y2));

    x1 -= bSize + buttonGap;
	x2 = x1 + bSize;
    mPlayPauseButton.setRect(Rectf(x1,y1,x2,y2));
    
	x1 -= bSize + buttonGap;
	x2 = x1 + bSize;    
    mPreviousTrackButton.setRect(Rectf(x1,y1,x2,y2));

    // LABEL TOGGLE BUTTON
    y1 += 5.0f;
	x1 -= bSize * 1.625f;
	x2 = x1 + bSizeSmall;    
    y2 = y1 + bSizeSmall;
    mLabelsButton.setRect(Rectf(x1,y1,x2,y2));
    
    // ORBIT RING TOGGLE BUTTON
	x1 -= bSizeSmall - 5.0f;
	x2 = x1 + bSizeSmall;
    mOrbitsButton.setRect(Rectf(x1,y1,x2,y2));
	
    // HELP TOGGLE BUTTON
    x1 -= bSizeSmall - 5.0f;
	x2 = x1 + bSizeSmall;
    mHelpButton.setRect(Rectf(x1,y1,x2,y2));

    float bgx1			= sliderInset;
    float bgx2			= bgx1 + sliderWidth;
    float bgy1			= 32.0f;
    float bgy2			= bgy1 + sliderHeight;
    mPlayheadSlider.setRect(Rectf(bgx1,bgy1,bgx2,bgy2));
    
    float ctx1 = bgx1 - 43.0f;
    float ctx2 = bgx2 + 50.0f;
    float cty1 = bgy1 - 14.0f;
    float cty2 = cty1; // NB:- will be overridden in ScrollingLabel::draw()
    mTrackInfoLabel.setRect(Rectf(ctx1, cty1, ctx2, cty2));

    // FIXME: bottom right coords are made up... maybe just setPos (and getWidth) for these?
    // at least use font height for calculating y2
    mElapsedTimeLabel.setRect(Rectf(bgx1-40.0f,bgy1+2.0f,bgx1,bgy1+12.0f));
    mRemainingTimeLabel.setRect(Rectf(bgx2+8.0f,bgy1+2.0f,bgx2+48.0f,bgy1+12.0f));

}

void PlayControls::draw(float y)
{
    mLastDrawY = y;
    
    gl::pushModelView();
    gl::multModelView( mOrientationMatrix );
    gl::translate( Vec2f(0, y) );
    
    mTexture.enableAndBind();
    
    mCurrentTrackButton.draw();
    
    mPlayheadSlider.draw();
    
    mAlphaWheelButton.draw();
    
    mHelpButton.draw();
    mOrbitsButton.draw();
    mLabelsButton.draw();
    
    mPreviousTrackButton.draw();
    mPlayPauseButton.draw();
    mNextTrackButton.draw();
    
    mTexture.unbind();

    mElapsedTimeLabel.draw();
    mRemainingTimeLabel.draw();
    mTrackInfoLabel.draw();

    gl::popModelView();
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

    vector<TouchEvent::Touch> touches = event.getTouches();
    if (touches.size() > 0 && transformedBounds.contains(touches[0].getPos())) { // FIXME: test all touches in this touchesBegan batch
        
        if (cbTouchesEnded == 0) {
            cbTouchesEnded = mApp->registerTouchesEnded( this, &PlayControls::touchesEnded );
            cbTouchesMoved = mApp->registerTouchesMoved( this, &PlayControls::touchesMoved );			
        }

        mLastTouchedType = findButtonUnderTouches(touches);
		
        if (mLastTouchedType == SLIDER) {
            mPlayheadSlider.setIsDragging(true);
            dragPlayheadToPos(touches.begin()->getPos());
        }
             
        return true;
    }
    else {
        mLastTouchedType = NO_BUTTON;
    }
    
    return false;
}

bool PlayControls::touchesMoved( TouchEvent event )
{
    vector<TouchEvent::Touch> touches = event.getTouches();

    mLastTouchedType = findButtonUnderTouches(touches);

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
    if( mLastTouchedType != NO_BUTTON && mLastTouchedType == findButtonUnderTouches(touches) ){
        mCallbacksButtonPressed.call(mLastTouchedType);
    }

    mLastTouchedType = NO_BUTTON;
    
    mPlayheadSlider.setIsDragging(false);

    return false;
}

PlayControls::ButtonId PlayControls::findButtonUnderTouches(vector<TouchEvent::Touch> touches) {
    Rectf transformedBounds = transformRect( Rectf(0, mLastDrawY, mInterfaceSize.x, mInterfaceSize.y), mOrientationMatrix );

    // FIXME: this is really dumb, make a base class for touchable things and use pointers in a vector instead
    vector<Rectf> touchRects;
    vector<int> touchTypes;
    touchRects.push_back(mCurrentTrackButton.getRect());
    touchRects.push_back(mPlayheadSlider.getRect());
    touchRects.push_back(mAlphaWheelButton.getRect());
    touchRects.push_back(mHelpButton.getRect());
    touchRects.push_back(mOrbitsButton.getRect());
    touchRects.push_back(mLabelsButton.getRect());
    touchRects.push_back(mPreviousTrackButton.getRect());
    touchRects.push_back(mPlayPauseButton.getRect());
    touchRects.push_back(mNextTrackButton.getRect());
    touchTypes.push_back(mCurrentTrackButton.getId());
    touchTypes.push_back(mPlayheadSlider.getId());
    touchTypes.push_back(mAlphaWheelButton.getId());
    touchTypes.push_back(mHelpButton.getId());
    touchTypes.push_back(mOrbitsButton.getId());
    touchTypes.push_back(mLabelsButton.getId());
    touchTypes.push_back(mPreviousTrackButton.getId());
    touchTypes.push_back(mPlayPauseButton.getId());
    touchTypes.push_back(mNextTrackButton.getId());    

    // offset and transform:
    for (int i = 0; i < touchRects.size(); i++) {
        Rectf rect = touchRects[i].getOffset( Vec2f(0, mLastDrawY) );
        touchRects[i] = transformRect( rect, mOrientationMatrix );
    }
    
    // check for touches and return first one found
    // TODO: accept more than one touch, by ID?
    for (int j = 0; j < touches.size(); j++) {
        TouchEvent::Touch touch = touches[j];
        Vec2f pos = touch.getPos();
        if ( transformedBounds.contains( pos ) ) {
            for (int i = 0; i < touchRects.size(); i++) {
                if (touchRects[i].contains(pos)) {
                    return ButtonId(touchTypes[i]);
                }
            }		
        }
    }
    
    return NO_BUTTON;
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
