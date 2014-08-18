/*
 *      Tree.cpp
 *      
 *      Copyright (C) 2008 Universitat Politècnica de Catalunya
 *      @author Albert Bifet - Ricard Gavaldà (abifet,gavalda@lsi.upc.edu)
 *       
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include "Tree.h"

 Tree::Tree(){ 
	// Implementation of constructor: Initialize an empty tree 
	#if defined(LABELLED)
	#else
		tree.push_back(0);
	#endif
	Support=0;
	lenTail=0;
	isNotMaximal=false;
	Update=false;
	}
		
////////////////////////////////////////////////////////////////////////////////

Tree::Tree(Tree *t){
	// Implementation of constructor: Initialize a tree as a copy of a tree t
	tree=t->tree;
	lenTail=t->lenTail;
	Support=t->Support;
	isNotMaximal=false;
	Update=t->Update;
	#if defined(LABELLED)
		parentsLabel=t->parentsLabel;
	#endif
     }  
	
////////////////////////////////////////////////////////////////////////////////

Tree::~Tree(){ 
	// Implementation of destructor
	}
  	
////////////////////////////////////////////////////////////////////////////////
 
void Tree::print(ofstream *ofile,DatasetReader *mdataset_reader){
	//Prints Tree

	int iSupport=Support;
	#if defined(LABELLED)
		for(TreeNodes::iterator it=tree.begin();it!=tree.end();it++) *ofile<<"("<<it->depth<<","<<it->label<<") "; 
	#else
		for(TreeNodes::iterator it=tree.begin();it!=tree.end();it++) *ofile<<*it<<" ";
	#endif
	*ofile<<"Support : "<<iSupport<< endl;
}
	
////////////////////////////////////////////////////////////////////////////////

void Tree::cout_print(){
	//Prints Tree
	#if defined(LABELLED)
		for(TreeNodes::iterator it=tree.begin();it!=tree.end();it++) cout<<"("<<it->depth<<","<<it->label<<") "; 
	#else
		for(TreeNodes::iterator it=tree.begin();it!=tree.end();it++) cout<<*it<<" "; 
	#endif
	int iSupport=Support;
			cout<<"Support : "<<iSupport<< endl;
}
	
////////////////////////////////////////////////////////////////////////////////

void Tree::print_l(TreeNodes  tree){
	//Prints Tree
	#if defined(LABELLED)
		for(TreeNodes::iterator it=tree.begin();it!=tree.end();it++) cout<<"("<<it->depth<<","<<it->label<<") "; 
	#else
		for(TreeNodes::iterator it=tree.begin();it!=tree.end();it++) cout<<*it<<" "; 
	#endif
} 
	
////////////////////////////////////////////////////////////////////////////////

 void Tree::printSupport(ofstream *ofile){
	//Prints Tree
	if (tree.size()==1) return;
	Integer old=-1;
	for(TreeNodes::iterator it=tree.begin();it!=tree.end();it++) {
		#if defined(LABELLED)
		if (it->depth<=old) 
			for (int j=0;j <= (old-it->depth);j++) 
				*ofile<<"]"; 
		old=it->depth;
		*ofile<<"["<<it->label;
		#else
		if (*it<=old) 
			for (int j=0;j<=old-*it;j++) 
				*ofile<<"]"; 
		old=*it;
		*ofile<<"["
		#endif
;
		}
	for (int j=0;j<=old;j++) *ofile<<"]";                         
	int iSupport=Support;

	*ofile << " "<< iSupport << endl;
    }

