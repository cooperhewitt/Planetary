/*
 *  UiLayer.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2013 Smithsonian Institution. All rights reserved.
 *
 */

#include "HelpLayer.h"
#include "cinder/Text.h"
#include "cinder/gl/gl.h"
#include "cinder/Color.h"
#include "cinder/Utilities.h" // for launchWebBrowser()
#include "Globals.h"          // for color constants
#include "BloomScene.h"       // for getRoot() functionality (FIXME)
#include "StringHelpers.h"    // for string/wstring UTF8/UTF32 conversion

using namespace ci;
using namespace ci::app;
using namespace std;

void HelpLayer::setup( const ci::Font &smallFont, const ci::Font &bigFont, const ci::Font &bigBoldFont )
{
	mSmallFont = smallFont;
	mBigFont = bigFont;
	mBigBoldFont = bigBoldFont;
    mInterfaceSize = Vec2f::zero();

    ///////////
        
    // use TextLayouts for multicolored text runs...
    
    TextLayout layout;	
    layout.setColor( Color::white() );
    layout.setFont( mBigBoldFont );
    layout.addLine( "Planetary " );
    layout.setFont( mBigFont );
    layout.append( "Remastered" );
    mHeadingTex = layout.render( true, false );    
    
    layout = TextLayout();	
    layout.setColor( BRIGHT_BLUE );
    layout.setFont( mSmallFont );
    layout.append( "Original by Bloom Studio. Remastered in 2020 by @kemalenver. " );
    layout.setColor( BRIGHT_BLUE );
    layout.append("Get in touch: ");
    layout.setColor( Color::white() );
    layout.append("Visit the website");
    layout.setColor( BRIGHT_BLUE );
    layout.append(" or ");
    layout.setColor( Color::white() );
    layout.append("tweet @kemalenver");
    layout.setColor( BRIGHT_BLUE );
    layout.append("." );
    mBodyTex = layout.render( true, false ); 
    
    ///////////////////////
    
    // calculate layout...

    const Vec2f padding(18,15);
    const float w = mInterfaceSize.x; // initally 0, see update() for correct value
    const float h = mHeadingTex.getHeight() + mBodyTex.getHeight() + (padding.y * 2);
    
    mHeadingPos = padding;
    mBodyPos = padding + Vec2f(0,mHeadingTex.getHeight());
    mBgRect = Rectf( 0, 0, w, h );    
    
    // and animation...
    mAnimating = false;
    mShowing = false;
    mCurrentY = -h;
    mTargetY = -h;
    
    ///////////////////////
    
    // use TextBox to measure glyphs and generate hit areas...
    
    // make a wide string for counting characters (because © is double-wide)
    wstring bodyText = L"Original by Bloom Studio. Remastered in 2020 by @kemalenver. Get in touch: Visit the website or tweet @kemalenver.";
    
    // make a normal string to pass to cinder text routines
    string strBodyText = bloom::wstringToUtf8( bodyText );
        
    // TODO: use some sort of markup so that text doesn't have to be repeated
    // and then we can generate hit rects lazily on demand
    
    TextBox box;
    box.setFont( mSmallFont );
    box.setText( strBodyText );

    std::vector<std::pair<uint16_t,Vec2f> > glyphPositions = box.measureGlyphs();
    
    updateRect( &mWebRect, bodyText, L"Visit the website", glyphPositions );
    updateRect( &mEmailRect, bodyText, L"tweet @kemalenver", glyphPositions );
    
    mWebRect.offset( mBodyPos );
    mEmailRect.offset( mBodyPos );    
}

void HelpLayer::updateRect( Rectf *rect, const std::wstring &fullStr, const std::wstring &rectStr, const std::vector<std::pair<uint16_t,Vec2f> > &glyphPositions )
{
    const size_t startIndex = fullStr.find(rectStr);
    const size_t endIndex = startIndex + rectStr.size();
    
//    wcout << rectStr << L" length is: " << rectStr.size() << endl; 
    
    // glyph the first
    uint16_t glyph = glyphPositions[startIndex].first;
    Vec2f pos = glyphPositions[startIndex].second;
    Rectf bbox = mSmallFont.getGlyphBoundingBox(glyph);
    // account for flipped axis:
    bbox.y1 *= -1.0f;
    bbox.y2 *= -1.0f;
    Rectf bounds = bbox.getOffset(pos);
    rect->set(bounds.x1, bounds.y1, bounds.x2, bounds.y2);
    
    // glyph the rest
    for (int i = startIndex + 1; i < endIndex; i++) {
        glyph = glyphPositions[i].first;
        pos = glyphPositions[i].second;
        bbox = mSmallFont.getGlyphBoundingBox(glyph);
        // account for flipped axis:
        bbox.y1 *= -1.0f;
        bbox.y2 *= -1.0f;
        rect->include( bbox.getOffset(pos) );
    }
}
 
bool HelpLayer::touchBegan( TouchEvent::Touch touch )
{
	return mBgRect.contains( globalToLocal( touch.getPos() ) );
}

bool HelpLayer::touchEnded( TouchEvent::Touch touch )
{
    Vec2f pos = globalToLocal( touch.getPos() );
    
    // TODO: should we use a callback for these and handle the actions in the main app?
	
    Url mailToLink( "https://www.twitter.com/kemalenver" );
    Url planetaryWebsite( "https://www.kemalenver.com" );
    
    const Vec2f linkPadding(5,5);
    
    if( mEmailRect.inflated( linkPadding ).contains( pos ) ){
        launchWebBrowser( mailToLink );
    } else if( mWebRect.inflated( linkPadding ).contains( pos ) ){
        launchWebBrowser( planetaryWebsite );
    }
	
	return mBgRect.contains( pos );
}

void HelpLayer::show( bool show, bool animate )
{
    mShowing = show; 
    mAnimating = animate; 
    if (mShowing) {
        mTargetY = 0.0f;
        // ensure we're visible
        setVisible(true);
    }
    else {
        mTargetY = -mBgRect.getHeight();
        // don't set invisible yet, do that in update when we're done animating
    }
}

void HelpLayer::update()
{
    mInterfaceSize = getRoot()->getInterfaceSize();
    mBgRect.x2 = mInterfaceSize.x;

    if (mCurrentY != mTargetY) {
        if (mAnimating) {
            mCurrentY += (mTargetY - mCurrentY) * 0.2f;
            if (fabs(mCurrentY - mTargetY) < 0.01f) {
                mCurrentY = mTargetY;
            }
        }
        else {
            mCurrentY = mTargetY;
        }
        setTransform( Matrix44f::createTranslation( Vec3f(0, round(mCurrentY), 0) ) );
    }
    else {
        if (!mShowing) {
            setVisible(false);
        }
    }
}

void HelpLayer::draw()
{	
    gl::color( Color::black() );
    gl::drawSolidRect( mBgRect );
    
    const float dragAlphaPer = min( pow( 1.0f - (fabs(mCurrentY) / mBgRect.getHeight()), 2.0f ), 1.0f ); 
    gl::color( Color( dragAlphaPer, dragAlphaPer, dragAlphaPer ) );
    gl::draw( mHeadingTex, mHeadingPos );
    gl::draw( mBodyTex, mBodyPos );
    
    gl::color( ColorA(BRIGHT_BLUE, 0.15f * dragAlphaPer) );
    gl::drawLine( mBgRect.getLowerLeft(), mBgRect.getLowerRight() );

//    gl::color( Color::white() );
//    gl::drawStrokedRect( mCinderRect );
//    gl::drawStrokedRect( mWebRect );
//    gl::drawStrokedRect( mEmailRect );

    glPushMatrix();
    gl::translate( Vec2f(0, 2.0f) );
    gl::color( ColorA(dragAlphaPer, dragAlphaPer, dragAlphaPer, 0.5f) );
    gl::drawLine( mCinderRect.getLowerLeft(), mCinderRect.getLowerRight() );
    gl::drawLine( mWebRect.getLowerLeft(), mWebRect.getLowerRight() );
    gl::drawLine( mEmailRect.getLowerLeft(), mEmailRect.getLowerRight() );
    glPopMatrix();
}
