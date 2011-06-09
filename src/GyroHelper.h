//
//  GyroHelper.h
//  Kepler
//
//  Created by Tom Carden on 6/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#import <CoreMotion/CoreMotion.h>
#include "cinder/Quaternion.h"

class GyroHelper {
public:
    
    GyroHelper();
    ~GyroHelper();
    
    void setup();
    void update();
    
    ci::Quatf getQuat() { return mQuat; }
private:
    ci::Quatf mQuat;
    bool mActive;
    
	// Objective C 
    CMMotionManager *motionManager;
    CMAttitude *referenceAttitude;
    
};