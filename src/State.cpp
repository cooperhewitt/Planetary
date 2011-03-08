/*
 *  State.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#include "State.h"
#include "cinder/gl/gl.h"
#include "cinder/Rect.h"

using std::stringstream;
using namespace ci;
using namespace ci::app;
using namespace std;

State::State()
{
	mAlphaChar = ' ';
	mSelectedNode = NULL;
}

void State::draw( const Font &font )
{
}

void State::setAlphaChar( char c )
{
	std::cout << "State::setAlphaChar " << c << std::endl;
	mAlphaChar = c;
	mCallbacksAlphaCharStateChanged.call( this );
}

void State::setSelectedNode( Node* node )
{
	if (node == NULL) {
		// clear currently selected node and all parents
		Node *selection = mSelectedNode;
		while( selection != NULL ) {
			selection->deselect();
			selection = selection->mParentNode;
		}
		mSelectedNode = NULL;
	}
	else {
		// clear currently selected node
		// assuming that the only thing that can be selected is at the same level or higher
		if( mSelectedNode != NULL && mSelectedNode->mGen >= node->mGen ){
			
			Node *parent = mSelectedNode;
			while( parent != NULL && parent->mGen >= node->mGen ){
				parent->deselect();
				parent = parent->mParentNode;
			}
			
			// TODO: if parent != NULL && parent->mGen == node->mGen
			// this would happen if we had a track selected but then
			// we selected a *different* artist
			// right now that can happen but we should prevent it in
			// selection code
		}
		// ensure that the new selection is selected
		node->select();
		mSelectedNode = node;
	}
	// and then spread the good word
	mCallbacksNodeSelected.call(mSelectedNode);
}

vector<string> State::getHierarchy()
{
	vector<string> hierarchy;
	if( mSelectedNode != NULL ){
		Node *parent = mSelectedNode;
		while( parent != NULL ){
			hierarchy.push_back( parent->mName );
			parent = parent->mParentNode;
		}		
	}

	if(mAlphaChar != ' ') {
		string s;
		s += mAlphaChar;
		hierarchy.push_back( s );
	}
	
	hierarchy.push_back("Kepler");
	
	if (hierarchy.size() > 1) {
		reverse( hierarchy.begin(), hierarchy.end() );
	}
	
	return hierarchy;
}
