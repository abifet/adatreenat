/*
 *      UnorderedTree.cpp
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

#include "UnorderedTree.h"
UnorderedTree::UnorderedTree(): Tree(){ 
	// Implementation of constructor: Initialize an empty tree 
	}
	
////////////////////////////////////////////////////////////////////////////////

UnorderedTree::UnorderedTree(UnorderedTree *t): Tree(t){
	// Implementation of constructor: Initialize a tree as a copy of a tree t
	IdTrans=t->IdTrans;
	CanonicalNew=t->CanonicalNew;
	CanonicalOld=t->CanonicalOld;
     }  
    	
////////////////////////////////////////////////////////////////////////////////

#if defined(LABELLED) 
UnorderedTree::UnorderedTree(TreeDataSet* D): Tree(){};	
UnorderedTree::UnorderedTree(TreeDataSet* D, int label): Tree(){
	// Implementation of constructor: Initialize init_tree
	Integer min_depth=0;
	Node init_node( min_depth,label);
	extend(init_node);
	SupportId supp;
	supp.iNode=0;
	Support=0;

	for (int i=0;i<D->size();i++) {
		for (int j=0;j<D->at(i).size();j++) {
			if (D->at(i).at(j).label==label){ //Induced
				Support++;               
				supp.Id=i;
				supp.iNode=j;
				IdTrans.push_back(supp);
				break;
			}
		}
	}
	lenTail=min_depth;
	isNotMaximal=false;
	Update=false;
	
}
	
////////////////////////////////////////////////////////////////////////////////

bool UnorderedTree::isCanonicalRepresentative(Node ext){
	Integer i;
	bool isCanonical=true;

	// Add vectorTreeNodes
	if (ext.depth==lenTail+1) {
		TreeNodes vTN1;
		TreeNodes vTN2;        
		CanonicalNew.push_back(vTN1);
		CanonicalOld.push_back(vTN2);        
	} 
	// Remove vectorTreeNodes
	if (ext.depth<lenTail) for(i=ext.depth+1;i<lenTail+1;i++) {
		CanonicalNew.pop_back();
		CanonicalOld.pop_back();
	}

	//Update statistics
	for(i=1;i<ext.depth;i++) {
		CanonicalNew[i-1].push_back(ext);                
	}  
	CanonicalOld[ext.depth-1]=CanonicalNew[ext.depth-1];
	CanonicalNew[ext.depth-1].clear();
	CanonicalNew[ext.depth-1].push_back(ext);

	//Check if it is canonical
	for(i=1;i<ext.depth;i++) {
		if (CanonicalOld[i-1].size()>0 &&CanonicalNew[i-1]>CanonicalOld[i-1]) {
			isCanonical=false;}  
	} 
	return isCanonical;
} 
	
////////////////////////////////////////////////////////////////////////////////

#else
UnorderedTree::UnorderedTree(TreeDataSet* D, int label): Tree(){};
UnorderedTree::UnorderedTree(TreeDataSet* D): Tree(){
	// Implementation of constructor: Initialize init_tree
	Integer min_depth=1;
	for (Integer i=1;i<=min_depth;i++) 
		extend(i);
	SupportId supp;
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
	
	TreeNodes vTN1;
	TreeNodes vTN2; 

	vTN1.push_back(0);

	CanonicalNew.push_back(vTN1);
	CanonicalOld.push_back(vTN2);

    } 
	
////////////////////////////////////////////////////////////////////////////////

bool UnorderedTree::isCanonicalRepresentative(int ext){
	Integer i;
	bool isCanonical=true;

	// Add vectorTreeNodes
	if (ext==lenTail+1) {
		TreeNodes vTN1;
		TreeNodes vTN2;        
		CanonicalNew.push_back(vTN1);
		CanonicalOld.push_back(vTN2);        
	} 
	// Remove vectorTreeNodes
	if (ext<lenTail) for(i=ext+1;i<lenTail+1;i++) {
		CanonicalNew.pop_back();
		CanonicalOld.pop_back();
	}

	//Update statistics
	for(i=1;i<ext;i++) {
		CanonicalNew[i-1].push_back(ext);                
	}  
	CanonicalOld[ext-1]=CanonicalNew[ext-1];
	CanonicalNew[ext-1].clear();
	CanonicalNew[ext-1].push_back(0);

	//Check if it is canonical
	for(i=1;i<ext;i++) {
		if (CanonicalOld[i-1].size()>0 &&CanonicalNew[i-1]>CanonicalOld[i-1]) {
			isCanonical=false;}  
	} 
	return isCanonical;
} 
#endif
	
////////////////////////////////////////////////////////////////////////////////

bool UnorderedTree::isSubtreeOf(TreeNodes *t0){
	//NonIncremental version
	Integer iN=0;
	return isSubtreeUnOrdInc(t0,&iN); 
}	  
	
////////////////////////////////////////////////////////////////////////////////

bool UnorderedTree::isSubtreeUnOrdInc(TreeNodes *supertree, Integer *iNode){
	//UnOrdered version
	bool blret=false;
	TreeNodes t0;
	Integer Size0=supertree->size();
	Integer Size1=tree.size();
	if (Size0-*iNode<Size1) return false;
	while(blret==false && Size0-*iNode>=Size1){
		t0=SubtreeAtNode(supertree,iNode);
		blret=isSubtree(&tree,&t0);
		if (blret==false) (*iNode)++;
	}
	return blret; 
}	  
	
////////////////////////////////////////////////////////////////////////////////

int UnorderedTree::supportInc(TreeDataSet *D,int min_sup){
	//Incremental version : calculates support         
	int isup=0;
	int deleted=0;
	ListSupport::iterator aIdList;
	for (aIdList=IdTrans.begin();aIdList!=IdTrans.end();) {
		//For each tree in the list 
		if (isSubtreeUnOrdInc(&(*D)[aIdList->Id],&aIdList->iNode)) {
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

bool UnorderedTree::isSubtree(TreeNodes *a, TreeNodes *b){
   // UnOrdered Subtree Test
      if (a->at(0)!=b->at(0)) //Root Node needs to have the same level
   	   return false;
   	   
      setTreeNodes aTreeComponents=TreeComponents(a);
   setTreeNodes bTreeComponents=TreeComponents(b);
   TreeNodes aa=*a;
   TreeNodes bb=*b;

   if (aTreeComponents.size()>bTreeComponents.size()||a->size()>b->size())
       return false;
   if (*a==*b || a->size()==0)
       return true;
   if (a->size()==1)
   	   return (a->at(0)==b->at(0));

   map<pair<TreeNodes,TreeNodes> , bool>::iterator itsub;
   itsub=Is_subtree_Dictionary.find(make_pair(*a,*b));
   if (itsub!=Is_subtree_Dictionary.end()) {
	return Is_subtree_Dictionary[make_pair(*a,*b)];}
   Graph graph; //graph={}  //Dictionary mapping members of U to a list of V : multimap<int,int>
   BipartiteGraph bipartite_matching_graph;
   setTreeNodes::iterator aiterset;
   setTreeNodes::iterator biterset;
   TreeNodes::iterator iteTreeNodes;
   int sa=0;

   for ( aiterset=aTreeComponents.begin();aiterset!=aTreeComponents.end();++aiterset,sa++){
        int sb=0; bool done=false;
        for (biterset=bTreeComponents.begin();biterset!=bTreeComponents.end();++biterset,sb++){
			TreeNodes aa=(*aiterset);
			TreeNodes bb=(*biterset);
            if (isSubtree(&aa,&bb)){
               graph.insert(make_pair(sa+1,sb+1)); 
			   if (!done && bipartite_matching_graph[sb+1]==0) {
                         done=true;
                         bipartite_matching_graph[sb+1] =sa+1;}
			   }
           }
        }

   for(int i=1;i<=aTreeComponents.size();i++) 
   		if (graph.count(i)==0) { 
   			Is_subtree_Dictionary[make_pair(*a,*b)]=false; 
   			return false;
   			}

   //Matching_Dictionary;
   /*map<Graph,bool>::iterator itsub;
   itsub = Matching_Dictionary.find(graph);
   if (itsub!=Matching_Dictionary.end()) {
	return Matching_Dictionary[graph];}
   */
   bipartiteMatch(&graph, &bipartite_matching_graph); 

   bool ret=(bipartite_matching_graph.size()>=aTreeComponents.size()) ;  
   Is_subtree_Dictionary[make_pair(*a,*b)]=ret;

   return ret;   
}

#else
	
////////////////////////////////////////////////////////////////////////////////

bool UnorderedTree::isSubtree(TreeNodes *a, TreeNodes *b){
   // UnOrdered Subtree Test
   setTreeNodes aTreeComponents=TreeComponents(a);
   setTreeNodes bTreeComponents=TreeComponents(b);

   if (aTreeComponents.size()>bTreeComponents.size()||a->size()>b->size())
       return false;
   if (*a==*b || a->size()<=1)
       return true;

   map<pair<TreeNodes,TreeNodes> , bool>::iterator itsub;
   itsub=Is_subtree_Dictionary.find(make_pair(*a,*b));
   if (itsub!=Is_subtree_Dictionary.end()) {
	return Is_subtree_Dictionary[make_pair(*a,*b)];}
   Graph graph; //graph={}  //Dictionary mapping members of U to a list of V : multimap<int,int>
   BipartiteGraph bipartite_matching_graph;
   setTreeNodes::iterator aiterset;
   setTreeNodes::iterator biterset;
   TreeNodes::iterator iteTreeNodes;
   int sa=0;

   for ( aiterset=aTreeComponents.begin();aiterset!=aTreeComponents.end();++aiterset,sa++){
        int sb=0; bool done=false;
        for (biterset=bTreeComponents.begin();biterset!=bTreeComponents.end();++biterset,sb++){
			TreeNodes aa=(*aiterset);
			TreeNodes bb=(*biterset);
            if (isSubtree(&aa,&bb)){
               graph.insert(make_pair(sa+1,sb+1));
			   if (!done && bipartite_matching_graph[sb+1]==0) {
                         done=true;
                         bipartite_matching_graph[sb+1] =sa+1;}
			   }
           }
        }

   for(int i=1;i<=aTreeComponents.size();i++) if (graph.count(i)==0) 
   {Is_subtree_Dictionary[make_pair(*a,*b)]=false; 
   return false;
   }

   //Matching_Dictionary;
   /*map<Graph,bool>::iterator itsub;
   itsub = Matching_Dictionary.find(graph);
   if (itsub!=Matching_Dictionary.end()) {
	return Matching_Dictionary[graph];}
   */
   bipartiteMatch(&graph, &bipartite_matching_graph); 

   bool ret=(bipartite_matching_graph.size()>=aTreeComponents.size()) ;  
   Is_subtree_Dictionary[make_pair(*a,*b)]=ret;
   /*Matching_Dictionary[graph]=ret;*/
   return ret;   
}
#endif

////////////////////////////////////////////////////////////////////////////////

bool UnorderedTree::recurse(int v, map <int, list<int> > *preds, map<int,int> *pred, list<int> *unmatched,BipartiteGraph *matching) {
    // recursively search backward through layers to find alternating paths
	//recursion returns true if found path, false otherwise
    if ((*preds)[v].size()>0) {//if v in preds {
		list<int> L = (*preds)[v];
		(*preds)[v].clear(); //del preds[v]

		for (list<int>::iterator u=L.begin();u!=L.end();++u)
		//for u in L
			if (pred->count(*u)>0) {
			//if u in pred {
				int pu = (*pred)[*u];//pu = pred[u]
				pred->erase(pred->find(*u));//del pred[u]
				if (pu==-1||(unmatched->size()==1 && pu==unmatched->front() )|| recurse(pu,preds,pred,unmatched,matching)){
					(*matching)[v] = *u;
					return true;
					}
				}
	}
	return false;
}
	
////////////////////////////////////////////////////////////////////////////////

void UnorderedTree::bipartiteMatch(Graph *graph, BipartiteGraph *matching){
	//Find maximum cardinality matching of a bipartite graph (U,V,E).
	//The input format is a dictionary mapping members of U to a list
	//of their neighbors in V.  The output is a triple (M,A,B) where M is a
	//dictionary mapping members of V to their matches in U, A is the part
	//of the maximum independent set in U, and B is the part of the MIS in V.
	//The same object may occur in both U and V, and is treated as two
	//distinct vertices if this happens.'''
	// Hopcroft-Karp bipartite max-cardinality matching and max independent set
	// David Eppstein, UC Irvine, 27 Apr 2002
	
	// initialize greedy matching (redundant, but faster than full search)
	/*Graph matching;//matching = {}
	for u in graph:
		for v in graph[u]:
			if v not in matching:
				matching[v] = u
				break
	*/
	map <int,list<int> > preds; // preds = {}
	list<int> unmatched; // unmatched = [] 
	map<int,int> pred;
	list<int> layer;
	map<int, list<int> > newlayer;

	while (true){
		// structure residual graph into layers
		// pred[u] gives the neighbor in the previous layer for u in U
		// preds[v] gives a list of neighbors in the previous layer for v in V
		// unmatched gives a list of unmatched vertices in final layer of V,
		// and is also used as a flag value for pred[u] when u is in the first layer
		preds.clear(); // preds = {}
		unmatched.clear(); // unmatched = [] 
		pred.clear();

        for (Graph::iterator itergraph=graph->begin();itergraph!=graph->end();++itergraph) //for sa in range(0,len(a)):
		    pred[(*itergraph).first]=-1;
		//pred = dict([(u,unmatched) for u in graph])
	
		for (BipartiteGraph::iterator itergraph=matching->begin();itergraph!=matching->end();++itergraph) //for sa in range(0,len(a)):
		    {
             //pred.erase(pred.find((*itergraph).second));
             pred.erase((*itergraph).second);
             }
        //for v in matching:
			//del pred[matching[v]]	
	
		layer.clear();
		for (map <int,int>::iterator iterpred=pred.begin();iterpred!=pred.end();++iterpred)
		    layer.push_back(iterpred->first);//layer = list(pred)

		// repeatedly extend layering structure by another pair of layers
		while ( !layer.empty() && unmatched.empty()) { //while layer and not unmatched:
			newlayer.clear(); //newLayer = {}
			for (list<int>::iterator u=layer.begin();u!=layer.end();++u){
			//for u in layer:
                typedef Graph::const_iterator iG;
			    pair<iG,iG> listv = graph->equal_range(*u);	
			    for (iG v=listv.first;v!=listv.second;++v){
				//for v in graph[u]:
						if(preds[(*v).second].size()==0) {//if v not in preds:
						if (newlayer.count((*v).second)==0) {
                           list<int> lu;
						   lu.push_back(*u);
						   newlayer[(*v).second]=lu;
                           }
                        else newlayer[(*v).second].insert(newlayer[(*v).second].end(), *u) ;
                        }
               }
            }

            layer.clear();//layer = []
			
			for (int iv=0;iv<newlayer.size();++iv) {
				if (newlayer[iv].size()>0){
                //for v in newLayer:
                preds[iv] = newlayer[iv];
				//preds[v] = newLayer[v]
				if (matching->count(iv)>0) { //if v in matching:
					layer.push_back((*matching)[iv]) ;
					pred[(*matching)[iv]] = iv;
					}
				else
					unmatched.push_back(iv) ;
            }   
            }
        }
		// did we finish layering without finding any alternating paths?
        if (unmatched.empty()) //if not unmatched:
			return; // matching
		for (list<int>::iterator it=unmatched.begin();it!=unmatched.end();++it)
				recurse(*it, &preds, &pred,&unmatched,matching);
		//for v in unmatched: recurse(v)
	}	
}
	
////////////////////////////////////////////////////////////////////////////////

#if defined(LABELLED)
setTreeNodes UnorderedTree::TreeComponents(TreeNodes *t,bool bCanonical=false){
    //Returns the components ( subtree of next level) of a tree
    setTreeNodes setTreeComponents;
    //Insert into a set, the components
    TreeNodes::iterator itTreeNodes;
    Node level=(*t)[0];
    Node zero(0,0);
    TreeNodes component;
    itTreeNodes=t->begin();
    int i=0;
	bool blnend=false;
    while(i<t->size() &&itTreeNodes!=t->end()) {
		//The components start at level 0, the canonical form no
		if (*itTreeNodes!=level) component.push_back( (*itTreeNodes)-(bCanonical ? zero :level));
        ++itTreeNodes;i++;
        if (itTreeNodes==t->end()) 
			blnend=true;
		else if(itTreeNodes->depth<= level.depth+1 )
			blnend=true;
		if (blnend==true){
                  blnend=false;
				  if(component.size()>0){ 
					  setTreeComponents.insert(bCanonical ? CanonicalForm(component): component );
                          component.clear();   
                  }
        }                        
    }
    return setTreeComponents;       
}
	
////////////////////////////////////////////////////////////////////////////////

TreeNodes UnorderedTree::CanonicalForm(TreeNodes t){
    //Returns recursively the CanonicalForm of a tree
    if (t.size()<3) return t;
    setTreeNodes setTreeComponents = TreeComponents(&t,true);
    Node level=t[0]; 
  
    TreeNodes tn;
    tn.push_back(level);
    setTreeNodes::reverse_iterator itset;
    TreeNodes::iterator iteTreeNodes;
    for (itset=setTreeComponents.rbegin();itset!=setTreeComponents.rend();++itset){
        TreeNodes t1=(*itset);
        for (iteTreeNodes=t1.begin();iteTreeNodes!=t1.end();++iteTreeNodes){
            tn.push_back((*iteTreeNodes));
            }
            }
    return tn;   
}
	
////////////////////////////////////////////////////////////////////////////////

TreeNodes UnorderedTree::SubtreeAtNode(TreeNodes *supertree,Integer *iNode){
    //Outputs the bigest bottom subtree rooted at iNode
    TreeNodes tn;
	int i=*iNode;
	int v=(*supertree)[i].depth;
	int init_depth=v;
	do {	
		tn.push_back(Node(v-init_depth,(*supertree)[i].label));
		i++;
		v=(*supertree)[i].depth;
	} while (v-init_depth>0 && i<supertree->size() );
	
    return CanonicalForm(tn);   
}
	
////////////////////////////////////////////////////////////////////////////////

UnorderedTree_TopDown::UnorderedTree_TopDown(TreeDataSet* D,int label):UnorderedTree(){
	// Implementation of constructor: Initialize init_tree
	Integer min_depth=0;
	Node init_node( min_depth,label);
	extend(init_node);
	SupportId supp;
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

#else
setTreeNodes UnorderedTree::TreeComponents(TreeNodes *t,bool bCanonical=false){
    //Returns the components ( subtree of next level) of a tree
    setTreeNodes setTreeComponents;
    //Insert into a set, the components
    TreeNodes::iterator itTreeNodes;
    Integer level=(*t)[0];
    TreeNodes component;
    itTreeNodes=t->begin();
    int i=0;
	bool blnend=false;
    while(i<t->size() &&itTreeNodes!=t->end()) {
		//The components start at level 0, the canonical form no
		if (*itTreeNodes!=level) component.push_back( (*itTreeNodes)-(bCanonical ? 0 :level));
        ++itTreeNodes;i++;
        if (itTreeNodes==t->end()) 
			blnend=true;
		else if((*itTreeNodes)<= level+1 )
			blnend=true;
		if (blnend==true){
                  blnend=false;
				  if(component.size()>0){ 
					  setTreeComponents.insert(bCanonical ? CanonicalForm(component): component );
                          component.clear();   
                  }
        }                        
    }
    return setTreeComponents;       
}
	
////////////////////////////////////////////////////////////////////////////////

TreeNodes UnorderedTree::CanonicalForm(TreeNodes t){
    //Returns recursively the CanonicalForm of a tree
    if (t.size()<2) return t;
    setTreeNodes setTreeComponents = TreeComponents(&t,true);
    Integer level=t[0]; 
  
    TreeNodes tn;
    tn.push_back(level);
    setTreeNodes::reverse_iterator itset;
    TreeNodes::iterator iteTreeNodes;
    for (itset=setTreeComponents.rbegin();itset!=setTreeComponents.rend();++itset){
        TreeNodes t1=(*itset);
        for (iteTreeNodes=t1.begin();iteTreeNodes!=t1.end();++iteTreeNodes){
            tn.push_back((*iteTreeNodes));
            }
            }
    return tn;   
}
	
////////////////////////////////////////////////////////////////////////////////

TreeNodes UnorderedTree::SubtreeAtNode(TreeNodes *supertree,Integer *iNode){
    //Outputs the bigest induced subtree rooted at iNode
    TreeNodes tn; 
    
	int i=*iNode;
	int v=(*supertree)[i];
	int init_depth=v;
	do {
		tn.push_back(v-init_depth);
		i++;
		v=(*supertree)[i];
	} while (v-init_depth>0 && i<supertree->size() );
	
    return CanonicalForm(tn);   
}
UnorderedTree_TopDown::UnorderedTree_TopDown(TreeDataSet* D,int label):UnorderedTree(){};
#endif
