#pragma once

#include "cinder/Matrix.h"

#include "PhasedGestureRecognizer.h"
#include "PinchEvent.h"

#include <vector>

namespace cinder {


class PinchRecognizer : public PhasedGestureRecognizer<PinchEvent> {
protected:

    std::pair<PinchEvent::Touch, PinchEvent::Touch> mTouchPair;
    
    uint32_t mLastTouchId;
    
    // are we pinching yet?
    bool mIsPinching;
    
   
    
    void beginPinch(const TouchEvent::Touch &t1, const TouchEvent::Touch &t2)
    {
        mLastTouchId = 0;
        mIsPinching = true;
        
        PinchEvent::Touch pt1 = { t1.getId(), t1.getPos(), t1.getPos(), t1.getPos() };
        PinchEvent::Touch pt2 = { t2.getId(), t2.getPos(), t2.getPos(), t2.getPos() };
        
        mTouchPair = pt1.mId < pt2.mId ? std::make_pair(pt1, pt2) : std::make_pair(pt2, pt1);
        
        mCallbacksBegan.call(PinchEvent(mTouchPair, mApp->getWindowSize()));
    }

public:
    
    // handle basic touch events from mApp...
       bool touchesBegan(TouchEvent event);
       bool touchesMoved(TouchEvent event);
       bool touchesEnded(TouchEvent event);
    
    PinchRecognizer()
    : PhasedGestureRecognizer<PinchEvent>(), mIsPinching(false), mLastTouchId(0)
    {
    }
    
    void init(AppType *app)
    {
        PhasedGestureRecognizer<PinchEvent>::init(app);
        mIsPinching = false;
    }
    
    void rejectPinch()
    {
        mIsPinching = false;
    }

};


bool PinchRecognizer::touchesBegan(TouchEvent event)
{
    const std::vector<TouchEvent::Touch> &touches = event.getTouches();
    if(!mIsPinching){
        if(touches.size() == 1){
            if(mLastTouchId){
                const vector<TouchEvent::Touch> active = mApp->getActiveTouches();
                for(vector<TouchEvent::Touch>::const_iterator it = active.begin(); it != active.end(); ++it){
                    if(it->getId() == mLastTouchId){
                        beginPinch(*it, touches[0]);
                        mLastTouchId = 0;
                        return true;
                    }
                }
            }
            else{
                mLastTouchId = touches[0].getId();
            }
        }
        else if(touches.size() == 2){
            beginPinch(touches[0], touches[1]);
            return true;
        }
    }
    return false;
}

bool PinchRecognizer::touchesMoved(TouchEvent event)
{
    if(mIsPinching){
        bool shouldFireEvent = false;
        const vector<TouchEvent::Touch> &touches = event.getTouches();
        for(vector<TouchEvent::Touch>::const_iterator it = touches.begin(); it != touches.end(); ++it){
            if(it->getId() == mTouchPair.first.mId){
                mTouchPair.first.mPos = it->getPos();
                shouldFireEvent = true;
            }
            else if(it->getId() == mTouchPair.second.mId){
                mTouchPair.second.mPos = it->getPos();
                shouldFireEvent = true;
            }
        }
        if(shouldFireEvent){
            mCallbacksMoved.call(PinchEvent(mTouchPair, mApp->getWindowSize()));
            mTouchPair.first.mPosPrev  = mTouchPair.first.mPos;
            mTouchPair.second.mPosPrev = mTouchPair.second.mPos;
        }
    }
    return false;
}

bool PinchRecognizer::touchesEnded(TouchEvent event)
{
    if(mIsPinching){
        for(vector<TouchEvent::Touch>::const_iterator it = event.getTouches().begin(); it != event.getTouches().end(); ++it){
            if(it->getId() == mTouchPair.first.mId || it->getId() == mTouchPair.second.mId){
                mCallbacksEnded.call(PinchEvent(mTouchPair, mApp->getWindowSize()));
                mIsPinching = false;
                break;
            }
        }
    }
    else if(mLastTouchId){
        for(vector<TouchEvent::Touch>::const_iterator it = event.getTouches().begin(); it != event.getTouches().end(); ++it){
            if(it->getId() == mLastTouchId){
                mLastTouchId = 0;
                break;
            }
        }
    }
    return false;
}

    
} // namespace cinder
