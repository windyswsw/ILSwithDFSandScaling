bool InitialPerturbationWithOnePolicyMove(FullSol* InitialFullSolution, CommonList* CurCommonList, int policyIndex, int CurrentNF, float NoOfServers, float NoOfLinks, short int MaxLinksCap, PartialSol* SelectedPolicyTemp, PartialSol* SelectedPolicyOriginal, int w1, int w2, int w3, int w4, int w5){

	float OriginalObj = InitialFullSolution->ObjVal;
	short int BW = CurCommonList->PolicyList[policyIndex].BW;
	short int NoOfNFs = CurCommonList->PolicyList[policyIndex].NFIDs.size();

	NFAllocation NF = SelectedPolicyTemp->VMsForAPolicy[CurrentNF];
	int CurSvr = NF.ServerID;

	short int NFID = CurCommonList->PolicyList[policyIndex].NFIDs[CurrentNF];
	short int NFCapacity = CurCommonList->NFCap[NFID];

	// find all servers
	vector<int> ServerArr;

	for (std::map<short int,short int>::iterator itlow=  InitialFullSolution->CurLists.ServerCapMap.lower_bound(NFCapacity); itlow!=InitialFullSolution->CurLists.ServerCapMap.end(); ++itlow){
		int FoundCap = itlow->first;
		auto it2 = InitialFullSolution->CurLists.ServerCapHashTable.find(FoundCap);

		for (std::set<int>::iterator itv = it2->second.ServerIndex.begin(); itv != it2->second.ServerIndex.end(); ++itv){
			if(*itv != CurSvr){
				ServerArr.push_back(*itv);
			}
		}
	}

	int SvrSize  = ServerArr.size() - 1;

	int a = 0;
	if(SvrSize > 0){
		a =  rand() % SvrSize + 1;
	}

	// vector with new server
	vector<int> SelectedServerArr;
	SelectedServerArr.push_back(ServerArr[a]);

	bool sucess = false;

	for(int h=0; h< SelectedServerArr.size();h++){
		
		bool Assigned = false;

		int next_pos = CurrentNF + 1;

		if(!(CurrentNF == (NoOfNFs-1)) ){ // not the last NF in the chain

			if(CurrentNF == 0 ){ // first NF in the chain

				short int NFSvr = SelectedServerArr[h];

				bool NoInPath = false;
				bool NoRightPath = false;

				ReturnPath RPath;
				RPath = Find_InPath(NFSvr,CurCommonList, &InitialFullSolution->CurLists, BW);
				SinglePath InPath = RPath.NewPath;
				NoInPath = RPath.NoPathFound;

				if( NoInPath == false){ // there is an InPath

					bool NoNewLinks = NewLinks_Check(InPath, &InitialFullSolution->CurLists);

					Update_LinksMatrix(InitialFullSolution, InPath, BW,"D"); // add the new in path by deleting BW from relavant links

						// update the new server
						// First delete the entry for current capacity		
					int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
					auto it5 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
					it5->second.ServerIndex.erase(NFSvr);

					if(it5->second.ServerIndex.size() <= 0){
						InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
						InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
					}
				
					// update the server with new capacity
					int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
					InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
					InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] + 1;

					std::map<short int,short int>::iterator  it3 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
					if( it3 != InitialFullSolution->CurLists.ServerCapMap.end()){
						auto it4 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
						it4->second.ServerIndex.insert(NFSvr);
					}
					else{
						InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfServer,1));
						set<int> ValsNew;
						ValsNew.insert(NFSvr);
						VecForHashTable NewVecForHashTable;
						NewVecForHashTable.ServerIndex = ValsNew;
						InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfServer,NewVecForHashTable));
					}
					
					NF.ServerID = NFSvr;
					NF.path = InPath;
					SelectedPolicyTemp->VMsForAPolicy[CurrentNF] = NF;
					
					Assigned = true;						
					sucess = InitialPerturbationWithOnePolicyMove(InitialFullSolution, CurCommonList, policyIndex, next_pos, NoOfServers, NoOfLinks, MaxLinksCap, SelectedPolicyTemp, SelectedPolicyOriginal, w1, w2, w3, w4, w5); // call to next NF
				}
			}

			if(CurrentNF != 0 ) { // not the first NF or not the last NF in the chain

				short int NFSvr;
				
				NFAllocation LeftNF = SelectedPolicyTemp->VMsForAPolicy[CurrentNF - 1];
				short int LeftNFSvr = LeftNF.ServerID;

				bool NoLeftServer = ServerCapCheck(&InitialFullSolution->CurLists,LeftNFSvr, NFCapacity); // check whether we can put in the server where left NF is
				if(NoLeftServer == true){
					NFSvr = SelectedServerArr[h];
				}
				else{
					NFSvr = LeftNFSvr;
				}

				SinglePath NFPath;
				ReturnPath RPath1;

				if(NFSvr != LeftNFSvr){

					bool NoNFPath = false;
					RPath1 = Find_Shortest_PathRandom(LeftNFSvr, NFSvr, CurCommonList, &InitialFullSolution->CurLists, BW);
					NFPath = RPath1.NewPath;
					NoNFPath = RPath1.NoPathFound;

					if(NoNFPath == false){ // there is a path so continue
						
						if(NFPath.Path.size() > 1 ){
							Update_LinksMatrix(InitialFullSolution, NFPath, BW,"D"); // add the new path by deleting BW from relavant links
						}

						// update the new server
					
						// First delete the entry for current capacity		
						int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
						auto it5 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
						it5->second.ServerIndex.erase(NFSvr);

						if(it5->second.ServerIndex.size() <= 0){
							InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
							InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
						}
				
						// update the server with new capacity
						int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
						InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
						InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] + 1;

						std::map<short int,short int>::iterator  it3 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
						if( it3 != InitialFullSolution->CurLists.ServerCapMap.end()){
							auto it4 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
							it4->second.ServerIndex.insert(NFSvr);
						}
						else{
							InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfServer,1));
							set<int> ValsNew;
							ValsNew.insert(NFSvr);
							VecForHashTable NewVecForHashTable;
							NewVecForHashTable.ServerIndex = ValsNew;
							InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfServer,NewVecForHashTable));
						}
					
						NF.ServerID = NFSvr;
						NF.path = NFPath;
						SelectedPolicyTemp->VMsForAPolicy[CurrentNF] = NF;
						if(NFSvr != LeftNFSvr){
							SelectedPolicyTemp->Splits = SelectedPolicyTemp->Splits + 1;
						}
							
						Assigned = true;
						sucess = InitialPerturbationWithOnePolicyMove(InitialFullSolution, CurCommonList, policyIndex, next_pos, NoOfServers, NoOfLinks, MaxLinksCap, SelectedPolicyTemp, SelectedPolicyOriginal, w1, w2, w3, w4, w5); // call to next NF
					}			
				}

				if(NFSvr == LeftNFSvr){ // NF and left NF in same server
					NFPath.Path.push_back(0);

					// update the new server
					
					// First delete the entry for current capacity		
					int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
					auto it5 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
					it5->second.ServerIndex.erase(NFSvr);

					if(it5->second.ServerIndex.size() <= 0){
						InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
						InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
					}
				
					// update the server with new capacity
					int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
					InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
					InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] + 1;

					std::map<short int,short int>::iterator  it3 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
					if( it3 != InitialFullSolution->CurLists.ServerCapMap.end()){
						auto it4 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
						it4->second.ServerIndex.insert(NFSvr);
					}
					else{
						InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfServer,1));
						set<int> ValsNew;
						ValsNew.insert(NFSvr);
						VecForHashTable NewVecForHashTable;
						NewVecForHashTable.ServerIndex = ValsNew;
						InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfServer,NewVecForHashTable));
					}
		
					NF.ServerID = NFSvr;
					NF.path = NFPath;
					SelectedPolicyTemp->VMsForAPolicy[CurrentNF] = NF;
				
					Assigned = true;
					sucess = InitialPerturbationWithOnePolicyMove(InitialFullSolution, CurCommonList, policyIndex, next_pos, NoOfServers, NoOfLinks, MaxLinksCap, SelectedPolicyTemp, SelectedPolicyOriginal, w1, w2, w3, w4, w5); // call to next NF		
				
				}
			}

			if(sucess == true){
				return sucess;
			}
			else{ // revert back all

				if( Assigned == true ){
					NFAllocation NFtoRevert = SelectedPolicyTemp->VMsForAPolicy[CurrentNF];
					short int NFtoRevertID = NFtoRevert.NFID;
					short int NFtoRevertCurSvr = NFtoRevert.ServerID;
					int NFtoRevertPolicyIndex = NFtoRevert.PolicyID - 1;
					SinglePath NFtoChangePath = NFtoRevert.path;

					short int BW = CurCommonList->PolicyList[NFtoRevertPolicyIndex].BW;
					short int NFtoRevertCapacity = CurCommonList->NFCap[NFtoRevertID];

					if(NFtoChangePath.Path.size() >0){
						Update_LinksMatrix(InitialFullSolution, NFtoChangePath, BW,"A"); // update the path
					}

					// update the server

					// First delete the entry for current capacity		
					int CurrentCapOfNFtoRevertServer = InitialFullSolution->CurLists.ServerCapArray[NFtoRevertCurSvr];
					auto it5 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfNFtoRevertServer);
					it5->second.ServerIndex.erase(NFtoRevertCurSvr);

					if(it5->second.ServerIndex.size() <= 0){
						InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfNFtoRevertServer);
						InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfNFtoRevertServer);
					}
				
					// update the server with new capacity
					int NewCapOfNFtoRevertServer = InitialFullSolution->CurLists.ServerCapArray[NFtoRevertCurSvr] + NFtoRevertCapacity;
					InitialFullSolution->CurLists.ServerCapArray[NFtoRevertCurSvr] = NewCapOfNFtoRevertServer;
					InitialFullSolution->CurLists.ServerUsedList[NFtoRevertCurSvr] = InitialFullSolution->CurLists.ServerUsedList[NFtoRevertCurSvr] - 1;

					std::map<short int,short int>::iterator  it3 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfNFtoRevertServer);
					if( it3 != InitialFullSolution->CurLists.ServerCapMap.end()){
						auto it4 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfNFtoRevertServer);
						it4->second.ServerIndex.insert(NFtoRevertCurSvr);
					}
					else{
						InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfNFtoRevertServer,1));
						set<int> ValsNew;
						ValsNew.insert(NFtoRevertCurSvr);
						VecForHashTable NewVecForHashTable;
						NewVecForHashTable.ServerIndex = ValsNew;
						InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfNFtoRevertServer,NewVecForHashTable));
					}

					NFtoRevert.ServerID = 0;
					NFtoRevert.path.Path.push_back(0);
					SelectedPolicyTemp->VMsForAPolicy[CurrentNF] = NFtoRevert;
				}
			}
				
		}

		if(CurrentNF == (NoOfNFs-1)) { // last NF in the chain

			// proceed with new allocation

			short int NFSvr;
				
			NFAllocation LeftNF = SelectedPolicyTemp->VMsForAPolicy[CurrentNF - 1];
			short int LeftNFSvr = LeftNF.ServerID;

			bool NoLeftServer = ServerCapCheck(&InitialFullSolution->CurLists,LeftNFSvr, NFCapacity); // check whether we can put in the server where left NF is
			if(NoLeftServer == true){
				NFSvr = SelectedServerArr[h];
			}
			else{
				NFSvr = LeftNFSvr;
			}

			bool NoOutPath = false;
			ReturnPath RPath;
			RPath = Find_OutPath(NFSvr,CurCommonList, &InitialFullSolution->CurLists, BW);
			SinglePath OutPath = RPath.NewPath;
			NoOutPath = RPath.NoPathFound;

			if( NoOutPath == false){
				Update_LinksMatrix(InitialFullSolution, OutPath, BW,"D"); // add the new out path by deleting BW from relavant links

				SinglePath NFPath;
				ReturnPath RPath2;
				bool NoInPath = false;

				if(NFSvr != LeftNFSvr){

					bool NoNFPath = false;
					RPath2 = Find_Shortest_PathRandom(LeftNFSvr, NFSvr, CurCommonList, &InitialFullSolution->CurLists, BW);
					NFPath = RPath2.NewPath;
					NoNFPath = RPath2.NoPathFound;

					if(NoNFPath == false){ // there is a path so continue
						bool NoNewLinks = NewLinks_Check(NFPath, &InitialFullSolution->CurLists);
						
						if(NFPath.Path.size() > 1 ){
							Update_LinksMatrix(InitialFullSolution, NFPath, BW,"D"); // add the new out path by deleting BW from relavant links
						}

						// update the new server
					
						// First delete the entry for current capacity		
						int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
						auto it5 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
						it5->second.ServerIndex.erase(NFSvr);

						if(it5->second.ServerIndex.size() <= 0){
							InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
							InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
						}
				
						// update the server with new capacity
						int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
						InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
						InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] + 1;

						std::map<short int,short int>::iterator  it3 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
						if( it3 != InitialFullSolution->CurLists.ServerCapMap.end()){
							auto it4 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
							it4->second.ServerIndex.insert(NFSvr);
						}
						else{
							InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfServer,1));
							set<int> ValsNew;
							ValsNew.insert(NFSvr);
							VecForHashTable NewVecForHashTable;
							NewVecForHashTable.ServerIndex = ValsNew;
							InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfServer,NewVecForHashTable));
						}

						// everything went fine, this is the last NF, so save all changes in the original policy
																	
						NF.ServerID = NFSvr;
						NF.path = NFPath;
						SelectedPolicyTemp->VMsForAPolicy[CurrentNF] = NF;
						if(NFSvr != LeftNFSvr){
							SelectedPolicyTemp->Splits = SelectedPolicyTemp->Splits + 1;
						}

						InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy = SelectedPolicyTemp->VMsForAPolicy;
						InitialFullSolution->VectorOfPartialSols[policyIndex].Splits = SelectedPolicyTemp->Splits;
						GlobalObjectValueCalculation(InitialFullSolution, NoOfServers, NoOfLinks, MaxLinksCap, w1, w2, w3, w4, w5);	
						
						PartialSol TempP;
						TempP.VMsForAPolicy = SelectedPolicyTemp->VMsForAPolicy;
						TempP.Splits = SelectedPolicyTemp->Splits;
						InitialFullSolution->VectorOfChangedSols.push_back(TempP);

						PartialSol OriginP;
						OriginP.VMsForAPolicy = SelectedPolicyOriginal->VMsForAPolicy;
						OriginP.Splits = SelectedPolicyOriginal->Splits;
						InitialFullSolution->VectorOfOriginalSols.push_back(OriginP);

						sucess = true;
						return sucess;
					}

					else{ // no inpaths, so remove out path
						Update_LinksMatrix(InitialFullSolution, OutPath, BW,"A");
					}
				}

				if(NFSvr == LeftNFSvr){  // NF and left NF in same server
					NFPath.Path.push_back(0);

					// update the new server
					
					// First delete the entry for current capacity		
					int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
					auto it5 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
					it5->second.ServerIndex.erase(NFSvr);

					if(it5->second.ServerIndex.size() <= 0){
						InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
						InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
					}
				
					// update the server with new capacity
					int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
					InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
					InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] + 1;

					std::map<short int,short int>::iterator  it3 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
					if( it3 != InitialFullSolution->CurLists.ServerCapMap.end()){
						auto it4 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
						it4->second.ServerIndex.insert(NFSvr);
					}
					else{
						InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfServer,1));
						set<int> ValsNew;
						ValsNew.insert(NFSvr);
						VecForHashTable NewVecForHashTable;
						NewVecForHashTable.ServerIndex = ValsNew;
						InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfServer,NewVecForHashTable));
					}
																	
					NF.ServerID = NFSvr;
					NF.path = NFPath;
					
					SelectedPolicyTemp->VMsForAPolicy[CurrentNF] = NF;
						
					InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy = SelectedPolicyTemp->VMsForAPolicy;
					InitialFullSolution->VectorOfPartialSols[policyIndex].Splits = SelectedPolicyTemp->Splits;
					GlobalObjectValueCalculation(InitialFullSolution, NoOfServers, NoOfLinks, MaxLinksCap, w1, w2, w3, w4, w5);	

					PartialSol TempP;
					TempP.VMsForAPolicy = SelectedPolicyTemp->VMsForAPolicy;
					TempP.Splits = SelectedPolicyTemp->Splits;
					InitialFullSolution->VectorOfChangedSols.push_back(TempP);

					PartialSol OriginP;
					OriginP.VMsForAPolicy = SelectedPolicyOriginal->VMsForAPolicy;
					OriginP.Splits = SelectedPolicyOriginal->Splits;
					InitialFullSolution->VectorOfOriginalSols.push_back(OriginP);
								
					sucess = true;
					return sucess;
				}
			}
		}
	}

	sucess = false;
	return sucess;
}
