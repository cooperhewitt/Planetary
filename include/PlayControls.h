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

// TODO: should enum be a member of PlayControls?
enum PlayButton { NO_BUTTON, PLAY_PAUSE, NEXT_TRACK, PREVIOUS_TRACK };

class PlayControls {
public:

	void setup( AppCocoaTouch *app )
	{
		// TODO: remember app and callback ID so we can tidy up after ourselves?
		app->registerTouchesEnded( this, &PlayControls::touchesEnded );
		app->registerTouchesMoved( this, &PlayControls::touchesMoved );
		app->registerTouchesBegan( this, &PlayControls::touchesBegan );
		// TODO: listen to touches began/moved so we can do pressed states of buttons
		lastTouchedType = NO_BUTTON;
	}
	
	void update()
	{
		// TODO: update anything time based here, e.g. elapsed time of track playing
		// or e.g. button animation
	}

	bool touchesBegan( TouchEvent event )
	{
		vector<TouchEvent::Touch> touches = event.getTouches();
		lastTouchedType = findButtonUnderTouches(touches);
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
		if (lastTouchedType == findButtonUnderTouches(touches)) {
			mCallbacksButtonPressed.call(lastTouchedType);
		}
		lastTouchedType = NO_BUTTON;
		return false;
	}
	
	void draw( float y )
	{
		
		gl::color( Color::white() );
		gl::drawSolidRect( Rectf(0, y, getWindowWidth(), y + 40.0f ) ); // TODO: make height settable in setup()?

		touchRects.clear();
		touchTypes.clear(); // technically touch types never changes, but whatever

		Rectf prevButton( 10, y+10, 10+20, y+10+20 );
		Rectf playButton( 40, y+10, 40+20, y+10+20 );
		Rectf nextButton( 70, y+10, 70+20, y+10+20 );
		
		touchRects.push_back( prevButton );
		touchTypes.push_back( PREVIOUS_TRACK );
		touchRects.push_back( playButton );
		touchTypes.push_back( PLAY_PAUSE );
		touchRects.push_back( nextButton );
		touchTypes.push_back( NEXT_TRACK );
		
		Color yellow( 1.0f, 1.0f, 0.0f );
		
		// TODO: I'm guessing textures go here?
		gl::color( lastTouchedType == PREVIOUS_TRACK ? yellow : Color( 1.0f, 0.0f, 0.0f ) );
		gl::drawSolidRect( prevButton );		
		gl::color( lastTouchedType == PLAY_PAUSE ? yellow : Color( 0.0f, 1.0f, 0.0f ) );
		gl::drawSolidRect( playButton );		
		gl::color( lastTouchedType == NEXT_TRACK ? yellow : Color( 0.0f, 0.0f, 1.0f ) );
		gl::drawSolidRect( nextButton );
	}
	
	// !!! EVENT STUFF (slightly nicer interface for adding listeners)
	template<typename T>
	CallbackId registerButtonPressed( T *obj, bool (T::*callback)(PlayButton) )
	{
		return mCallbacksButtonPressed.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}	
	
private:
					  
	// updated by draw() so that we can test in touchesEnded
	vector<Rectf> touchRects;
	vector<PlayButton> touchTypes;
	PlayButton lastTouchedType;

	PlayButton findButtonUnderTouches(vector<TouchEvent::Touch> touches) {
		for (int j = 0; j < touches.size(); j++) {
			TouchEvent::Touch touch = touches[j];
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