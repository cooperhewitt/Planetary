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
                              Rectf(0,0,0,0),  // position (updateUIRects sets this correctly)
                              Rectf(uw*0.0f,v2,uw*1.0f,v3),  // on texture
                              Rectf(uw*0.0f,v1,uw*1.0f,v2)); // off texture

    mAlphaWheelButton.setup(SHOW_WHEEL,      // ID
                            Rectf(0,0,0,0),  // position (updateUIRects sets this correctly)
                            false,           // initial toggle state
                            Rectf(uw*1.0f,v2,uw*2.0f,v3),  // on texture
                            Rectf(uw*1.0f,v1,uw*2.0f,v2)); // off texture    

    mPreviousTrackButton.setup(PREVIOUS_TRACK, 
                               Rectf(0,0,0,0),  // position (updateUIRects sets this correctly)
                               Rectf(uw*2.0f,v2,uw*3.0f,v3),  // on texture
                               Rectf(uw*2.0f,v1,uw*3.0f,v2)); // off texture

    mPlayPauseButton.setup(PLAY_PAUSE, 
                           Rectf(0,0,0,0),      // position (updateUIRects sets this correctly)
                           false,               // on
                           Rectf(uw*3.0f,v2,uw*4.0f,v3), // onUp   
                           Rectf(uw*3.0f,v1,uw*4.0f,v2), // onDown
                           Rectf(uw*4.0f,v2,uw*5.0f,v3), // offUp
                           Rectf(uw*4.0f,v1,uw*5.0f,v2));// offDown

    mNextTrackButton.setup(NEXT_TRACK, 
                           Rectf(0,0,0,0),      // position (updateUIRects sets this correctly)
                           Rectf(uw*5.0f,v2,uw*6.0f,v3),  // on texture
                           Rectf(uw*5.0f,v1,uw*6.0f,v2)); // off texture
    
    // !!! SMALL BUTTONS !!!
    uw = 1.0/10.0f;
	v1 = 0.5f; 
    v2 = 0.7f; 
    v3 = 0.9f;
    
    mHelpButton.setup(HELP, 
                      Rectf(0,0,0,0),  // position (updateUIRects sets this correctly)
                      false, 
                      Rectf(uw*0.0f,v2,uw*1.0f,v3),  // on texture
                      Rectf(uw*0.0f,v1,uw*1.0f,v2)); // off texture
    
    mOrbitsButton.setup(DRAW_RINGS, 
                        Rectf(0,0,0,0),  // position (updateUIRects sets this correctly)
                        false, 
                        Rectf(uw*1.0f,v2,uw*2.0f,v3),  // on texture
                        Rectf(uw*1.0f,v1,uw*2.0f,v2)); // off texture
    
    mLabelsButton.setup(DRAW_TEXT, 
                        Rectf(0,0,0,0),  // position (updateUIRects sets this correctly)
                        false, 
                        Rectf(uw*2.0f,v2,uw*3.0f,v3),  // on texture
                        Rectf(uw*2.0f,v1,uw*3.0f,v2)); // off texture

    mPlayheadSlider.setup(SLIDER,          // ID
                          Rectf(0,0,0,0),  // position (updateUIRects sets this correctly)
                          Rectf(uw * 0.0f, 0.9f, uw * 1.0f, 1.0f),  // bg texture
                          Rectf(uw * 2.0f, 0.9f, uw * 3.0f, 1.0f),  // fg texture
                          Rectf(uw * 3.0f, 0.7f, uw * 4.0f, 0.9f),  // thumb on texture
                          Rectf(uw * 3.0f, 0.5f, uw * 4.0f, 0.7f)); // thumb off texture

    /////// no textures please, we're British...
    
    mTrackInfoLabel.setup(Rectf(0,0,0,0), // position (updateUIRects sets this correctly)
                          font, 
                          Color::white());
    
    mElapsedTimeLabel.setup(Rectf(0,0,0,0), // position (updateUIRects sets this correctly)
                            font, 
                            Color::white());
    
    mRemainingTimeLabel.setup(Rectf(0,0,0,0), // position (updateUIRects sets this correctly)
                              font, 
                              Color::white());
        
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
    float cty2 = cty1 + 10.0f; // FIXME: 10.0f was currentTrackTex.getHeight();
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
    
    mTrackInfoLabel.draw();
    mElapsedTimeLabel.draw();
    mPlayheadSlider.draw();
    mRemainingTimeLabel.draw();
    
    mAlphaWheelButton.draw();
    
    mHelpButton.draw();
    mOrbitsButton.draw();
    mLabelsButton.draw();
    
    mPreviousTrackButton.draw();
    mPlayPauseButton.draw();
    mNextTrackButton.draw();
    
    mTexture.unbind();
    
    gl::popModelView();
}

bool PlayControls::touchesBegan( TouchEvent event )
{
//    Rectf transformedBounds = transformRect( lastDrawnBoundsRect, mOrientationMatrix );
//    vector<TouchEvent::Touch> touches = event.getTouches();
//    if (touches.size() > 0 && transformedBounds.contains(touches[0].getPos())) {
//        if (cbTouchesEnded == 0) {
//            cbTouchesEnded = mApp->registerTouchesEnded( this, &PlayControls::touchesEnded );
//            cbTouchesMoved = mApp->registerTouchesMoved( this, &PlayControls::touchesMoved );			
//        }
//
//        lastTouchedType = findButtonUnderTouches(touches);
//		
//        if( lastTouchedType == SLIDER ){
//            mIsDraggingPlayhead = true;
//        }
//        
//        return true;
//    }
//    else {
//        lastTouchedType = NO_BUTTON;
//    }
    return false;
}

bool PlayControls::touchesMoved( TouchEvent event )
{
    vector<TouchEvent::Touch> touches = event.getTouches();

    mLastTouchedType = findButtonUnderTouches(touches);

//    if( mIsDraggingPlayhead ){
//        if( touches.size() == 1 ){
//            
//            Vec2f pos = touches.begin()->getPos();
//            pos = (mOrientationMatrix.inverted() * Vec3f(pos,0)).xy();
//            
//			// MAGIC NUMBER: beware these hard-coded nasties!
//            float border = 110.0f;
//			float sliderLength = mInterfaceSize.x * 0.3f;
//            float playheadPer = ( pos.x - border ) / sliderLength;
//            
//            // TODO: Make this callback take a double (between 0 and 1?)
//            mCallbacksPlayheadMoved.call( constrain( playheadPer, 0.0f, 1.0f ) );
//        }
//    }
    
    return false;
}	

bool PlayControls::touchesEnded( TouchEvent event )
{
    vector<TouchEvent::Touch> touches = event.getTouches();
    if( mLastTouchedType != NO_BUTTON && mLastTouchedType == findButtonUnderTouches(touches) ){
        mCallbacksButtonPressed.call(mLastTouchedType);
    }

    mLastTouchedType = NO_BUTTON;
    //mIsDraggingPlayhead = false;
    return false;
}

PlayControls::ButtonId PlayControls::findButtonUnderTouches(vector<TouchEvent::Touch> touches) {
//    Rectf transformedBounds = transformRect( lastDrawnBoundsRect, mOrientationMatrix );
//    for (int j = 0; j < touches.size(); j++) {
//        TouchEvent::Touch touch = touches[j];
//        Vec2f pos = touch.getPos();
//        if ( transformedBounds.contains( pos ) ) {
//            for (int i = 0; i < touchRects.size(); i++) {
//                Rectf rect = transformRect( touchRects[i], mOrientationMatrix );
//                if (rect.contains(pos)) {
//                    return touchTypes[i];
//                }
//            }		
//        }
//    }		
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
