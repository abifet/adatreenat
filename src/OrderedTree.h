/*
 *      OrderedTree.h
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

#ifndef _orderedtree_h_
#define _orderedtree_h_
typedef int Integer;

class OrderedTree: public Tree{
	protected:
	struct SupportId{
      int Id;
	  Integer iNode; //Node occurrence
      Integer sup; //Position of subtree
      Integer pos; //Position of tree
	};
	typedef list<SupportId> ListSupport;
	public:
		OrderedTree();
		OrderedTree(OrderedTree *t);
		OrderedTree(TreeDataSet* D);
		OrderedTree(TreeDataSet* D,int label);
		inline void compress(){IdTrans.clear();}	
		inline int Id_Front(){return IdTrans.front().Id;};

		inline bool isCanonicalRepresentative(Node ext){return true;};
     	inline bool isSubtreeOf(TreeNodes *t0);  
		int supportInc(TreeDataSet *D,int min_sup);	

     	inline ListSupport* get_ListSupport(){return &IdTrans;}

	protected:
  		virtual bool isSubtreeInc(TreeNodes *supertree, Integer *post0,Integer *post1,Integer *iNode); 

	    ListSupport IdTrans;

}; 

class OrderedTree_TopDown: public OrderedTree{
	public:
		OrderedTree_TopDown():OrderedTree(){};
		OrderedTree_TopDown(OrderedTree_TopDown *t):OrderedTree((OrderedTree*)t){};
		OrderedTree_TopDown(TreeDataSet* D):OrderedTree(D){};
		OrderedTree_TopDown(TreeDataSet* D,int label);
	private:
		bool isSubtreeInc(TreeNodes *supertree, Integer *post0,Integer *post1,Integer *iNode);  
};

#endif
