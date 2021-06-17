#pragma once

#include "cinder/Cinder.h"
#include "cinder/Function.h"
#include "Orientation.h"
#include "OrientationEvent.h"

namespace cinder { namespace app {

    class OrientationHelper {
    public:
        
        // TODO: should this be a singleton and give access to an instance?
        void setup();
        Orientation getInterfaceOrientation() const { return mInterfaceOrientation; }
        Orientation getDeviceOrientation() const { return mDeviceOrientation; }
        
        //! Registers a callback for orientationChanged events. Returns a unique identifier which can be used as a parameter to unregisterOrientationChanged().
        CallbackId		registerOrientationChanged( std::function<bool (OrientationEvent)> callback ) { return mCallbacksOrientationChanged.registerCb( callback ); }
        //! Registers a callback for orientationChanged events. Returns a unique identifier which can be used as a parameter to unregisterOrientationChanged().
        template<typename T>
        CallbackId		registerOrientationChanged( T *obj, bool (T::*callback)(OrientationEvent) ) { return mCallbacksOrientationChanged.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
        //! Unregisters a callback for orientationChanged events.
        void			unregisterOrientationChanged( CallbackId id ) { mCallbacksOrientationChanged.unregisterCb( id ); }        

        // should be private, but is public so it can be accessed from setupNotifications
        void onOrientationChanged( const Orientation &orientation );

    private:

        // is a friend method so that Objective C's nice closure/blocks work
        friend void	setupNotifications( OrientationHelper *helper );

        CallbackMgr<bool (OrientationEvent)> mCallbacksOrientationChanged;
        Orientation mDeviceOrientation, mInterfaceOrientation;
    };
    
} }