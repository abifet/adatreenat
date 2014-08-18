/*
 *      AdaTreeNat_Mining.cpp
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


#include "AdaTreeNat_Mining.h"

AdaTreeNat_Mining::AdaTreeNat_Mining(DatasetReader* dataset_reader,bool is_Relaxed_Support, bool is_Log_Relaxed_Support,
		int NumberRelaxedIntervals, double relative_min_sup, int BatchSize, TreeFactory *tree_factory,
		bool is_Adaptive1, bool is_Adaptive2, int SlidingWindowSize_orig, int min_sup_orig, bool is_SlidingWindow) { 
			    

	mBatchSize=BatchSize;
	int min_sup=min_sup_orig;
	int SlidingWindowSize=SlidingWindowSize_orig;
	int WindowSize=BatchSize;
	mWindowSize=&WindowSize;
	mdataset_reader= dataset_reader;

	mdblmin_sup=relative_min_sup;
	mtree_factory=tree_factory;
	if (is_Relaxed_Support==true){
		if (is_Log_Relaxed_Support==true) 
			support_factory = new Log_Relaxed_Support_Factory(NumberRelaxedIntervals, mdblmin_sup); 
		else
			support_factory = new Linear_Relaxed_Support_Factory(NumberRelaxedIntervals, mdblmin_sup); 
	} else {
			support_factory = new Linear_Relaxed_Support_Factory(SlidingWindowSize, mdblmin_sup); 
	}
	mclosedTrees = new ClosedTrees(support_factory, tree_factory, is_Adaptive2, &mBatchSize, mWindowSize, mdblmin_sup);

	//AdaTreeNat Algorithm
	
	 //Init sliding Window
	list<TreeDataSet> SlidingWindow;
	#if defined(LABELLED)	
	list<TreeNodesAux> SlidingWindowAux;
	#endif
	if ( is_SlidingWindow==true ){
		SlidingWindow.push_back(dataset_reader->get_dataset());
		#if defined(LABELLED)	
			SlidingWindowAux.push_back(dataset_reader->get_dataset_aux());;
		#endif
	}
	int NumberofBatches=0;	
	

	
	//Mining first batch of data
	TreeMining *closed_tree_mining= new TreeMining(tree_factory, dataset_reader, min_sup);
	mapTree *ClosedMapTree= closed_tree_mining->get_ClosedMapTree();
	for( mapTreeIterator iter = ClosedMapTree->begin(); iter != ClosedMapTree->end(); ++iter ) {
		mclosedTrees->insert(iter->second->get_TreeNodes(),support_factory->RelaxedSupport(iter->first,mBatchSize),false);
	}
	mclosedTrees->EraseNotClosedTrees(); // Cleans not closed items due to new relaxed support

	//Init Lattice	
	closed_tree_mining->Compress_MapTree();	
	cout<< NumberofBatches<<";"<< mclosedTrees->get_size()<<";"<<closed_tree_mining->get_ClosedMapTree()->size()<<endl;
	delete closed_tree_mining;

	Adwin AdNumberClosedTrees(5);
	if ( is_Adaptive1==true ){
		AdNumberClosedTrees.update(mclosedTrees->NumberClosedTrees());
		SlidingWindowSize=2*BatchSize;	
	}

	bool blEndOfFile=false;
	while (!blEndOfFile){
		if ( is_SlidingWindow==true ){
			//Delete transactions
			
			if (WindowSize>=SlidingWindowSize){
				for (int i=WindowSize-SlidingWindowSize;i>=0;i-=BatchSize){
					WindowSize-=BatchSize;			
					TreeDataSet D = SlidingWindow.front();
					SlidingWindow.pop_front();
					#if defined(LABELLED)	
					TreeNodesAux Daux = SlidingWindowAux.front();
					SlidingWindowAux.pop_front();
					#endif
					min_sup= (int) (relative_min_sup*BatchSize);
					if (min_sup<1) min_sup=1;	
					#if defined(LABELLED)
					Delete(&D,&Daux,min_sup);	
					#else		
					Delete(&D,min_sup);
					#endif
				}
			}
		}	
		
		//Add transactions 
		blEndOfFile=dataset_reader->read_Next_Batch();
		mBatchSize=dataset_reader->size(); 
		if (dataset_reader->size()==0) break;
		min_sup= (int) (relative_min_sup*dataset_reader->size());
		if (min_sup<1) min_sup=1;
		
		//Add treedataset to Sliding Window
		if ( is_SlidingWindow==true ){
			SlidingWindow.push_back(dataset_reader->get_dataset()); 
			#if defined(LABELLED)	
			SlidingWindowAux.push_back(dataset_reader->get_dataset_aux());;
			#endif
		}
		NumberofBatches++;
		WindowSize+=mBatchSize;  
		

		closed_tree_mining= new TreeMining(tree_factory, dataset_reader, min_sup);
		closed_tree_mining->Compress_MapTree();	
		Add(closed_tree_mining->get_ClosedMapTree());
		delete closed_tree_mining;
		
		if ( is_Adaptive2==false ){
			mclosedTrees->EraseNotClosedTrees();
		}

		//ADWIN1
		if (is_Adaptive1==true ){
			if (AdNumberClosedTrees.update(mclosedTrees->NumberClosedTrees()))
				SlidingWindowSize=AdNumberClosedTrees.length()*BatchSize;
			else 
				SlidingWindowSize+=BatchSize;
		} 

		//ADWIN2
		if ( is_Adaptive2 == true ){
			if (mclosedTrees->get_Change()>1) {
				mclosedTrees->set_Change(0);
				mclosedTrees->EraseNotClosedTrees();
				}
		}
		if (NumberofBatches>2) cout<< NumberofBatches<<";"<<mclosedTrees->NumberClosedTrees()<<";"<<closed_tree_mining->get_ClosedMapTree()->size()<<";"<<SlidingWindowSize<<endl;
	}
	mclosedTrees->print();
}

////////////////////////////////////////////////////////////////////////////////

void AdaTreeNat_Mining::Add(mapTree *ClosedMapTree){	
	list<TreeNodes*> list_Insert_TreeNodes;
	list<int> list_Insert_Support;
	TreeDataSet *D;
	SupportContainer *SupportTreesDataSet =new SupportContainer;
	D = MapTreeToDataset(ClosedMapTree,SupportTreesDataSet);
	
	#if defined(LABELLED)	
        for(Labels_iterator label=mdataset_reader->get_labels()->begin(); label != mdataset_reader->get_labels()->end(); ++label){
			Tree*t =mtree_factory->create_tree(D,*label);
			if (t->get_Support()==0) break;
			t->parentsLabel.push_back(*label);
	#else
	Tree *t= mtree_factory->create_tree(D); 
	#endif
	int SupportT2=SupportTreesDataSet->at(t->Id_Front());	
		
	// if it is In Lattice, updateSupport otherwise insert it
	if (mclosedTrees->updateSupportAdd(t->get_TreeNodes_(), SupportT2)==false) {
		int closure_support=0;
		TreeNodes treeclosure=mclosedTrees->closure(t,&closure_support);						
		int AbsSupport= (int) (support_factory->RelativeSupport(closure_support)*(*mWindowSize-mBatchSize)+SupportT2);
		if ( AbsSupport>mdblmin_sup* (*mWindowSize)){						
			if (treeclosure.size()<=1 && t->get_Size()==D->at(t->Id_Front()).size()) {
				//There is no supertree with the same support	
				list_Insert_TreeNodes.push_back(t->get_TreeNodes());
				list_Insert_Support.push_back(support_factory->RelaxedSupport(SupportT2,*mWindowSize));											
			}
			else if (treesIntersection(treeclosure,D->at(t->Id_Front())) == t->get_TreeNodes_()){
				//Insert a new closed tree into Lattice
				list_Insert_TreeNodes.push_back(t->get_TreeNodes());
				list_Insert_Support.push_back(support_factory->RelaxedSupport(AbsSupport,*mWindowSize));
				
			}
		}
	}		
   
	recursive_Add(t,D,SupportTreesDataSet,&list_Insert_TreeNodes,&list_Insert_Support);
	#if defined(LABELLED)	
        }
    #endif	
	
	//Add new closed trees
	list<int>::iterator it_sup=list_Insert_Support.begin();
	for (list<TreeNodes*>::iterator it=list_Insert_TreeNodes.begin();it!=list_Insert_TreeNodes.end();it++){
		mclosedTrees->insert(*it, *it_sup);
		it_sup++;
	}
	mclosedTrees->UpdateSupport(ADD_OPTION);
}

////////////////////////////////////////////////////////////////////////////////

void AdaTreeNat_Mining::recursive_Add(Tree *t,TreeDataSet *D,SupportContainer *SupportDataset, list<TreeNodes*> *list_Insert_TreeNodes, list<int> *list_Insert_Support){		
	Integer len_tail=t->get_lenTail();
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
			tt->supportInc(D,1);
			  #if defined(LABELLED) 				
			  tt->set_lenTail(depth);
				if (tt->parentsLabel.size()==depth) 
					tt->parentsLabel.push_back(*label);
				else
					tt->parentsLabel.at(depth)=*label;	
			  #else
              tt->set_lenTail(extension);
              #endif
			
			if (tt->get_Support()!=0) {
				int SupportT2=SupportDataset->at(tt->Id_Front());

				if (mclosedTrees->updateSupportAdd(tt->get_TreeNodes_(), SupportT2)==false) {
					//Check closure
					int closure_support=0;
					TreeNodes treeclosure=mclosedTrees->closure(tt,&closure_support);
					int AbsSupport= (int) (support_factory->RelativeSupport(closure_support)*(*mWindowSize-mBatchSize)+SupportT2);
					if ( AbsSupport>mdblmin_sup* (*mWindowSize)){					
						if (treeclosure.size()<=1 && tt->get_Size()==D->at(tt->Id_Front()).size()) {
							list_Insert_TreeNodes->push_back(tt->get_TreeNodes());
							list_Insert_Support->push_back(support_factory->RelaxedSupport(SupportT2,*mWindowSize));							
						}					
						else if (treeclosure.size()>0 &&treesIntersection(treeclosure,D->at(tt->Id_Front())) == tt->get_TreeNodes_()){

							list_Insert_TreeNodes->push_back(tt->get_TreeNodes());
							list_Insert_Support->push_back(support_factory->RelaxedSupport(AbsSupport,*mWindowSize));
						}
					}
				}
				recursive_Add(tt,D, SupportDataset,list_Insert_TreeNodes, list_Insert_Support);
			}
		}
	} 
	#if defined(LABELLED)    
    } 
    #endif
}

////////////////////////////////////////////////////////////////////////////////

#if defined(LABELLED)
void AdaTreeNat_Mining::Delete(TreeDataSet *D,TreeNodesAux *Daux, int min_sup){
#else
void AdaTreeNat_Mining::Delete(TreeDataSet *D,int min_sup){
#endif
	SupportContainer *SupportTreesDataSet =new SupportContainer;
	listTree *TreesCheckClosureList= new listTree;
	#if defined(LABELLED)	
	mDatasetAux=Daux;
	for(Labels_iterator label=mdataset_reader->get_labels()->begin(); label != mdataset_reader->get_labels()->end(); ++label){
		Tree* t=mtree_factory->create_tree(D,*label);
		if (t->get_Support()==0) break;
		t->parentsLabel.push_back(*label);
	#else
		Tree* t=mtree_factory->create_tree(D);
	#endif

	
	mclosedTrees->updateSupportDelete(t->get_TreeNodes_(), t->get_Support(),TreesCheckClosureList);
	recursive_Delete(t,D,min_sup,SupportTreesDataSet, TreesCheckClosureList);		

	delete t;
	#if defined(LABELLED)	
	}
	#endif
	
	mclosedTrees->UpdateSupport(DELETE_OPTION);
	mclosedTrees->Check_Closure_List_Delete(TreesCheckClosureList);		
	delete TreesCheckClosureList;

}

////////////////////////////////////////////////////////////////////////////////

void AdaTreeNat_Mining::recursive_Delete(Tree *t,TreeDataSet *D,int min_sup,SupportContainer *SupportDataset, listTree *TreesCheckClosureList){   		
	Integer len_tail=t->get_lenTail();
		//For  every tt  that can be extended from t in one step 
		  #if defined(LABELLED) 
    for (int depth=len_tail+1;depth>0;depth--){
		 Labels *labels;
		 int level=len_tail+1-depth;
		 if (depth==0)
			labels=mDatasetAux->get_labels();
		 else if (depth==1)
			labels=mDatasetAux->get_labels(t->currentAncestorLabel(level));
		 else if (depth==2)
			labels=mDatasetAux->get_labels(t->currentAncestorLabel(level),t->currentAncestorLabel(level+1));
		 else
			labels=mDatasetAux->get_labels(t->currentAncestorLabel(level),t->currentAncestorLabel(level+1),t->currentAncestorLabel(level+2));	
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
			  #if defined(LABELLED) 					
			  tt->set_lenTail(depth);
				if (tt->parentsLabel.size()==depth) 
					tt->parentsLabel.push_back(*label);
				else
					tt->parentsLabel.at(depth)=*label;
			  #else
              tt->set_lenTail(extension);
              #endif	
			if (tt->get_Support()!=0) {				
				mclosedTrees->updateSupportDelete(tt->get_TreeNodes_(), tt->get_Support(),TreesCheckClosureList);
				recursive_Delete(tt,D,min_sup, SupportDataset,TreesCheckClosureList);				
				}
		}
	} 
	#if defined(LABELLED)    
    }
    #endif
}

////////////////////////////////////////////////////////////////////////////////

TreeDataSet* AdaTreeNat_Mining::MapTreeToDataset(mapTree *ClosedMapTree, SupportContainer *SupportDataset) {
	 //MapTreeToDataset: Gets a TreeDataset from a mapTree        
	TreeDataSet *D=new TreeDataSet;

	for( mapTreeRevIterator iter = ClosedMapTree->rbegin(); iter != ClosedMapTree->rend(); ++iter ) {
		TreeNodes tn=(iter->second)->get_TreeNodes_();
		int Support=iter->first;
		D->push_back(tn);
		SupportDataset->push_back(Support);	
	} 
	return D;
}
	
////////////////////////////////////////////////////////////////////////////////

TreeNodes AdaTreeNat_Mining::treesIntersection(TreeNodes t1, TreeNodes t2){
	//Intersection: returns the intersection of two trees
    TreeDataSet *D=new TreeDataSet;
	
	D->push_back(t1);	
    D->push_back(t2);	

	DatasetReader* dataset_reader= new DatasetReader(D);
	TreeMining *closed_tree_mining= new TreeMining(mtree_factory, dataset_reader, 2);
    TreeNodes ret=(closed_tree_mining->get_ClosedMapTree()->begin()->second)->get_TreeNodes_();

	return 	ret;
}


