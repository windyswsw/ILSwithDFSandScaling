bool ScalePerturbationWithOneNFMove(FullSol* InitialFullSolution, CommonList* CurCommonListNew, int policyIndex, int PosOfChangingNF, float NoOfServers, float NoOfLinks, short int MaxLinksCap, PartialSol* SelectedPolicyOriginal, int w1, int w2, int w3, int w4, int w5, ChangedPolicies* TrackChangedPolicies){

	short int BW = CurCommonListNew->PolicyList[policyIndex].BW;
	short int TotNFs = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy.size();

	float OldObjVal = InitialFullSolution->ObjVal;

	bool SvrSucess = false;
	bool NoPathForNF = false;
	bool NoPathForRightNF = false;
	bool NoOutPath = false;
	bool NoInPath = false;

	NFAllocation NF = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF];
	short int NFID = NF.NFID;
	short int NFCurSvr = NF.ServerID;
	short int NFPolicyID = NF.PolicyID;

	int NFCapacity = CurCommonListNew->NFCap[NFID];

	short int NFSvr = 1000; // find a new server for NF

	vector<int> ServerArr;

	for (std::map<short int,short int>::iterator itlow=  InitialFullSolution->CurLists.ServerCapMap.lower_bound(NFCapacity); itlow!=InitialFullSolution->CurLists.ServerCapMap.end(); ++itlow){
		int FoundCap = itlow->first;
		auto it2 = InitialFullSolution->CurLists.ServerCapHashTable.find(FoundCap);

		for (std::set<int>::iterator itv = it2->second.ServerIndex.begin(); itv != it2->second.ServerIndex.end(); ++itv){
			ServerArr.push_back(*itv);
		}
	}

	int loopID = 0;
	if( ServerArr.size() > 0){

		while( SvrSucess == false  && loopID < ServerArr.size()){

			NFSvr = ServerArr[loopID];

			if ( NFCurSvr != NFSvr){
		
				if(PosOfChangingNF == (TotNFs-1) ){ // last NF in the chain
			
					NFAllocation LeftNF = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF-1];
					short int LeftNFSvr = LeftNF.ServerID;

					bool NoOutPath = false;
					ReturnPath RPath3;
					RPath3 = Find_OutPath(NFSvr,CurCommonListNew, &InitialFullSolution->CurLists, BW);
					SinglePath OutPath = RPath3.NewPath;
					NoOutPath = RPath3.NoPathFound;

					if( NoOutPath == false){
						Update_LinksMatrix(InitialFullSolution, OutPath, BW,"D"); // add the new out path by deleting BW from relavant links

						SinglePath NFPath;
						ReturnPath RPath4;
						bool NoInPath = false;

						if(NFSvr != LeftNFSvr){

							int NoOfPaths = CurCommonListNew->PathsList.PathsList[LeftNFSvr][NFSvr].size();

							for (int h=0; h<NoOfPaths; h++){
								bool NoNFPath = false;
								RPath4 = Find_Shortest_PathAll(LeftNFSvr, NFSvr, CurCommonListNew, &InitialFullSolution->CurLists, BW, h);
								NFPath = RPath4.NewPath;
								NoNFPath = RPath4.NoPathFound;

								if(NoNFPath == false){ // there is a path so continue
									bool NoNewLinks = NewLinks_Check(NFPath, &InitialFullSolution->CurLists);

									if(NoNewLinks == false){ // there are 2 or more new links in the path

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
											
										// NF to change
										NFAllocation NewNF;
										NewNF.ServerID = NFSvr;
										NewNF.NFID = NFID;
										NewNF.PolicyID = NFPolicyID;
										NewNF.path = NFPath;
										InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF] = NewNF;

										PartialSol TempP;
										TempP.VMsForAPolicy = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy;
										TempP.Splits = InitialFullSolution->VectorOfPartialSols[policyIndex].Splits;
										InitialFullSolution->VectorOfChangedSols.push_back(TempP);

										PartialSol OriginP;
										OriginP.VMsForAPolicy = SelectedPolicyOriginal->VMsForAPolicy;
										OriginP.Splits = SelectedPolicyOriginal->Splits;
										InitialFullSolution->VectorOfOriginalSols.push_back(OriginP);
										
										SvrSucess = true;
									}
								}
								if(SvrSucess == true){
									break;
								}
							}	
							if(SvrSucess == false){ // no inpaths, so remove out path
								Update_LinksMatrix(InitialFullSolution, OutPath, BW,"A");
							}
						}

						else{ // NF and left NF in same server
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
																	
							// NF to change
							NFAllocation NewNF;
							NewNF.ServerID = NFSvr;
							NewNF.NFID = NFID;
							NewNF.PolicyID = NFPolicyID;
							NewNF.path = NFPath;
							InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF] = NewNF;
	
							PartialSol TempP;
							TempP.VMsForAPolicy = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy;
							TempP.Splits = InitialFullSolution->VectorOfPartialSols[policyIndex].Splits;
							InitialFullSolution->VectorOfChangedSols.push_back(TempP);

							PartialSol OriginP;
							OriginP.VMsForAPolicy = SelectedPolicyOriginal->VMsForAPolicy;
							OriginP.Splits = SelectedPolicyOriginal->Splits;
							InitialFullSolution->VectorOfOriginalSols.push_back(OriginP);
							
							SvrSucess = true;

						}
					}
				}

				else if(PosOfChangingNF == 0 ){ // 1st NF of the chain
			
					NFAllocation RightOfNFtoChange = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF+1];
					short int RightOfNFtoChangeSvr = RightOfNFtoChange.ServerID;
					short int RightOfNFtoChangePolicyID = RightOfNFtoChange.PolicyID;
					short int RightOfNFtoChangeNFID = RightOfNFtoChange.NFID;

					SinglePath RightOfNFPath;
					SinglePath InPath;
			
					ReturnPath RPath;
					RPath = Find_InPath(NFSvr,CurCommonListNew, &InitialFullSolution->CurLists, BW);
					InPath = RPath.NewPath;
					NoInPath = RPath.NoPathFound;

					if( NoInPath == false){ // there is an InPath

						bool NoNewLinks = NewLinks_Check(InPath, &InitialFullSolution->CurLists);

						if(NoNewLinks == false){
				
							Update_LinksMatrix(InitialFullSolution,  InPath, BW,"D");

							if(NFSvr != RightOfNFtoChangeSvr){
								ReturnPath RPathNew;
								RPathNew = Find_Shortest_Path(NFSvr,RightOfNFtoChangeSvr, CurCommonListNew, &InitialFullSolution->CurLists, BW);
								RightOfNFPath = RPathNew.NewPath;
								NoPathForRightNF = RPathNew.NoPathFound;
							}
							else{
								RightOfNFPath.Path.push_back(0);
							}
				
							if( NoPathForRightNF == false){ // there is a path to right NF or they are in same server
						
								if(RightOfNFPath.Path.size() > 1 ){
									Update_LinksMatrix(InitialFullSolution,  RightOfNFPath, BW,"D");
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

								NFAllocation NewNF;
								NFAllocation NewNFforRightNF;

								// NF to change
								NewNF.ServerID = NFSvr;
								NewNF.NFID = NFID;
								NewNF.PolicyID = NFPolicyID;
								NewNF.path = InPath;
								InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF] = NewNF;

								// Right of the NF to change
								NewNFforRightNF.ServerID = RightOfNFtoChangeSvr;
								NewNFforRightNF.PolicyID = RightOfNFtoChangePolicyID;
								NewNFforRightNF.NFID = RightOfNFtoChangeNFID;
								NewNFforRightNF.path = RightOfNFPath;
								InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF+1] = NewNFforRightNF;

								PartialSol TempP;
								TempP.VMsForAPolicy = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy;
								TempP.Splits = InitialFullSolution->VectorOfPartialSols[policyIndex].Splits;
								InitialFullSolution->VectorOfChangedSols.push_back(TempP);

								PartialSol OriginP;
								OriginP.VMsForAPolicy = SelectedPolicyOriginal->VMsForAPolicy;
								OriginP.Splits = SelectedPolicyOriginal->Splits;
								InitialFullSolution->VectorOfOriginalSols.push_back(OriginP);

								SvrSucess = true;

							}
							else{ // there is no path to right NF, so revert back
								Update_LinksMatrix(InitialFullSolution,  InPath, BW,"A"); // remove the newly assigned In path
							}
						}
					}
				}

				else if ((PosOfChangingNF != 0) && (PosOfChangingNF != (TotNFs-1))) {

					NFAllocation LeftNF = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF-1];
					short int LeftNFSvr = LeftNF.ServerID;

					NFAllocation RightOfNFtoChange = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF+1];
					short int RightOfNFtoChangeSvr = RightOfNFtoChange.ServerID;
					short int RightOfNFtoChangePolicyID = RightOfNFtoChange.PolicyID;
					short int RightOfNFtoChangeNFID = RightOfNFtoChange.NFID;

					SinglePath NFPath1;
					ReturnPath RPath1;
					SinglePath RightOfNFPath;
					ReturnPath RPathNew;

					if(NFSvr != LeftNFSvr){
						int NoOfPaths = CurCommonListNew->PathsList.PathsList[LeftNFSvr][NFSvr].size();

						for (int h=0; h<NoOfPaths; h++){
							bool NoNFPath = false;
							RPath1 = Find_Shortest_PathAll(LeftNFSvr, NFSvr, CurCommonListNew, &InitialFullSolution->CurLists, BW, h);
							NFPath1 = RPath1.NewPath;
							NoNFPath = RPath1.NoPathFound;

							if(NoNFPath == false){ // there is a path so continue
								bool NoNewLinks = NewLinks_Check(NFPath1, &InitialFullSolution->CurLists);

								if(NoNewLinks == false){ // there are 2 or more new links in the path

									if(NFPath1.Path.size() > 1 ){
											Update_LinksMatrix(InitialFullSolution, NFPath1, BW,"D"); // add the new path by deleting BW from relavant links
									}

									if(NFSvr != RightOfNFtoChangeSvr){
										RPathNew = Find_Shortest_Path(NFSvr,RightOfNFtoChangeSvr, CurCommonListNew, &InitialFullSolution->CurLists, BW);
										RightOfNFPath = RPathNew.NewPath;
										NoPathForRightNF = RPathNew.NoPathFound;
									}
									else{
										RightOfNFPath.Path.push_back(0);
									}
				
									if( NoPathForRightNF == false){ // there is a path to right NF or they are in same server
						
										if(RightOfNFPath.Path.size() > 1 ){
											Update_LinksMatrix(InitialFullSolution,  RightOfNFPath, BW,"D"); // add the new right path by deleting BW from relavant links
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

										NFAllocation NewNF;
										NFAllocation NewNFforRightNF;

										// NF to change
										NewNF.ServerID = NFSvr;
										NewNF.NFID = NFID;
										NewNF.PolicyID = NFPolicyID;
										NewNF.path = NFPath1;
										InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF] = NewNF;

										// Right of the NF to change
										NewNFforRightNF.ServerID = RightOfNFtoChangeSvr;
										NewNFforRightNF.PolicyID = RightOfNFtoChangePolicyID;
										NewNFforRightNF.NFID = RightOfNFtoChangeNFID;
										NewNFforRightNF.path = RightOfNFPath;
										InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF+1] = NewNFforRightNF;

										PartialSol TempP;
										TempP.VMsForAPolicy = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy;
										TempP.Splits = InitialFullSolution->VectorOfPartialSols[policyIndex].Splits;
										InitialFullSolution->VectorOfChangedSols.push_back(TempP);

										PartialSol OriginP;
										OriginP.VMsForAPolicy = SelectedPolicyOriginal->VMsForAPolicy;
										OriginP.Splits = SelectedPolicyOriginal->Splits;
										InitialFullSolution->VectorOfOriginalSols.push_back(OriginP);

										SvrSucess = true;
									}
									else{ // no right path, so revert back the new NF path
										if(NFPath1.Path.size() > 1 ){
											Update_LinksMatrix(InitialFullSolution, NFPath1, BW,"A"); // delete the new path by adding BW from relavant links
										}
									}
								}
							}
							if(SvrSucess == true){
								break;
							}
						}				
					}

					else{ // NF and left NF in same server
						NFPath1.Path.push_back(0);

						if(NFSvr != RightOfNFtoChangeSvr){
							RPathNew = Find_Shortest_Path(NFSvr,RightOfNFtoChangeSvr, CurCommonListNew, &InitialFullSolution->CurLists, BW);
							RightOfNFPath = RPathNew.NewPath;
							NoPathForRightNF = RPathNew.NoPathFound;
						}
						else{
							RightOfNFPath.Path.push_back(0);
						}
				
						if( NoPathForRightNF == false){ // there is a path to right NF or they are in same server
						
							if(RightOfNFPath.Path.size() > 1 ){
								Update_LinksMatrix(InitialFullSolution,  RightOfNFPath, BW,"D"); // add the new right path by deleting BW from relavant links
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
																	
							NFAllocation NewNF;
							NFAllocation NewNFforRightNF;

							// NF to change
							NewNF.ServerID = NFSvr;
							NewNF.NFID = NFID;
							NewNF.PolicyID = NFPolicyID;
							NewNF.path = NFPath1;
							InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF] = NewNF;

								// Right of the NF to change
							NewNFforRightNF.ServerID = RightOfNFtoChangeSvr;
							NewNFforRightNF.PolicyID = RightOfNFtoChangePolicyID;
							NewNFforRightNF.NFID = RightOfNFtoChangeNFID;
							NewNFforRightNF.path = RightOfNFPath;
							InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF+1] = NewNFforRightNF;

							PartialSol TempP;
							TempP.VMsForAPolicy = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy;
							TempP.Splits = InitialFullSolution->VectorOfPartialSols[policyIndex].Splits;
							InitialFullSolution->VectorOfChangedSols.push_back(TempP);

							PartialSol OriginP;
							OriginP.VMsForAPolicy = SelectedPolicyOriginal->VMsForAPolicy;
							OriginP.Splits = SelectedPolicyOriginal->Splits;
							InitialFullSolution->VectorOfOriginalSols.push_back(OriginP);

							SvrSucess = true;
						}
					}
			
				}
			}
			loopID ++;
		}
	}

	if ( SvrSucess == true){

		// check whether it has been already added to original policies
		PartialSol OriginP;
		OriginP.VMsForAPolicy = SelectedPolicyOriginal->VMsForAPolicy;
		OriginP.Splits = SelectedPolicyOriginal->Splits;
		
		std::map<short int,PartialSol>::iterator  it3 = TrackChangedPolicies->OriginalPolicies.find(policyIndex);
		if( it3 == TrackChangedPolicies->OriginalPolicies.end()){
			TrackChangedPolicies->OriginalPolicies.insert(std::pair<short int,PartialSol>(policyIndex,OriginP));
		}

		// check whether it has been already added to currently changed policies
		std::map<short int,PartialSol>::iterator  it4 = TrackChangedPolicies->CurrentlyChangedPolicies.find(policyIndex);
		if( it4 != TrackChangedPolicies->CurrentlyChangedPolicies.end()){
			it4->second = InitialFullSolution->VectorOfPartialSols[policyIndex];
		}
		else{
			TrackChangedPolicies->CurrentlyChangedPolicies.insert(std::pair<short int,PartialSol>(policyIndex,InitialFullSolution->VectorOfPartialSols[policyIndex]));
		}

		LocalObjectValueCalculation(InitialFullSolution, CurCommonListNew, NoOfServers, NoOfLinks, MaxLinksCap, w1, w2, w3, w4, w5, TrackChangedPolicies);
	}

	return SvrSucess;
}
