//
//  BloomNode.cpp
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "BloomNode.h"
#include "BloomScene.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app; // for TouchEvent::Touch

int BloomNode::sNextNodeId = 10000000; // start high

void BloomNode::addChild( BloomNodeRef child )
{
    mChildren.push_back( child );
    child->mParent = BloomNodeWeakRef( shared_from_this() );
    child->mRoot = mRoot;    
    child->addedToScene(); // notify child that mRoot and mParent are set
}

void BloomNode::addChildAt( BloomNodeRef child, const int &index )
{
    mChildren.insert( mChildren.begin() + index, child );
    child->mParent = BloomNodeWeakRef( shared_from_this() );
    child->mRoot = mRoot;
    child->addedToScene(); // notify child that mRoot and mParent are set
}

void BloomNode::removeChild( BloomNodeRef child )
{
    for (std::vector<BloomNodeRef>::iterator i = mChildren.begin(); i != mChildren.end(); i++) {
        if ( (*i) == child ) {
            mChildren.erase( i );
            child->removedFromScene(); // notify child that mRoot and mParent are about to be invalid
            child->mParent = BloomNodeWeakRef();
            child->mRoot = BloomSceneWeakRef();
            break;
        }
    }    
}

BloomNodeRef BloomNode::removeChildAt( int index )
{
    BloomNodeRef child = *mChildren.erase( mChildren.begin() + index );
    child->removedFromScene(); // notify child that mRoot and mParent are about to be invalid    
    child->mParent = BloomNodeRef();
    child->mRoot = BloomSceneRef();
    return child;
}

BloomNodeRef BloomNode::getChildById( const int &childId ) const
{
    for (std::vector<BloomNodeRef>::const_iterator i = mChildren.begin(); i != mChildren.end(); i++) {
        if ( (*i)->getId() == childId ) {
            return *i;
        }
    }
    return BloomNodeRef(); // aka NULL
}

void BloomNode::privateUpdate()
{
    if (mVisible) {
        // update self
        update();
        // update children
        for (std::vector<BloomNodeRef>::const_iterator i = mChildren.begin(); i != mChildren.end(); i++) {
            (*i)->privateUpdate();
        }
    }
}

void BloomNode::privateDraw()
{
    if (mVisible) {
        glPushMatrix();
        glMultMatrixf(mTransform); // FIXME only push/mult/pop if mTransform isn't identity
        // draw self    
        draw();
        // draw children
        for (std::vector<BloomNodeRef>::const_iterator i = mChildren.begin(); i != mChildren.end(); i++) {
            (*i)->privateDraw();
        }
        glPopMatrix();
    }        
}

Matrix44f BloomNode::getConcatenatedTransform() const
{
    if ( BloomNodeRef parent = mParent.lock() ) {
        return parent->getConcatenatedTransform() * mTransform;
    }
    return mTransform;
}

Vec2f BloomNode::localToGlobal(const Vec2f pos)
{
    return (getConcatenatedTransform() * Vec3f(pos.x,pos.y,0)).xy();
}

Vec2f BloomNode::globalToLocal(const Vec2f pos)
{
    Matrix44f invMtx = getConcatenatedTransform().inverted();
    return (invMtx * Vec3f(pos.x,pos.y,0)).xy();    
}

bool BloomNode::privateTouchBegan( TouchEvent::Touch touch )
{
    if (!mVisible) {
        return false;
    }
    bool consumed = false;
    // check children
    for (std::vector<BloomNodeRef>::const_iterator j = mChildren.begin(); j != mChildren.end(); j++) {
        BloomNodeRef childRef = *j;
        if (childRef->privateTouchBegan(touch)) {
            consumed = true;
            mActiveTouches[touch.getId()] = childRef;
            break; // first child wins (touch can't affect more than one child node)
        }
    }    
    if (!consumed) {
        // check self
        if (touchBegan(touch)) {
            BloomNodeRef thisRef = shared_from_this();
            mActiveTouches[touch.getId()] = thisRef;
            if ( BloomSceneRef root = mRoot.lock() ) {
                root->onBloomNodeTouchBegan(thisRef);
            }
            consumed = true;
        }
    }
    return consumed;
}

bool BloomNode::privateTouchMoved( TouchEvent::Touch touch )
{
    if (!mVisible) {
        return false;
    }    
    // in this current implementation, children only receive touchMoved calls 
    // if they returned true for the touch with the same ID in touchesBegan
    bool consumed = false;
    if ( mActiveTouches.find(touch.getId()) != mActiveTouches.end() ) {
        BloomNodeRef nodeRef = mActiveTouches[touch.getId()];
        if (nodeRef->getId() == this->getId()) {
            // check self
            consumed = touchMoved(touch);
            if (consumed) {
                if ( BloomSceneRef root = mRoot.lock() ) {
                    root->onBloomNodeTouchMoved(nodeRef);
                }
            }
        }
        else {
            consumed = nodeRef->privateTouchMoved(touch);
        }
    }
    return consumed;
}

bool BloomNode::privateTouchEnded( TouchEvent::Touch touch )
{
    if (!mVisible) {
        return false;
    }    
    // in this current implementation, children only receive touchEnded calls 
    // if they returned true for the touch with the same ID in touchesBegan
    bool consumed = false;
    if ( mActiveTouches.find(touch.getId()) != mActiveTouches.end() ) {
        BloomNodeRef nodeRef = mActiveTouches[touch.getId()];
        if (nodeRef->getId() == this->getId()) {
            // check self
            consumed = touchEnded(touch);
            if (consumed) {
                if ( BloomSceneRef root = mRoot.lock() ) {
                    root->onBloomNodeTouchEnded(nodeRef);
                }
            }
        }
        else {
            consumed = nodeRef->privateTouchEnded(touch);
        }
        mActiveTouches.erase(touch.getId());
    }
    return consumed;
}