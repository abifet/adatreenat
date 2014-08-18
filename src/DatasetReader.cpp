/*
 *      DatasetReader.cpp
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

#include "DatasetReader.h"

DatasetReader::DatasetReader(const char* filenameOrig,  bool is_Ordered, bool is_Labelled,double min_sup, int BatchSize) {
    // Implementation of constructor: reads a dataset from a text file
	mis_Labelled=is_Labelled;
	mis_Ordered=is_Ordered;
	mmin_sup=min_sup;
	mBatchSize=BatchSize;
	//Detects file format: TreeNat or Zaki
	ifstream ifile(filenameOrig); 
	mis_formatTreeNat=false; 
	if (ifile.is_open()){
		    string line;                    
			getline (ifile,line);
			if (line.at(0)=='[') mis_formatTreeNat=true;
			ifile.close();
	}
	else error("Unable to open input file"); 
	if (mis_formatTreeNat==true && is_Labelled==false){ 
		open_TreeNat(filenameOrig);
		read_Next_Batch_TreeNat();
	}
	else{
		open_Zaki(filenameOrig);
		read_Next_Batch_Zaki();
	}
}
	
////////////////////////////////////////////////////////////////////////////////

DatasetReader::DatasetReader(TreeDataSet *Dataset){
	mDataset=Dataset;
	#if defined(LABELLED)
		mDatasetAux=new TreeNodesAux;
		mmin_sup=1;	
		mDatasetAux_Compute();
	#endif
}
	
////////////////////////////////////////////////////////////////////////////////

void DatasetReader::open_TreeNat(const char* filenameOrig) {
	mDataset=new TreeDataSet;
	#if defined(LABELLED)
		mDatasetAux=new TreeNodesAux;
	#endif
	cout<<"TreeNat format file"<<endl; 
    minFile.open(filenameOrig);
    if (!minFile) 
        error("Unable to open input file");       
}
	
////////////////////////////////////////////////////////////////////////////////

void DatasetReader::read_Next_Batch_TreeNat() {
	Integer depth=0;
	string line;
	mDataset->clear();
	#if !defined(LABELLED)
	oneTreesDataSet.clear();
	twoTreesDataSet.clear();
	#endif
	int tid=0;
		bool blEnd=false;                  
		while (! minFile.eof() && blEnd==false){
			getline (minFile,line);
			depth=0;
			#if !defined(LABELLED)
			if (line.length()==2){
				oneTreesDataSet.push_back(tid);
			}
			else if (line.length()==4){
				twoTreesDataSet.push_back(tid);
			} 
			else 
			#endif
			if (line.length()>0){
				TreeNodes tn;
			#if defined(LABELLED)
				tn.push_back(Node(0,0));
			#else
				tn.push_back(0);
			#endif
				for ( Integer pos = 1; pos < line.length(); ++pos ){
					if(line.at(pos)=='['){ 
						++depth;
						#if defined(LABELLED)
							tn.push_back(Node(depth,0));
						#else
							tn.push_back(depth);
						#endif			
					}  
					if(line.at(pos)==']') depth--;   	
				 }					  
				mDataset->push_back(mis_Ordered ? tn : UnorderedTree::CanonicalForm(tn));
				#if !defined(LABELLED)
				IdTreesDataSet.push_back(tid);
				#endif
			}
			tid++;
			if (tid==mBatchSize) blEnd=true;
		}
	mDataSetsize=tid;
}  
	
////////////////////////////////////////////////////////////////////////////////

void DatasetReader::open_Zaki(const char* filenameOrig) {
	mDataset=new TreeDataSet;
	#if defined(LABELLED)
		mDatasetAux=new TreeNodesAux;
	#endif
	cout<<"Zaki format file"<<endl; 
    minFile.open(filenameOrig);
    if (!minFile) 
        error("Unable to open input file");  
}
	
////////////////////////////////////////////////////////////////////////////////

void DatasetReader::read_Next_Batch_Zaki() {
    // Implementation of constructor: reads a dataset from a Zaki text file
	
	Integer depth=0;
	#if defined(LABELLED)
 	mDatasetAux->clear();
		
	set<int> labels;
	
	set<int> labelsTree;
	map<int,int> freqLabels;
	
	set<OccurrenceLabel2> labelsTree2;
	map<OccurrenceLabel2,int> freqLabels2;
	set<Occurrence> labelsTree3;
	map<Occurrence,int> freqLabels3;
	
	set<OccurrenceLabel> OccurrenceTree;
	map<OccurrenceLabel,int> freqOccurrenceLabels;
	
	map <int,set<int> > setLabelsNode;
	map <int,set<int> >::iterator setLabelsNode_it;
	map <pair<int,int>,set<int> > setLabelsNodeFather;
	map <pair<int,int>,set<int> >::iterator setLabelsNodeFather_it;
	
	map <Occurrence,set<int> > setLabelsNodeGrandFather;
	map <Occurrence,set<int> >::iterator setLabelsNodeGrandFather_it;
	vector<int> parentsLabel;
	#else
	oneTreesDataSet.clear();
	twoTreesDataSet.clear();
	#endif

	mDataset->clear();
    int x;int len;
    
    bool blnFirst=true;	    
    int pos=0;
    TreeNodes tn;
    int tid=0;
    bool blEnd=false;
    while (blEnd==false) {
    	if ((minFile >> x)==false ) break;
		if (blnFirst) {
			blnFirst=false;
			minFile>>x;minFile>>len;minFile>>x; 
			pos=1;
			depth=0;
			tn.clear();
			#if defined(LABELLED)
			labelsTree.clear();
			labelsTree2.clear();
			labelsTree3.clear();
			OccurrenceTree.clear();
			parentsLabel.clear();
			#endif
		}
		if (x!=-1) {
			#if defined(LABELLED)
				tn.push_back(Node(depth,x));
				labels.insert(x);
				labelsTree.insert(x);
				if (parentsLabel.size()==depth) 
					parentsLabel.push_back(x);
				else
					parentsLabel.at(depth)=x;
				if (depth>0) {
					OccurrenceLabel2 ocl(x,parentsLabel[depth-1]);
					labelsTree2.insert(ocl);
					setLabelsNode_it=setLabelsNode.find(parentsLabel[depth-1]);
					if (setLabelsNode_it==setLabelsNode.end()){
						set<int> labelset;
						labelset.insert(x);
						setLabelsNode.insert(pair <int,set<int> >(parentsLabel[depth-1],labelset));
					}
					else
						setLabelsNode_it->second.insert(x);
				}
				if (depth>1) {
					Occurrence ocl(x,parentsLabel[depth-1],parentsLabel[depth-2]);
					labelsTree3.insert(ocl);
					setLabelsNodeFather_it=setLabelsNodeFather.find(pair<int,int>(parentsLabel[depth-1],parentsLabel[depth-2]));
					if (setLabelsNodeFather_it==setLabelsNodeFather.end()){
						set<int> labelset;
						labelset.insert(x);
						setLabelsNodeFather.insert(pair <pair<int,int>,set<int> >(pair<int,int> (parentsLabel[depth-1],parentsLabel[depth-2]),labelset));
					}
					else
						setLabelsNodeFather_it->second.insert(x);
				}
				if (depth>2) {
					OccurrenceLabel ocl(x,parentsLabel[depth-1],parentsLabel[depth-2],parentsLabel[depth-3]);
					OccurrenceTree.insert(ocl);
					Occurrence oc(parentsLabel[depth-1],parentsLabel[depth-2],parentsLabel[depth-3]);
					setLabelsNodeGrandFather_it=setLabelsNodeGrandFather.find(oc);
					if (setLabelsNodeGrandFather_it==setLabelsNodeGrandFather.end()){
						set<int> labelset;
						labelset.insert(x);
						setLabelsNodeGrandFather.insert(pair<Occurrence,set<int> >(oc,labelset));
					}
					else
						setLabelsNodeGrandFather_it->second.insert(x);
				}
			#else
				tn.push_back(depth);
			#endif
			++depth;
    	}
		else 
			depth--;
		if (pos==len) {
			blnFirst=true; 
			//Add frequence statistics tree  
			#if defined(LABELLED)
			for (set<int>::iterator iter=labelsTree.begin(); iter!=labelsTree.end(); iter++){
    			freqLabels[(*iter)]++; 
			}
			for (set<OccurrenceLabel2>::iterator iter=labelsTree2.begin(); iter!=labelsTree2.end(); iter++){
    			freqLabels2[(*iter)]++; 
			}
			for (set<Occurrence>::iterator iter=labelsTree3.begin(); iter!=labelsTree3.end(); iter++){
    			freqLabels3[(*iter)]++; 
			}
			for (set<OccurrenceLabel>::iterator iter=OccurrenceTree.begin(); iter!=OccurrenceTree.end(); iter++){
    			freqOccurrenceLabels[(*iter)]++; 
			}
			#endif
			#if !defined(LABELLED)
			if (pos==1){
					oneTreesDataSet.push_back(tid);
			}
			else if (pos==2){
					twoTreesDataSet.push_back(tid);
			} 
			else
			#endif
			{
				mDataset->push_back(mis_Ordered ? tn : UnorderedTree::CanonicalForm(tn));
				#if !defined(LABELLED)
				IdTreesDataSet.push_back(tid);
				#endif
			}	
			tid++;
			if (tid==mBatchSize) blEnd=true;
		}
		pos++;
    }

	mDataSetsize=tid;

	#if defined(LABELLED)
		int min_sup=(int)mmin_sup;
		if (mmin_sup<1) min_sup= (int) (mmin_sup* (double) mDataSetsize);
		
		map<int,int>::iterator freq_iter;
		for (set<int>::iterator iter=labels.begin(); iter!=labels.end(); iter++){
			freq_iter=freqLabels.find(*iter);
			if (freq_iter!= freqLabels.end() && freq_iter->second>=min_sup) 
				 mDatasetAux->get_labels()->push_back(*iter); 
		}
		

		for (setLabelsNode_it=setLabelsNode.begin(); setLabelsNode_it!=setLabelsNode.end(); setLabelsNode_it++){
			 Labels possibleLabelsNode;
			 for (set<int>::iterator iter=setLabelsNode_it->second.begin(); iter!=setLabelsNode_it->second.end(); iter++){
				OccurrenceLabel2 oc(*iter,setLabelsNode_it->first);
				if (freqLabels2.find(oc)->second >=min_sup)
					possibleLabelsNode.push_back(*iter);
			 }
			 if (possibleLabelsNode.size()>0)
			 	mDatasetAux->LabelsNode[setLabelsNode_it->first]= possibleLabelsNode;
		}
		
		for (setLabelsNodeFather_it=setLabelsNodeFather.begin(); setLabelsNodeFather_it!=setLabelsNodeFather.end(); setLabelsNodeFather_it++){
			 Labels possibleLabelsNode;
			 for (set<int>::iterator iter=setLabelsNodeFather_it->second.begin(); iter!=setLabelsNodeFather_it->second.end(); iter++){
				Occurrence oc(*iter,setLabelsNodeFather_it->first.first,setLabelsNodeFather_it->first.second);
				if (freqLabels3.find(oc)->second >=min_sup)
					possibleLabelsNode.push_back(*iter);
			 }
			if (possibleLabelsNode.size()>0)
				mDatasetAux->LabelsNodeFather.insert(pair <pair<int,int>,Labels >(setLabelsNodeFather_it->first,possibleLabelsNode));
		}
		
		for (setLabelsNodeGrandFather_it=setLabelsNodeGrandFather.begin(); setLabelsNodeGrandFather_it!=setLabelsNodeGrandFather.end(); setLabelsNodeGrandFather_it++){
			 Labels possibleLabelsNode;
			 for (set<int>::iterator iter=setLabelsNodeGrandFather_it->second.begin(); iter!=setLabelsNodeGrandFather_it->second.end(); iter++){
				OccurrenceLabel oc(*iter,setLabelsNodeGrandFather_it->first);
				if (freqOccurrenceLabels.find(oc)->second >=min_sup)
					possibleLabelsNode.push_back(*iter);
			 }
				if (possibleLabelsNode.size()>0)
					mDatasetAux->LabelsNodeGrandFather.insert(pair <Occurrence,Labels >(setLabelsNodeGrandFather_it->first,possibleLabelsNode));
		}
		
    #endif

}  

///////////////////////////////////////////////////////////////////////////////

DatasetReader::~DatasetReader()
{
     // Implementation of destructor
     delete mDataset;
     #if defined(LABELLED)
     delete mDatasetAux;
     #endif
}
