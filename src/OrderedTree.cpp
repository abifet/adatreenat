/*
 *      OrderedTree.cpp
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

#include "OrderedTree.h"

OrderedTree::OrderedTree(): Tree(){ 
	// Implementation of constructor: Initialize an empty tree 
	}
		
////////////////////////////////////////////////////////////////////////////////

OrderedTree::OrderedTree(OrderedTree *t): Tree(t){
	// Implementation of constructor: Initialize a tree as a copy of a tree t
	IdTrans=t->IdTrans; 

     }  
	
////////////////////////////////////////////////////////////////////////////////
    
inline bool OrderedTree::isSubtreeOf(TreeNodes *t0){
	//NonIncremental version
	Integer p0=0; Integer p1=0; Integer iN=0;
	return isSubtreeInc(t0,&p0,&p1,&iN); 
}
	
////////////////////////////////////////////////////////////////////////////////

#if defined(LABELLED)
OrderedTree::OrderedTree(TreeDataSet* D):Tree(){};
OrderedTree::OrderedTree(TreeDataSet* D,int label):Tree(){
	// Implementation of constructor: Initialize init_tree
	Integer min_depth=0;
	Node init_node( min_depth,label);
	extend(init_node);
	SupportId supp;
	supp.sup=min_depth;
	supp.pos=min_depth;
	supp.iNode=0;
	Support=0;
	int i=0;
	if (label==60) cout<<"Size: "<<D->size()<<endl;
	for(TreeDataSet::iterator data_it=D->begin();data_it!=D->end();data_it++) {
		int j=0;
		bool hies=false;
		for(TreeNodes::iterator node_it=data_it->begin();node_it!=data_it->end();node_it++) { 
			if (node_it->label==label){ //Bottom_up
				Support++;               
				supp.Id=i;
				supp.sup=min_depth;
				supp.pos=j;
				supp.iNode=j;
				IdTrans.push_back(supp);
				hies=true;
				break;
			}
			j++;
		}
		i++;
	}
	lenTail=min_depth;
	isNotMaximal=false;
	Update=false;
	
}
	
////////////////////////////////////////////////////////////////////////////////

bool OrderedTree::isSubtreeInc(TreeNodes *supertree, Integer *post0,Integer *post1,Integer *iNode){
	//Incremental Version: Is tree a subtree of supertree?
	//We test for each possible subtree rooted at any node of the tree 
	Integer Size0=supertree->size();
	Integer Size1=tree.size();
	if (*post0==Size0) {
		(*iNode)++;	
		(*post1)=0; //return to the begining of the tree 
		(*post0)=(*iNode);//Start at the node
	}
	if (Size0-*iNode<Size1||*post1==Size1) return false;
	Node st0;
	Node st1;
	Node zero(0,0); //Change
	while(Size0-*iNode>=Size1) { //While we didn't begin in all nodes of the tree 
		bool blnend=false;
		st1=tree[*post1];
		st0=(*supertree)[*post0] -(*supertree)[*iNode];
		for (;;st0=(*supertree)[++(*post0)]-(*supertree)[*iNode]){//Decrement the depth of the subtree to compare
			if (st0<=zero)  
				if (blnend==false) 
					blnend=true; //First node is not the end
				else {
					break;} //End of the subtree test for this node, test next iNode
			if (st1.depth>st0.depth){ 
				while ( *post1>0 && (st1.depth>st0.depth || (st1.depth==st0.depth && st0.label!=st1.label)))
					st1=tree[--(*post1)]; 
				if (st1<st0) 
					st1=tree[++(*post1)];
			}
			if (st0==st1) {
				if (*post1!=Size1-1) 
					st1=tree[++(*post1)]; 
					
				else {
					(*post1)++;
					(*post0)++;
					return true;
				}
			}
			if (*post0==Size0-1) {
				++(*post0);break;
			}
		}
		if ((*post1==Size1)) {
			return true;}	
		(*iNode)++;	
		(*post1)=0; //return to the begining of the tree 
		(*post0)=(*iNode);//Start at the node
	}	
	return (*post1==Size1);   
	}
 #else
 	
////////////////////////////////////////////////////////////////////////////////

 OrderedTree::OrderedTree(TreeDataSet* D,int label):Tree(){};
 OrderedTree::OrderedTree(TreeDataSet* D):Tree(){
	// Implementation of constructor: Initialize init_tree
	Integer min_depth=1;
	for (Integer i=1;i<=min_depth;i++) 
		extend(i);
	SupportId supp;
	supp.sup=min_depth;
	supp.pos=min_depth;
	supp.iNode=0;
	Support=0;
	for (int i=0;i<D->size();i++) {
		if (D->at(i).size()>1){
			Support++;               
			supp.Id=i;
			IdTrans.push_back(supp);
		}
	}
	lenTail=min_depth;
	isNotMaximal=false;
	Update=false;
	
    } 
	
////////////////////////////////////////////////////////////////////////////////
        
bool OrderedTree::isSubtreeInc(TreeNodes *supertree, Integer *post0,Integer *post1,Integer *iNode){
	//Incremental Version: Is tree a subtree of supertree?
	//We test for each possible subtree rooted at any node of the tree 
	Integer Size0=supertree->size();
	Integer Size1=tree.size();
	if (*post0==Size0) {
		(*iNode)++;	
		(*post1)=0; //return to the begining of the tree 
		(*post0)=(*iNode);//Start at the node
	}
	if (Size0-*iNode<Size1||*post1==Size1) return false;
	Integer st0;
	Integer st1;
	while(Size0-*iNode>=Size1) { //While we didn't begin in all nodes of the tree 
		bool blnend=false;
		st1=tree[*post1];
		st0=(*supertree)[*post0] -(*supertree)[*iNode];
		for (;;st0=(*supertree)[++(*post0)]-(*supertree)[*iNode]){//Decrement the depth of the subtree to compare
			if (st0<=0)  
				if (blnend==false) 
					blnend=true; //First node is not the end
				else {
					break;} //End of the subtree test for this node, test next iNode
			if (st1>st0) while (st1!=st0 && *post1>0) st1=tree[--(*post1)]; 
			if (st0==st1) {
				if (*post1!=Size1-1) 
					st1=tree[++(*post1)];
					
				else {
					(*post1)++;
					(*post0)++;
					
					return true;
				}
			}
			if (*post0==Size0-1) {
				++(*post0);break;}
			}
		if ((*post1==Size1)) {
			return true;}	
		(*iNode)++;	
		(*post1)=0; //return to the begining of the tree 
		(*post0)=(*iNode);//Start at the node
	}	
	return (*post1==Size1);   
	}
#endif
	
////////////////////////////////////////////////////////////////////////////////
	
int OrderedTree::supportInc(TreeDataSet *D,int min_sup){
	//Incremental version : calculates support         
	int isup=0;
	int deleted=0;
	ListSupport::iterator aIdList;
	for (aIdList=IdTrans.begin();aIdList!=IdTrans.end();) {
		//For each tree in the list 
		if (isSubtreeInc(&(*D)[aIdList->Id],&aIdList->pos,&aIdList->sup,&aIdList->iNode)) {
			//Increment support if it is subtree
			isup++;
			++aIdList;
		}
		else {
			aIdList=IdTrans.erase(aIdList);
			deleted++;
		}
		if (Support-deleted<min_sup) {
			Support=0;
			return 0;
		}; //Is not frequent;
	}
	Support=isup;  
	return isup;
}  
	
////////////////////////////////////////////////////////////////////////////////

#if defined(LABELLED)
OrderedTree_TopDown::OrderedTree_TopDown(TreeDataSet* D,int label):OrderedTree(){
	// Implementation of constructor: Initialize init_tree
	Integer min_depth=0;
	Node init_node( min_depth,label);
	extend(init_node);
	SupportId supp;
	supp.sup=min_depth;
	supp.pos=min_depth;
	supp.iNode=0;
	Support=0;
	for (int i=0;i<D->size();i++) {
		if (D->at(i).at(0)==init_node){ //TopDown
			Support++;               
			supp.Id=i;
			IdTrans.push_back(supp);
		}
	}
	lenTail=min_depth;
	isNotMaximal=false;
	Update=false;
	
    }  
	
////////////////////////////////////////////////////////////////////////////////

bool OrderedTree_TopDown::isSubtreeInc(TreeNodes *supertree, Integer *post0,Integer *post1,Integer *iNode){
	//Incremental Version: Is tree a subtree of supertree?
	Integer Size0=supertree->size();
	Integer Size1=tree.size();
	if (Size0<Size1||*post0==Size0||*post1==Size1) return false;

	Node st0= (*supertree)[*post0]; 
	Node st1=tree[*post1]; 
	bool blnend=false;
	for (;blnend==false;st0=(*supertree)[++(*post0)]){ 
		if (st1.depth>st0.depth ) {
			while (st1.depth>st0.depth || (st1.depth==st0.depth && st0.label!=st1.label)) 
				st1=tree[--(*post1)];
			if (st1<st0) 
				st1=tree[++(*post1)];
		}
		if (st0==st1) {
			if (*post1!=Size1-1) 
				st1=tree[++(*post1)];
			else {
				(*post1)++;
				(*post0)++;
				break;
				}
			}
		if (*post0==Size0-1) {++(*post0);break;}
		}  
	return (*post1==Size1);   
	}	
#else
	
////////////////////////////////////////////////////////////////////////////////

OrderedTree_TopDown::OrderedTree_TopDown(TreeDataSet* D,int label):OrderedTree(){};
bool OrderedTree_TopDown::isSubtreeInc(TreeNodes *supertree, Integer *post0,Integer *post1,Integer *iNode){
	//Incremental Version: Is tree a subtree of supertree?
	Integer Size0=supertree->size();
	Integer Size1=tree.size();
	if (Size0<Size1||*post0==Size0||*post1==Size1) return false;

	Integer st0= (*supertree)[*post0];
	Integer st1=tree[*post1];
	bool blnend=false;
	for (;blnend==false;st0=(*supertree)[++(*post0)]){
		if (st1>st0) while (st1!=st0) st1=tree[--(*post1)];
		if (st0==st1) {
			if (*post1!=Size1-1) 
				st1=tree[++(*post1)];
			else {
				(*post1)++;
				(*post0)++;
				break;
				}
			}
		if (*post0==Size0-1) {++(*post0);break;}
		}   
	return (*post1==Size1);   
	}
#endif
