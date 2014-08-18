/*
 *      TreeNat.cpp
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

#include "TreeNat.h"
#include "Tree.cpp"
#include "OrderedTree.cpp"
#include "UnorderedTree.cpp"
#include "DatasetReader.cpp"
#include "TreeMining.cpp"
//#include "Lattice.cpp"
#include "ClosedTrees.cpp"
#include "AdaTreeNat_Mining.cpp"

void display_help(string app){
	cout << endl;
	cout << "Usage: " << app << " [Options] " << endl;
	cout << "-s <support>    Set the absolute or relative minimum support  (default 10%)"<< endl;
	cout << "-ord            Ordered Trees "<< endl;
    cout << "-uno            Unordered Trees (default)"<< endl;
    cout << "-topdown        Top-down subtrees "<< endl;
    cout << "-bottomup       Bottom-up subtrees (default)"<< endl;
	cout << "-in <filestem>  Set the name of the dataset (default 'input.trees')"<< endl;
	cout << "-out <filestem> Set the name of the output file (default 'output.trees')"<< endl;
	cout << endl;
	cout << "-TreeNat        TreeNat(default)"<< endl;
	cout << "-inc            IncTreeNat: Incremental Tree Mining "<< endl;
	cout << "-win            WinTreeNat:Sliding Window Tree Mining "<< endl;
	cout << "-ada1           AdaTreeNat1:Adaptive Sliding Window Tree Mining "<< endl;
	cout << "-ada2           AdaTreeNat2:Support ADWIN Tree Mining "<< endl;
	cout << "-relaxed        Uses relaxed support (default false)"<< endl;
	cout << "-log            Uses logarithmic relaxed support "<< endl;
	cout << "-b <batchSize>  Set size of the batches (default 10,000)"<< endl;
	cout << "-w <windowSize> Set size of the sliding window (default 100,000)"<< endl;
	cout << "-r <intervals>  Set the number of relaxed intervals (default 10,000)"<< endl;
	cout << endl;
	cout << "-help           Display this ussage help"<<endl;
	cout << endl;
}	

int main (int argc, char *argv[]) { 
    bool is_OrderedTree=false;
    bool is_TopDownSubtree=false;
    #if defined(LABELLED)
    bool is_labelled=true;
    #else
    bool is_labelled=false;
    #endif
    //Variables for AdaTreeNat
    bool is_Incremental=false;
    bool is_SlidingWindow=false;
    bool is_Adaptive1=false;
    bool is_Adaptive2=false;
    bool is_Relaxed_Support=false;
    bool is_Log_Relaxed_Support=false;
    int  BatchSize=10000; 		
    int SlidingWindowSize=10*BatchSize; 
    int NumberRelaxedIntervals=10000;	
    
    const char* fileinput = "trees.input";
	const char* fileoutput = "trees.output";
    int min_sup=0;
    double relative_min_sup=0.1;
    
    //Check input parameters
    if (argc==1) display_help((string)argv[0]);
    int optind=1;
    while ((optind < argc) && (argv[optind][0]=='-')) {
        string sw = argv[optind];
        if (sw=="-ord"||sw=="-ordered") {
            is_OrderedTree=true;
        }
        else if (sw=="-uno"||sw=="-unordered") {
			is_OrderedTree=false;
        }
        else if (sw=="-topdown") {
			is_TopDownSubtree=true;
        }
        else if (sw=="-bottomup") {
			is_TopDownSubtree=false;
        }
        else if (sw=="-inc") {
			is_Incremental=true;
        }
        else if (sw=="-win") {
			is_SlidingWindow=true;
        }
        else if (sw=="-ada1") {
			is_Adaptive1=true;
        }
        else if (sw=="-ada2") {
			is_Adaptive2=true;
        }
        else if (sw=="-relaxed") {
			is_Relaxed_Support=true;
        }
        else if (sw=="-log") {
			is_Log_Relaxed_Support=true;
        }
        else if (sw=="-in"||sw=="-input") {
            optind++;
            fileinput = argv[optind];
        }
        else if (sw=="-out"||sw=="-output") {
            optind++;
            fileoutput = argv[optind];
        }
        else if (sw=="-s"||sw=="-support") {
            optind++;
            min_sup = atoi(argv[optind]);
			relative_min_sup = atof(argv[optind]); //We need size of dataset to compute min_sup
        }
        else if (sw=="-b"||sw=="-batchsize") {
            optind++;
            BatchSize = atoi(argv[optind]);
        }
        else if (sw=="-w"||sw=="-windowsize") {
            optind++;
            SlidingWindowSize = atoi(argv[optind]);
        }
        else if (sw=="-r"||sw=="-relaxedintervals") {
            optind++;
            NumberRelaxedIntervals = atoi(argv[optind]);
        }
        else if (sw=="-h"||sw=="-help"||sw=="--h"||sw=="--help") {
            display_help((string)argv[0]);
            exit(0);
        }		
        else {
            cout << "Unknown switch: " << argv[optind] << endl;
            display_help((string)argv[0]);
            exit(1);
        }    
        optind++;
    }
	
	cout << (is_OrderedTree ? "Ordered Trees - ":"Unordered Trees - ") << 
			 (is_TopDownSubtree ? "Top-Down Subtrees ":"Bottom-Up Subtrees") << endl;
	cout << "Input file:" << fileinput << " - Output file:" << fileoutput << endl;
	
	bool is_TreeNat= (!is_Incremental && !is_SlidingWindow &&!is_Adaptive1 && !is_Adaptive2);
	
	if (is_TreeNat==true) 
		cout <<"<TreeNat> ";
	else {
		if (is_Incremental) cout <<"<IncTreeNat> ";
		if (is_SlidingWindow) cout <<"<WinTreeNat> ";
		if (is_Adaptive1) cout <<"<AdaTreeNat1> ";
		if (is_Adaptive2) cout <<"<AdaTreeNat2> ";
		cout <<" BatchSize:"<<BatchSize;
		if (is_Adaptive1) is_SlidingWindow=true;
		if (is_SlidingWindow) cout <<" SlidingWindowSize:"<<SlidingWindowSize;
		if (is_Relaxed_Support) {
			cout <<endl<<" Relaxed Intervals:"<<NumberRelaxedIntervals;
			if (is_Log_Relaxed_Support) 
				cout <<" Log Relaxed Support";
			else 
				cout <<" Linear Relaxed Support";	
		}
		cout<<endl;
	}
   	


	TreeFactory *tree_factory;
    if (is_OrderedTree==true){
    	if (is_TopDownSubtree==true) 
			tree_factory= new OrderedTree_TopDown_Factory; 
		else
			tree_factory= new OrderedTreeFactory; 
	} else {
	    if (is_TopDownSubtree==true) 
			tree_factory= new UnorderedTree__TopDown_Factory; 
		else
			tree_factory= new UnorderedTreeFactory; 
	}

	

	time_t start_time = time(0);
	time_t reading_time;
	
	if (is_TreeNat==true) {
		//Read Dataset	
		DatasetReader *dataset_reader;
		dataset_reader = new DatasetReader(fileinput,is_OrderedTree, is_labelled,relative_min_sup,0); 
		dataset_reader->close();	

		if (relative_min_sup<1) 
			min_sup= (int) (relative_min_sup*dataset_reader->size());
		cout << "Minimum support: " << min_sup ;//" ( "<< dataset_reader->size() <<" )"<<endl;
		if (min_sup==0) 
			error(" Support can not be zero.");
		reading_time = time(0);
		cout << " Dataset Size: "<< dataset_reader->size();
		#if defined(LABELLED)
		cout << " Number of Labels: "<< dataset_reader->number_labels();
		#endif
		cout<<endl;

		//Tree Mining
		TreeMining *closed_tree_mining= new TreeMining(tree_factory, dataset_reader, min_sup);
		closed_tree_mining->print_Closed_Trees(fileoutput);
		delete dataset_reader;
		delete closed_tree_mining;
	}

	if (is_TreeNat==false) {
		if (relative_min_sup>1)
			error(" Support must be a number between 0 and 1.");
		relative_min_sup/=2;
		//Read first batch of data		
		DatasetReader *dataset_reader;
		dataset_reader = new DatasetReader(fileinput,is_OrderedTree, is_labelled,relative_min_sup,BatchSize); 
 		reading_time = time(0);
 		
		if (relative_min_sup<1) 
			min_sup= (int) (relative_min_sup*dataset_reader->size());
		cout << "Minimum support: " << relative_min_sup ;
		if ( is_SlidingWindow==true && is_Adaptive1==false)
			cout << "-" << min_sup <<" ( "<< dataset_reader->size() <<" )";
		cout <<endl;
		if (min_sup==0) 
			min_sup=1;
       
        AdaTreeNat_Mining *adatrenat=new AdaTreeNat_Mining(dataset_reader, is_Relaxed_Support,
        	is_Log_Relaxed_Support,NumberRelaxedIntervals,relative_min_sup,BatchSize, 
        	tree_factory, is_Adaptive1, is_Adaptive2, SlidingWindowSize, min_sup, is_SlidingWindow);
		
		dataset_reader->close();
		delete dataset_reader;
	}

	//Output time results
	cout<< "Time reading " <<difftime(reading_time, start_time)<<endl;
	cout<< "Time processing " <<difftime(time(0),reading_time)<<endl; 
	

	cout << "Press ENTER to continue..." << endl; cin.get();
	
	return 0;
}
