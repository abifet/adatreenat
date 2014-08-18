/*
 *      TreeMining.h
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

#ifndef _treemining_h_
#define _treemining_h_

typedef multimap<int, Tree *> mapTree;
typedef multimap<int, Tree *>::iterator mapTreeIterator;
typedef multimap<int, Tree *>::reverse_iterator mapTreeRevIterator;

class TreeMining {
    public:
    	TreeMining(TreeFactory *tree_factory,DatasetReader *dataset_reader, int min_sup);
    	virtual ~TreeMining();  
    	void print_Closed_Trees(const char* filenameOrig);
    	inline mapTree* get_ClosedMapTree() {return mClosedMapTree;};
    	void Compress_MapTree();
    private:
        void closed_subtree_mining(Tree *t);
        DatasetReader *mdataset_reader; 
        TreeDataSet *D;
        int min_sup;
		mapTree *mClosedMapTree;
		TreeFactory *mtree_factory;
};
#endif
// END TreeMining.h
