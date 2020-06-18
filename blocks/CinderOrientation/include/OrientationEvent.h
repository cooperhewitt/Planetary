/*
 Copyright (c) 2011, The Cinder Project: http://libcinder.org
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <ostream>
#include "cinder/app/Event.h"
#include "Orientation.h"

namespace cinder { namespace app {
    
    class OrientationEvent : public Event {
    public:
        
        OrientationEvent( const Orientation deviceOrientation, 
                         const Orientation prevDeviceOrientation, 
                         const Orientation interfaceOrientation, 
                         const Orientation prevInterfaceOrientation ):
        Event(), 
        mDeviceOrientation( deviceOrientation ), 
        mPrevDeviceOrientation( prevDeviceOrientation ),
        mInterfaceOrientation( interfaceOrientation ), 
        mPrevInterfaceOrientation( prevInterfaceOrientation )
        {
        }
        
        Orientation getDeviceOrientation() const { return mDeviceOrientation; }
        Orientation getPrevDeviceOrientation() const { return mPrevDeviceOrientation; }
        
        Orientation getInterfaceOrientation() const { return mInterfaceOrientation; }
        Orientation getPrevInterfaceOrientation() const { return mPrevInterfaceOrientation; }
        
    private:
        
        const Orientation mDeviceOrientation, mPrevDeviceOrientation;
        const Orientation mInterfaceOrientation, mPrevInterfaceOrientation;
        
    };
    
    // For convenience only
    inline std::ostream& operator<<( std::ostream &out, const OrientationEvent &event )
    {
        out << "OrientationEvent (device: " 
        << event.getPrevDeviceOrientation() 
        << " -> " << event.getDeviceOrientation() 
        << "; interface: "
        << event.getPrevInterfaceOrientation() 
        << " -> " << event.getInterfaceOrientation()
        << ")";
        return out;
    }
    
    // same behaviour as iOS's UIDeviceOrientationIsValidInterfaceOrientation() macro
    inline bool isValidInterfaceOrientation(const Orientation &orientation)
    {
        return (orientation == PORTRAIT_ORIENTATION || orientation == UPSIDE_DOWN_PORTRAIT_ORIENTATION || orientation == LANDSCAPE_LEFT_ORIENTATION || orientation == LANDSCAPE_RIGHT_ORIENTATION);
    }
    
    // same as iOS's UIDeviceOrientationIsPortrait() macro
    inline bool isPortaitOrientation(const Orientation &orientation)
    {
        return (orientation == PORTRAIT_ORIENTATION || orientation == UPSIDE_DOWN_PORTRAIT_ORIENTATION);
    }
    
    // same as iOS's UIDeviceOrientationIsLandscape() macro
    inline bool isLandscapeOrientation(const Orientation &orientation)
    {
        return (orientation == LANDSCAPE_LEFT_ORIENTATION || orientation == LANDSCAPE_RIGHT_ORIENTATION);
    }
    
} } // namespace cinder::app