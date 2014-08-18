/*
 *      ClosedTrees.cpp
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


#include "ClosedTrees.h"
#define CORRECTION .05 

ClosedTrees::ClosedTrees(Support_Factory *support_factory, TreeFactory *tree_factory, bool is_Adaptive2, int *BatchSize,int *WindowSize, double dblmin_sup){
	LatticeTreeList = new mapTree;
	LatticeSupLenTree = new mapSupLenTree;
	mis_Adaptive2=is_Adaptive2;
	mWindowSize=WindowSize;
	mBatchSize=BatchSize;
	msupport_factory=support_factory;
	mtree_factory=tree_factory;
	mdblmin_sup=dblmin_sup;
	mChange=0;
}

////////////////////////////////////////////////////////////////////////////////

void ClosedTrees::print(){
	//Prints Lattice
	cout <<"LatticeTreeList"<<endl;
		for( mapTreeIterator iter = LatticeTreeList->begin(); iter != LatticeTreeList->end(); ++iter ) {	
		if (!isInsufficientRelaxedInputSupport(iter->second->get_Support())) (iter->second)->cout_print();	
	}
	cout <<"LatticeSupLenTree"<<endl;
	for( mapSupLenTreeIterator iter1 = LatticeSupLenTree->begin(); iter1 != LatticeSupLenTree->end(); ++iter1 ) {	
		if (!isInsufficientRelaxedInputSupport(iter1->first))	{
			for( mapTreeIterator iter = iter1->second.begin(); iter != iter1->second.end(); ++iter ) {	
				cout <<"Support "<< (iter->second->get_Support())<< ":" ;
				(iter->second)->cout_print();	
			}
			//cout <<"-------------------"<<endl;
			}
		}
	cout <<"====================="<<endl;
}

////////////////////////////////////////////////////////////////////////////////

void ClosedTrees::insert(TreeNodes *tn, int iSupport, bool blUpdate){
		//Insert into LatticeTreeList and mapSupLenTree	
		//Called from: AdaTreeNat_Mining, Add and recursive_Add
		
		//Insert into LatticeTreeList	
		Tree* LatticeNode=mtree_factory->create_tree();
		LatticeNode->set_TreeNodes(*tn);
		LatticeNode->set_Update(blUpdate); //false in init, true in add
		LatticeNode->set_Support(iSupport);
		if (mis_Adaptive2){
			LatticeNode->AdSupport= new Adwin(5);
			LatticeNode->AdSupport->update(msupport_factory->RelativeSupport(iSupport));	
		} 
		LatticeTreeList->insert(pair<int,Tree *>(tn->size(),LatticeNode));
		
		//Insert into mapSupLenTree
		insertMapSupLenTree(LatticeNode, iSupport);
	}

////////////////////////////////////////////////////////////////////////////////
	
void ClosedTrees::insertMapSupLenTree(Tree* LatticeNode, int iSupport){	
		//Insert into mapSupLenTree
		//Called from: insert, UpdateSupportAdd, UpdateSupportDelete, UpdateSupport
		mapSupLenTreeIterator p=LatticeSupLenTree->find(iSupport);		
		if (p==LatticeSupLenTree->end()) {
			mapTree *mT = new mapTree;
			mT->insert(pair<int,Tree *>(LatticeNode->get_Size(),LatticeNode));
			LatticeSupLenTree->insert(pair<int,mapTree> (iSupport,*mT));
		}
		else{
		     p->second.insert(pair<int,Tree *>(LatticeNode->get_Size(),LatticeNode));
		}
	}

////////////////////////////////////////////////////////////////////////////////

void ClosedTrees::eraseMapSupLenTree(TreeNodes tn, int iSupport){	
	//Delete a tree from MapSupLenTree
	//Called from: UpdateSupportAdd, UpdateSupportDelete, UpdateSupport
		
	mapSupLenTreeIterator mp=LatticeSupLenTree->find(iSupport);	
	int aux=0;	
	if (mp!=LatticeSupLenTree->end()) {
		mapTree *mT = &mp->second;
		for (mapTreeIterator p = mT->lower_bound(tn.size()); p != mT->upper_bound(tn.size()); p++){
			if (p->second->get_TreeNodes_()==tn) {
				//cout <<"eraseMapSupLenTree: Delete " <<aux++<<" times with support: "<<iSupport<<" "; p->second->print_l(tn);cout <<" - "; p->second->cout_print();
				mT->erase(p);
				
				break;
			}
		}
	}
	if (mp->second.size()==0) LatticeSupLenTree->erase(mp);
}		

////////////////////////////////////////////////////////////////////////////////

bool ClosedTrees::updateSupportAdd(TreeNodes tn,int incSupport){
	//Update suport in Lattice: LatticeTreeList and MapSupLenTree
	//Called from: Add and recursive_Add
	int l=tn.size();
	bool blFound=false; 
		TreeNodes x;
	for (mapTreeIterator p = LatticeTreeList->lower_bound(l); p != LatticeTreeList->upper_bound(l); p++){
		if (p->second->get_TreeNodes_()==tn) {
			blFound=true;
			int NewSupport;
			if (mis_Adaptive2){
				if (p->second->AdSupport->update((double)((double)incSupport)/((double)*mBatchSize)))
					mChange++; 
				NewSupport=	msupport_factory->RelaxedSupport(p->second->AdSupport->getEstimation());
			} else {
				NewSupport=msupport_factory->RelaxedSupport(int (((double)msupport_factory->RelativeSupport(p->second->get_Support())*((double)*mWindowSize-(double)*mBatchSize) + (double)incSupport)+CORRECTION),*mWindowSize);		
			}
			p->second->set_Update(true);			
			if (p->second->get_Support()!=NewSupport) {
				int OldSupport=p->second->get_Support();
				p->second->set_Support(NewSupport);				
				eraseMapSupLenTree(tn,OldSupport);
				insertMapSupLenTree(p->second,NewSupport);
			}		
			break;
		}
	}
	return blFound;
}

////////////////////////////////////////////////////////////////////////////////

TreeNodes ClosedTrees::closure(Tree *t, int *support){
   //Closure: smallest size super-tree 
   //Called from: Add and recursive_Add
   TreeNodes tn;
   for (mapTreeIterator p = LatticeTreeList->lower_bound(t->get_Size()+1); p != LatticeTreeList->end(); p++){
		if (t->isSubtreeOf(p->second->get_TreeNodes())) { 
			*support=p->second->get_Support();
			tn= p->second->get_TreeNodes_();
			break;
   		}
	}
   return tn;
}

////////////////////////////////////////////////////////////////////////////////

void ClosedTrees::UpdateSupport(int intOption) {
	//Update support for all trees without updated support
	//Called from: Add and Delete
	for (mapTreeIterator p = LatticeTreeList->begin(); p != LatticeTreeList->end();){
		bool blErased=false;
		if (p->second->get_Update()==true)
			p->second->set_Update(false);
		else{			
			//Calculate new support
			int NewSupport;
			if (intOption==ADD_OPTION) {
				if (mis_Adaptive2) {
					if (p->second->AdSupport->update(0)) 
						mChange++;
					NewSupport= msupport_factory->RelaxedSupport(p->second->AdSupport->getEstimation());
				} else {
					NewSupport=msupport_factory->RelaxedSupport((int) (((double)msupport_factory->RelativeSupport(p->second->get_Support())*((double)*mWindowSize-(double)*mBatchSize))+CORRECTION),*mWindowSize);
				}					
			}
			else //(intOption==DELETE_OPTION) 
				NewSupport=msupport_factory->RelaxedSupport((int)((double)msupport_factory->RelativeSupport(p->second->get_Support())*((double)*mWindowSize +(double)*mBatchSize)+CORRECTION),*mWindowSize);				
			
			//Check if support has changed and if there is enough support

			if (p->second->get_Support()!=NewSupport) {
				int OldSupport=p->second->get_Support();				
				eraseMapSupLenTree(p->second->get_TreeNodes_(),OldSupport);
				p->second->set_Support(NewSupport);					
				if (isInsufficientRelaxedSupport(NewSupport) ) {
					mapTreeIterator oldp=p;
					p++;
					blErased=true;
					delete oldp->second;
					LatticeTreeList->erase(oldp);
					}
				else {				
					insertMapSupLenTree(p->second,NewSupport);
				}
			}				
		}
		if (blErased==false)
				p++;		
	}
}

////////////////////////////////////////////////////////////////////////////////

void ClosedTrees::updateSupportDelete(TreeNodes tn,int incSupport, listTree *TreesCheckClosureList){
	//Update suport in Lattice: LatticeTreeList and MapSupLenTree
	//Called from: Delete and recursive_Delete
   int l=tn.size();
   for (mapTreeIterator p = LatticeTreeList->lower_bound(l); p != LatticeTreeList->upper_bound(l); p++){
	if (p->second->get_TreeNodes_()==tn) {
		int NewSupport=msupport_factory->RelaxedSupport((int)((double)msupport_factory->RelativeSupport(p->second->get_Support())*((double)*mWindowSize+(double)*mBatchSize) - (double)incSupport+CORRECTION),*mWindowSize);			
		p->second->set_Update(true);	

		if (p->second->get_Support()!=NewSupport) {
			int OldSupport=p->second->get_Support();
			p->second->set_Support(NewSupport);
			eraseMapSupLenTree(tn,OldSupport);
			if (isInsufficientRelaxedSupport(NewSupport)) {
				delete p->second; 
				LatticeTreeList->erase(p);
				break;
				}
			else {
				insertMapSupLenTree(p->second,NewSupport);
			}
		}
		TreesCheckClosureList->push_back(p->second);						
		break;
		}
	}	
}

////////////////////////////////////////////////////////////////////////////////

void ClosedTrees::EraseNotClosedTrees() {
	//For all supports
	listTree TreesCheckClosureList; 
	for(mapSupLenTreeIterator mp=LatticeSupLenTree->begin(); mp!=LatticeSupLenTree->end(); mp++) {
		mapTree *mT = &mp->second;
		bool SupportModified=false;	
		//For all trees with the same support		
		for (mapTreeIterator p = mT->begin(); p != mT->end();p++ ){
			bool blnFound=false;
			//Find a supertree with the same support
			for (mapTreeIterator pp = mT->upper_bound(p->second->get_Size()+1); pp != mT->end(); pp++)	{			
				if (p->second->isSubtreeOf(pp->second->get_TreeNodes())) {
					//It's not closed, so we delete it from the lattice
					TreesCheckClosureList.push_back(p->second);
					blnFound=true;
					break;
				}
			}
			if (blnFound) {
				SupportModified=true;
			}
		}	
	};
	Check_Closure_List(&TreesCheckClosureList);

}

////////////////////////////////////////////////////////////////////////////////

void ClosedTrees::Check_Closure_List(listTree *TreesCheckClosureList){
	for(listTreeIterator lIcandidate=TreesCheckClosureList->begin(); lIcandidate!=TreesCheckClosureList->end(); lIcandidate++) {
		//Find a supertree with the same support
		TreeNodes *t1=(*lIcandidate)->get_TreeNodes();
		mapSupLenTreeIterator mp=LatticeSupLenTree->find((*lIcandidate)->get_Support());		
		if (mp!=LatticeSupLenTree->end()) {
			mapTree *mT = &mp->second;
			int icandidateSize=(*lIcandidate)->get_Size();
			bool blnFound=false;
			for (mapTreeIterator p = mT->lower_bound(icandidateSize+1); p != mT->end(); p++){
				if ((*lIcandidate)->isSubtreeOf(p->second->get_TreeNodes())) {
					//It's not closed, so we delete it from the lattice
					for (mapTreeIterator pt = LatticeTreeList->lower_bound(icandidateSize); pt != LatticeTreeList->upper_bound(icandidateSize); pt++){				
						//find a supertree 	
						TreeNodes *t2=(pt->second)->get_TreeNodes();			
						if (*t1==*t2) {
							LatticeTreeList->erase(pt); 
							blnFound=true;
							break;
						}
					}
				}
				if (blnFound==true) break;
			}
			if (blnFound) {
				for (mapTreeIterator p = mT->lower_bound(icandidateSize); p != mT->upper_bound(icandidateSize); p++){
					TreeNodes *t2=(p->second)->get_TreeNodes();
					if (*t1==*t2){
						delete p->second;
						mT->erase(p);
						break;
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void ClosedTrees::Check_Closure_List_Delete(listTree *TreesCheckClosureList){
	for(listTreeIterator lIcandidate=TreesCheckClosureList->begin(); lIcandidate!=TreesCheckClosureList->end(); lIcandidate++) {
		//Find a supertree with the same support
		TreeNodes *t1=(*lIcandidate)->get_TreeNodes();
		mapSupLenTreeIterator mp=LatticeSupLenTree->find((*lIcandidate)->get_Support());	
		if (mp!=LatticeSupLenTree->end()) {
			mapTree *mT = &mp->second;
			int icandidateSize=(*lIcandidate)->get_Size();
			bool blnFound=false;
			for (mapTreeIterator p = mT->lower_bound(icandidateSize+1); p != mT->end(); p++){
				if ((*lIcandidate)->isSubtreeOf(p->second->get_TreeNodes())) {
					//It's not closed, so we delete it from the lattice
					for (mapTreeIterator pt = LatticeTreeList->lower_bound(icandidateSize); pt != LatticeTreeList->upper_bound(icandidateSize); pt++){				
						//find a supertree 	
						TreeNodes *t2=(pt->second)->get_TreeNodes();				
						if (*t1==*t2) {
							LatticeTreeList->erase(pt); 
							blnFound=true;
							break;
						}
					}
				}
				if (blnFound==true) break;
			}
			if (blnFound) {
				//Delete Candidate in LatticeSupLenTree
				for (mapTreeIterator p = mT->lower_bound(icandidateSize); p != mT->upper_bound(icandidateSize); p++){
					TreeNodes *t2=(p->second)->get_TreeNodes();
					if (*t1==*t2){
						delete p->second;
						mT->erase(p);
						break;
					}
				}
			}
		}
	}
}
