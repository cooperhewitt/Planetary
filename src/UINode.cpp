//
//  UINode.cpp
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "UINode.h"
#include "UIController.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app; // for TouchEvent::Touch

int UINode::sTotalNodeCount = 10000000; // start high

UINode::UINode()
{
    sTotalNodeCount++;
    mId = sTotalNodeCount;
}
UINode::UINode( const int &nodeId )
{
    sTotalNodeCount++;
    mId = nodeId;
}
UINode::~UINode()
{
    sTotalNodeCount--;
}
void UINode::addChild( UINodeRef child )
{
    mChildren.push_back( child );
    child->mParent = UINodeRef(this);
    child->mRoot = UIControllerRef(mParent->getRoot());    
    child->addedToScene(); // notify child that mRoot and mParent are set
}
void UINode::addChildAt( UINodeRef child, const int &index )
{
    mChildren.insert( mChildren.begin() + index, child );
    child->mParent = UINodeRef(this);
    child->mRoot = UIControllerRef(mParent->getRoot());
    child->addedToScene(); // notify child that mRoot and mParent are set
}
void UINode::removeChild( UINodeRef child )
{
    for (std::vector<UINodeRef>::iterator i = mChildren.begin(); i != mChildren.end(); i++) {
        if ( (*i) == child ) {
            mChildren.erase( i );
            child->removedFromScene(); // notify child that mRoot and mParent are about to be invalid
            child->mParent = UINodeRef();
            child->mRoot = UIControllerRef();
            break;
        }
    }    
}
UINodeRef UINode::removeChildAt( int index )
{
    UINodeRef child = *mChildren.erase( mChildren.begin() + index );
    child->removedFromScene(); // notify child that mRoot and mParent are about to be invalid    
    child->mParent = UINodeRef();
    child->mRoot = UIControllerRef();
    return child;
}
int UINode::getNumChildren() const
{
    return mChildren.size();
}
UINodeRef UINode::getChildAt( int index ) const
{
    return mChildren[index];
}
UINodeRef UINode::getChildById( const int &childId ) const
{
    for (std::vector<UINodeRef>::const_iterator i = mChildren.begin(); i != mChildren.end(); i++) {
        if ( (*i)->getId() == childId ) {
            return *i;
        }
    }
    return UINodeRef(); // aka NULL
}
void UINode::privateDraw()
{
    glPushMatrix();
    glMultMatrixf(mTransform); // FIXME only push/mult/pop if mTransform isn't identity
    // draw self
    draw();
    // draw children
    for (std::vector<UINodeRef>::const_iterator i = mChildren.begin(); i != mChildren.end(); i++) {
        (*i)->privateDraw();
    }
    glPopMatrix();
}
void UINode::setTransform(const Matrix44f &transform)
{
    mTransform = transform; // copy OK
}
Matrix44f UINode::getTransform() const
{
    return mTransform; // copy OK
}
Matrix44f UINode::getConcatenatedTransform() const
{
    // TODO: cache and invalidate in setTransform? (needs to check dirty parent? bah)
    return mTransform * mParent->getConcatenatedTransform();
}
int UINode::getId() const
{
    return mId;
}

Vec2f UINode::localToGlobal(const Vec2f pos)
{
    return (getConcatenatedTransform() * Vec3f(pos.x,pos.y,0)).xy();
}

Vec2f UINode::globalToLocal(const Vec2f pos)
{
    Matrix44f invMtx = getConcatenatedTransform().inverted();
    return (invMtx * Vec3f(pos.x,pos.y,0)).xy();    
}

bool UINode::privateTouchBegan( TouchEvent::Touch touch )
{
    bool consumed = false;
    for (std::vector<UINodeRef>::const_iterator j = mChildren.begin(); j != mChildren.end(); j++) {
        UINodeRef childRef = *j;
        if (childRef->touchBegan(touch)) {
            consumed = true;
            mActiveTouches[touch.getId()] = childRef;
            mRoot->onUINodeTouchBegan(childRef);
            break; // check next touch
        }
    }    
    if (!consumed) {
        // check self
        if (touchBegan(touch)) {
            UINodeRef thisRef = UINodeRef(this);
            mActiveTouches[touch.getId()] = thisRef;                
            mRoot->onUINodeTouchBegan(thisRef);
            consumed = true;
        }
    }
    return consumed;
}

bool UINode::privateTouchMoved( TouchEvent::Touch touch )
{
    // in this current implementation, children only receive touchMoved calls 
    // if they returned true for the touch with the same ID in touchesBegan
    bool consumed = false;
    if ( mActiveTouches.find(touch.getId()) != mActiveTouches.end() ) {
        UINodeRef nodeRef = mActiveTouches[touch.getId()];
        consumed = nodeRef->touchMoved(touch);
        mRoot->onUINodeTouchMoved(nodeRef);
    }
    // no need to check self, should be in mActiveTouches if needed
    return consumed;
}

bool UINode::privateTouchEnded( TouchEvent::Touch touch )
{
    // in this current implementation, children only receive touchEnded calls 
    // if they returned true for the touch with the same ID in touchesBegan
    bool consumed = false;
    if ( mActiveTouches.find(touch.getId()) != mActiveTouches.end() ) {
        UINodeRef nodeRef = mActiveTouches[touch.getId()];
        consumed = nodeRef->touchEnded(touch);
        mActiveTouches.erase(touch.getId());
        mRoot->onUINodeTouchEnded(nodeRef);
    }
    // no need to check self, should be in mActiveTouches if needed
    return consumed;
}