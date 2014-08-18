/*
 *      Tree.h
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

#ifndef _tree_h_
#define _tree_h_
typedef int Integer;

typedef vector<TreeNodes> vectorTreeNodes; //Canonical
typedef vector<TreeNodes> TreeDataSet; 
typedef multiset<TreeNodes> setTreeNodes; //TreeComponents

class Tree{
	public:
		Tree();
		Tree(Tree *t);
		virtual ~Tree();
		
		#if defined(LABELLED)
    	inline void extend(Node node){
           //Extends a tree one step : adds node at depth ext   
           tree.push_back(node);
    	};
    	int inline currentLabel(){
			return tree.back().label;
		};
		vector<int> parentsLabel;
		int inline currentAncestorLabel(int depth){return parentsLabel.at(lenTail-depth);};
		int inline currentFatherLabel(){return parentsLabel.at(lenTail-1);};
		int inline currentGrandFatherLabel(){return parentsLabel.at(lenTail-2);};
    	#else
    	inline void extend(Integer node){
           //Extends a tree one step : adds node at depth ext   
           tree.push_back(node);
    	};  	
		#endif
		
		void print(ofstream *ofile,DatasetReader *mdataset_reader);
		void cout_print();
		void print_l(TreeNodes  tree); // Debug
    	void printSupport(ofstream *ofile);
		virtual bool isCanonicalRepresentative(Node ext)=0;
     	virtual bool isSubtreeOf(TreeNodes *t0)=0;  
		virtual int supportInc(TreeDataSet *D,int min_sup)=0;
		virtual void compress()=0;
		virtual int Id_Front()=0;

    	inline int get_Size(){return tree.size();}     	
     	inline TreeNodes* get_TreeNodes(){return &tree;};
     	inline TreeNodes get_TreeNodes_(){return tree;};
     	inline void set_TreeNodes(TreeNodes tn){tree=tn;};
		inline Integer get_Support(){return Support;}
		inline void set_Support(int sup){Support=sup;}
		inline Integer get_lenTail(){return lenTail;}
		inline void set_lenTail(int len){lenTail=len;}
		inline bool get_isNotMaximal(){return isNotMaximal;}
		inline void set_isNotMaximal(bool is){isNotMaximal=is;}
		inline bool get_Update(){return Update;}
		inline void set_Update(bool is){Update=is;}
		Adwin* AdSupport;	

	protected:
		TreeNodes  tree;
	    Integer Support;
	    Integer lenTail;
	    bool isNotMaximal;
	    bool Update;
	private:
}; 

#endif
