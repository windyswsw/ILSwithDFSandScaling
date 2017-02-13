/*
 * Main.cpp
 *
 *  Created on: Oct 19, 2015
 *      Author: u99319
 */

/*
 * Test.cpp
 *
 *  Created on: 25 June 2014
 *      Author: Windhya Rankothge
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iterator>
#include <map>
#include <time.h>
#include <algorithm>
#include <stdio.h>
#include <cstring>
#include <iomanip>
#include <stdarg.h>
#include <cstdarg>
#include <ctime>
//#include <chrono>
#include <unordered_map>
#include <set>

#include "StructsUse.h"
#include "FormatTime.h"
#include "GenLists.h"
#include "InitialListsGeneration.h"
#include "LinksPathsCheck.h"
#include "UpdateLists.h"
#include "ObjectValueCalculation.h"
#include "Print.h"
#include "PrintAllocation.h"
////#include "PrintTime.h"
#include "DFS.h"
//#include "Random.h"
#include "InitialPolicyImplementation.h"
#include "AddRemovePolicy.h"
#include "AddRemoveNF.h"
#include "InitialAcceptanceCriteria.h"
#include "ScaleAcceptanceCriteria.h"
#include "InitialPerturbationWithOnePolicyMove.h"
#include "InitialPerturbation.h"
#include "ScalePerturbationWithOneNFMove.h"
#include "ScalePerturbation.h"
#include "InitialLocalSearchWithOnePolicyMove.h"
#include "InitialLocalSearch.h"
#include "ScaleLocalSearchWithOneNFMove.h"
#include "ScaleLocalSearch.h"
#include "InitialRepeatProcedure.h"
#include "ScaleRepeatProcedure.h"
#include "PolicyChangeListForRound.h"
#include "AddInstance.h"
#include "DeleteInstance.h"
#include "ScaleGlobal.h"
#include "ScaleLocal.h"

//int main(int argc, char* argv[]){

 int main(){

	/*if( (argc < 6) || argc > 6){
		cout << "Usage: <NoOfExperiments> <NoOfGenerations> <NoOfServers> <ServerCap> <NoOfLinks> <LinksCap>" << "\n";
		return -1;
	}
	
	else{*/

		/*int experiments = atoi(argv[1]);
		float NoOfServers = atof(argv[2]);
		int ServerCap = atoi(argv[3]);
		float NoOfLinks = atof(argv[4]);
		int MaxLinksCap = atoi(argv[5]);*/

		//int InitialTerminationConditionRounds = atoi(argv[1]);
		//int ScaleTerminationConditionRounds = atoi(argv[2]);

		int InitialNoOfPoliciesToChangeInPerturbation = 5;
		int InitialTerminationConditionRounds = 1;

		int ScalePercentageOfPoliciesToChangeInPerturbation = 50;
		int ScaleTerminationConditionRounds = 2;

		int experiments = 1;
		float NoOfServers = 64;
		float NoOfLinks = 96;
		float MaxLinksCap = 3000;
		float ServerCap = 1000;

		bool initial = true;
		bool scaleglobal = true;
		bool scalelocal1 = true;
		bool scalelocal2 = true;
    
			// weights to use for initial policy implementation (Here we use only first 3 parameters) 
		int w1 = 1;
		int w2 = 1;
		int w3 = 1;
		int w4 = 0;
		int w5 = 0;

			// weights to use for local1 (All for full eq, last two when changes eq) 
		int l1w1 = 1;
		int l1w2 = 1;
		int l1w3 = 1;
		int l1w4 = 1;
		int l1w5 = 1;

		// weights to use for local2 (All for full eq, last two when changes eq) 
		int l2w1 = 0;
		int l2w2 = 0;
		int l2w3 = 0;
		int l2w4 = 1;
		int l2w5 = 1;

			// weights to use for global
		int gw1 = 1;
		int gw2 = 1;
		int gw3 = 1;
		int gw4 = 0;
		int gw5 = 0;


		time_t currentTime;
    	struct tm *currentDate;

    	time(&currentTime);
    	currentDate = localtime(&currentTime);
    	
		char filename1[256] = {0};
		strcpy(filename1,"Time.csv");
		//strcat(filename1, fmt("-%d-%d-%d@%d.%d.%d.csv", currentDate->tm_mday, currentDate->tm_mon+1, currentDate->tm_year+1900, currentDate->tm_hour, currentDate->tm_min, currentDate->tm_sec).c_str());

		ofstream myfile1;
		myfile1.open (filename1, ios::out | ios::app);

		/*typedef std::chrono::high_resolution_clock Time1;
		typedef std::chrono::microseconds ms;*/

		char filename3[256] = {0};
		strcpy(filename3,"Summary");
		strcat(filename3, fmt("-%d-%d-%d@%d.%d.%d.csv", currentDate->tm_mday, currentDate->tm_mon+1, currentDate->tm_year+1900, currentDate->tm_hour, currentDate->tm_min, currentDate->tm_sec).c_str());
		ofstream myfile3;
		myfile3.open (filename3, ios::out | ios::app);

		srand((unsigned int) time(0));

		for(int r=0; r<experiments;r++){

			CommonList NewCommonList;
			GenCommonList(&NewCommonList, NoOfServers, ServerCap, NoOfLinks, MaxLinksCap);

			UniqueList NewUniqueList;
			GenUniqueList(&NewUniqueList, NoOfServers, ServerCap, NoOfLinks, MaxLinksCap);

			FullSol InitialFullSolution;
			InitialFullSolution.CurLists = NewUniqueList;

			InitialFullSolution.ObjVal = 0;
			InitialFullSolution.InitialObjVal = 0;

			bool sucess = false;

			sucess = InitialPolicyImplementation(&InitialFullSolution, &NewCommonList, NoOfLinks, NoOfServers, MaxLinksCap);

 			if(sucess == true){

				GlobalObjectValueCalculation(&InitialFullSolution, NoOfServers, NoOfLinks, MaxLinksCap, w1, w2, w3, w4, w5);
				InitialFullSolution.InitialObjVal = InitialFullSolution.ObjVal;
				PrintSummaryGlobal(r, &InitialFullSolution, myfile3, NoOfServers, NoOfLinks, MaxLinksCap,'D', w1, w2, w3, w4, w5);

				//PrintAllocation(0, &InitialFullSolution, myfile1, NoOfServers, NoOfLinks);
				//auto start2 = Time1::now();

				InitialLocalSearch(&InitialFullSolution, &NewCommonList, NoOfServers, NoOfLinks, MaxLinksCap, myfile1, myfile3, w1, w2, w3, w4, w5);
				InitialFullSolution.InitialObjVal = InitialFullSolution.ObjVal;	
				PrintSummaryGlobal(r, &InitialFullSolution, myfile3, NoOfServers, NoOfLinks, MaxLinksCap,'I', w1, w2, w3, w4, w5);
				InitialRepeatProcedure(&InitialFullSolution,  &NewCommonList, InitialNoOfPoliciesToChangeInPerturbation, NoOfLinks, NoOfServers, MaxLinksCap, myfile1, myfile1,myfile3, InitialTerminationConditionRounds, w1, w2, w3, w4, w5);
				PrintSummaryGlobal(r, &InitialFullSolution, myfile3, NoOfServers, NoOfLinks, MaxLinksCap,'R', w1, w2, w3, w4, w5);

				if(scaleglobal == true && initial == true){
					vector<ChangeInfo> FullChangeListCur1;
					gen_PolicyChangeListSpecial(&FullChangeListCur1, "ChangeInfo.txt");
					NewCommonList.FullChangeListCur = FullChangeListCur1;
					ScaleGlobal(&InitialFullSolution, &NewCommonList, NoOfServers, ServerCap, NoOfLinks, MaxLinksCap, gw1, gw2, gw3, gw4, gw5, ScalePercentageOfPoliciesToChangeInPerturbation, ScaleTerminationConditionRounds, myfile3);
				}

				if(scalelocal1 == true && initial == true){
					vector<ChangeInfo> FullChangeListCur2;
					gen_PolicyChangeListSpecial(&FullChangeListCur2, "ChangeInfo.txt");
					NewCommonList.FullChangeListCur = FullChangeListCur2;
					ScaleLocal(&InitialFullSolution, &NewCommonList, NoOfServers, ServerCap, NoOfLinks, MaxLinksCap, l1w1, l1w2, l1w3, l1w4, l1w5, ScalePercentageOfPoliciesToChangeInPerturbation, ScaleTerminationConditionRounds, myfile3);
				}

				/*if(scalelocal2 == true && initial == true){
					vector<ChangeInfo> FullChangeListCur3;
					gen_PolicyChangeListSpecial(&FullChangeListCur3, "ChangeInfo.txt");
					NewCommonList.FullChangeListCur = FullChangeListCur3;
					ScaleLocal(&InitialFullSolution, &NewCommonList, NoOfServers, ServerCap, NoOfLinks, MaxLinksCap, l1w1, l1w2, l1w3, l1w4, l1w5, ScalePercentageOfPoliciesToChangeInPerturbation, ScaleTerminationConditionRounds, myfile3);
				}*/

				/*auto end2 = Time1::now();
    			double elapsed2 = std::chrono::duration_cast<ms> (end2 - start2).count();
    			myfile1 <<  r << "," << elapsed2 << "\n";*/

			}
		}
	
		myfile1.close();
		//myfile2.close();
		myfile3.close();
	//}
	return 0;
}



