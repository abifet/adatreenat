/*
 *      UnorderedTree.h
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

#ifndef _unorderedtree_h_
#define _unorderedtree_h_

typedef multimap <int,int> Graph;
typedef map <int,int> BipartiteGraph;
map<pair<TreeNodes, TreeNodes>,bool> Is_subtree_Dictionary;
map<Graph,bool> Matching_Dictionary;

class UnorderedTree: public Tree{
	protected:
		struct SupportId{
		  int Id;
		  Integer iNode; //Node occurrence
		};
		typedef list<SupportId> ListSupport;
	public:
		UnorderedTree();
		UnorderedTree(UnorderedTree *t);
		UnorderedTree(TreeDataSet* D);
		UnorderedTree(TreeDataSet* D,int label);
		inline void compress(){IdTrans.clear();}	
		inline int Id_Front(){return IdTrans.front().Id;};

		bool isCanonicalRepresentative(Node ext);
     	bool isSubtreeOf(TreeNodes *t0);  
		int supportInc(TreeDataSet *D,int min_sup);	

     	inline ListSupport* get_ListSupport(){return &IdTrans;}
	    static TreeNodes CanonicalForm(TreeNodes t);

	protected:
		virtual bool isSubtreeUnOrdInc(TreeNodes *supertree, Integer *iNode);
		static bool isSubtree(TreeNodes *a, TreeNodes *b);
	    static bool recurse(int v, map <int, list<int> > *preds, map<int,int> *pred, list<int> *unmatched,BipartiteGraph *matching) ;
        static void bipartiteMatch(Graph *graph, BipartiteGraph *matching);
        static inline setTreeNodes TreeComponents(TreeNodes *t) {return TreeComponents(t,false);};
  		static setTreeNodes TreeComponents(TreeNodes *t,bool bCanonical);
  		static TreeNodes SubtreeAtNode(TreeNodes *supertree,Integer *iNode);

	    ListSupport IdTrans;
		vectorTreeNodes CanonicalNew;
		vectorTreeNodes CanonicalOld;

}; 

class UnorderedTree_TopDown: public UnorderedTree{
	public:
		UnorderedTree_TopDown():UnorderedTree(){};
		UnorderedTree_TopDown(UnorderedTree_TopDown *t):UnorderedTree((UnorderedTree*)t){};
		UnorderedTree_TopDown(TreeDataSet* D):UnorderedTree(D){};
		UnorderedTree_TopDown(TreeDataSet* D, int label);
	private:
		bool isSubtreeUnOrdInc(TreeNodes *supertree, Integer *iNode) {return isSubtree(&tree,supertree);};
};
#endif
