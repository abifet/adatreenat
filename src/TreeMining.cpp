/*
 *      TreeMining.cpp
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

#include "TreeMining.h"

TreeMining::TreeMining(TreeFactory *tree_factory, DatasetReader *dataset_reader, int sup){
	// Implementation of constructor
	mapTree *cmt=new mapTree;
	mClosedMapTree=cmt;
    mtree_factory=tree_factory;
    mdataset_reader=dataset_reader;
	D=dataset_reader->dataset(); 
	min_sup=sup;
	
	#if defined(LABELLED)
        for(Labels_iterator label=mdataset_reader->get_labels()->begin(); label != mdataset_reader->get_labels()->end(); ++label){
			Tree* init_tree=mtree_factory->create_tree(D,*label);
			if (init_tree->get_Support()>=min_sup){
				init_tree->parentsLabel.push_back(*label);
				closed_subtree_mining(init_tree);
			}
		}
	#else
		Tree* init_tree=mtree_factory->create_tree(D);
		closed_subtree_mining(init_tree);
	#endif
}

	
////////////////////////////////////////////////////////////////////////////////

TreeMining::~TreeMining(){
     // Implementation of destructor
     delete mClosedMapTree;
}
	
////////////////////////////////////////////////////////////////////////////////

void TreeMining::print_Closed_Trees(const char* filenameOrig){
	//Prints list of tree to fileputput
    ofstream ofile;
	ofile.open(filenameOrig); 
	if (ofile.is_open()){  
		int auxsupport = 0;
		mapTreeIterator iter_First_Same_Support = mClosedMapTree->begin(); 

		for(mapTreeIterator iter = mClosedMapTree->begin(); iter != mClosedMapTree->end(); ++iter ) {
			//Print tree data
			if ( (iter->second)->get_isNotMaximal() == false )  ofile<< "M ";
			(iter->second)->print(&ofile,mdataset_reader);
			
			//Check if there is a tree with the same support list
			if (auxsupport != iter->first){
				auxsupport = iter->first;
				iter_First_Same_Support = iter; 
			} 
		}
		ofile.close();
	}
	else error("Unable to open output file");   
}

////////////////////////////////////////////////////////////////////////////////

void TreeMining::closed_subtree_mining(Tree *t){
	//Implicit parameters: TreeDataSet *D, int min_sup, mapTree *mClosedMapTree
    bool is_closed=true;
    int len_tail=t->get_lenTail();
    //For  every tt  that can be extended from t in one step 
    #if defined(LABELLED) 
    for (int depth=len_tail+1;depth>0;depth--){
		 Labels *labels;
		 int level=len_tail+1-depth;
		 if (depth==0)
			labels=mdataset_reader->get_labels();
		 else if (depth==1)
			labels=mdataset_reader->get_labels(t->currentAncestorLabel(level));
		 else if (depth==2)
			labels=mdataset_reader->get_labels(t->currentAncestorLabel(level),t->currentAncestorLabel(level+1));
		 else	
			labels=mdataset_reader->get_labels(t->currentAncestorLabel(level),t->currentAncestorLabel(level+1),t->currentAncestorLabel(level+2));	
		 for(Labels_iterator label=labels->begin(); label != labels->end(); ++label){
			Tree *tt = mtree_factory->create_tree(t); 
			Node extension(depth,*label);
	#else
    for (int extension=len_tail+1;extension>0;extension--){
    	Tree *tt = mtree_factory->create_tree(t); 
    #endif
        tt->extend(extension);
       if (tt->isCanonicalRepresentative(extension)) {
           tt->supportInc(D,min_sup);
           if (tt->get_Support()>=min_sup) {
			  t->set_isNotMaximal(true);
			  #if defined(LABELLED) 					
			  tt->set_lenTail(depth);
				if (tt->parentsLabel.size()==depth) 
					tt->parentsLabel.push_back(*label);
				else
					tt->parentsLabel.at(depth)=*label;
			  #else
              tt->set_lenTail(extension);
              #endif
              closed_subtree_mining(tt);  
              } else delete tt; 
           if (tt->get_Support()==t->get_Support()){
              is_closed=false;
              }
           } else delete tt; 
        #if defined(LABELLED)    
    	}
    	#endif
    } 
    if (is_closed){
       //Check that it is not included in subtrees of mClosedMapTree               
       for(mapTreeIterator cpos = mClosedMapTree->begin(); cpos != mClosedMapTree->end(); ++cpos ) {
           Tree *tpos=cpos->second;
		   if (tpos->get_Support()==t->get_Support()) {
			   if (tpos->get_Size()>t->get_Size()){
				   if (t->isSubtreeOf(tpos->get_TreeNodes())) is_closed=false;
			       }
				else{
					if (tpos->isSubtreeOf(t->get_TreeNodes())) {
						   mClosedMapTree->erase(cpos);
					   }
					}
				}
			else{ //Check for maximals
			   if (tpos->get_Size()>t->get_Size()){
				   if (t->isSubtreeOf(tpos->get_TreeNodes())) t->set_isNotMaximal(true);
			       }
				 else{
					if (tpos->isSubtreeOf(t->get_TreeNodes())) {
						   ((cpos->second))->set_isNotMaximal(true);
					   }
					} 
				}
			}	
           
       if (is_closed) {
          int iSup=t->get_Support();
          #if !defined(LABELLED)
          if (t->get_Size()==2) {
          		iSup=mdataset_reader->size()-mdataset_reader->oneTreesDataSet_size() ;
          		t->set_Support(iSup);
          }
          #endif
          mClosedMapTree->insert(pair<int,Tree *>(iSup,t) );

       }
        else delete t;
    }
    else 
    	delete t;             
}
	
////////////////////////////////////////////////////////////////////////////////


void TreeMining::Compress_MapTree(){
	for( mapTreeIterator iter = mClosedMapTree->begin(); iter != mClosedMapTree->end(); ++iter ) {
			iter->second->compress();
		}
}


