bool InitialLocalSearchWithOnePolicyMove(FullSol* InitialFullSolution, CommonList* CurCommonList, int policyIndex, int CurrentNF, float NoOfServers, float NoOfLinks, short int MaxLinksCap, PartialSol* SelectedPolicyTemp, PartialSol* SelectedPolicyOriginal, int w1, int w2, int w3, int w4, int w5){

	float OriginalObj = InitialFullSolution->ObjVal;
	short int BW = CurCommonList->PolicyList[policyIndex].BW;
	short int NoOfNFs = CurCommonList->PolicyList[policyIndex].NFIDs.size();

	NFAllocation NF = SelectedPolicyTemp->VMsForAPolicy[CurrentNF];
	int CurSvr = NF.ServerID;

	short int NFID = CurCommonList->PolicyList[policyIndex].NFIDs[CurrentNF];
	short int NFCapacity = CurCommonList->NFCap[NFID];

	// find a new server
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

	bool sucess = false;

	for(int h=0; h< ServerArr.size();h++){
		
		bool Assigned = false;

		int next_pos = CurrentNF + 1;

		if(!(CurrentNF == (NoOfNFs-1)) ){ // not the last NF in the chain

			if(CurrentNF == 0 ){ // first NF in the chain

				short int NFSvr = ServerArr[h];

				bool NoInPath = false;
				bool NoRightPath = false;

				ReturnPath RPath;
				RPath = Find_InPath(NFSvr,CurCommonList, &InitialFullSolution->CurLists, BW);
				SinglePath InPath = RPath.NewPath;
				NoInPath = RPath.NoPathFound;

				if( NoInPath == false){ // there is an InPath

					bool NoNewLinks = NewLinks_Check(InPath, &InitialFullSolution->CurLists);

					if(NoNewLinks == false){ // there are 2 or more new links in the path
						Update_LinksMatrix(InitialFullSolution, InPath, BW,"D"); // add the new in path by deleting BW from relavant links

						// update the new server
						// First delete the entry for current capacity		
						int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
						auto it55 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
						it55->second.ServerIndex.erase(NFSvr);

						if(it55->second.ServerIndex.size() <= 0){
							InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
							InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
						}
				
						// update the server with new capacity
						int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
						InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
						InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] + 1;

						std::map<short int,short int>::iterator  it33 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
						if( it33 != InitialFullSolution->CurLists.ServerCapMap.end()){
							auto it44 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
							it44->second.ServerIndex.insert(NFSvr);
						}
						else{
							InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfServer,1));
							set<int> ValsNew1;
							ValsNew1.insert(NFSvr);
							VecForHashTable NewVecForHashTable1;
							NewVecForHashTable1.ServerIndex = ValsNew1;
							InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfServer,NewVecForHashTable1));
						}

						float NewObj = GlobalReturnObjectValue(InitialFullSolution, NoOfServers, NoOfLinks, MaxLinksCap, w1, w2, w3, w4, w5);

						if(OriginalObj <= NewObj){
							// obj is not better, so revert back the in path
							Update_LinksMatrix(InitialFullSolution, InPath, BW,"A");

							// revert back the new server

							// First delete the entry for current capacity		
							int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
							auto it51 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
							it51->second.ServerIndex.erase(NFSvr);

							if(it51->second.ServerIndex.size() <= 0){
								InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
								InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
							}
				
							// update the server with new capacity
							int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] + NFCapacity;
							InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
							InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] - 1;

							std::map<short int,short int>::iterator  it31 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
							if( it31 != InitialFullSolution->CurLists.ServerCapMap.end()){
								auto it41 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
								it41->second.ServerIndex.insert(NFSvr);
							}
							else{
								InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfServer,1));
								set<int> ValsNew11;
								ValsNew11.insert(NFSvr);
								VecForHashTable NewVecForHashTable11;
								NewVecForHashTable11.ServerIndex = ValsNew11;
								InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfServer,NewVecForHashTable11));
							}
						}

						if(OriginalObj > NewObj){ // obj is better, so proceed
							NF.ServerID = NFSvr;
							NF.path = InPath;
							SelectedPolicyTemp->VMsForAPolicy[CurrentNF] = NF;

							Assigned = true;						
							sucess = InitialLocalSearchWithOnePolicyMove(InitialFullSolution, CurCommonList, policyIndex, next_pos, NoOfServers, NoOfLinks, MaxLinksCap, SelectedPolicyTemp, SelectedPolicyOriginal, w1, w2, w3, w4, w5); // call to next NF
						}
					}
				}
			}

			if(CurrentNF != 0 ) { // not the first NF or not the last NF in the chain

				// proceed with new allocation
				
				short int NFSvr = ServerArr[h];

				NFAllocation LeftNF = SelectedPolicyTemp->VMsForAPolicy[CurrentNF - 1];
				short int LeftNFSvr = LeftNF.ServerID;

				SinglePath NFPath;
				ReturnPath RPath1;

				if(NFSvr != LeftNFSvr){
					int NoOfPaths = CurCommonList->PathsList.PathsList[LeftNFSvr][NFSvr].size();

					for (int h=0; h<NoOfPaths; h++){
						bool NoNFPath = false;
						bool AssignedInThisLoop = false;
						RPath1 = Find_Shortest_PathAll(LeftNFSvr, NFSvr, CurCommonList, &InitialFullSolution->CurLists, BW, h);
						NFPath = RPath1.NewPath;
						NoNFPath = RPath1.NoPathFound;

						if(NoNFPath == false){ // there is a path so continue
							bool NoNewLinks = NewLinks_Check(NFPath, &InitialFullSolution->CurLists);

							if(NoNewLinks == false){ // there are 2 or more new links in the path
								if(NFPath.Path.size() > 1 ){
									Update_LinksMatrix(InitialFullSolution, NFPath, BW,"D"); // add the new out path by deleting BW from relavant links
								}

								// update the new server
					
								// First delete the entry for current capacity		
								int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
								auto it52 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
								it52->second.ServerIndex.erase(NFSvr);

								if(it52->second.ServerIndex.size() <= 0){
									InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
									InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
								}
				
								// update the server with new capacity
								int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
								InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
								InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] + 1;

								std::map<short int,short int>::iterator  it32 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
								if( it32 != InitialFullSolution->CurLists.ServerCapMap.end()){
									auto it42 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
									it42->second.ServerIndex.insert(NFSvr);
								}
								else{
									InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfServer,1));
									set<int> ValsNew22;
									ValsNew22.insert(NFSvr);
									VecForHashTable NewVecForHashTable22;
									NewVecForHashTable22.ServerIndex = ValsNew22;
									InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfServer,NewVecForHashTable22));
								}

								float NewObj = GlobalReturnObjectValue(InitialFullSolution, NoOfServers, NoOfLinks, MaxLinksCap, w1, w2, w3, w4, w5);
		
								if(OriginalObj <= NewObj){
									// obj is not better, so revert back the in path
									Update_LinksMatrix(InitialFullSolution, NFPath, BW,"A");

									// revert back the new server

									// First delete the entry for current capacity		
									int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
									auto it53 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
									it53->second.ServerIndex.erase(NFSvr);

									if(it53->second.ServerIndex.size() <= 0){
										InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
										InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
									}
				
									// update the server with new capacity
									int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] + NFCapacity;
									InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
									InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] - 1;

									std::map<short int,short int>::iterator  it33 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
									if( it33 != InitialFullSolution->CurLists.ServerCapMap.end()){
										auto it43 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
										it43->second.ServerIndex.insert(NFSvr);
									}
									else{
										InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfServer,1));
										set<int> ValsNew33;
										ValsNew33.insert(NFSvr);
										VecForHashTable NewVecForHashTable33;
										NewVecForHashTable33.ServerIndex = ValsNew33;
										InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfServer,NewVecForHashTable33));
									}
								}

								if(OriginalObj > NewObj){ // obj is better, so proceed

									NF.ServerID = NFSvr;
									NF.path = NFPath;
									SelectedPolicyTemp->VMsForAPolicy[CurrentNF] = NF;
									if(NFSvr != LeftNFSvr){
										SelectedPolicyTemp->Splits = SelectedPolicyTemp->Splits + 1;
									}

									Assigned = true;
									AssignedInThisLoop = true;
									sucess = InitialLocalSearchWithOnePolicyMove(InitialFullSolution, CurCommonList, policyIndex, next_pos, NoOfServers, NoOfLinks, MaxLinksCap, SelectedPolicyTemp, SelectedPolicyOriginal, w1, w2, w3, w4, w5); // call to next NF
								}
							}
						}
						if(sucess == true){
							break;
						}
						else if ((sucess == false) && (h != (NoOfPaths-1)) && (AssignedInThisLoop == true)){ // next NF was not sucessful, and this is not last path for current NF, before going to next path, undo currect configs

							Update_LinksMatrix(InitialFullSolution, NFPath, BW,"A");

							// revert back the new server

							// First delete the entry for current capacity		
							int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
							auto it53 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
							it53->second.ServerIndex.erase(NFSvr);

							if(it53->second.ServerIndex.size() <= 0){
								InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
								InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
							}
				
							// update the server with new capacity
							int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] + NFCapacity;
							InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
							InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] - 1;

							std::map<short int,short int>::iterator  it33 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
							if( it33 != InitialFullSolution->CurLists.ServerCapMap.end()){
								auto it43 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
								it43->second.ServerIndex.insert(NFSvr);
							}
							else{
								InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfServer,1));
								set<int> ValsNew33;
								ValsNew33.insert(NFSvr);
								VecForHashTable NewVecForHashTable33;
								NewVecForHashTable33.ServerIndex = ValsNew33;
								InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfServer,NewVecForHashTable33));
							}
						}
					}				
				}

				if(NFSvr == LeftNFSvr){ // NF and left NF in same server
					NFPath.Path.push_back(0);

					// update the new server
					
					// First delete the entry for current capacity		
					int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
					auto it544 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
					it544->second.ServerIndex.erase(NFSvr);

					if(it544->second.ServerIndex.size() <= 0){
						InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
						InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
					}
				
					// update the server with new capacity
					int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] - NFCapacity;
					InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
					InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] + 1;

					std::map<short int,short int>::iterator  it344 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
					if( it344 != InitialFullSolution->CurLists.ServerCapMap.end()){
						auto it444 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
						it444->second.ServerIndex.insert(NFSvr);
					}
					else{
						InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfServer,1));
						set<int> ValsNew44;
						ValsNew44.insert(NFSvr);
						VecForHashTable NewVecForHashTable44;
						NewVecForHashTable44.ServerIndex = ValsNew44;
						InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfServer,NewVecForHashTable44));
					}

					float NewObj = GlobalReturnObjectValue(InitialFullSolution, NoOfServers, NoOfLinks, MaxLinksCap, w1, w2, w3, w4, w5);
											
					if(OriginalObj <= NewObj){
						// obj is not better, so revert back 
						// no in path to revert, because they are in same server

						// revert back the new server

						// First delete the entry for current capacity		
						int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
						auto it555 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
						it555->second.ServerIndex.erase(NFSvr);

						if(it555->second.ServerIndex.size() <= 0){
							InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
							InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
						}
				
						// update the server with new capacity
						int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] + NFCapacity;
						InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
						InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] - 1;

						std::map<short int,short int>::iterator  it355 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
						if( it355 != InitialFullSolution->CurLists.ServerCapMap.end()){
							auto it455 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
							it455->second.ServerIndex.insert(NFSvr);
						}
						else{
							InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfServer,1));
							set<int> ValsNew55;
							ValsNew55.insert(NFSvr);
							VecForHashTable NewVecForHashTable55;
							NewVecForHashTable55.ServerIndex = ValsNew55;
							InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfServer,NewVecForHashTable55));
						}
					}

					if(OriginalObj > NewObj){ // obj is better, so proceed
																	
						NF.ServerID = NFSvr;
						NF.path = NFPath;
						SelectedPolicyTemp->VMsForAPolicy[CurrentNF] = NF;

						Assigned = true;
						sucess = InitialLocalSearchWithOnePolicyMove(InitialFullSolution, CurCommonList, policyIndex, next_pos, NoOfServers, NoOfLinks, MaxLinksCap, SelectedPolicyTemp, SelectedPolicyOriginal, w1, w2, w3, w4, w5); // call to next NF
					}
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
					int NFtoRevertPolicyIndex = (NFtoRevert.PolicyID - 1);
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
					
				}
			}
				
		}

		if(CurrentNF == (NoOfNFs-1) ) { // last NF in the chain

			// proceed with new allocation

			short int NFSvr = ServerArr[h];
				
			NFAllocation LeftNF = SelectedPolicyTemp->VMsForAPolicy[CurrentNF - 1];
			short int LeftNFSvr = LeftNF.ServerID;

			bool NoOutPath = false;
			ReturnPath RPath3;
			RPath3 = Find_OutPath(NFSvr,CurCommonList, &InitialFullSolution->CurLists, BW);
			SinglePath OutPath = RPath3.NewPath;
			NoOutPath = RPath3.NoPathFound;

			if( NoOutPath == false){
				Update_LinksMatrix(InitialFullSolution, OutPath, BW,"D"); // add the new out path by deleting BW from relavant links

				SinglePath NFPath;
				ReturnPath RPath4;
				bool NoInPath = false;

				if(NFSvr != LeftNFSvr){

					int NoOfPaths = CurCommonList->PathsList.PathsList[LeftNFSvr][NFSvr].size();

					for (int h=0; h<NoOfPaths; h++){
						bool NoNFPath = false;
						bool AssignedInThisLoop = false;
						RPath4 = Find_Shortest_PathAll(LeftNFSvr, NFSvr, CurCommonList, &InitialFullSolution->CurLists, BW, h);
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

								float NewObj = GlobalReturnObjectValue(InitialFullSolution, NoOfServers, NoOfLinks, MaxLinksCap, w1, w2, w3, w4, w5);
											
								if(OriginalObj <= NewObj){

									// obj is not better, so revert back the in path
									Update_LinksMatrix(InitialFullSolution, NFPath, BW,"A");

									// revert back the new server

									// First delete the entry for current capacity		
									int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
									auto it5 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
									it5->second.ServerIndex.erase(NFSvr);

									if(it5->second.ServerIndex.size() <= 0){
										InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
										InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
									}
				
									// update the server with new capacity
									int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] + NFCapacity;
									InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
									InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] - 1;

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
								}

								if(OriginalObj > NewObj){ // obj is better, so proceed // everything went fine, this is the last NF, so save all changes in the original policy
																	
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
									
									AssignedInThisLoop = true;
									sucess = true;
									return sucess;
								}
							}
						}
						if(sucess == true){
							break;
						}
						else if ((sucess == false) && (h != (NoOfPaths-1)) && (AssignedInThisLoop == true)){ // next NF was not sucessful, and this is not last path for current NF, before going to next path, undo currect configs

							Update_LinksMatrix(InitialFullSolution, NFPath, BW,"A");

							// revert back the new server

							// First delete the entry for current capacity		
							int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
							auto it53 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
							it53->second.ServerIndex.erase(NFSvr);

							if(it53->second.ServerIndex.size() <= 0){
								InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
								InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
							}
				
							// update the server with new capacity
							int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] + NFCapacity;
							InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
							InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] - 1;

							std::map<short int,short int>::iterator  it33 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfServer);
							if( it33 != InitialFullSolution->CurLists.ServerCapMap.end()){
								auto it43 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfServer);
								it43->second.ServerIndex.insert(NFSvr);
							}
							else{
								InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfServer,1));
								set<int> ValsNew33;
								ValsNew33.insert(NFSvr);
								VecForHashTable NewVecForHashTable33;
								NewVecForHashTable33.ServerIndex = ValsNew33;
								InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfServer,NewVecForHashTable33));
							}
						}

					}	
					if(sucess == false){ // no inpaths, so remove out path
						Update_LinksMatrix(InitialFullSolution, OutPath, BW,"A");
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

					float NewObj = GlobalReturnObjectValue(InitialFullSolution, NoOfServers, NoOfLinks, MaxLinksCap, w1, w2, w3, w4, w5);
											
					if(OriginalObj <= NewObj){
						// obj is not better, so revert back
						// no inpath because they are in same server

						Update_LinksMatrix(InitialFullSolution, OutPath, BW,"A"); // remove out path

						// revert back the new server

						// First delete the entry for current capacity		
						int CurrentCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr];
						auto it5 = InitialFullSolution->CurLists.ServerCapHashTable.find(CurrentCapOfServer);
						it5->second.ServerIndex.erase(NFSvr);

						if(it5->second.ServerIndex.size() <= 0){
							InitialFullSolution->CurLists.ServerCapHashTable.erase(CurrentCapOfServer);
							InitialFullSolution->CurLists.ServerCapMap.erase(CurrentCapOfServer);
						}
				
						// update the server with new capacity
						int NewCapOfServer = InitialFullSolution->CurLists.ServerCapArray[NFSvr] + NFCapacity;
						InitialFullSolution->CurLists.ServerCapArray[NFSvr] = NewCapOfServer;
						InitialFullSolution->CurLists.ServerUsedList[NFSvr] = InitialFullSolution->CurLists.ServerUsedList[NFSvr] - 1;

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
					}

					if(OriginalObj > NewObj){ // obj is better, // everything went fine, this is the last NF, so save all changes in the original policy
																	
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
	}

	sucess = false;
	return sucess;
}
