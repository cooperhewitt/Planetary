/*
 *  PlayControls.h
 *  Kepler
 *
 *  Created by Tom Carden on 3/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

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
	
	void draw( const vector<gl::Texture> &texs, float y, float playheadPer )
	{
		prevDrawY = y;
		
		gl::color( Color( 0.0f, 0.0f, 0.0f ) );
		gl::drawSolidRect( Rectf(0, y, getWindowWidth(), y + 45.0f ) ); // TODO: make height settable in setup()?

		touchRects.clear();
		touchTypes.clear(); // technically touch types never changes, but whatever
		
		float bWidth = 40.0f;
		float bHeight = 30.0f;
	

		// TODO: make these members?
		float x = 18.0f;
		Rectf prevButton( x,				 y+7, x + bWidth,		 y+7+bHeight );
		Rectf playButton( x + bWidth,		 y+7, x + bWidth * 2.0f, y+7+bHeight );
		Rectf nextButton( x + bWidth * 2.0f, y+7, x + bWidth * 3.0f, y+7+bHeight );
		Rectf debugButton( app::getWindowWidth() - x - 60.0f, y+7, app::getWindowWidth() - x, y+7+bHeight );
		
		float sliderWidth		= 200.0f;
		float sliderHeight		= 15.0f;
		float windowHalfWidth	= app::getWindowWidth() * 0.5f;
		float x1 = windowHalfWidth - sliderWidth;
		float x2 = windowHalfWidth;
		float x3 = x1 + sliderWidth * playheadPer;
		Rectf playheadSliderBg(  x1,	 y+14, x2,		y+14+sliderHeight );
		Rectf playheadSliderBar( x1 + 1, y+15, x3 - 1,  y+13+sliderHeight );
						
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
		
		glDisable( GL_TEXTURE_2D );
		gl::color( ColorA( 0.1f, 0.2f, 0.5f, 0.25f ) );
		gl::drawSolidRect( playheadSliderBg );
		
		gl::color( ColorA( 0.4353f, 0.7647f, 0.9176f, 1.0f ) );
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