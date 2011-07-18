//
//  UINode.h
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <map>
#include <vector>
#include "cinder/app/TouchEvent.h"
#include "cinder/Matrix.h"

// FIXME: namespace this stuff

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
    
    // subclasses should mess with these, just draw yourself (privateDraw draws children in correct order)
    virtual void draw() {}
    virtual bool touchBegan(ci::app::TouchEvent::Touch touch) { return false; }
    virtual bool touchMoved(ci::app::TouchEvent::Touch touch) { return false; }
    virtual bool touchEnded(ci::app::TouchEvent::Touch touch) { return false; }
    
    // if you need mRoot to setup, override this:
    virtual bool addedToScene() { return false; }
    // if you attach things to mRoot in setup, override this too:
    virtual bool removedFromScene() { return false; }
    
    ci::Vec2f localToGlobal(const ci::Vec2f pos);
    ci::Vec2f globalToLocal(const ci::Vec2f pos);
    
    UINodeRef getParent() const { return mParent; }
    UIControllerRef getRoot() const { return mRoot; }
    
protected:
    
    // recurse to children and call draw()
    virtual void privateDraw();
    // recurse to children and call touchBegan/Moved/Ended
    bool privateTouchBegan(ci::app::TouchEvent::Touch touch);
    bool privateTouchMoved(ci::app::TouchEvent::Touch touch);
    bool privateTouchEnded(ci::app::TouchEvent::Touch touch);
    
    static int sTotalNodeCount;
    int mId;
    std::vector<UINodeRef> mChildren;
    UINodeRef mParent;
    UIControllerRef mRoot;
    ci::Matrix44f mTransform;
    std::map<uint64_t, UINodeRef> mActiveTouches;
    
};