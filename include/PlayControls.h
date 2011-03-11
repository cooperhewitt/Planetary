/*
 *  PlayControls.h
 *  Kepler
 *
 *  Created by Tom Carden on 3/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/TouchEvent.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PlayControls {
public:

	enum PlayButton { NO_BUTTON, PLAY_PAUSE, NEXT_TRACK, PREVIOUS_TRACK, SLIDER, DEBUG };
	enum ButtonTexId { TEX_PLAY, TEX_PAUSE, TEX_PREV, TEX_NEXT, TEX_DEBUG, TEX_DEBUGON };
	
	void setup( AppCocoaTouch *app, bool initialPlayState )
	{
		mApp = app;
		cbTouchesBegan = mApp->registerTouchesBegan( this, &PlayControls::touchesBegan );
		cbTouchesEnded = 0;
		cbTouchesMoved = 0;		
		lastTouchedType = NO_BUTTON;
		prevDrawY = 0;
		mIsPlaying = initialPlayState;
	}
	
	void update()
	{
		// TODO: update anything time based here, e.g. elapsed time of track playing
		// or e.g. button animation

		// clean up listeners here, because if we remove in touchesEnded then things get crazy
		if (mApp->getActiveTouches().size() == 0 && cbTouchesEnded != 0) {
			mApp->unregisterTouchesEnded( cbTouchesEnded );
			mApp->unregisterTouchesMoved( cbTouchesMoved );
			cbTouchesEnded = 0;
			cbTouchesMoved = 0;
		}		
	}

	bool touchesBegan( TouchEvent event )
	{
		vector<TouchEvent::Touch> touches = event.getTouches();
		if (touches.size() > 0 && touches[0].getY() > prevDrawY) {
			if (cbTouchesEnded == 0) {
				cbTouchesEnded = mApp->registerTouchesEnded( this, &PlayControls::touchesEnded );
				cbTouchesMoved = mApp->registerTouchesMoved( this, &PlayControls::touchesMoved );			
			}
			lastTouchedType = findButtonUnderTouches(touches);
		}
		else {
			lastTouchedType = NO_BUTTON;
		}
		return false;
	}

	bool touchesMoved( TouchEvent event )
	{
		vector<TouchEvent::Touch> touches = event.getTouches();
		lastTouchedType = findButtonUnderTouches(touches);
		return false;
	}	
	
	bool touchesEnded( TouchEvent event )
	{
		vector<TouchEvent::Touch> touches = event.getTouches();
		if (lastTouchedType != NO_BUTTON && lastTouchedType == findButtonUnderTouches(touches)) {
			mCallbacksButtonPressed.call(lastTouchedType);
		}
		lastTouchedType = NO_BUTTON;
		return false;
	}
	
	void setPlaying(bool playing) {
		mIsPlaying = playing;
	}
	
	bool isPlaying()
	{
		return mIsPlaying;
	}
	
	void draw( const vector<gl::Texture> &texs, const gl::Texture &sliderBgTex, float y, float playheadPer )
	{
		prevDrawY = y;
		
		gl::color( Color( 0.0f, 0.0f, 0.0f ) );
		gl::drawSolidRect( Rectf(0, y, getWindowWidth(), y + 45.0f ) ); // TODO: make height settable in setup()?

		touchRects.clear();
		touchTypes.clear(); // technically touch types never changes, but whatever
		
		float bWidth = 40.0f;
		float bHeight = 30.0f;
	

		// TODO: make these members?
		float x = getWindowWidth() * 0.5f - bWidth * 1.5f;
		float y1 = y + 5;
		float y2 = y1 + bHeight;
		Rectf prevButton( x,				 y1, x + bWidth,		y2 );
		Rectf playButton( x + bWidth,		 y1, x + bWidth * 2.0f, y2 );
		Rectf nextButton( x + bWidth * 2.0f, y1, x + bWidth * 3.0f, y2 );
		Rectf debugButton( app::getWindowWidth() - x - 60.0f, y1, app::getWindowWidth() - x, y2 );
		
		float sliderWidth	= getWindowWidth();
		float sliderHeight	= 16.0f;
		float bgx1			= 0.0f;
		float bgx2			= sliderWidth;
		float bgy1			= getWindowHeight() - sliderHeight - 2;
		float bgy2			= bgy1 + sliderHeight;
		float fgx1 = bgx1 + 5;
		float fgy1 = bgy1 + 5;
		float fgx2 = fgx1 + ( sliderWidth - 5 ) * playheadPer;
		float fgy2 = bgy2 - 5;
		
		Rectf playheadSliderBg(  bgx1, bgy1, bgx2, bgy2 );
		Rectf playheadSliderBar( fgx1, fgy1, fgx2, fgy2 );
						
		touchRects.push_back( prevButton );
		touchTypes.push_back( PREVIOUS_TRACK );
		touchRects.push_back( playButton );
		touchTypes.push_back( PLAY_PAUSE );
		touchRects.push_back( nextButton );
		touchTypes.push_back( NEXT_TRACK );
		touchRects.push_back( debugButton );
		touchTypes.push_back( DEBUG );
		
		Color yellow( 1.0f, 1.0f, 0.0f );
		Color blue( 0.2f, 0.2f, 0.5f );

		gl::color( lastTouchedType == PREVIOUS_TRACK ? yellow : Color::white() );
		texs[ TEX_PREV ].enableAndBind();
		gl::drawSolidRect( prevButton );
		
		gl::color( lastTouchedType == PLAY_PAUSE ? yellow : Color::white() );
		if (mIsPlaying) texs[ TEX_PAUSE ].enableAndBind();
		else			texs[ TEX_PLAY ].enableAndBind();
		gl::drawSolidRect( playButton );
		
		gl::color( lastTouchedType == NEXT_TRACK ? yellow : Color::white() );
		texs[ TEX_NEXT ].enableAndBind();
		gl::drawSolidRect( nextButton );
		
		gl::color( lastTouchedType == DEBUG ? yellow : Color::white() );
		if( G_DEBUG )	texs[ TEX_DEBUGON ].enableAndBind();
		else			texs[ TEX_DEBUG ].enableAndBind();
		gl::drawSolidRect( debugButton );
		
		gl::color( Color::white() );
		sliderBgTex.enableAndBind();
		gl::drawSolidRect( playheadSliderBg );
		
		glDisable( GL_TEXTURE_2D );
		
		gl::color( ColorA( 0.4353f, 0.7647f, 0.9176f, 0.5f ) );
		gl::drawSolidRect( playheadSliderBar );
	}
	
	// !!! EVENT STUFF (slightly nicer interface for adding listeners)
	template<typename T>
	CallbackId registerButtonPressed( T *obj, bool (T::*callback)(PlayButton) )
	{
		return mCallbacksButtonPressed.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}	
	
private:
					  
	AppCocoaTouch *mApp;
	
	// updated by draw() so that we can test in touchesEnded
	vector<Rectf> touchRects;
	vector<PlayButton> touchTypes;
	PlayButton lastTouchedType;
	float prevDrawY;
	bool mIsPlaying;

	CallbackId cbTouchesBegan;
	CallbackId cbTouchesMoved;
	CallbackId cbTouchesEnded;
			
	PlayButton findButtonUnderTouches(vector<TouchEvent::Touch> touches) {
		for (int j = 0; j < touches.size(); j++) {
			TouchEvent::Touch touch = touches[j];
			if (touch.getY() < prevDrawY) {
				continue;
			}
			for (int i = 0; i < touchRects.size(); i++) {
				Rectf rect = touchRects[i];
				if (rect.contains(touch.getPos())) {
					return touchTypes[i];
				}
			}		
		}		
		return NO_BUTTON;
	}
	
	// !!! EVENT STUFF (keep track of listeners)
	CallbackMgr<bool(PlayButton)> mCallbacksButtonPressed;
	
};