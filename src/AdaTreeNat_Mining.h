/*
 *      AdaTreeNat_Mining.h
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

#ifndef _adatreenat_mining_h_
#define _adatreenat_mining_h_

#define ADD_OPTION 1
#define DELETE_OPTION 2 

class AdaTreeNat_Mining{
	public:
        AdaTreeNat_Mining();                       // Constructor
		AdaTreeNat_Mining(DatasetReader* dataset_reader,bool is_Relaxed_Support, bool is_Log_Relaxed_Support,
			int NumberRelaxedIntervals, double dblmin_sup, int BatchSize, TreeFactory *tree_factory,
			bool is_Adaptive1, bool is_Adaptive2, int SlidingWindowSize_orig, int min_sup_orig, bool is_SlidingWindow );
        virtual ~AdaTreeNat_Mining(){delete support_factory; delete mclosedTrees;};  // Destructor

        void Add(mapTree *ClosedMapTree);
        #if defined(LABELLED)
        void Delete(TreeDataSet *D,TreeNodesAux *Daux, int min_sup);
        #else
        void Delete(TreeDataSet *D,int min_sup);
        #endif

    protected:
    private:
    	ClosedTrees *mclosedTrees;

    	double mdblmin_sup;
    	int mBatchSize;
    	int *mWindowSize;
    	TreeFactory *mtree_factory;
        Support_Factory *support_factory;
        DatasetReader* mdataset_reader;
        
        #if defined(LABELLED)
        TreeNodesAux *mDatasetAux;
        #endif

		void recursive_Add(Tree *t,TreeDataSet *D,SupportContainer *SupportDataset, list<TreeNodes*> *list_Insert_TreeNodes, list<int> *list_Insert_Support);
		void recursive_Delete(Tree *t,TreeDataSet *D,int min_sup,SupportContainer *SupportDataset, listTree *TreesCheckClosureList);
		TreeDataSet* MapTreeToDataset(mapTree *ClosedMapTree, SupportContainer *SupportDataset) ;
		TreeNodes treesIntersection(TreeNodes t1, TreeNodes t2);

};


#endif
// END AdaTreeNat_Mining.h
