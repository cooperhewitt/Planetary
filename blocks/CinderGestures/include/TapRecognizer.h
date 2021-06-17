/*
 *  TapAnalyzer.h
 *  CinderGestures
 *
 *  Created by Tom Carden on 2/8/11.
 *  Copyright 2011 Bloom Studio Inc. All rights reserved.
 */

#pragma once

#include "cinder/Timer.h"
#include "GestureRecognizer.h"
#include "DoubleTapEvent.h"
#include "SingleTapEvent.h"

namespace cinder {

	class TapRecognizer : public GestureRecognizer {

	protected:
				
		// to time how long since the last tap
		Timer timer;
		
		// keep track of the touches from the last touch ended event
		bool wasSingleTapped;
		Vec2f prevTouchPos;
		
		// override touch end handling from GestureAnalyzer base...
		bool touchesEnded(TouchEvent event);
		bool touchesBegan(TouchEvent event) { return false; } // ignore these for now
		bool touchesMoved(TouchEvent event) { return false; } // TODO: should they be optional not required?
		
		// keep track of listeners for our own events...
		CallbackMgr<bool(DoubleTapEvent)> mCallbacksDoubleTap;
		CallbackMgr<bool(SingleTapEvent)> mCallbacksSingleTap;

	public:		
		
		void init(AppType *app) {
			GestureRecognizer::init(app);
			wasSingleTapped = false;
			prevTouchPos = Vec2f(0,0);
		}
		
		void update() {
			// TODO: it's only a single tap if the touch didn't move (much) between touchStart and touchEnd
//			std::cout << "TapRecognizer::update: wasSingleTapped=" << wasSingleTapped << " dt=" << timer.getSeconds() << std::endl;
			if (wasSingleTapped && timer.getSeconds() > 0.25f) {
				mCallbacksSingleTap.call( SingleTapEvent( prevTouchPos, getElapsedSeconds()-timer.getSeconds() ) );
				prevTouchPos = Vec2f(0,0);
				wasSingleTapped = false;
			}
		}
		
		/* 
		 * these registerXXX methods are convenience functions 
		 * so that you can type:
		 *
		 *     mTapRecognizer.registerDoubleTap(this, &MyApp::doubleTap);
		 *
		 * rather than:
		 *
		 *     mTapRecognizer.mCallbacksDoubleTap.registerCb(std::bind1st(std::mem_fun(&MyApp::doubleTap), this));
		 *     
		 * where doubleTap is a function in your app:
		 *
		 *     bool MyApp::doubleTap(DoubleTapEvent event) { ... }
		 *
		 */
		template<typename T>
		CallbackId registerDoubleTap( T *obj, bool (T::*callback)(DoubleTapEvent) ){
			return mCallbacksDoubleTap.registerCb(std::bind1st(std::mem_fun(callback), obj));
		}

		template<typename T>
		CallbackId registerSingleTap( T *obj, bool (T::*callback)(SingleTapEvent) ){
			return mCallbacksSingleTap.registerCb(std::bind1st(std::mem_fun(callback), obj));
		}
		
	};
	
	bool TapRecognizer::touchesEnded(TouchEvent event)
	{
		std::vector<TouchEvent::Touch> touches = event.getTouches();
		bool doubleTapped = false;
		if (wasSingleTapped && timer.getSeconds() > 0.05f && timer.getSeconds() < 0.25f) {
			if (prevTouchPos.distance( touches[0].getPos()) < 25.0f ) {
				doubleTapped = true;
				mCallbacksDoubleTap.call( DoubleTapEvent( prevTouchPos, getElapsedSeconds()-timer.getSeconds() ) );
			}
		}
		if (doubleTapped) {
			prevTouchPos = Vec2f( 0, 0 );
			wasSingleTapped = false;
		}
		else {
			prevTouchPos = touches[0].getPos();
			wasSingleTapped	= true;
			timer.start();
		}
		return false;
	}
	
} // namespace cinder
