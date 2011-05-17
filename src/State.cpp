/*
 *  State.cpp
 *  Bloom
 *
 *  Created by Robert Hodgin on 2/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include <deque>
#include "State.h"
#include "cinder/gl/gl.h"
#include "cinder/Rect.h"

using std::stringstream;
using namespace ci;
using namespace ci::app;
using namespace std;

State::State()
{
    setup();
}

void State::setup()
{
	mAlphaChar = ' ';
	mSelectedNode = NULL;
	mPrevSelectedNode = NULL;
	mDistBetweenPrevAndCurrentNode = 1.0f;
}

void State::draw( const Font &font )
{
}

void State::setAlphaChar( char c )
{
	std::cout << "State::setAlphaChar " << c << std::endl;
	if (mAlphaChar != c) {
		mAlphaChar = c;
		mCallbacksAlphaCharStateChanged.call( this );
	}
}

void State::setAlphaChar( const string &name )
{
	char firstLetter = name[0];	
	if (name.size() > 4) {
		string the = name.substr( 0, 4 );
		if( the == "The " || the == "the " ){
			firstLetter = name[4];
		}	
	}
	if (!isalpha(firstLetter)) {
		firstLetter = '#';
	}
	setAlphaChar(firstLetter);
}

void State::setSelectedNode( Node* node )
{
//	if (node == mSelectedNode) {
//		return;
//	}
	
	if (node == NULL) {
		// clear currently selected node and all parents
		Node *selection = mSelectedNode;
		while( selection != NULL ) {
			selection->deselect();
			selection = selection->mParentNode;
		}
		mSelectedNode = NULL;
		mDistBetweenPrevAndCurrentNode = 20.0f;
	}
	else {
		
		// deque so we can push_front instead of messing with reverse()
		deque<Node*> currentChain;
		deque<Node*> nextChain;
		
		Node* current = mSelectedNode;	// track, album, artist
		while (current != NULL) {
			currentChain.push_front(current);
			current = current->mParentNode;
		}

		Node* next = node;				// new track, album, artist
		while (next != NULL) {
			nextChain.push_front(next);
			next = next->mParentNode;
		}

		vector<bool> sharedAncestor;
		for (int i = 0; i < currentChain.size(); i++) {
			if (nextChain.size() - 1 < i) {
				sharedAncestor.push_back(false);
			}
			else {
				sharedAncestor.push_back(currentChain[i] == nextChain[i]);
			}
		}
		
		for (int i = currentChain.size() - 1; i >= 0; i--) {
			if (!sharedAncestor[i]) {
				currentChain[i]->deselect();
			}
		}
		
		// ensure that the new selection is selected
		// node->select();
		mPrevSelectedNode = mSelectedNode;
		mSelectedNode = node;
		
		if( mPrevSelectedNode && mSelectedNode )
			mDistBetweenPrevAndCurrentNode = mPrevSelectedNode->mTransPos.distance( mSelectedNode->mTransPos );
		
		std::cout << "mDistBetweenPrevAndCurrentNode = " << mDistBetweenPrevAndCurrentNode << std::endl;
		
		// select everything in the next chain that isn't in the current chain
		// SANITY CHECK: I changed this for loop
		// from
		// for (int i = currentChain.size(); i < nextChain.size(); i++) {
		// to
        for (int i = 0; i < nextChain.size(); i++) {
            nextChain[i]->select();
        }
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
			hierarchy.push_back( parent->getName() );
			parent = parent->mParentNode;
		}		
	}

	if(mAlphaChar != ' ') {
		string s;
		s += mAlphaChar;
		hierarchy.push_back( s );
	}
	
	hierarchy.push_back("Planetary");
	
	if (hierarchy.size() > 1) {
		reverse( hierarchy.begin(), hierarchy.end() );
	}
	
	return hierarchy;
}
