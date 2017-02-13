/*
 * Print.h
 *
 *  Created on: Oct 19, 2015
 *      Author: u99319
 */

void PrintAllocation(int exp, FullSol* FinalSolution, ofstream &myfile){

	myfile << exp << "\n";

	for(int i=0; i<FinalSolution->VectorOfPartialSols.size();i++){
		for(int j=0; j<FinalSolution->VectorOfPartialSols[i].VMsForAPolicy.size();j++){
			NFAllocation valnew = FinalSolution->VectorOfPartialSols[i].VMsForAPolicy[j];
			myfile << valnew.PolicyID << " ";
			myfile << valnew.ServerID << " ";
			for(int g=0; g< valnew.path.Path.size(); g++){
				myfile << valnew.path.Path[g] << "-";
			}
			myfile << "\n";
		}
	}

	for(int i=1; i<FinalSolution->CurLists.ServerCapArray.size();i++){
		myfile << i << "-" << FinalSolution->CurLists.ServerCapArray[i] << "," ;
		myfile << "\n";
	}

	myfile << "\n";

	int a  = sizeof(FinalSolution->CurLists.LinksLists.LinksUse)/sizeof(FinalSolution->CurLists.LinksLists.LinksUse[0]);

	for(int i=0; i < a; ++i ){
		for(int j = 0; j < a; ++j){
			if(FinalSolution->CurLists.LinksLists.LinksUse[i][j] >= 0){
				float usedBW = FinalSolution->CurLists.LinksLists.LinksBW[i][j];
				myfile << i << "-" << j << " " << usedBW;
				myfile << "\n";
			}
		}
	}
	myfile << "\n";
}
