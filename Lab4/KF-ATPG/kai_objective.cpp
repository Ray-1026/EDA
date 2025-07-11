
/* =========FOR INTERNAL USE ONLY. NO DISTRIBUTION PLEASE ========== */

/*********************************************************************
 Copyright 2002-2006, University of California at Santa Barbara (UCSB).  
 All rights reserved. 

 --- This software and any associated documentation is provided "as is" 

 UCSB and the authors MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS 
 OR IMPLIED, INCLUDING THOSE OF MERCHANTABILITY OR FITNESS FOR A 
 PARTICULAR PURPOSE, OR THAT  USE OF THE SOFTWARE, MODIFICATIONS, OR 
 ASSOCIATED DOCUMENTATION WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS, 
 TRADEMARKS OR OTHER INTELLECTUAL PROPERTY RIGHTS OF A THIRD PARTY.  

 UCSB and the authors shall not be liable under any circumstances for 
 any direct, indirect, special, incidental, or consequential damages 
 with respect to any claim by USER or any third party on account of 
 or arising from the use, or inability to use, this software or its 
 associated documentation, even if UCSB and/or the authors has been advised
 of the possibility of those damages.
*********************************************************************/
// Author: Kai Yang (kyang@ece.ucsb.edu)

using namespace std;

#include <string.h>
#include <cstdlib>
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "GetLongOpt.h"
#include "kai_gate.h"
#include "kai_path.h"
#include "kai_objective.h"
#include "kai_typeemu.h"

extern GetLongOpt option;
extern map<string, KaiGATE*> CUTNameTable;

string ToCUTName(KaiGATE *gptr, int TF)
{
	char tmp[100];
	sprintf(tmp, "%d", TF);
	string dupname;
	if(gptr->GetFunction()!=G_DFF)
		dupname=gptr->GetName()+"_dup_t"+tmp;
	else
		dupname=gptr->GetName()+"_ppi_t"+tmp;
	KaiGATE *tg=CUTNameTable[dupname];
	if(!tg) { cerr<<"Miss ToCUTName()"<<endl; exit(-1); }
	return dupname;
}

void AtpgObj::BuildFromPath_NR(PATH *pptr)
{
	cleanup();
	KaiGATE *CurG, *PreG;
	TRANSITION CurT, PreT;
	assert(pptr->NoGate()==pptr->NoTrans());

	PreG=pptr->GetGate(0);	/* input gate on sensitive path*/
	PreT=pptr->GetTrans(0);	/* input transition on sensitive path*/
	
	// Fault Activation at 1st TimeFrame
	if(PreT==R) AddObj(ToCUTName(PreG, 0), /*value*/ 0);
	else if(PreT==F) AddObj(ToCUTName(PreG, 0), /*value*/ 1);
	else { cerr<<"R/F Error !"<<endl; exit(-1); }

	// Fault Activation at 2nd TimeFrame 
	if(PreT==R) AddObj(ToCUTName(PreG, 1), /*value*/ 1);
	else if(PreT==F) AddObj(ToCUTName(PreG, 1), /*value*/ 0);
	else { cerr<<"R/F Error !"<<endl; exit(-1); }
	
   	/*Fault Propagation = off-input setting on sensitive path */
	for(int i=1;i<pptr->NoGate();++i){	
		CurG=pptr->GetGate(i);	// current gate
		CurT=pptr->GetTrans(i);	// current transition

		switch(CurG->GetFunction()){
			case G_PI:	// Primary Input
				if(CurT==R)
					AddObj(ToCUTName(CurG, 0), 0);
				else if(CurT==F)
					AddObj(ToCUTName(CurG, 0), 1);
				else{
					cerr<<"R/F Error !"<<endl; 
					exit(-1);
				}

				if(CurT==R)
					AddObj(ToCUTName(CurG, 1), 1);
				else if(CurT==F)
					AddObj(ToCUTName(CurG, 1), 0);
				else{
					cerr<<"R/F Error !"<<endl; 
					exit(-1);
				}
				break;

			case G_AND:		// AND gate, cv=0, ncv=1
			case G_NAND:	// NAND gate, cv=0, ncv=1
				if(PreT==R || PreT==F){	// Non-Robust: x->ncv
					for(int j=0;j<CurG->NoFanin();++j){
						if(PreG!=CurG->Fanin(j))
							AddObj(ToCUTName(CurG->Fanin(j), 1), 1);
					}
				}
				else {
					cerr<<"R/F Error !"<<endl; 
					exit(-1);
				}
				break;

			case G_OR:		// OR gate, cv=1, ncv=0
			case G_NOR:		// NOR gate, cv=1, ncv=0
				if(PreT==R || PreT==F){	// Non-Robust: x->ncv
					for(int j=0;j<CurG->NoFanin();++j){
						if(PreG!=CurG->Fanin(j))
							AddObj(ToCUTName(CurG->Fanin(j), 1), 0);
					}
				}
				else {
					cerr<<"R/F Error !"<<endl; 
					exit(-1);
				}
				break;
		}
		PreG=CurG;	// update PreG
		PreT=CurT;	// update PreT
	}
}

void AtpgObj::BuildFromPath_R(PATH *pptr)
{
	cleanup();
	KaiGATE *CurG, *PreG;
	TRANSITION CurT, PreT;
	assert(pptr->NoGate()==pptr->NoTrans());
    
	/*Do Fault Activation & Fault Propagation under Robust test setting*/
	PreG=pptr->GetGate(0);
	PreT=pptr->GetTrans(0);

	// Fault Activation at 1st TimeFrame
	if(PreT==R) AddObj(ToCUTName(PreG, 0), 0);
	else if(PreT==F) AddObj(ToCUTName(PreG, 0), 1);
	else { cerr<<"R/F Error !"<<endl; exit(-1); }

	// Fault Activation at 2nd TimeFrame 
	if(PreT==R) AddObj(ToCUTName(PreG, 1), 1);
	else if(PreT==F) AddObj(ToCUTName(PreG, 1), 0);
	else { cerr<<"R/F Error !"<<endl; exit(-1); }

	for(int i=1;i<pptr->NoGate();++i){
		CurG=pptr->GetGate(i);
		CurT=pptr->GetTrans(i);

		switch(CurG->GetFunction()){
			case G_PI:	// Primary Input
				if(CurT==R)
					AddObj(ToCUTName(CurG, 0), 0);
				else if(CurT==F)
					AddObj(ToCUTName(CurG, 0), 1);
				else{
					cerr<<"R/F Error !"<<endl; 
					exit(-1);
				}

				if(CurT==R)
					AddObj(ToCUTName(CurG, 1), 1);
				else if(CurT==F)
					AddObj(ToCUTName(CurG, 1), 0);
				else{
					cerr<<"R/F Error !"<<endl; 
					exit(-1);
				}
				break;

			case G_AND:		// AND gate, cv=0, ncv=1
			case G_NAND:	// NAND gate, cv=0, ncv=1
				if(PreT==R){	// Robust: x->ncv
					for(int j=0;j<CurG->NoFanin();++j){
						if(PreG!=CurG->Fanin(j))
							AddObj(ToCUTName(CurG->Fanin(j), 1), 1);
					}
				}
				else if(PreT==F){	// Robust: ncv->ncv
					for(int j=0;j<CurG->NoFanin();++j){
						if(PreG!=CurG->Fanin(j)){
							AddObj(ToCUTName(CurG->Fanin(j), 1), 1);
							AddObj(ToCUTName(CurG->Fanin(j), 0), 1);
						}
					}
				}
				else {
					cerr<<"R/F Error !"<<endl; 
					exit(-1);
				}
				break;

			case G_OR:		// OR gate, cv=1, ncv=0
			case G_NOR:		// NOR gate, cv=1, ncv=0
				if(PreT==R){	// Robust: ncv->ncv
					for(int j=0;j<CurG->NoFanin();++j){
						if(PreG!=CurG->Fanin(j)){
							AddObj(ToCUTName(CurG->Fanin(j), 1), 0);
							AddObj(ToCUTName(CurG->Fanin(j), 0), 0);
						}
					}
				}
				else if(PreT==F){	// Robust: x->ncv
					for(int j=0;j<CurG->NoFanin();++j){
						if(PreG!=CurG->Fanin(j))
							AddObj(ToCUTName(CurG->Fanin(j), 1), 0);
					}
				}
				else {
					cerr<<"R/F Error !"<<endl; 
					exit(-1);
				}
				break;
		}
		PreG=CurG;	// update PreG
		PreT=CurT;	// update PreT
	}
}


void AtpgObj::Dump()
{
	assert(_name.size()==_value.size());
	for(unsigned int i=0; i<_name.size(); ++i) {
		cout<<_name[i]<<"("<<_value[i]<<") ";
	}
	cout<<endl;
}
