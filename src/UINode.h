//
//  UINode.h
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <vector>
#include "cinder/app/TouchEvent.h"
#include "cinder/Matrix.h"

class UIController; // for root
class UINode; // for UINodeRef

typedef std::shared_ptr<UIController> UIControllerRef;
typedef std::shared_ptr<UINode> UINodeRef;

class UINode {

public:
    
    UINode();
    UINode( const int &nodeId );
    virtual ~UINode();
    
    void addChild( UINodeRef child );
    void addChildAt( UINodeRef child, const int &index );
    
    void removeChild( UINodeRef child );
    UINodeRef removeChildAt( int index );
    
    int getNumChildren() const;
    UINodeRef getChildAt( int index ) const;
    UINodeRef getChildById( const int &childId ) const;

    void setTransform(const ci::Matrix44f &transform);
    ci::Matrix44f getTransform() const;
    virtual ci::Matrix44f getConcatenatedTransform() const;
    
    int getId() const;
    
    // subclasses should mess with these:
    virtual void draw() const;
    virtual bool touchBegan(ci::app::TouchEvent::Touch touch) { return false; };
    virtual bool touchMoved(ci::app::TouchEvent::Touch touch) { return false; };
    virtual bool touchEnded(ci::app::TouchEvent::Touch touch) { return false; };
    
    ci::Vec2f localToGlobal(const ci::Vec2f pos);
    ci::Vec2f globalToLocal(const ci::Vec2f pos);
    
    UINodeRef getParent() const { return mParent; }
    UIControllerRef getRoot() const { return mRoot; }
    
protected:
    
    static int sTotalNodeCount;
    int mId;
    std::vector<UINodeRef> mChildren;
    UINodeRef mParent;
    UIControllerRef mRoot;
    ci::Matrix44f mTransform;
    
};