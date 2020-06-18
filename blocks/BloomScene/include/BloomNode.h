//
//  BloomNode.h
//  Kepler
//
//  Created by Tom Carden on 7/17/11.
//  Copyright 2011 Bloom Studio, Inc. All rights reserved.
//

#pragma once

#include <map>
#include <vector>
#include "cinder/Cinder.h"
#include "cinder/Function.h"
#include "cinder/app/TouchEvent.h"
#include "cinder/Matrix.h"

// FIXME: namespace this stuff (and maybe don't prefix Bloom- if we do)

class BloomScene; // for root
class BloomNode; // for BloomNodeRef

// for sharing ownership:
typedef std::shared_ptr<BloomScene> BloomSceneRef;
typedef std::shared_ptr<BloomNode> BloomNodeRef;

// for avoiding circular refs:
typedef std::weak_ptr<BloomScene> BloomSceneWeakRef;
typedef std::weak_ptr<BloomNode> BloomNodeWeakRef;

class BloomSceneEvent
{
public:
    BloomSceneEvent( BloomNodeRef nodeRef, ci::app::TouchEvent::Touch touch ): mNodeRef(nodeRef), mTouch(touch) {}
    ~BloomSceneEvent() {}

    BloomNodeRef getNodeRef() { return mNodeRef; }
    ci::app::TouchEvent::Touch getTouch() { return mTouch; }    
    
private:
    BloomNodeRef mNodeRef;
    ci::app::TouchEvent::Touch mTouch;
    
};

typedef std::shared_ptr<BloomSceneEvent> BloomSceneEventRef;

class BloomNode : public std::enable_shared_from_this<BloomNode> {

public:

    // supply an id...
    BloomNode( const int &nodeId ): mVisible(true), mId(nodeId) {}
    // ... or one will be generated for you
    BloomNode(): mVisible(true), mId(sNextNodeId++) {}
    
    // sub-classing is to be expected
    virtual ~BloomNode() {}
    
// CONTAINER METHODS
    
    void addChild( BloomNodeRef child );
    void addChildAt( BloomNodeRef child, const int &index );
    void removeChild( BloomNodeRef child );
    BloomNodeRef removeChildAt( const int &index );
    int getNumChildren() const { return mChildren.size(); }
    BloomNodeRef getChildAt( const int &index ) const { return mChildren[index]; }
    BloomNodeRef getChildById( const int &childId ) const;

// VISIBILITY
    
    // if you have behaviors to toggle when things are visible or not, override these
    virtual void setVisible( bool visible = true ) { mVisible = visible; }
    virtual bool isVisible() { return mVisible; }
    
// LOOP METHODS
    
    // subclasses should mess with these, just draw/update yourself (not children)
    // (deepDraw/Update draws children in correct order)
    virtual void update() {}
    virtual void draw() {}
    
// TRANFORM METHODS
    
    void setTransform( const ci::Matrix44f &transform ) { mTransform = transform; /* copy OK */ }
    ci::Matrix44f getTransform() const { return mTransform; /* copy OK */ }
    
    // override getConcatenatedTransform to change the behavior of these:
    ci::Vec2f localToGlobal( const ci::Vec2f &pos );
    ci::Vec2f globalToLocal( const ci::Vec2f &pos );    
    
    // if you have extra transforms/rotations that get applied in draw, override this too:
    virtual ci::Matrix44f getConcatenatedTransform() const;    
    
// ID METHODS
        
    int getId() const { return mId; }
    void setId( int newId ) { mId = newId; }
    
// HIERARCHY METHODS
    
    BloomNodeRef getParent() const { return mParent.lock(); }
    BloomSceneRef getRoot() const { return mRoot.lock(); }
    
    // if you need mRoot to setup, override this:
    virtual bool addedToScene() { return false; }
    // if you attach things to mRoot in setup, override this too:
    virtual bool removedFromScene() { return false; }
    
// INTERACTION HELPERS
            
    // interactions, one touch at a time
    virtual bool touchBegan( ci::app::TouchEvent::Touch touch ) { return false; }
    // touchMoved/Ended will only be called for touch IDs that returned true in touchBegan
    virtual bool touchMoved( ci::app::TouchEvent::Touch touch ) { return false; }
    virtual bool touchEnded( ci::app::TouchEvent::Touch touch ) { return false; }
    
    // override deepHitTest as well if you want to skip hit-testing children
    virtual bool hitTest( const ci::Vec2f &screenPos ) { return false; }
    
// RECURSIVE METHODS
    
    // recurse to children and call draw()
    // (public so it can be overridden, but generally considered internal)
    virtual void deepDraw();
    
    // recurse to children and call update()
    // (public so it can be overridden, but generally considered internal)
    virtual void deepUpdate();
    
    // recurse to children and call touchBegan/Moved/Ended
    // (public so they can be overridden, but generally considered internal)
    virtual bool deepTouchBegan( ci::app::TouchEvent::Touch touch );
    virtual bool deepTouchMoved( ci::app::TouchEvent::Touch touch );
    virtual bool deepTouchEnded( ci::app::TouchEvent::Touch touch );
    
    // recurse to children and call hitTest
    // override this if you want to skip hitTesting children
    virtual bool deepHitTest( const ci::Vec2f &screenPos );
    
// EVENT STUFF
    
	template<typename T>
    ci::CallbackId registerTouchBegan( T *obj, bool (T::*callback)(BloomSceneEventRef) )
	{
		return mCbTouchBegan.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}    
	template<typename T>
    ci::CallbackId registerTouchMoved( T *obj, bool (T::*callback)(BloomSceneEventRef) )
	{
		return mCbTouchMoved.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}    
	template<typename T>
    ci::CallbackId registerTouchEnded( T *obj, bool (T::*callback)(BloomSceneEventRef) )
	{
		return mCbTouchEnded.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}
	
    void unregisterTouchBegan( ci::CallbackId cbId ) { mCbTouchBegan.unregisterCb( cbId ); }
    void unregisterTouchMoved( ci::CallbackId cbId ) { mCbTouchMoved.unregisterCb( cbId ); }
    void unregisterTouchEnded( ci::CallbackId cbId ) { mCbTouchEnded.unregisterCb( cbId ); }
    
    // bubbles events up to parents...
    void dispatchTouchBegan( BloomSceneEventRef eventRef );
    void dispatchTouchMoved( BloomSceneEventRef eventRef );
    void dispatchTouchEnded( BloomSceneEventRef eventRef );
    
protected:
    
    // weakrefs because we don't "own" these, they're just convenient
    // (full shared pointers would be circular references, we'd never clean up)
    BloomNodeWeakRef mParent;
    BloomSceneWeakRef mRoot; // aka "stage"

    // and this is really all we have, everything else is recursion
    int mId;
    bool mVisible;    
    ci::Matrix44f mTransform;
    
    // normal shared_ptrs because we "own" children
    std::vector<BloomNodeRef> mChildren;
    
    // keep track of interactions claimed by ID
    std::map<uint64_t, BloomNodeRef> mActiveTouches;

    // for generating IDs:
    static int sNextNodeId;
    
    // for event passing
	ci::CallbackMgr<bool(BloomSceneEventRef)> mCbTouchBegan;
	ci::CallbackMgr<bool(BloomSceneEventRef)> mCbTouchMoved;
	ci::CallbackMgr<bool(BloomSceneEventRef)> mCbTouchEnded;
};