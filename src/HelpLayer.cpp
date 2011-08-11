/*
 *  UiLayer.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "HelpLayer.h"
#include "cinder/Text.h"
#include "cinder/gl/gl.h"
#include "cinder/Color.h"
#include "cinder/Utilities.h" // for launchWebBrowser()
#include "CinderFlurry.h"     // for logging
#include "Globals.h"          // for color constants
#include "BloomScene.h"       // for getRoot() functionality (FIXME)

using namespace pollen::flurry;
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
    layout.addLine( "Planetary" );
    layout.setFont( mBigFont );
    layout.append( " by " );
    layout.setFont( mBigBoldFont );
    layout.append( "bloom." );
    mHeadingTex = layout.render( true, false );    
    
    layout = TextLayout();	
    layout.setColor( BRIGHT_BLUE );
    layout.setFont( mSmallFont );
    layout.addLine( "© 2011 Bloom Studio, Inc. All Rights Reserved. Made with " );
    layout.setColor( Color::white() );
    layout.append("Cinder");
    layout.setColor( BRIGHT_BLUE );
    layout.append(". Questions? Comments? ");
    layout.setColor( Color::white() );
    layout.append("Visit the website");
    layout.setColor( BRIGHT_BLUE );
    layout.append(" or ");
    layout.setColor( Color::white() );
    layout.append("send us an email");
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
    
    ///////////////////////
    
    // use TextBox to measure glyphs and generate hit areas...
    
    // make a wide string for counting characters (because © is double-wide)
    wstring bodyText = L"© 2011 Bloom Studio, Inc. All Rights Reserved. Made with Cinder. Questions? Comments? Visit the website or send us an email.";
    
    // make a normal string to pass to cinder
    // TODO: find a reliable way to convert this automatically, the std::copy routine below is screwy
    string strBodyText = "© 2011 Bloom Studio, Inc. All Rights Reserved. Made with Cinder. Questions? Comments? Visit the website or send us an email.";
//    std::copy(bodyText.begin(), bodyText.end(), std::back_inserter(strBodyText));
    
    // TODO: use some sort of markup so that text doesn't have to be repeated
    // and then we can generate hit rects lazily on demand
    
    TextBox box;
    box.setFont( mSmallFont );
    box.setText( strBodyText );

    std::vector<std::pair<uint16_t,Vec2f> > glyphPositions = box.measureGlyphs();
    
//    cout << glyphPositions.size() << " glyph positions available" << endl;
//    cout << bodyText.size() << " characters in bodyText" << endl;
//    cout << strBodyText.size() << " characters in strBodyText" << endl;
    
    updateRect( &mCinderRect, bodyText, L"Cinder", glyphPositions );
    updateRect( &mWebRect, bodyText, L"Visit the website", glyphPositions );
    updateRect( &mEmailRect, bodyText, L"send us an email", glyphPositions );
    
    mCinderRect.offset( mBodyPos );
    mWebRect.offset( mBodyPos );
    mEmailRect.offset( mBodyPos );
    
//    const Vec2f linkPadding(5,5);
//    inflate( &mCinderRect, linkPadding );
//    inflate( &mWebRect, linkPadding );
//    inflate( &mEmailRect, linkPadding );
}

void HelpLayer::inflate( ci::Rectf *rect, const ci::Vec2f &amount )
{
    rect->canonicalize();
    rect->x1 -= amount.x;
    rect->x2 += amount.x;
    rect->y1 -= amount.y;
    rect->y2 += amount.y;
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
	
    Url mailToLink( "mailto:planetary@bloom.io?subject=Planetary feedback" );
    Url planetaryWebsite( "http://planetary.bloom.io" );
//    Url bloomWebsite( "http://bloom.io" );
    Url cinderWebsite( "http://libcinder.org" );
    
    if( mEmailRect.contains( pos ) ){
        Flurry::getInstrumentation()->logEvent("Email Link Selected");            
        launchWebBrowser( mailToLink );
    } else if( mWebRect.contains( pos ) ){
        Flurry::getInstrumentation()->logEvent("Planetary Link Selected");            
        launchWebBrowser( planetaryWebsite );
        
//    } else if( mBloomButton.contains( pos ) ){
//        Flurry::getInstrumentation()->logEvent("Bloom Link Selected");            
//        launchWebBrowser( bloomWebsite );
//        
    } else if( mCinderRect.contains( pos ) ){
        Flurry::getInstrumentation()->logEvent("Cinder Link Selected");            
        launchWebBrowser( cinderWebsite );
    }
	
	return mBgRect.contains( pos );
}

void HelpLayer::update()
{
    mInterfaceSize = getRoot()->getInterfaceSize();
    mBgRect.x2 = mInterfaceSize.x;
}

void HelpLayer::draw()
{	
    gl::color( Color::black() );
    gl::drawSolidRect( mBgRect );
    
    gl::color( Color::white() );
    gl::draw( mHeadingTex, mHeadingPos );
    gl::draw( mBodyTex, mBodyPos );
    
    gl::color( ColorA(BRIGHT_BLUE, 0.15f) );
    gl::drawLine( mBgRect.getLowerLeft(), mBgRect.getLowerRight() );

//    gl::color( Color::white() );
//    gl::drawStrokedRect( mCinderRect );
//    gl::drawStrokedRect( mWebRect );
//    gl::drawStrokedRect( mEmailRect );

    glPushMatrix();
    gl::translate( Vec2f(0, 2.0f) );
    gl::color( ColorA(1.0f, 1.0f, 1.0f, 0.5f) );
    gl::drawLine( mCinderRect.getLowerLeft(), mCinderRect.getLowerRight() );
    gl::drawLine( mWebRect.getLowerLeft(), mWebRect.getLowerRight() );
    gl::drawLine( mEmailRect.getLowerLeft(), mEmailRect.getLowerRight() );
    glPopMatrix();
}
