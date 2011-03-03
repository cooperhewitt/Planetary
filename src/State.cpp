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
		// clear currently selected node and all parents
		Node *selection = mSelectedNode;
		while( selection != NULL ) {
			selection->deselect();
			selection = selection->mParentNode;
		}
		// ensure that the new selection hierarchy is selected
		selection = node;
		while( selection != NULL ) {
			if ( !selection->mIsSelected ) {
				selection->select();
			}
			selection = selection->mParentNode;
		}
		mSelectedNode = node;
	}
	// tell everyone the good news
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
