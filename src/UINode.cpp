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

int UINode::sTotalNodeCount = 0;

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
}
void UINode::addChildAt( UINodeRef child, const int &index )
{
    mChildren.insert( mChildren.begin() + index, child );
    child->mParent = UINodeRef(this);
    child->mRoot = UIControllerRef(mParent->getRoot());
}
void UINode::removeChild( UINodeRef child )
{
    for (std::vector<UINodeRef>::iterator i = mChildren.begin(); i != mChildren.end(); i++) {
        if ( (*i) == child ) {
            child->mParent = UINodeRef();
            child->mRoot = UIControllerRef();
            mChildren.erase( i );
            break;
        }
    }    
}
UINodeRef UINode::removeChildAt( int index )
{
    UINodeRef child = *mChildren.erase( mChildren.begin() + index );
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
void UINode::draw() const
{
    glPushMatrix();
    glMultMatrixf(mTransform);
    for (std::vector<UINodeRef>::const_iterator i = mChildren.begin(); i != mChildren.end(); i++) {
        (*i)->draw();
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

