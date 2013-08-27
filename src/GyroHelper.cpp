//
//  GyroHelper.cpp
//  Kepler
//
//  Created by Tom Carden on 6/9/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#include "GyroHelper.h"

GyroHelper::GyroHelper()
{
    mActive = false; // call setup() to activate
}

GyroHelper::~GyroHelper()
{
    if (mActive) {
        [motionManager stopDeviceMotionUpdates];
        [motionManager release];
        [referenceAttitude release];    
    }
}

void GyroHelper::setup()
{
    motionManager = [[CMMotionManager alloc] init];
    [motionManager startDeviceMotionUpdates];
    
    CMDeviceMotion *dm = motionManager.deviceMotion;
    referenceAttitude = [dm.attitude retain];

    mActive = true;
}

void GyroHelper::update()
{
    if (!mActive) return;
    
	CMQuaternion quat;
	
    CMDeviceMotion *deviceMotion = motionManager.deviceMotion;		
    CMAttitude *attitude = deviceMotion.attitude;
    
    // If we have a reference attitude, multiply attitude by its inverse
    // After this call, attitude will contain the rotation from referenceAttitude
    // to the current orientation instead of from the fixed reference frame to the
    // current orientation
	/*
     if (referenceAttitude != nil) {
     [attitude multiplyByInverseOfAttitude:referenceAttitude];
     }
     */
    
	quat = attitude.quaternion;
    
    // flip w around so quat can be applied to camera eye/up
    // flip y around because who knows why
	mQuat.set( -quat.w, quat.x, -quat.y, quat.z );
}