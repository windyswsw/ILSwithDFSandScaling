void RemovePolicy(FullSol* FullSolution, CommonList* CurCommonList, int policyIndex, float NoOfServers, float NoOfLinks, short int MaxLinksCap){

	short int BW = CurCommonList->PolicyList[policyIndex].BW;
	short int NoOfNFs = CurCommonList->PolicyList[policyIndex].NFIDs.size();

	for(int loop=0; loop<NoOfNFs; loop++){
		NFAllocation NF = FullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[loop];
		int NFSvr = NF.ServerID;	
		short int NFID = NF.NFID;
		int NFCapacity = CurCommonList->NFCap[NFID];
		SinglePath CurPath = NF.path;

		int OldServerCap = FullSolution->CurLists.ServerCapArray[NFSvr];
		auto  it5Again = FullSolution->CurLists.ServerCapHashTable.find(OldServerCap);
		it5Again->second.ServerIndex.erase(NFSvr);
			
		if(it5Again->second.ServerIndex.size() <= 0){
			FullSolution->CurLists.ServerCapHashTable.erase(OldServerCap);
			FullSolution->CurLists.ServerCapMap.erase(OldServerCap);
		}
						
		int OldServerNewCap = FullSolution->CurLists.ServerCapArray[NFSvr] + NFCapacity;
		FullSolution->CurLists.ServerCapArray[NFSvr] = OldServerNewCap;
		FullSolution->CurLists.ServerUsedList[NFSvr] = FullSolution->CurLists.ServerUsedList[NFSvr] - 1;
						
		std::map<short int,short int>::iterator  it6Again = FullSolution->CurLists.ServerCapMap.find(OldServerNewCap);
			if( it6Again != FullSolution->CurLists.ServerCapMap.end()){
				auto it7Again = FullSolution->CurLists.ServerCapHashTable.find(OldServerNewCap);
				it7Again->second.ServerIndex.insert(NFSvr);
			}
		else{
			FullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(OldServerNewCap,1));
			set<int> ValsNew;
			ValsNew.insert(NFSvr);
			VecForHashTable NewVecForHashTable;
			NewVecForHashTable.ServerIndex = ValsNew;
			FullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(OldServerNewCap,NewVecForHashTable));
		}

		// remove the old in path / path

		if(CurPath.Path.size() > 1){
			Update_LinksMatrix(FullSolution, CurPath, BW,"A"); // remove old assigned path by adding BW from relavant links
		}

		// remove the old out path
		if(loop == (NoOfNFs-1)){
			SinglePath OutPathOld = Check_OutPath(NFSvr,CurCommonList);
			Update_LinksMatrix(FullSolution, OutPathOld, BW,"A"); // remove old assigned path by adding BW from relavant links
		}
	}

	//ObjectValueCalculation(FullSolution, NoOfServers, NoOfLinks, MaxLinksCap);
}

void AddPolicy(FullSol* FullSolution, CommonList* CurCommonList, int policyIndex, float NoOfServers, float NoOfLinks, short int MaxLinksCap, int w1, int w2, int w3, int w4, int w5){

	short int BW = CurCommonList->PolicyList[policyIndex].BW;
	short int NoOfNFs = CurCommonList->PolicyList[policyIndex].NFIDs.size();

	for(int loop=0; loop<NoOfNFs; loop++){
		NFAllocation NF = FullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[loop];
		int NFSvr = NF.ServerID;	
		short int NFID = NF.NFID;
		int NFCapacity = CurCommonList->NFCap[NFID];
		SinglePath CurPath = NF.path;

		int OldServerCap = FullSolution->CurLists.ServerCapArray[NFSvr];
		auto  it5Again = FullSolution->CurLists.ServerCapHashTable.find(OldServerCap);
		it5Again->second.ServerIndex.erase(NFSvr);
			
		if(it5Again->second.ServerIndex.size() <= 0){
			FullSolution->CurLists.ServerCapHashTable.erase(OldServerCap);
			FullSolution->CurLists.ServerCapMap.erase(OldServerCap);
		}
						
		int OldServerNewCap = FullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
		FullSolution->CurLists.ServerCapArray[NFSvr] = OldServerNewCap;
		FullSolution->CurLists.ServerUsedList[NFSvr] = FullSolution->CurLists.ServerUsedList[NFSvr] + 1;
						
		std::map<short int,short int>::iterator  it6Again = FullSolution->CurLists.ServerCapMap.find(OldServerNewCap);
			if( it6Again != FullSolution->CurLists.ServerCapMap.end()){
				auto it7Again = FullSolution->CurLists.ServerCapHashTable.find(OldServerNewCap);
				it7Again->second.ServerIndex.insert(NFSvr);
			}
		else{
			FullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(OldServerNewCap,1));
			set<int> ValsNew;
			ValsNew.insert(NFSvr);
			VecForHashTable NewVecForHashTable;
			NewVecForHashTable.ServerIndex = ValsNew;
			FullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(OldServerNewCap,NewVecForHashTable));
		}

		// remove the old in path / path

		if(CurPath.Path.size() > 1){
			Update_LinksMatrix(FullSolution, CurPath, BW,"D"); // assign old  path by deleting BW from relavant links
		}

		// remove the old out path
		if(loop == (NoOfNFs-1)){
			SinglePath OutPathOld = Check_OutPath(NFSvr,CurCommonList);
			Update_LinksMatrix(FullSolution, OutPathOld, BW,"D"); // assign old path by deleting BW from relavant links
		}
	}

	GlobalObjectValueCalculation(FullSolution, NoOfServers, NoOfLinks, MaxLinksCap, w1, w2, w3, w4, w5);
}
