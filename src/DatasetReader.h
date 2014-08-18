/*
 *      DatasetReader.h
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

#ifndef _datasetreader_h_
#define _datasetreader_h_

typedef int Integer;


typedef list<int> Labels; 
typedef list<int>::iterator Labels_iterator;

const Integer END_OF_TREE=-1;

//Labelled

#if defined(LABELLED)
	struct Node {
		Integer depth;
		Integer label;
		Node(){Node(0,0);};
		Node(int d,int l){depth=d;label=l;};
		friend const Node operator-(const Node& lhs, const Node& rhs)
			{ return Node(lhs.depth-rhs.depth,lhs.label);};
		friend bool operator==(const Node& lhs,const Node& rhs)
			{return (lhs.depth == rhs.depth && lhs.label == rhs.label );};
		friend bool operator<(const Node& lhs,const Node& rhs)
			{return (lhs.depth < rhs.depth ||(lhs.depth == rhs.depth && lhs.label < rhs.label ));};
		friend bool operator>(const Node& lhs,const Node& rhs)
			{return (lhs.depth > rhs.depth ||(lhs.depth == rhs.depth && lhs.label > rhs.label ));};
		friend bool operator!=(const Node& lhs,const Node& rhs)
			{return (lhs.depth != rhs.depth || lhs.label != rhs.label );};
		friend bool operator<=(const Node& lhs,const Node& rhs)
			{return (lhs.depth < rhs.depth ||(lhs.depth == rhs.depth && lhs.label <= rhs.label ));};					
		friend bool operator>=(const Node& lhs,const Node& rhs)
			{return (lhs.depth > rhs.depth ||(lhs.depth == rhs.depth && lhs.label >= rhs.label ));};	
	};
	typedef vector<Node> TreeNodes;

	struct Occurrence {
		int label[3];
		Occurrence(){Occurrence(0,0,0);};
		Occurrence(int l0,int l1, int l2){label[0]=l0;label[1]=l1;label[2]=l2;};
		friend bool operator<(const Occurrence& lhs,const Occurrence& rhs)
			{return (lhs.label[0] < rhs.label[0] 
				||(lhs.label[0] == rhs.label[0]  && lhs.label[1] < rhs.label[1]  )
				||(lhs.label[0] == rhs.label[0]  && lhs.label[1] == rhs.label[1]   && lhs.label[2] < rhs.label[2]    )
				);};

	};
	struct OccurrenceLabel2 {
		int label[2];
		OccurrenceLabel2(){OccurrenceLabel2(0,0);};
		OccurrenceLabel2(int l0,int l1){label[0]=l0;label[1]=l1;};
		friend bool operator<(const OccurrenceLabel2& lhs,const OccurrenceLabel2& rhs)
			{return (lhs.label[0] < rhs.label[0] 
				||(lhs.label[0] == rhs.label[0]  && lhs.label[1] < rhs.label[1]  )
				);};
	};

	struct OccurrenceLabel {
		int label[4];
		OccurrenceLabel(){OccurrenceLabel(0,0,0,0);};
		OccurrenceLabel(int l0,int l1, int l2, int l3){label[0]=l0;label[1]=l1;label[2]=l2;label[3]=l3;};
		OccurrenceLabel(int l0,Occurrence oc){label[0]=l0;label[1]=oc.label[0];label[2]=oc.label[1];label[3]=oc.label[2];};
		friend bool operator<(const OccurrenceLabel& lhs,const OccurrenceLabel& rhs)
			{return (lhs.label[0] < rhs.label[0] 
				||(lhs.label[0] == rhs.label[0]  && lhs.label[1] < rhs.label[1]  )
				||(lhs.label[0] == rhs.label[0]  && lhs.label[1] == rhs.label[1]   && lhs.label[2] < rhs.label[2]    )
				||(lhs.label[0] == rhs.label[0]  && lhs.label[1] == rhs.label[1]   && lhs.label[2] == rhs.label[2] && lhs.label[3] < rhs.label[3]   )
				);};
	};
	
	struct TreeNodesAux {
		Labels mlabels;
		map <int,Labels> LabelsNode;
		map <pair<int,int>,Labels> LabelsNodeFather;
		map <Occurrence,Labels> LabelsNodeGrandFather;
		void clear(){
				mlabels.clear();
				LabelsNode.clear();
				LabelsNodeFather.clear();
				LabelsNodeGrandFather.clear();
		};
		inline int number_labels() { return mlabels.size();};
			inline Labels* get_labels() { return &mlabels;};
			inline Labels* get_labels(int label) { return &LabelsNode[label];};
			inline Labels* get_labels(int label,int father){return &LabelsNodeFather[pair<int,int> (label,father)];};
			inline Labels* get_labels(int label,int father,int grand_father){return &LabelsNodeGrandFather[Occurrence(label,father,grand_father)];};
	};
		
#else
	typedef Integer Node;
	typedef vector<Integer> TreeNodes;
#endif 

typedef vector<TreeNodes> TreeDataSet; 

class DatasetReader{
    public:
        DatasetReader(const char* filenameOrig, bool is_Ordered, bool is_Labelled,double min_sup, int BatchSize);  // Constructor
        DatasetReader(TreeDataSet *Dataset);  // Constructor
        virtual ~DatasetReader();              // Destructor
        inline TreeDataSet* dataset() { return mDataset;};
        inline TreeDataSet get_dataset() { return *mDataset;};
        
        inline int size() { return mDataSetsize;};
        inline bool is_Ordered() { return mis_Ordered;};
        inline bool is_Labelled() { return mis_Labelled;};
        inline void close() {minFile.close();};
        #if defined(LABELLED)
       	    inline TreeNodesAux get_dataset_aux() { return *mDatasetAux;};
			inline int number_labels() { return mDatasetAux->number_labels();};
			inline Labels* get_labels() { return mDatasetAux->get_labels();};
			inline Labels* get_labels(int label) { return mDatasetAux->get_labels(label);};
			inline Labels* get_labels(int label,int father){return mDatasetAux->get_labels(label,father);};
			inline Labels* get_labels(int label,int father,int grand_father){return mDatasetAux->get_labels(label,father,grand_father);};
		#else
       		inline int oneTreesDataSet_size() { return oneTreesDataSet.size();};
        #endif
        	inline bool read_Next_Batch(){ 
        		if (mis_formatTreeNat==true) read_Next_Batch_TreeNat(); else read_Next_Batch_Zaki();
        		return minFile.eof();
        		};

    private:
    	void open_TreeNat(const char* filenameOrig);
    	void read_Next_Batch_TreeNat();
    	void open_Zaki(const char* filenameOrig);
    	void read_Next_Batch_Zaki();


		TreeDataSet *mDataset;
		int mDataSetsize;
		int mBatchSize;
		ifstream minFile;
		double mmin_sup;
		bool mis_Ordered;
		bool mis_Labelled;
		bool mis_formatTreeNat;
		TreeNodes DataSet_append(const int *v);
		#if defined(LABELLED)
			TreeNodesAux *mDatasetAux;
			void mDatasetAux_Compute();
		#else
			list<int> oneTreesDataSet;
			list<int> twoTreesDataSet;
			vector<int> IdTreesDataSet;
		#endif
};
#endif
