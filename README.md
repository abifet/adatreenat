AdaTreeNat
==========

Mining Adaptively Frequent Closed Unlabeled Rooted Trees in Data Streams.

Albert Bifet and Ricard Gavaldà.

14th ACM SIGKDD Intl. Conference on Knowledge Discovery and Data Mining (KDD'08)

This software is an implementation of three closed unlabeled tree mining algorithms:
 - an incremental one, INCTREENAT
 - a sliding-window based one, WINTREENAT 
 - one that mines closed trees adaptively from data streams, ADATREENAT

INSTALL:
---------

To compile the program : type 'make' in the test directory
To clean: type 'make clean' in the test directory

PLATFORM:
---------
This program has been compiled using g++ 4.2 and tested on Linux.

LICENSE TERMS:
---------
Under GNU Public License terms. 

USAGE:
---------
The usage of the program is: AdaTreeNat [Options], where Options are:

- -s <support>    Set the absolute or relative minimum support  (default 10%)
- -ord            Ordered Trees
- -uno            Unordered Trees (default)
- -topdown        Top-down subtrees
- -induced        Induced subtrees (default)
- -in <filestem>  Set the name of the dataset (default 'input.trees')
- -out <filestem> Set the name of the output file (default 'output.trees')
- -TreeNat        TreeNat(default)
- -inc            IncTreeNat: Incremental Tree Mining
- -win            WinTreeNat:Sliding Window Tree Mining
- -ada1           AdaTreeNat1:Adaptive Sliding Window Tree Mining
- -ada2           AdaTreeNat2:Support ADWIN Tree Mining
- -relaxed        Uses relaxed support (default false)
- -log            Uses logarithmic relaxed support
- -b <batchSize>  Set size of the batches (default 10,000)
- -w <windowSize> Set size of the sliding window (default 100,000)
- -r <intervals>  Set the number of relaxed intervals (default 10,000)
- -help           Display this ussage help



The input should be a text file that uses:

- TreeNat's string encoding form :
   * each line represents a transaction
   * the string is a depth-first traversal of the rooted tree
     where "[" represents a new node and "]" means a backtrack
   
   For example, here is a database with 3 rooted trees.

	[[[][][]][[][]]]
	
	[[[][][]][[[]]]]
	
	[[[[[]]]]] 


- Professor Zaki's string encoding form: 
  * each line represents a transaction
  * the first two numbers are identical, they are tids
  * the 3rd number represents the length of the remaining string
  * the rest string is a depth-first traversal of the rooted tree, where "-1" means a backtrack.
  
   For example, here is a database with 3 rooted trees.

	0 0 15 1 1 1 -1 1 -1 1 -1 -1 1 1 -1 1 -1 -1
	
	1 1 15 1 1 1 -1 1 -1 1 -1 -1 1 1 1 -1 -1 -1
	
	2 2 9 1 1 1 1 1 -1 -1 -1 -1
