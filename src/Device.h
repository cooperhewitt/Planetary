//
//  Device.h
//  Kepler
//
//  Created by Tom Carden on 6/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include <sys/sysctl.h>

namespace bloom {

    bool isIpad2()
    {
        // http://stackoverflow.com/questions/448162/determine-device-iphone-ipod-touch-with-iphone-sdk/1561920#1561920
        // http://www.clintharris.net/2009/iphone-model-via-sysctlbyname/
        size_t size;
        sysctlbyname("hw.machine", NULL, &size, NULL, 0);  
        char *machine = new char[size];
        sysctlbyname("hw.machine", machine, &size, NULL, 0);    
        bool isIpad2 = (strcmp("iPad1,1",machine) != 0);
        delete[] machine;

        return isIpad2;
    }

}