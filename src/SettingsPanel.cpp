//
//  SettingsPanel.cpp
//  Kepler
//
//  Created by Tom Carden on 8/25/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include <boost/foreach.hpp>
#include "SettingsPanel.h"
#include "Globals.h"
#include "BloomGl.h" // for batch drawing
#include "BloomScene.h" // for mRoot

using namespace ci;
using namespace ci::app;
using namespace std;

void SettingsPanel::setup( const Vec2f &interfaceSize, ipod::Player *player, const Font &font, const gl::Texture &uiSmallButtonsTex )
{   
    // create, add, and position everything...
    createChildren( font, uiSmallButtonsTex );
    setInterfaceSize( interfaceSize );
    
    // set initial state...
    setOrbitsOn( G_DRAW_RINGS );
    setLabelsOn( G_DRAW_TEXT );
    setHelpOn( false ); // this is the default in KeplerApp
    setDebugOn( G_DEBUG );	    
    setShuffleOn( player->getShuffleMode() != ipod::Player::ShuffleModeOff );
    setRepeatMode( player->getRepeatMode() );    

    if( G_IS_IPAD2 ) {
        setGyroOn( G_USE_GYRO );
    }
}

void SettingsPanel::createChildren( const Font &font, const gl::Texture &uiSmallButtonsTex )
{
    // !!! SMALL BUTTONS !!!
    float uw = uiSmallButtonsTex.getWidth() / 5.0f;
	float uh = uiSmallButtonsTex.getHeight() / 5.0f;
    
    mHelpButton = new ToggleButton( HELP, 
                                   false, 
                                   uiSmallButtonsTex,
                                   Area( uw*0, uh*1, uw*1, uh*2 ),  // on texture
                                   Area( uw*0, uh*0, uw*1, uh*1 ) ); // off texture
    
    if( G_IS_IPAD2 ){
        mGyroButton = new ToggleButton( USE_GYRO, 
                                       false, 
                                       uiSmallButtonsTex,
                                       Area( uw*1, uh*1, uw*2, uh*2 ),  // on texture
                                       Area( uw*1, uh*0, uw*2, uh*1 ) ); // off texture
    }
    mScreensaverButton = new ToggleButton( AUTO_MOVE,
                                          false,
                                          uiSmallButtonsTex,
                                          Area( uw*4, uh*3, uw*5, uh*4 ),
                                          Area( uw*4, uh*2, uw*5, uh*3 ) );
    
    mOrbitsButton = new ToggleButton( DRAW_RINGS, 
                                     false, 
                                     uiSmallButtonsTex,
                                     Area( uw*2, uh*1, uw*3, uh*2 ),  // on texture
                                     Area( uw*2, uh*0, uw*3, uh*1 ) ); // off texture
    
    mLabelsButton = new ToggleButton( DRAW_TEXT, 
                                     false, 
                                     uiSmallButtonsTex,
                                     Area( uw*3, uh*1, uw*4, uh*2 ),  // on texture
                                     Area( uw*3, uh*0, uw*4, uh*1 ) ); // off texture
    
    mDebugButton = new ToggleButton( DEBUG_FEATURE, 
                                    false, 
                                    uiSmallButtonsTex,
                                    Area( uw*4, uh*1, uw*5, uh*2 ),  // on texture
                                    Area( uw*4, uh*0, uw*5, uh*1 ) ); // off texture
    
    
    mShuffleButton = new ToggleButton( SHUFFLE, 
                                      false, 
                                      uiSmallButtonsTex,
                                      Area( uw*0, uh*3, uw*1, uh*4 ),  // on texture
                                      Area( uw*0, uh*2, uw*1, uh*3 ) ); // off texture
    
    mRepeatButton = new ThreeStateButton( REPEAT, 
                                         0, 
                                         uiSmallButtonsTex,
                                         Area( uw*1, uh*2, uw*2, uh*3 ),   // first texture  (off)
                                         Area( uw*1, uh*3, uw*2, uh*4 ),   // second texture (repeat all)
                                         Area( uw*2, uh*2, uw*3, uh*3 ) ); // third texture  (repeat one)
    
    ///////
	// TODO: add initial value
	mParamSlider1 = new Slider( PARAMSLIDER1,          // ID
							   uiSmallButtonsTex,
							   Area( uw*2.2f, uh*3, uw*2.3f, uh*4 ),  // bg texture
							   Area( uw*2.7f, uh*3, uw*2.8f, uh*4 ),  // fg texture
							   Area( uw*3, uh*3, uw*4, uh*4 ),  // thumb on texture
							   Area( uw*3, uh*2, uw*4, uh*3 )); // thumb off texture
	mParamSlider1->setValue( 0.25f );
	mParamSlider1Label = new TextLabel( NO_BUTTON, font, BRIGHT_BLUE );
	mParamSlider1Label->setText( "Scale" );
	
	mParamSlider2 = new Slider( PARAMSLIDER2,          // ID
							   uiSmallButtonsTex,
							   Area( uw*2.2f, uh*3, uw*2.3f, uh*4 ),  // bg texture
							   Area( uw*2.7f, uh*3, uw*2.8f, uh*4 ),  // fg texture
							   Area( uw*3, uh*3, uw*4, uh*4 ),  // thumb on texture
							   Area( uw*3, uh*2, uw*4, uh*3 )); // thumb off texture
    
	mParamSlider2->setValue( 0.15f );
	mParamSlider2Label = new TextLabel( NO_BUTTON, font, BRIGHT_BLUE );
	mParamSlider2Label->setText( "Speed" );    
}    

bool SettingsPanel::addedToScene()
{
    // now mRoot is valid we can add children
    addChildren(); // FIXME: make it so you can add children even if mRoot is invalid
    mCbTouchEnded = getRoot()->registerBloomNodeTouchEnded( this, &SettingsPanel::onBloomNodeTouchEnded );    
    return false;
}

bool SettingsPanel::removedFromScene()
{
    // remove listeners...
    // FIXME: this should also be done in destructor (?)
    getRoot()->unregisterBloomNodeTouchEnded( mCbTouchEnded );    
    return false;
}

void SettingsPanel::addChildren()
{
    addChild( BloomNodeRef(mShuffleButton) );
    addChild( BloomNodeRef(mRepeatButton) );
	addChild( BloomNodeRef(mScreensaverButton) );
    addChild( BloomNodeRef(mHelpButton) );
    addChild( BloomNodeRef(mOrbitsButton) );
    addChild( BloomNodeRef(mLabelsButton) );
    addChild( BloomNodeRef(mDebugButton) );
    if( G_IS_IPAD2 ) addChild( BloomNodeRef(mGyroButton) );
	addChild( BloomNodeRef(mParamSlider1) );
	addChild( BloomNodeRef(mParamSlider2) );
	addChild( BloomNodeRef(mParamSlider1Label) );
    addChild( BloomNodeRef(mParamSlider2Label) );    
}

void SettingsPanel::setInterfaceSize( const Vec2f &interfaceSize )
{
    mInterfaceSize = interfaceSize;
    
	const float topBorder	 = 5.0f;
	const float sideBorder	 = 10.0f;
    const float bSizeSmall	 = 40.0f;
    
	float y1 = topBorder;
    float y2 = y1 + bSizeSmall;
	
    // SHUFFLE TOGGLE BUTTON
	float x1 = interfaceSize.x - sideBorder - bSizeSmall - 3.0f;
	float x2 = x1 + bSizeSmall;    
    mShuffleButton->setRect( x1, y1, x2, y2 );
    
    // REPEAT TOGGLE BUTTON
	x1 -= bSizeSmall;
	x2 = x1 + bSizeSmall;
    mRepeatButton->setRect( x1, y1, x2, y2 );
	
    // TEXT LABELS TOGGLE BUTTON
	float gap = 25.0f;
	x1 -= bSizeSmall + gap;
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
	
    // SCREENSAVER TOGGLE BUTTON
	x1 -= bSizeSmall;
	x2 = x1 + bSizeSmall;
    mScreensaverButton->setRect( x1, y1, x2, y2 );
	
    //// HELP TOGGLE BUTTON
    x1 -= bSizeSmall;
	x2 = x1 + bSizeSmall;
    mHelpButton->setRect( x1, y1, x2, y2 );
	
    const bool  landscape        = interfaceSize.x > interfaceSize.y;
	const float paramSliderWidth = landscape ? 250.0f : 150.0f;
	const float slider1X         = 60.0f;
	const float slider2X         = slider1X + paramSliderWidth + 75.0f;
	const float sliderYOff       = 16.0f;
    const float sliderHeight     = 20.0f;
    
    mParamSlider1->setRect( slider1X, sliderYOff, slider1X + paramSliderWidth, sliderYOff + sliderHeight );
    mParamSlider2->setRect( slider2X, sliderYOff, slider2X + paramSliderWidth, sliderYOff + sliderHeight );
	
	mParamSlider1Label->setRect( slider1X - 40.0f, sliderYOff, slider1X, sliderYOff + sliderHeight );
	mParamSlider2Label->setRect( slider2X - 45.0f, sliderYOff, slider2X, sliderYOff + sliderHeight);	
}

void SettingsPanel::update()
{
    Vec2f interfaceSize = getRoot()->getInterfaceSize();
    if ( mInterfaceSize != interfaceSize ) {
        setInterfaceSize( interfaceSize );
    }    
}

void SettingsPanel::deepDraw()
{
    if (mVisible) {
        glPushMatrix();
        glMultMatrixf(mTransform); // FIXME only push/mult/pop if mTransform isn't identity
        gl::color( Color::black() );
        gl::drawSolidRect( Rectf(0,0,mInterfaceSize.x,getHeight()) );
        gl::color( Color::white() ); // FIXME: set alpha from UiLayer?
        bloom::gl::beginBatch();
        // draw children
        BOOST_FOREACH(BloomNodeRef child, mChildren) {        
            child->deepDraw();
        }
        bloom::gl::endBatch();
        glPopMatrix();
    }        
}

bool SettingsPanel::onBloomNodeTouchEnded( BloomNodeRef nodeRef )
{
    if ( nodeRef->getId() > NO_BUTTON && nodeRef->getId() < LAST_BUTTON ) {
        mCallbacksButtonPressed.call(ButtonId(nodeRef->getId()));
    }
    return false;
}

float SettingsPanel::getHeight()
{
    return 50.0f;
}

