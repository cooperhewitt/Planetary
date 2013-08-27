//
//  StringHelpers.h
//  Kepler
//
//  Created by Tom Carden on 8/31/11.
//  Copyright 2013 Smithsonian Institution. All rights reserved.
//

#ifndef Kepler_StringHelpers_h
#define Kepler_StringHelpers_h

#include <string>

namespace bloom {
    // because wstring has 32-byte chars on iOS
    std::string wstringToUtf8(const std::wstring &s);
    std::wstring utf8ToWstring(const std::string &s);
}

#endif
