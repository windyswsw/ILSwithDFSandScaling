void InitialAcceptanceCriteria(FullSol* FullSolution, CommonList* CurCommonLists, float NoOfServers, float NoOfLinks, short int MaxLinksCap){

	float InitialObj = FullSolution->InitialObjVal;
	float CurObj = FullSolution->ObjVal;

	if(InitialObj < CurObj){ // Initial Solution is better

		int NoOfChanges = FullSolution->VectorOfChangedSols.size() - 1;

		for(int i=NoOfChanges; i>=0; i--){

			//////////////////////////////////////////////////////////////
			// undo the changed NF
			short int NoOfNFs = FullSolution->VectorOfChangedSols[i].VMsForAPolicy.size();
			NFAllocation NF1 = FullSolution->VectorOfChangedSols[i].VMsForAPolicy[0];
			int policyIndex = NF1.PolicyID;
			short int BW = CurCommonLists->PolicyList[policyIndex-1].BW;

			for(int loop=0; loop<NoOfNFs; loop++){
				NFAllocation NFO = FullSolution->VectorOfChangedSols[i].VMsForAPolicy[loop];
				int NFSvrO = NFO.ServerID;	
				short int NFIDO = NFO.NFID;
				int NFCapacityO = CurCommonLists->NFCap[NFIDO];
				SinglePath CurPathO = NFO.path;

				int OldServerCapO = FullSolution->CurLists.ServerCapArray[NFSvrO];
				auto  it5AgainO = FullSolution->CurLists.ServerCapHashTable.find(OldServerCapO);
				it5AgainO->second.ServerIndex.erase(NFSvrO);
			
				if(it5AgainO->second.ServerIndex.size() <= 0){
					FullSolution->CurLists.ServerCapHashTable.erase(OldServerCapO);
					FullSolution->CurLists.ServerCapMap.erase(OldServerCapO);
				}
						
				int OldServerNewCapO = FullSolution->CurLists.ServerCapArray[NFSvrO] + NFCapacityO;
				FullSolution->CurLists.ServerCapArray[NFSvrO] = OldServerNewCapO;
				FullSolution->CurLists.ServerUsedList[NFSvrO] = FullSolution->CurLists.ServerUsedList[NFSvrO] - 1;
						
				std::map<short int,short int>::iterator  it6AgainO = FullSolution->CurLists.ServerCapMap.find(OldServerNewCapO);
					if( it6AgainO != FullSolution->CurLists.ServerCapMap.end()){
						auto it7AgainO = FullSolution->CurLists.ServerCapHashTable.find(OldServerNewCapO);
						it7AgainO->second.ServerIndex.insert(NFSvrO);
					}
				else{
					FullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(OldServerNewCapO,1));
					set<int> ValsNewO;
					ValsNewO.insert(NFSvrO);
					VecForHashTable NewVecForHashTableO;
					NewVecForHashTableO.ServerIndex = ValsNewO;
					FullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(OldServerNewCapO,NewVecForHashTableO));
				}

				// remove the old in path / path

				if(CurPathO.Path.size() > 1){
					Update_LinksMatrix(FullSolution, CurPathO, BW,"A"); // remove old assigned path by adding BW from relavant links
				}

				// remove the old out path
				if(loop == (NoOfNFs-1)){
					SinglePath OutPathOldO = Check_OutPath(NFSvrO,CurCommonLists);
					Update_LinksMatrix(FullSolution, OutPathOldO, BW,"A"); // remove old assigned out path by adding BW from relavant links
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// redo the original NF

				NFAllocation NF = FullSolution->VectorOfOriginalSols[i].VMsForAPolicy[loop];
				int NFSvr = NF.ServerID;	
				short int NFID = NF.NFID;
				int NFCapacity = CurCommonLists->NFCap[NFID];
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

				// add the old in path / path

				if(CurPath.Path.size() > 1){
					Update_LinksMatrix(FullSolution, CurPath, BW,"D"); // add old assigned path by deleting BW from relavant links
				}

				// add the old out path
				if(loop == (NoOfNFs-1)){
					SinglePath OutPathOld = Check_OutPath(NFSvr,CurCommonLists);
					Update_LinksMatrix(FullSolution, OutPathOld, BW,"D"); // add old assigned out path by deliting BW from relavant links
				}
			}

			FullSolution->VectorOfPartialSols[policyIndex-1] = FullSolution->VectorOfOriginalSols[i];
		}

		FullSolution->ObjVal = InitialObj;

	}

}
