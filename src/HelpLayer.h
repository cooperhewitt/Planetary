/*
 *  UiLayer.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2013 Smithsonian Institution. All rights reserved.
 *
 */

#pragma once

#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include "cinder/Font.h"
#include "cinder/Rect.h"
#include "cinder/app/TouchEvent.h"
#include "BloomNode.h"

class HelpLayer : public BloomNode {
 public:
	
    HelpLayer() {};
	~HelpLayer() {};
    
	void	setup( const ci::Font &smallFont, const ci::Font &bigFont, const ci::Font &bigBoldFont );
	
    bool	touchBegan( ci::app::TouchEvent::Touch touch );
	bool	touchEnded( ci::app::TouchEvent::Touch touch );

	void	update();
	void	draw();
	
    void    show( bool show = true, bool animate = true );
    void    hide( bool animate = true ) { show( false, animate ); }
    void    toggle() { show( !isShowing() ); }
    bool    isShowing() { return mShowing; }
    
    float   getHeight() { return mBgRect.getHeight() + mCurrentY; }
        
 private:
    
    void updateRect( ci::Rectf *rect, const std::wstring &fullStr, const std::wstring &rectStr, const std::vector<std::pair<uint16_t,ci::Vec2f> > &glyphPositions );
    
    // control visibility, animation
    bool mShowing, mAnimating;
    float mCurrentY, mTargetY;
    
    // fonts
    ci::Font mBigFont, mBigBoldFont, mSmallFont;

    // textures
    ci::gl::Texture mHeadingTex, mBodyTex;
    
    // dimensions and positions
    ci::Vec2f mInterfaceSize, mHeadingPos, mBodyPos;
    ci::Rectf mBgRect;
    
    // hit rects for links:
    ci::Rectf mCinderRect, mWebRect, mEmailRect;
};

