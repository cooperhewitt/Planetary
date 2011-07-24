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
#include "cinder/Cinder.h"
#include "cinder/app/TouchEvent.h"
#include "cinder/Matrix.h"

// FIXME: namespace this stuff

class UIController; // for root
class UINode; // for UINodeRef

typedef std::shared_ptr<UIController> UIControllerRef;
typedef std::shared_ptr<UINode> UINodeRef;

class UINode : public std::enable_shared_from_this<UINode> {

public:
    
    UINode(): mVisible(true), mId(sNextNodeId++) {}
    UINode( const int &nodeId ): mVisible(true), mId(nodeId) {}
    virtual ~UINode() {}
    
    void addChild( UINodeRef child );
    void addChildAt( UINodeRef child, const int &index );
    
    void removeChild( UINodeRef child );
    UINodeRef removeChildAt( int index );
    
    int getNumChildren() const { return mChildren.size(); }
    UINodeRef getChildAt( int index ) const { return mChildren[index]; }
    UINodeRef getChildById( const int &childId ) const;

    void setTransform(const ci::Matrix44f &transform) { mTransform = transform; /* copy OK */ }
    ci::Matrix44f getTransform() const { return mTransform; /* copy OK */ }
    
    int getId() const { return mId; }
    void setId( int newId ) { mId = newId; }
    
    UINodeRef getParent() const { return mParent; }
    UIControllerRef getRoot() const { return mRoot; }
    
    // override getConcatenatedTransform to change the behavior of these:
    ci::Vec2f localToGlobal(const ci::Vec2f pos);
    ci::Vec2f globalToLocal(const ci::Vec2f pos);    
    
    // subclasses should mess with these, just draw/update yourself (privateDraw/Update draws children in correct order)
    virtual void draw() {}
    virtual void update() {}
    virtual bool touchBegan(ci::app::TouchEvent::Touch touch) { return false; }
    virtual bool touchMoved(ci::app::TouchEvent::Touch touch) { return false; }
    virtual bool touchEnded(ci::app::TouchEvent::Touch touch) { return false; }
    
    // if you need mRoot to setup, override this:
    virtual bool addedToScene() { return false; }
    // if you attach things to mRoot in setup, override this too:
    virtual bool removedFromScene() { return false; }
    
    // if you have extra transforms/rotations that get applied in draw, override this too:
    virtual ci::Matrix44f getConcatenatedTransform() const;    
    
    // if you have behaviors to toggle when things are visible or not, override these
    virtual void setVisible( bool visible = true ) { mVisible = visible; }
    virtual bool isVisible() { return mVisible; }
    
protected:
    
    // all access to privateDraw from UIController::draw()
    // FIXME: is there a better way to do this in C++?
    friend class UIController;
    
    // recurse to children and call draw()
    virtual void privateDraw();

    // recurse to children and call update()
    virtual void privateUpdate();
    
    // recurse to children and call touchBegan/Moved/Ended
    bool privateTouchBegan(ci::app::TouchEvent::Touch touch);
    bool privateTouchMoved(ci::app::TouchEvent::Touch touch);
    bool privateTouchEnded(ci::app::TouchEvent::Touch touch);
    
    UINodeRef mParent;
    UIControllerRef mRoot; // aka "stage"

    int mId;
    bool mVisible;    
    ci::Matrix44f mTransform;
    std::vector<UINodeRef> mChildren;
    
    // keep track of interactions claimed by ID
    std::map<uint64_t, UINodeRef> mActiveTouches;

    // for generating IDs:
    static int sNextNodeId;
};