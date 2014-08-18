/*
 *      SupportFactory.h
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

//Lattice.h
#ifndef _supportfactory_h_
#define _supportfactory_h_

#include <cmath>

class Support_Factory {

public:

   Support_Factory(int NumberRelaxedIntervals,double dblmin_sup) {
   		mNumberRelaxedIntervals=NumberRelaxedIntervals;
		mdblmin_sup=dblmin_sup;
   }
   virtual int RelaxedSupport(double dblSupport)  = 0;
   virtual double RelativeSupport(int iSupport)= 0;
   inline int RelaxedSupport(int AbsSupport,int Size) {
   		return RelaxedSupport(((double) AbsSupport)/((double) Size));
	};
   
protected:
	int mNumberRelaxedIntervals;
	double mdblmin_sup;
};



class Log_Relaxed_Support_Factory : public Support_Factory {

public:

	Log_Relaxed_Support_Factory(int NumberRelaxedIntervals,double dblmin_sup):Support_Factory(NumberRelaxedIntervals,dblmin_sup){};
	inline int RelaxedSupport(double dblSupport) {
		return (int)((double)mNumberRelaxedIntervals -((double)(mNumberRelaxedIntervals-1))*log (dblSupport)/log(mdblmin_sup));
	};
	inline double RelativeSupport(int iSupport) { //From Relaxed to Relative
		return exp( ( ((double)mNumberRelaxedIntervals - (double)iSupport) * log(mdblmin_sup)  )/ ( (double) (mNumberRelaxedIntervals -1))) ;
	};

};

class Linear_Relaxed_Support_Factory : public Support_Factory {

public:
	Linear_Relaxed_Support_Factory(int NumberRelaxedIntervals,double dblmin_sup):Support_Factory(NumberRelaxedIntervals,dblmin_sup){};
	inline int RelaxedSupport(double dblSupport) {
		//cout << "RELAXED SUPPORT "<<dblSupport << "-"<< (((double) mNumberRelaxedIntervals))<< endl;
		return (int) (((double) mNumberRelaxedIntervals)*dblSupport);
	};
	inline double RelativeSupport(int iSupport) { //From Relaxed to Relative
		return ((double) iSupport/(double)mNumberRelaxedIntervals);
	};
};
/*
class Not_Relaxed_Support_Factory : public Support_Factory {
//Is a Linear_Relaxed_Support_Factory where mNumberRelaxedIntervals is the SlidingWindowSize
public:
	Not_Relaxed_Support_Factory(int NumberRelaxedIntervals,double dblmin_sup):Support_Factory(NumberRelaxedIntervals,dblmin_sup){};
	inline int RelaxedSupport(double dblSupport) {
		return (int) (((double) mNumberRelaxedIntervals)*dblSupport);
	};
	inline double RelativeSupport(int iSupport) { //From Relaxed to Relative
		return ((double) iSupport/(double)mNumberRelaxedIntervals);
	};
};
*/
#endif
