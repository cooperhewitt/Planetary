//
//  BloomScene.cpp
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#include <boost/foreach.hpp>
#include "BloomScene.h"
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/gl/gl.h"
#include "cinder/Function.h"

using namespace ci;
using namespace ci::app;

BloomSceneRef BloomScene::create( AppCocoaTouch *app )
{
    BloomSceneRef ref = BloomSceneRef( new BloomScene( app ) );
    ref->mRoot = BloomSceneWeakRef( ref );
    return ref;
}

BloomScene::BloomScene( AppCocoaTouch *app ): 
    mApp( app ), 
    mInterfaceSize( 0.0f, 0.0f )
{
    mParent = BloomNodeRef(); // NULL, we are the parent (crash rather than recurse)
    mRoot = BloomSceneRef();  // NULL, will be set in create() because we are the root
    
    // KEMAL - TODO - hacked how this works...remove?
//    cbTouchesBegan = mApp->registerTouchesBegan( this, &BloomScene::touchesBegan );
//    cbTouchesMoved = mApp->registerTouchesMoved( this, &BloomScene::touchesMoved );
//    cbTouchesEnded = mApp->registerTouchesEnded( this, &BloomScene::touchesEnded );
    
    mInterfaceSize = mApp->getWindowSize();
}

BloomScene::~BloomScene()
{
    // KEMAL - TODO - hacked how this works...remove?
//    unregisterTouchBegan(cbTouchesBegan);
//    unregisterTouchBegan(cbTouchesMoved);
//    unregisterTouchBegan(cbTouchesEnded);
//    mApp->unregisterTouchesBegan( cbTouchesBegan );
//    mApp->unregisterTouchesMoved( cbTouchesMoved );
//    mApp->unregisterTouchesEnded( cbTouchesEnded );
}

bool BloomScene::touchesBegan( TouchEvent event )
{
    bool consumed = true;
    BOOST_FOREACH(TouchEvent::Touch touch, event.getTouches()) {
        consumed = deepTouchBegan( touch ) && consumed; // recurses to children
    }    
    return consumed; // only true if all touches were consumed
}

bool BloomScene::touchesMoved( TouchEvent event )
{
    bool consumed = true;
    BOOST_FOREACH(TouchEvent::Touch touch, event.getTouches()) {
        consumed = deepTouchMoved( touch ) && consumed; // recurses to children
    }
    return consumed; // only true if all touches were consumed
}

bool BloomScene::touchesEnded( TouchEvent event )
{
    bool consumed = true;
    BOOST_FOREACH(TouchEvent::Touch touch, event.getTouches()) {
        consumed = deepTouchEnded( touch ) && consumed; // recurses to children
    }    
    return consumed; // only true if all touches were consumed
}

void BloomScene::draw()
{
    gl::setMatricesWindow( mApp->getWindowSize() ); 
}
