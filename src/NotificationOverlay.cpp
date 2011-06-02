//
//  NotificationOverlay.cpp
//  Kepler
//
//  Created by Tom Carden on 6/2/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "NotificationOverlay.h"

NotificationOverlay::NotificationOverlay() {
    mActive = false;
    mFadeDelay = 1.0f;
    mFadeDuration = 1.0f;
}

NotificationOverlay::~NotificationOverlay() {
    hide();
}

void NotificationOverlay::setup( AppCocoaTouch *app, const Orientation &orientation )
{
    mApp = app;
    setInterfaceOrientation(orientation);
}

void NotificationOverlay::update()
{
    if (!mActive) return;
    
    // if enough time has passed, hide the overlay (cleanup happens in hide())
    float elapsedSince = mApp->getElapsedSeconds() - mLastShowTime;
    if (elapsedSince > mFadeDelay + mFadeDuration) {
        hide();
    }
}

void NotificationOverlay::draw()
{
    if (!mActive) return;
    
    float elapsedSince = mApp->getElapsedSeconds() - mLastShowTime;
    
    float alpha = 1.0;
        
    if (elapsedSince > mFadeDelay) {
        alpha = (elapsedSince - mFadeDelay) / mFadeDuration;
    }
    
    Vec2f topLeft( mInterfaceSize.x - mCurrentSrcArea.getWidth()/2.0,
                  mInterfaceSize.y - mCurrentSrcArea.getHeight()/2.0 );
    Vec2f bottomRight( mInterfaceSize.x + mCurrentSrcArea.getWidth()/2.0,
                  mInterfaceSize.y + mCurrentSrcArea.getHeight()/2.0 );    
    Rectf dstRect( topLeft, bottomRight );
    
    gl::draw(mCurrentTexture, mCurrentSrcArea, dstRect);
             
    // TODO: draw mCurrentMessage as well (probably want to nudge the positions up for that)
}

void NotificationOverlay::setInterfaceOrientation( const Orientation &orientation )
{
    mInterfaceOrientation = orientation;
    mOrientationMatrix = getOrientationMatrix44(mInterfaceOrientation, getWindowSize());
    
    mInterfaceSize = getWindowSize();
    
    if ( isLandscapeOrientation( mInterfaceOrientation ) ) {
        mInterfaceSize = mInterfaceSize.yx(); // swizzle it!
    }
}

void NotificationOverlay::show(const gl::Texture &texture, const Area &srcArea, const string &message)
{
    mCurrentTexture = texture;
    mCurrentSrcArea = srcArea;
    mCurrentMessage = message;
    mActive = true;
    mLastShowTime = mApp->getElapsedSeconds();
}

void NotificationOverlay::hide()
{
    mActive = false;
    mCurrentTexture.reset();
}
