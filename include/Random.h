/*
 * DFS.h
 *
 *  Created on: Oct 19, 2015
 *      Author: u99319
 */

void Random(FullSol* InitialFullSolution, CommonList* CurCommonList, int policyIndex, int NoOfServers){

	bool sol = false;
	short int BW = CurCommonList->PolicyList[policyIndex].BW;
	short int NoOfNFs = CurCommonList->PolicyList[policyIndex].NFIDs.size();

	short int NF1ID = CurCommonList->PolicyList[policyIndex].NFIDs[0];
	short int NF1Capacity = CurCommonList->NFCap[NF1ID];

	int NF1SvrID = 0;

	std::map<short int,short int>::iterator itb = InitialFullSolution->CurLists.ServerCapMap.lower_bound(NF1Capacity);
	int B = itb->first;

	std::map<short int,short int>::reverse_iterator ite = InitialFullSolution->CurLists.ServerCapMap.rbegin();
	int E = ite->first;

	int FoundCap = 0;

	if( B != E){
		int R = rand() % (E-B) + B;
		std::map<short int,short int>::iterator itr = InitialFullSolution->CurLists.ServerCapMap.lower_bound(R);
		FoundCap = itr->first;
	}
	else{
		FoundCap = B;
	}

	auto it2 = InitialFullSolution->CurLists.ServerCapHashTable.find(FoundCap);

	int RandLimit = NoOfServers-1;
	int RandNum = rand() % RandLimit + 1;
	std::set<int>::iterator itv = it2->second.ServerIndex.lower_bound(RandNum);
	if(itv != it2->second.ServerIndex.end()){
		NF1SvrID = *itv;
		it2->second.ServerIndex.erase(itv);
	}
	else{
		std::set<int>::iterator itv1 = it2->second.ServerIndex.upper_bound(RandNum);
		if(itv1 != it2->second.ServerIndex.end()){
			NF1SvrID = *itv1;
			it2->second.ServerIndex.erase(itv1);
		}
		else{
			std::set<int>::iterator itv2 = it2->second.ServerIndex.begin();
			NF1SvrID = *itv2;
			it2->second.ServerIndex.erase(itv2);
		}
	}

	if(it2->second.ServerIndex.size() <= 0){
		InitialFullSolution->CurLists.ServerCapHashTable.erase(FoundCap);
		InitialFullSolution->CurLists.ServerCapMap.erase(FoundCap);
	}

	int NewCapOfNewServer = InitialFullSolution->CurLists.ServerCapArray[NF1SvrID] - NF1Capacity;
	InitialFullSolution->CurLists.ServerCapArray[NF1SvrID] = NewCapOfNewServer;
	InitialFullSolution->CurLists.ServerUsedList[NF1SvrID] = InitialFullSolution->CurLists.ServerUsedList[NF1SvrID] + 1;

	std::map<short int,short int>::iterator  it3 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfNewServer);
	if( it3 != InitialFullSolution->CurLists.ServerCapMap.end()){
		auto it4 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfNewServer);
		it4->second.ServerIndex.insert(NF1SvrID);
	}
	else{
		InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfNewServer,1));
		set<int> ValsNew;
		ValsNew.insert(NF1SvrID);
		VecForHashTable NewVecForHashTable;
		NewVecForHashTable.ServerIndex = ValsNew;
		InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfNewServer,NewVecForHashTable));
	}

	SinglePath InPNew;
	InPNew = Find_InPath(NF1SvrID, CurCommonList);

	PartialSol newVMMap;
	NFAllocation NF1;
	NF1.NFID = NF1ID;
	NF1.ServerID = NF1SvrID;
	NF1.path = InPNew;

	Update_LinksMatrix(InitialFullSolution, InPNew , BW,"D");

	NF1.PolicyID = policyIndex + 1;
	newVMMap.VMsForAPolicy.push_back(NF1);

	//InitialFullSolution.CurLists = Update_ServerList(InitialFullSolution.CurLists, NF1SvrID, NF1Capacity, "D");

	if(NoOfNFs > 1){
		do{
			if((!(newVMMap.VMsForAPolicy.empty())) && (newVMMap.VMsForAPolicy.size() >= 1) && (sol == false)){
				if((newVMMap.VMsForAPolicy.size() <= NoOfNFs-1) && (sol == false)){
					short int NFsCount = newVMMap.VMsForAPolicy.size();

					if(NFsCount < NoOfNFs){

						int FindPrevNFPosition = newVMMap.VMsForAPolicy.size()- 1;

						bool IsLastNF = false;
						if((NFsCount + 1) == NoOfNFs){
							IsLastNF = true;
						}

						NFAllocation PrevNF = newVMMap.VMsForAPolicy[FindPrevNFPosition];
						int PrevNFServer = PrevNF.ServerID;

						int CurrentLoopNFID = CurCommonList->PolicyList[policyIndex].NFIDs[NFsCount];
						int CurrentLoopNFCapacity = CurCommonList->NFCap[CurrentLoopNFID];

						int CurrentLoopNFSvr = 0;
						std::map<short int,short int>::iterator itb2 = InitialFullSolution->CurLists.ServerCapMap.lower_bound(CurrentLoopNFCapacity);
						int B2 = itb2->first;

						std::map<short int,short int>::reverse_iterator ite2 = InitialFullSolution->CurLists.ServerCapMap.rbegin();
						int E2 = ite2->first;

						int FoundCap2 = 0;

						if( B2 != E2){
							int R2 = rand() % (E2-B2) + B2;
							std::map<short int,short int>::iterator itr2 = InitialFullSolution->CurLists.ServerCapMap.lower_bound(R2);
							FoundCap2 = itr2->first;
						}
						else{
							FoundCap2 = B2;
						}

						auto it22 = InitialFullSolution->CurLists.ServerCapHashTable.find(FoundCap2);

						int RandLimit2 = NoOfServers-1;
						int RandNum2 = rand() % RandLimit2 + 1;
						std::set<int>::iterator itv2 = it22->second.ServerIndex.lower_bound(RandNum2);
						if(itv2 != it22->second.ServerIndex.end()){
							CurrentLoopNFSvr = *itv2;
							it22->second.ServerIndex.erase(itv2);
						}
						else{
							std::set<int>::iterator itv12 = it22->second.ServerIndex.upper_bound(RandNum);
							if(itv12 != it22->second.ServerIndex.end()){
								CurrentLoopNFSvr = *itv12;
								it22->second.ServerIndex.erase(itv12);
							}
							else{
								std::set<int>::iterator itv22 = it22->second.ServerIndex.begin();
								CurrentLoopNFSvr = *itv22;
								it22->second.ServerIndex.erase(itv22);
							}
						}

						if(it22->second.ServerIndex.size() <= 0){
							InitialFullSolution->CurLists.ServerCapHashTable.erase(FoundCap2);
							InitialFullSolution->CurLists.ServerCapMap.erase(FoundCap2);
						}

						//if(SvrArray.size()>0){	//No servers

							if(PrevNFServer != CurrentLoopNFSvr){
								SinglePath PathToNext = Find_Shortest_PathRandom(PrevNFServer, CurrentLoopNFSvr, CurCommonList);

								if(PathToNext.Path.size() != 0 ){

									NFAllocation valLoop;
									valLoop.ServerID = CurrentLoopNFSvr;
									valLoop.NFID = CurrentLoopNFID;
									valLoop.PolicyID = policyIndex + 1;
									valLoop.path = PathToNext;

									Update_LinksMatrix(InitialFullSolution, PathToNext, BW,"D");

									if(IsLastNF == true){
										SinglePath OutP = Find_OutPath(CurrentLoopNFSvr,CurCommonList);
										Update_LinksMatrix(InitialFullSolution, OutP, BW,"D");
									}

									//InitialFullSolution.CurLists = Update_ServerList(InitialFullSolution.CurLists, CurrentLoopNFSvr, CurrentLoopNFCapacity, "D");

									int NewCapOfNewServer1 = InitialFullSolution->CurLists.ServerCapArray[CurrentLoopNFSvr] - CurrentLoopNFCapacity;
									InitialFullSolution->CurLists.ServerCapArray[CurrentLoopNFSvr] = NewCapOfNewServer1;
									InitialFullSolution->CurLists.ServerUsedList[CurrentLoopNFSvr] = InitialFullSolution->CurLists.ServerUsedList[CurrentLoopNFSvr] + 1;

									std::map<short int,short int>::iterator  it31 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfNewServer1);
									if( it31 != InitialFullSolution->CurLists.ServerCapMap.end()){
										auto it41 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfNewServer1);
										it41->second.ServerIndex.insert(CurrentLoopNFSvr);
									}
									else{
										InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfNewServer1,1));
										set<int> ValsNew1;
										ValsNew1.insert(CurrentLoopNFSvr);
										VecForHashTable NewVecForHashTable1;
										NewVecForHashTable1.ServerIndex = ValsNew1;
										InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfNewServer1,NewVecForHashTable1));
									}

									newVMMap.VMsForAPolicy.push_back(valLoop);
								}
							}
							else{

								if(IsLastNF == true){
									SinglePath OutP = Find_OutPath(CurrentLoopNFSvr,CurCommonList);
									Update_LinksMatrix(InitialFullSolution, OutP, BW,"D");
								}

								NFAllocation valLoop;
								valLoop.ServerID = CurrentLoopNFSvr;
								valLoop.NFID = CurrentLoopNFID;
								valLoop.PolicyID = policyIndex + 1;
								valLoop.path.Path.push_back(0); // since they are in same server, no path, so it is indicated by a zero

								//InitialFullSolution.CurLists = Update_ServerList(InitialFullSolution.CurLists, CurrentLoopNFSvr, CurrentLoopNFCapacity, "D");

								int NewCapOfNewServer1 = InitialFullSolution->CurLists.ServerCapArray[CurrentLoopNFSvr] - CurrentLoopNFCapacity;
								InitialFullSolution->CurLists.ServerCapArray[CurrentLoopNFSvr] = NewCapOfNewServer1;
								InitialFullSolution->CurLists.ServerUsedList[CurrentLoopNFSvr] = InitialFullSolution->CurLists.ServerUsedList[CurrentLoopNFSvr] + 1;

								std::map<short int,short int>::iterator  it31 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfNewServer1);
								if( it31 != InitialFullSolution->CurLists.ServerCapMap.end()){
									auto it41 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfNewServer1);
									it41->second.ServerIndex.insert(CurrentLoopNFSvr);
								}
								else{
									InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfNewServer1,1));
									set<int> ValsNew1;
									ValsNew1.insert(CurrentLoopNFSvr);
									VecForHashTable NewVecForHashTable1;
									NewVecForHashTable1.ServerIndex = ValsNew1;
									InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfNewServer1,NewVecForHashTable1));
								}

								newVMMap.VMsForAPolicy.push_back(valLoop);
							}
						//}
						//else{ // else for no servers
							//NewPartialSolListTemp.NewParSol.erase(NewPartialSolListTemp.NewParSol.begin());
						//}
					}
				}
				else if(!newVMMap.VMsForAPolicy.empty()){ // if a suggestion has VMs for all NFs, save that suggestion in permenant list
					InitialFullSolution->VectorOfPartialSols.push_back(newVMMap);
					sol = true;
				}
			}
		} while((!(newVMMap.VMsForAPolicy.empty())) && (newVMMap.VMsForAPolicy.size() >= 1) && (sol == false));

	}

	else if((NoOfNFs = 1) && (!(newVMMap.VMsForAPolicy.empty()))){
		InitialFullSolution->VectorOfPartialSols.push_back(newVMMap);
	}

}

