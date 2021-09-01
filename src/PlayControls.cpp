/*
 *  PlayControls.h
 *  Kepler
 *
 *  Created by Tom Carden on 3/7/11.
 *  Copyright 2013 Smithsonian Institution. All rights reserved.
 *
 */

#include <boost/foreach.hpp>
#include "PlayControls.h"
#include "Globals.h"
#include "BloomGl.h" // for batch drawing
#include "BloomScene.h" // for mRoot

using namespace ci;
using namespace ci::app;
using namespace std;

void PlayControls::setup( Vec2f interfaceSize, ipod::Player *player, const Font &font, const Font &fontSmall, const gl::Texture &uiButtonsTex )
{   

    // create, add, and position everything...
    createChildren( font, fontSmall, uiButtonsTex );
    setInterfaceSize( interfaceSize );
        
    // set initial state...
    setPlayingOn( player->getPlayState() == ipod::Player::StatePlaying );    
    setAlphaOn( false ); // this is the default in Vignette::setup()
    setPlaylistOn( false );
    setShowSettingsOn( G_SHOW_SETTINGS );
}

void PlayControls::createChildren( const Font &font, const Font &fontSmall, const gl::Texture &uiButtonsTex )
{
	float uw = 50.0f;   // button tex width
	float uh = 50.0f;	// button tex height
	float y0 = uh * 6.0f;
	float y1 = uh * 7.0f;
	float y2 = uh * 8.0f;
	float y3 = uh * 9.0f;
	
    mGalaxyButton = new SimpleButton(		GOTO_GALAXY,   // ID
											uiButtonsTex,
											Area( uw*0, y1, uw*1, y2 ),  // on texture
											Area( uw*0, y0, uw*1, y1 ) );// off texture
    
    mCurrentTrackButton = new SimpleButton( GOTO_CURRENT_TRACK,   // ID
                                            uiButtonsTex,
                                            Area( uw*1, y1, uw*2, y2 ),  // on texture
                                            Area( uw*1, y0, uw*2, y1 ) );// off texture

	mShowSettingsButton = new ToggleButton( SETTINGS, 
                                            false, 
                                            uiButtonsTex,
                                            Area( uw*2, y1, uw*3, y2 ),  // on texture
                                            Area( uw*2, y0, uw*3, y1 ) );// off texture   

    mPreviousTrackButton = new SimpleButton( PREV_TRACK, 
                                             uiButtonsTex,
                                             Area( uw*3, y1, uw*4, y2 ),  // on texture
                                             Area( uw*3, y0, uw*4, y1 ) );// off texture
	
    mPlayPauseButton = new TwoStateButton(	PLAY_PAUSE, 
											false, // initial state is updated in setup()
											uiButtonsTex,
											Area( uw*4, y1, uw*5, y2 ),  // offDown   
											Area( uw*4, y0, uw*5, y1 ),  // offUp
											Area( uw*5, y1, uw*6, y2 ),  // onDown
											Area( uw*5, y0, uw*6, y1 ) );// onUp

    mNextTrackButton = new SimpleButton(	NEXT_TRACK, 
											uiButtonsTex,
											Area( uw*6, y1, uw*7, y2 ),  // on texture
											Area( uw*6, y0, uw*7, y1 ) );// off texture
	
    mAlphaButton = new ToggleButton(		SHOW_ALPHA_FILTER,      // ID
											false,           // initial toggle state
											uiButtonsTex,
											Area( uw*2, y2, uw*3, y3 ),  // on texture
											Area( uw*0, y2, uw*1, y3 ) );// off texture    

    // FIXME: need a new texture for playlist button :(
    mPlaylistButton = new ToggleButton(		SHOW_PLAYLIST_FILTER,      // ID
											false,           // initial toggle state
											uiButtonsTex,
											Area( uw*3, y2, uw*4, y3 ),  // on texture
											Area( uw*1, y2, uw*2, y3 ) );// off texture         

    // !!! SMALL BUTTONS !!!
    uw = 40.0f;
	uh = 40.0f;
    
    mPlayheadSlider = new Slider( SLIDER,          // ID
                                  uiButtonsTex,
								  Area( 351, 460, 358, 500 ),  // bg texture
								  Area( 362, 460, 369, 500 ),  // fg texture
                                  Area( 390, 460, 430, 500 ),  // thumb on texture
                                  Area( 430, 460, 470, 500 ) ); // thumb off texture

    /////// no textures please, we're British...
    
    mTrackInfoLabel = new ScrollingLabel(NO_BUTTON, font, BRIGHT_BLUE);
    
    mElapsedTimeLabel = new TimeLabel(NO_BUTTON, fontSmall, BRIGHT_BLUE);
    
    mRemainingTimeLabel = new TimeLabel(NO_BUTTON, fontSmall, BRIGHT_BLUE);
    
    //////// little fady bits to cover the edges of scrolling bits:
    Area aLeft = Area( 380, 460, 390, 500 ); // references the uiButtons image    
    mCoverLeftTextureRect = new TextureRect( uiButtonsTex, aLeft );
    // NB:- when rect is set for the right side, x1 > x2 so it is flipped
    mCoverRightTextureRect = new TextureRect( uiButtonsTex, aLeft );    
}    

bool PlayControls::addedToScene()
{
    // now mRoot is valid we can add children
    addChildren(); // FIXME: make it so you can add children even if mRoot is invalid
    return false;
}

void PlayControls::addChildren()
{
    // bit of hack, labels first for batch reasons because they all use unique text textures
    // (we want the little fadey bits to be drawn on top at the end)
    addChild( BloomNodeRef(mTrackInfoLabel) );
    addChild( BloomNodeRef(mElapsedTimeLabel) );
    addChild( BloomNodeRef(mRemainingTimeLabel) ); 
    
    // and then shaded bits on top of scrolling mTrackInfoLabel
    // (these use uiButtonsTex)
    addChild( BloomNodeRef(mCoverLeftTextureRect) );
    addChild( BloomNodeRef(mCoverRightTextureRect) );        
    
    // and then all the other things that use uiButtonsTex
	addChild( BloomNodeRef(mPlayheadSlider) );
    
    addChild( BloomNodeRef(mGalaxyButton) );
	addChild( BloomNodeRef(mCurrentTrackButton) );
    
    addChild( BloomNodeRef(mShowSettingsButton) );
	addChild( BloomNodeRef(mAlphaButton) );
	addChild( BloomNodeRef(mPlaylistButton) );
    
    addChild( BloomNodeRef(mPreviousTrackButton) );
    addChild( BloomNodeRef(mPlayPauseButton) );
    addChild( BloomNodeRef(mNextTrackButton) );	    
}


//bool PlayControls::onBloomNodeTouchMoved( BloomNodeRef nodeRef )
//{
//    if ( nodeRef->getId() == mPlayheadSlider->getId() ) {
//        mCallbacksPlayheadMoved.call( mPlayheadSlider->getValue() );
//    }
//    return false;
//}
//
//bool PlayControls::onBloomNodeTouchEnded( BloomNodeRef nodeRef )
//{
//    if ( nodeRef->getId() == mPlayheadSlider->getId() ) {
//        mCallbacksPlayheadMoved.call( mPlayheadSlider->getValue() );
//    }
//    else if ( nodeRef->getId() > NO_BUTTON && nodeRef->getId() < LAST_BUTTON ) {
//        mCallbacksButtonPressed.call(ButtonId(nodeRef->getId()));
//    }
//    return false;
//}

void PlayControls::setInterfaceSize( Vec2f interfaceSize )
{
    mInterfaceSize = interfaceSize;
    
	const float topBorder	 = 5.0f;
	const float sideBorder	 = 5.0f;
    
    const float bSize		 = 50.0f;

	const float timeTexWidth = 60.0f;
    const float sliderHeight = 20.0f;
    const float sliderInset  = bSize * 2.0f + timeTexWidth;
    const bool  landscape    = interfaceSize.x > interfaceSize.y;
    const float sliderWidth  = landscape ? 328.0f : 170.0f;
	const float buttonGap	 = landscape ? bSize*2.0f : bSize*1.5f;
    
    // FLY TO CURRENT TRACK-MOON BUTTON
	float y1 = topBorder;
	float y2 = y1 + bSize;
	float x1 = sideBorder;
	float x2 = x1 + bSize;        
    mGalaxyButton->setRect( x1, y1, x2, y2 );
    
	x1 += bSize;
	x2 = x1 + bSize;
    mCurrentTrackButton->setRect( x1, y1, x2, y2 );

    // NEXT / PLAY-PAUSE / PREVIOUS TRACK BUTTONS
    x1 = interfaceSize.x - sideBorder - bSize;
	x2 = x1 + bSize;
    mNextTrackButton->setRect( x1, y1, x2, y2 );

    x1 -= bSize;
	x2 = x1 + bSize;
    mPlayPauseButton->setRect( x1, y1, x2, y2 );
    
	x1 -= bSize;
	x2 = x1 + bSize;    
    mPreviousTrackButton->setRect( x1, y1, x2, y2 );

    // SETTINGS BUTTON
	x1 -= buttonGap;
	x2 = x1 + bSize;    
    mShowSettingsButton->setRect( x1, y1, x2, y2 );

    // PLAYLIST BUTTON
	x1 -= buttonGap;
	x2 = x1 + bSize;
    mPlaylistButton->setRect( x1, y1, x2, y2 );	
	
	// ALPHA BUTTON
	x1 -= bSize;
	x2 = x1 + bSize;
    mAlphaButton->setRect( x1, y1, x2, y2 );


	
    const float bgx1 = sliderInset;
    const float bgx2 = bgx1 + sliderWidth;
    const float bgy1 = 32.0f;
    const float bgy2 = bgy1 + sliderHeight;
    mPlayheadSlider->setRect( bgx1, bgy1, bgx2, bgy2 );
	
    const float ctx1 = bgx1 - 43.0f;
    const float ctx2 = bgx2 + 48.0f;
    const float cty1 = bgy1 - 16.0f;
    const float cty2 = cty1 + mTrackInfoLabel->getFontHeight();
    mTrackInfoLabel->setRect( ctx1, cty1, ctx2, cty2 );

    // FIXME: bottom right coords are made up... maybe just setPos (and getWidth) for these?
    // at least use font height for calculating y2
//    mElapsedTimeLabel->setRect( bgx1-40.0f, bgy1+2.0f, bgx1, bgy1+12.0f );
	
	mElapsedTimeLabel->setRect( ctx1 + 3.0f, bgy1+2.0f, bgx1, bgy1+12.0f );
    mRemainingTimeLabel->setRect( bgx2+18.0f, bgy1+2.0f, bgx2+58.0f, bgy1+12.0f );
}

void PlayControls::update()
{
    Vec2f interfaceSize = getRoot()->getInterfaceSize();
    if ( mInterfaceSize != interfaceSize ) {
        setInterfaceSize( interfaceSize );
    }    

    const float w	 = 15.0f;
	Rectf infoRect   = mTrackInfoLabel->getRect();
	
	if( mTrackInfoLabel->isScrollingText() ) {
        mCoverLeftTextureRect->setRect( infoRect.x1, infoRect.y1, infoRect.x1 + w, infoRect.y2 );
    } else {
        mCoverLeftTextureRect->setRect( infoRect.x1, infoRect.y1, infoRect.x1, infoRect.y1 ); // zero size (FIXME: visible true/false? or remove from scene?
    }
    mCoverRightTextureRect->setRect( infoRect.x2 + 1.0f, infoRect.y1, infoRect.x2 - ( w - 1.0f ), infoRect.y2 );
    
}

void PlayControls::enablePlayerControls( bool enable )
{
    // FIXME: perhaps grayed out or dimmed would be more subtle?
    mPreviousTrackButton->setVisible(enable);
    mPlayPauseButton->setVisible(enable);
    mNextTrackButton->setVisible(enable);    
}

void PlayControls::deepDraw()
{
    if (mVisible) {
        glPushMatrix();
        glMultMatrixf(mTransform);
        
        // opacity is set by UiLayer...
        gl::color( ColorA(1,1,1,mOpacity) );
        
        bloom::gl::beginBatch();
        // draw children
        BOOST_FOREACH(BloomNodeRef child, mChildren) {        
            child->deepDraw();
        }
        bloom::gl::endBatch();
        
        glPopMatrix();
    }        
}

float PlayControls::getHeight()
{
    return 60.0f;
}

void PlayControls::setPlaylistButtonVisible( bool visible )
{
    mPlaylistButton->setVisible( visible );
    
    if ( mPlaylistButton->isVisible() ) {
        // FIXME: make a bsize a class level constant and use if for the offset here?
        mAlphaButton->setRect( mPlaylistButton->getRect().getOffset( Vec2f(-50.0f,0.0f) ) );        
    }
    else {
        mAlphaButton->setRect( mPlaylistButton->getRect() );        
    }
    
}
