/*
 *      TreeFactory.h
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

class TreeFactory {

public:

   virtual Tree* create_tree() = 0;
   virtual Tree* create_tree(TreeDataSet* D) = 0;
   virtual Tree* create_tree(TreeDataSet* D,int label) = 0;
   virtual Tree* create_tree(Tree *t) = 0;

};

 

class OrderedTreeFactory : public TreeFactory {

public:

   Tree* create_tree() { return new OrderedTree();}
   Tree* create_tree(TreeDataSet* D){ return new OrderedTree(D);};
   Tree* create_tree(TreeDataSet* D,int label){ return new OrderedTree(D,label);};
   Tree* create_tree(Tree *t){ return new OrderedTree((OrderedTree*) t);};

};

class UnorderedTreeFactory : public TreeFactory {

public:

   Tree* create_tree() { return new UnorderedTree();}
   Tree* create_tree(TreeDataSet* D){ return new UnorderedTree(D);};
   Tree* create_tree(TreeDataSet* D,int label){ return new UnorderedTree(D,label);};
   Tree* create_tree(Tree *t){ return new UnorderedTree((UnorderedTree*)t);};

}; 

class OrderedTree_TopDown_Factory : public TreeFactory {

public:

   Tree* create_tree() { return new OrderedTree_TopDown();}
   Tree* create_tree(TreeDataSet* D){ return new OrderedTree_TopDown(D);};
   Tree* create_tree(TreeDataSet* D,int label){ return new OrderedTree_TopDown(D,label);};
   Tree* create_tree(Tree *t){ return new OrderedTree_TopDown((OrderedTree_TopDown*) t);};

};

class UnorderedTree__TopDown_Factory : public TreeFactory {

public:

   Tree* create_tree() { return new UnorderedTree_TopDown();}
   Tree* create_tree(TreeDataSet* D){ return new UnorderedTree_TopDown(D);};
   Tree* create_tree(TreeDataSet* D,int label){ return new UnorderedTree_TopDown(D,label);};
   Tree* create_tree(Tree *t){ return new UnorderedTree_TopDown((UnorderedTree_TopDown*)t);};

}; 

 
