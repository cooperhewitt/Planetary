//
//  SettingsPanel.cpp
//  Kepler
//
//  Created by Tom Carden on 8/25/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
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
	float x0 = 350.0f;
	float x1 = 390.0f;
	float x2 = 430.0f;
	float x3 = 470.0f;
	float x4 = 510.0f;

	float y0 = 300.0f;
	float y1 = 340.0f;
	float y2 = 380.0f;
	float y3 = 420.0f;
	float y4 = 460.0f;
	float y5 = 500.0f;

    
    mHelpButton = new ToggleButton(			HELP, 
											false, 
											uiSmallButtonsTex,
											Area( x0, y1, x1, y2 ),  // on texture
											Area( x0, y0, x1, y1 ) ); // off texture
    
    if( G_IS_IPAD2 ){
        mGyroButton = new ToggleButton(		USE_GYRO, 
											false, 
											uiSmallButtonsTex,
											Area( x1, y1, x2, y2 ),  // on texture
											Area( x1, y0, x2, y1 ) ); // off texture
    }
	
	mOrbitsButton = new ToggleButton(		DRAW_RINGS, 
											false, 
											uiSmallButtonsTex,
											Area( x2, y1, x3, y2 ),  // on texture
											Area( x2, y0, x3, y1 ) ); // off texture
	
	mLabelsButton = new ToggleButton(		DRAW_TEXT, 
											false, 
											uiSmallButtonsTex,
											Area( x3, y1, x4, y2 ),  // on texture
											Area( x3, y0, x4, y1 ) ); // off texture
	
	mDebugButton = new ToggleButton(		DEBUG_FEATURE, 
											false, 
											uiSmallButtonsTex,
											Area( x0, y3, x1, y4 ),  // on texture
											Area( x0, y2, x1, y3 ) ); // off texture
	
    mScreensaverButton = new ToggleButton(	AUTO_MOVE,
											false,
											uiSmallButtonsTex,
											Area( x1, y3, x2, y4 ),
											Area( x1, y2, x2, y3 ) );
    

    
   
    

    
    
    mShuffleButton = new ToggleButton(		SHUFFLE, 
											false, 
											uiSmallButtonsTex,
											Area( x2, y3, x3, y4 ),  // on texture
											Area( x2, y2, x3, y3 ) ); // off texture
    
    mRepeatButton = new ThreeStateButton(	REPEAT, 
											0, 
											uiSmallButtonsTex,
											Area( x3, y2, x4, y3 ),   // first texture  (off)
											Area( x3, y3, x4, y4 ),   // second texture (repeat all)
											Area( x3, y4, x4, y5 ) ); // third texture  (repeat one)
    
    ///////
	// TODO: add initial value
	mParamSlider1 = new Slider(				PARAMSLIDER1,          // ID
											uiSmallButtonsTex,
											Area( 351, 460, 358, 500 ),  // bg texture
											Area( 362, 460, 369, 500 ),  // fg texture
											Area( 390, 460, 430, 500 ),  // thumb on texture
											Area( 430, 460, 470, 500 ) ); // thumb off texture
	
	mParamSlider1->setValue( 0.25f );
	mParamSlider1Label = new TextLabel( NO_BUTTON, font, BRIGHT_BLUE );
	mParamSlider1Label->setText( "Scale" );
	
	mParamSlider2 = new Slider(				PARAMSLIDER2,          // ID
											uiSmallButtonsTex,
											Area( 351, 460, 358, 500 ),  // bg texture
											Area( 362, 460, 369, 500 ),  // fg texture
											Area( 390, 460, 430, 500 ),  // thumb on texture
											Area( 430, 460, 470, 500 ) ); // thumb off texture
    
	mParamSlider2->setValue( 0.15f );
	mParamSlider2Label = new TextLabel( NO_BUTTON, font, BRIGHT_BLUE );
	mParamSlider2Label->setText( "Speed" );    
}    

bool SettingsPanel::addedToScene()
{
    // now mRoot is valid we can add children
    addChildren(); // FIXME: make it so you can add children even if mRoot is invalid
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
//    addChild( BloomNodeRef(mDebugButton) );
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
	const bool  landscape    = interfaceSize.x > interfaceSize.y;   
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
	float gap = landscape ? 110.0f : 35.0f;
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
//	x1 -= bSizeSmall;
//	x2 = x1 + bSizeSmall;
//    mDebugButton->setRect( x1, y1, x2, y2 );
	
    // SCREENSAVER TOGGLE BUTTON
	x1 -= bSizeSmall;
	x2 = x1 + bSizeSmall;
    mScreensaverButton->setRect( x1, y1, x2, y2 );
	
    //// HELP TOGGLE BUTTON
    x1 -= bSizeSmall;
	x2 = x1 + bSizeSmall;
    mHelpButton->setRect( x1, y1, x2, y2 );
	

	const float paramSliderWidth = landscape ? 200.0f : 120.0f;
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
        glMultMatrixf(mTransform);

        // draw background so we can't see alpha/playlist chooser behind us in transitions
		gl::color( Color::black() );
        gl::drawSolidRect( Rectf( Vec2f( 0.0f, 0.0f ), Vec2f( mInterfaceSize.x, getHeight() ) ) );
       
        // draw line at the top (as for each ui panel)
		gl::color( ColorA( BRIGHT_BLUE, 0.125f ) );
        gl::drawLine( Vec2f( 0.0f, 0.0f ), Vec2f( mInterfaceSize.x, 0.0f ) );

        // tint children
		gl::color( ColorA( 1, 1, 1, mOpacity ) ); // mOpacity comes from UiLayer
		
        bloom::gl::beginBatch();
        // draw children
        BOOST_FOREACH(BloomNodeRef child, mChildren) {        
            child->deepDraw();
        }
        bloom::gl::endBatch();
        glPopMatrix();
    }        
}

//bool SettingsPanel::onBloomNodeTouchEnded( BloomNodeRef nodeRef )
//{
//    if ( nodeRef->getId() > NO_BUTTON && nodeRef->getId() < LAST_BUTTON ) {
//        mCallbacksButtonPressed.call(ButtonId(nodeRef->getId()));
//    }
//    return false;
//}

float SettingsPanel::getHeight()
{
    return 50.0f;
}