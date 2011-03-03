/*
 *  State.h
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/Vector.h"
#include "cinder/Font.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "Globals.h"
#include "Node.h"

using namespace ci;

class State {
 public:
	State();
	void draw( const ci::Font &font);
	// Alpha char for filtering artist name
	char getAlphaChar(){ return mAlphaChar; }
	void setAlphaChar( char c );
	template<typename T>
	CallbackId registerAlphaCharStateChanged( T *obj, bool ( T::*callback )( State* ) ){
		return mCallbacksAlphaCharStateChanged.registerCb(std::bind1st( std::mem_fun( callback ), obj ) );
	}

	Node* getSelectedNode() { return mSelectedNode; }
	void setSelectedNode(Node* selectedNode);	
	template<typename T>
	CallbackId registerNodeSelected( T *obj, bool (T::*callback)(Node*) ){
		return mCallbacksNodeSelected.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}	

	std::vector<std::string> getHierarchy();
	std::map<u_int8_t, Node*> mMapOfNodes;
 private:
	CallbackMgr<bool(State*)> mCallbacksAlphaCharStateChanged;	
	CallbackMgr<bool(Node*)> mCallbacksNodeSelected;

	// Keep track of selected NODES and levels
	Node			*mSelectedNode;
	//Node			*mPrevSelectedNode;
	
	
	char mAlphaChar;
};

