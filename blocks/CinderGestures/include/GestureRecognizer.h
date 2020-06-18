#pragma once

#include "cinder/Cinder.h"
#if defined (CINDER_COCOA_TOUCH)
#include "cinder/app/AppCocoaTouch.h"
#else
#include "cinder/app/AppBasic.h"
#endif

#include <set>

using namespace ci::app;

namespace cinder {

    
class GestureRecognizer {
protected:

#if defined (CINDER_COCOA_TOUCH)
    typedef AppCocoaTouch AppType;
#else
    typedef AppBasic AppType;
#endif
    
    // your Cinder App (usually passed in as 'this' in setup)
    AppType *mApp;
    
    // subclasses must handle basic touch events from mApp...
    virtual bool touchesBegan(TouchEvent event) = 0;
    virtual bool touchesMoved(TouchEvent event) = 0;
    virtual bool touchesEnded(TouchEvent event) = 0;
    
    // for cleanup in destructor/setup, keep callback ids
    CallbackId mTouchesBeganCb;
    CallbackId mTouchesMovedCb;
    CallbackId mTouchesEndedCb;
    
    std::function<bool(TouchEvent::Touch)> mKeepTouchCb;
    std::set<uint32_t>                     mKeptTouchIds;
    
    bool privateTouchesBegan(TouchEvent event){
        // Allow the Recognizer to Keep/Reject touches.
        // If no touches are kept then the event is not fired and we defer to the next listener.
        if(mKeepTouchCb){
            std::vector<TouchEvent::Touch> touchList;
            for(std::vector<TouchEvent::Touch>::const_iterator it = event.getTouches().begin(); it != event.getTouches().end(); ++it){
                if(mKeepTouchCb(*it)){
                    touchList.push_back(*it);
                    mKeptTouchIds.insert(it->getId());
                }
            }
            if(touchList.size() > 0)
                // KEM FIX
                return  touchesBegan(TouchEvent(getWindow(), touchList));
//                return false; //touchesBegan(TouchEvent(touchList));
            
            return false;
        }
        return touchesBegan(event);
    }
    
    bool privateTouchesMoved(TouchEvent event){
        if(mKeepTouchCb){
            std::vector<TouchEvent::Touch> touchList;
            for(std::vector<TouchEvent::Touch>::const_iterator it = event.getTouches().begin(); it != event.getTouches().end(); ++it){
                if(mKeptTouchIds.count(it->getId()) != 0)
                    touchList.push_back(*it);
            }
            if(touchList.size() > 0)
                // KEMAL - hacked old mechanism, delete old stuff?
                return touchesMoved(TouchEvent(getWindow(), touchList));
//                touchesMoved(TouchEvent(touchList));
            return false;
        }
        return touchesMoved(event);
    }
    
    bool privateTouchesEnded(TouchEvent event){
        if(mKeepTouchCb){
            std::vector<TouchEvent::Touch> touchList;
            for(std::vector<TouchEvent::Touch>::const_iterator it = event.getTouches().begin(); it != event.getTouches().end(); ++it){
                if(mKeptTouchIds.count(it->getId()) != 0){
                    touchList.push_back(*it);
                    mKeptTouchIds.erase(it->getId());
                }
            }
            if(touchList.size() > 0)
                // KEMAL - hacked old mechanism, delete old stuff?
                return  touchesEnded(TouchEvent(getWindow(), touchList));
//                return false; //touchesEnded(TouchEvent(touchList));
            return false;
        }
        return touchesEnded(event);
    }
    
    void registerTouchCallbacks(){
        // KEMAL - Probably dont need this anymore
        if(mApp){
//            mTouchesBeganCb = mApp->registerTouchesBegan(this, &GestureRecognizer::privateTouchesBegan);
//            mTouchesMovedCb = mApp->registerTouchesMoved(this, &GestureRecognizer::privateTouchesMoved);
//            mTouchesEndedCb = mApp->registerTouchesEnded(this, &GestureRecognizer::privateTouchesEnded);
        }
    }
    void unregisterTouchCallbacks(){
        // KEMAL - Probably don't need this anymore either.  Need to check for multiple registered callbacks though.
        if(mApp){
//            mApp->unregisterTouchesBegan(mTouchesBeganCb);
//            mApp->unregisterTouchesMoved(mTouchesMovedCb);
//            mApp->unregisterTouchesEnded(mTouchesEndedCb);
        }
    }
            
public:		

    GestureRecognizer() : mApp(0) {}

    ~GestureRecognizer(){
        unregisterTouchCallbacks();
    }
    
    virtual void init(AppType *app)
    {
        unregisterTouchCallbacks();
        mApp = app;
        registerTouchCallbacks();
    }
    
    template<typename T>
    void setKeepTouchCallback(T *obj, bool(T::*callback)(TouchEvent::Touch)){
        mKeepTouchCb = std::bind1st(std::mem_fun(callback), obj);
    }

};
	
    
} // namespace cinder::app
