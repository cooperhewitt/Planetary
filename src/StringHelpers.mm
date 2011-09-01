//
//  StringHelpers.mm
//  Kepler
//
//  Created by Tom Carden on 8/31/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include "StringHelpers.h"

using namespace std;

namespace bloom {

    // with thanks to Arial Malka
    // http://forum.libcinder.org/#Topic/23286000000959252
    
    string wstringToUtf8(const wstring &s)
    {
        NSString *utf32NS = [[NSString alloc] initWithBytes: s.data() 
                                                     length: s.size() * sizeof(wchar_t)
                                                   encoding: NSUTF32LittleEndianStringEncoding];
        string utf8 = string([utf32NS UTF8String]);
        [utf32NS release];
        return utf8;
    }

    wstring utf8ToWstring(const string &s)
    {
        NSString *utf8NS = [[NSString alloc] initWithUTF8String: s.c_str()];
        wstring utf32 = wstring(reinterpret_cast<const wchar_t*>([utf8NS cStringUsingEncoding:NSUTF32LittleEndianStringEncoding]));
        [utf8NS release];
        return utf32;
    }

}