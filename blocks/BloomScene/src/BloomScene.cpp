//
//  BloomScene.cpp
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "BloomScene.h"
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/gl/gl.h"
#include "OrientationHelper.h"

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
    mParent = BloomNodeRef();     // NULL, we are the parent (crash rather than recurse)
    mRoot = BloomSceneRef(); // NULL, will be set in create() because we are the root
    
    cbTouchesBegan = mApp->registerTouchesBegan( this, &BloomScene::touchesBegan );
    cbTouchesMoved = mApp->registerTouchesMoved( this, &BloomScene::touchesMoved );
    cbTouchesEnded = mApp->registerTouchesEnded( this, &BloomScene::touchesEnded );
    
    mInterfaceSize = mApp->getWindowSize();
}

BloomScene::~BloomScene()
{
    mApp->unregisterTouchesBegan( cbTouchesBegan );
    mApp->unregisterTouchesMoved( cbTouchesMoved );
    mApp->unregisterTouchesEnded( cbTouchesEnded );
}

bool BloomScene::touchesBegan( TouchEvent event )
{
    bool consumed = true;
    for (std::vector<TouchEvent::Touch>::const_iterator i = event.getTouches().begin(); i != event.getTouches().end(); i++) {
        consumed = privateTouchBegan(*i) && consumed; // recurses to children
    }    
    return consumed; // only true if all touches were consumed
}

bool BloomScene::touchesMoved( TouchEvent event )
{
    bool consumed = true;
    for (std::vector<TouchEvent::Touch>::const_iterator i = event.getTouches().begin(); i != event.getTouches().end(); i++) {
        consumed = privateTouchMoved(*i) && consumed; // recurses to children
    }
    return consumed; // only true if all touches were consumed
}

bool BloomScene::touchesEnded( TouchEvent event )
{
    bool consumed = true;
    for (std::vector<TouchEvent::Touch>::const_iterator i = event.getTouches().begin(); i != event.getTouches().end(); i++) {
        consumed = privateTouchEnded(*i) && consumed; // recurses to children
    }    
    return consumed; // only true if all touches were consumed
}

Matrix44f BloomScene::getConcatenatedTransform() const
{
    return mTransform;
}

void BloomScene::draw()
{
    if (mVisible) {
        glPushMatrix();
        glMultMatrixf( getConcatenatedTransform() );    
        // draw children
        for (std::vector<BloomNodeRef>::const_iterator i = mChildren.begin(); i != mChildren.end(); i++) {
            (*i)->privateDraw();
        }
        // dont' draw self or we'll recurse
        glPopMatrix();
    }
}

void BloomScene::update()
{
    if (mVisible) {
        // update children
        for (std::vector<BloomNodeRef>::const_iterator i = mChildren.begin(); i != mChildren.end(); i++) {
            (*i)->privateUpdate();
        }
        // dont' update self or we'll recurse
    }
}
