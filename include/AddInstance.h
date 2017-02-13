bool AddInstance(FullSol* InitialFullSolution, CommonList* CurCommonListNew, int Pol, int PosOfChangingNF, int BWChange, float NoOfServers, float NoOfLinks, short int MaxLinksCap, int w1, int w2, int w3, int w4, int w5, ChangedPolicies* TrackChangedPolicies){

	bool NoSucess = false;

	FullSol basesol;
	basesol.CurLists = InitialFullSolution->CurLists;
	basesol.ObjVal = InitialFullSolution->ObjVal;
	basesol.VectorOfPartialSols = InitialFullSolution->VectorOfPartialSols;

	short int policyIndex = Pol;
	int NoOfMutations = CurCommonListNew->PolicyList[policyIndex].NFIDs.size();

	short int OldBW = CurCommonListNew->PolicyList[policyIndex].BW;
	short int NewBW = OldBW + BWChange;
	short int TotNFs = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy.size();

	float OldObjVal = InitialFullSolution->ObjVal;

	bool SvrSucess = false;
	bool NoPathForNF = false;
	bool NoPathForRightNF = false;
	bool NoOutPath = false;
	bool NoInPath = false;

	NFAllocation NFtoChange = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF];
	short int NFtoChangeID = NFtoChange.NFID;
	short int NFtoChangeCurSvr = NFtoChange.ServerID;
	short int NFtoChangePolicyID = NFtoChange.PolicyID;
	SinglePath NFtoChangePath = NFtoChange.path;

	int NFtoChangeCapacity = CurCommonListNew->NFCap[NFtoChangeID];
	int NFtoChangeNewCapacity = NFtoChangeCapacity * 2; // double the capacity

	int NID = 0;

	for(int i=0; i<CurCommonListNew->NFCap.size(); i++){
		if(CurCommonListNew->NFCap[i] == NFtoChangeNewCapacity){
			NID = i;
			break;
		}
	}

	short int NFtoChangeNewSvr = 1000; // find a new server for NF

	vector<int> ServerArr;

	for (std::map<short int,short int>::iterator itlow=  InitialFullSolution->CurLists.ServerCapMap.lower_bound(NFtoChangeNewCapacity); itlow!=InitialFullSolution->CurLists.ServerCapMap.end(); ++itlow){
		int FoundCap = itlow->first;
		auto it2 = InitialFullSolution->CurLists.ServerCapHashTable.find(FoundCap);

		for (std::set<int>::iterator itv = it2->second.ServerIndex.begin(); itv != it2->second.ServerIndex.end(); ++itv){
			ServerArr.push_back(*itv);
		}
	}

	if( ServerArr.size() < 0){
		NoSucess = true;
	}
	else{

		int cl = 0;

		while( SvrSucess == false){

			NFtoChangeNewSvr = ServerArr[cl];

			NoPathForNF = false;
			NoPathForRightNF = false;
			NoOutPath = false;
			NoInPath = false;

			if ( NFtoChangeNewSvr != NFtoChangeCurSvr){
		
				if(PosOfChangingNF == (TotNFs-1) ){ // last NF in the chain
			
					NFAllocation LeftOfNFtoChange = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF-1];
					short int LeftOfNFtoChangeSvr = LeftOfNFtoChange.ServerID;

					if(NFtoChangePath.Path.size() > 1){ // there was a previously assigned path, so remove that path by adding the BW to relavant links, if no path then, NFtoChangePath.size() = 1
						Update_LinksMatrix(InitialFullSolution, NFtoChangePath, OldBW,"A");
					}

					SinglePath OutPathOld = Check_OutPath(NFtoChangeCurSvr,CurCommonListNew); // remove the previous out path by adding BW to relavant links
					Update_LinksMatrix(InitialFullSolution,OutPathOld, OldBW,"A");

					ReturnPath RPathNew;
					RPathNew = Find_OutPath(NFtoChangeNewSvr,CurCommonListNew, &InitialFullSolution->CurLists, NewBW);
					SinglePath OutPathNew = RPathNew.NewPath;
					NoOutPath = RPathNew.NoPathFound; 

					if( NoOutPath == false){ // there is an out path, so continue
				
						Update_LinksMatrix(InitialFullSolution, OutPathNew, NewBW,"D"); // add the new out path by deleting BW from relavant links
					
						NFAllocation NewNF;
						SinglePath NFtoChangeNewPath;

						if(NFtoChangeNewSvr != LeftOfNFtoChangeSvr){
							ReturnPath RPath;
							RPath = Find_Shortest_Path(LeftOfNFtoChangeSvr, NFtoChangeNewSvr, CurCommonListNew, &InitialFullSolution->CurLists, NewBW);
							NFtoChangeNewPath = RPath.NewPath;
							NoPathForNF = RPath.NoPathFound;
						}
						else{
							NFtoChangeNewPath.Path.push_back(0);
						}

						if(NoPathForNF == false){ // there is a path, or NF and left NF are in same server, so continue

							if(NFtoChangeNewPath.Path.size() > 1 ){
								Update_LinksMatrix(InitialFullSolution, NFtoChangeNewPath, NewBW,"D");
							}

							// update new server

							int NewServerCurCap = InitialFullSolution->CurLists.ServerCapArray[NFtoChangeNewSvr];
							auto  it51 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewServerCurCap);
							it51->second.ServerIndex.erase(NFtoChangeNewSvr);

							if(it51->second.ServerIndex.size() <= 0){
								InitialFullSolution->CurLists.ServerCapHashTable.erase(NewServerCurCap);
								InitialFullSolution->CurLists.ServerCapMap.erase(NewServerCurCap);
							}

							int NewCapOfNewServer = InitialFullSolution->CurLists.ServerCapArray[NFtoChangeNewSvr] - NFtoChangeNewCapacity;
							InitialFullSolution->CurLists.ServerCapArray[NFtoChangeNewSvr] = NewCapOfNewServer;
							InitialFullSolution->CurLists.ServerUsedList[NFtoChangeNewSvr] = InitialFullSolution->CurLists.ServerUsedList[NFtoChangeNewSvr] + 1;

							std::map<short int,short int>::iterator  it3 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfNewServer);
							if( it3 != InitialFullSolution->CurLists.ServerCapMap.end()){
								auto it4 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfNewServer);
								it4->second.ServerIndex.insert(NFtoChangeNewSvr);
							}
							else{
								InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfNewServer,1));
								set<int> ValsNew;
								ValsNew.insert(NFtoChangeNewSvr);
								VecForHashTable NewVecForHashTable;
								NewVecForHashTable.ServerIndex = ValsNew;
								InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfNewServer,NewVecForHashTable));
							}

							// update old server
							int OldServerCap = InitialFullSolution->CurLists.ServerCapArray[NFtoChangeCurSvr];
							auto  it5 = InitialFullSolution->CurLists.ServerCapHashTable.find(OldServerCap);
							it5->second.ServerIndex.erase(NFtoChangeCurSvr);

							if(it5->second.ServerIndex.size() <= 0){
								InitialFullSolution->CurLists.ServerCapHashTable.erase(OldServerCap);
								InitialFullSolution->CurLists.ServerCapMap.erase(OldServerCap);
							}

							int OldServerNewCap = InitialFullSolution->CurLists.ServerCapArray[NFtoChangeCurSvr] + NFtoChangeCapacity;
							InitialFullSolution->CurLists.ServerCapArray[NFtoChangeCurSvr] = OldServerNewCap;
							InitialFullSolution->CurLists.ServerUsedList[NFtoChangeCurSvr] = InitialFullSolution->CurLists.ServerUsedList[NFtoChangeCurSvr] - 1;
											
							std::map<short int,short int>::iterator  it6 = InitialFullSolution->CurLists.ServerCapMap.find(OldServerNewCap);
							if( it6 != InitialFullSolution->CurLists.ServerCapMap.end()){
								auto it7 = InitialFullSolution->CurLists.ServerCapHashTable.find(OldServerNewCap);
								it7->second.ServerIndex.insert(NFtoChangeCurSvr);
							}
							else{
								InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(OldServerNewCap,1));
								set<int> ValsNew1;
								ValsNew1.insert(NFtoChangeCurSvr);
								VecForHashTable NewVecForHashTable1;
								NewVecForHashTable1.ServerIndex = ValsNew1;
								InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(OldServerNewCap,NewVecForHashTable1));
							}

							NewNF.ServerID = NFtoChangeNewSvr;
							NewNF.NFID = NID;
							NewNF.PolicyID = NFtoChangePolicyID;
							NewNF.path = NFtoChangeNewPath;
							InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF] = NewNF;

							SvrSucess = true;
						}

						else{ // there is no new path, so revert back
							if(NFtoChangePath.Path.size() > 1){ // add the old path back
								Update_LinksMatrix( InitialFullSolution, NFtoChangePath, OldBW,"D");
							}
							Update_LinksMatrix(InitialFullSolution, OutPathOld, OldBW,"D");
							Update_LinksMatrix(InitialFullSolution, OutPathNew, NewBW,"A");
							InitialFullSolution->ObjVal = OldObjVal;

							NoSucess = true;
						}
					}
					else{ // there is no out path, so revert back
						if(NFtoChangePath.Path.size() > 1){ // add the old path back
							Update_LinksMatrix(InitialFullSolution,  NFtoChangePath, OldBW,"D");
						}
						Update_LinksMatrix(InitialFullSolution,  OutPathOld, OldBW,"D");
						InitialFullSolution->ObjVal = OldObjVal;
						NoSucess = true;
					}
				}

				if(PosOfChangingNF == 0 ){
			
					NFAllocation RightOfNFtoChange = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF+1];
					short int RightOfNFtoChangeSvr = RightOfNFtoChange.ServerID;
					short int RightOfNFtoChangePolicyID = RightOfNFtoChange.PolicyID;
					short int RightOfNFtoChangeNFID = RightOfNFtoChange.NFID;
					SinglePath RightOfNFtoChangePath = RightOfNFtoChange.path;

					NFAllocation NewNF;
					NFAllocation NewNFforRightNF;
					SinglePath RightOfNFtoChangeNewPath;
					SinglePath NFtoChangeNewPath;

					Update_LinksMatrix(InitialFullSolution,  NFtoChangePath, OldBW,"A"); // remove previous In path by adding the BW to relavant links
			
					ReturnPath RPath;
					RPath = Find_InPath(NFtoChangeNewSvr,CurCommonListNew, &InitialFullSolution->CurLists, NewBW);
					NFtoChangeNewPath = RPath.NewPath;
					NoInPath = RPath.NoPathFound;

					if( NoInPath == false){ // there is an InPath
				
						Update_LinksMatrix(InitialFullSolution,  NFtoChangeNewPath, NewBW,"D");
				
						if(RightOfNFtoChangePath.Path.size() > 1){ // there was a previously assigned path for right side NF, so remove that path by adding the BW to relavant links
							Update_LinksMatrix(InitialFullSolution, RightOfNFtoChangePath, OldBW,"A");
						}
						if(NFtoChangeNewSvr != RightOfNFtoChangeSvr){
							ReturnPath RPathNew;
							RPathNew = Find_Shortest_Path(NFtoChangeNewSvr,RightOfNFtoChangeSvr, CurCommonListNew, &InitialFullSolution->CurLists, NewBW);
							RightOfNFtoChangeNewPath = RPathNew.NewPath;
							NoPathForRightNF = RPathNew.NoPathFound;
						}
						else{
							RightOfNFtoChangeNewPath.Path.push_back(0);
						}
				
						if( NoPathForRightNF == false){ // there is a path to right NF or they are in same server
						
							if(RightOfNFtoChangeNewPath.Path.size() > 1 ){
								Update_LinksMatrix(InitialFullSolution,  RightOfNFtoChangeNewPath, NewBW,"D");
							}

							// update new server
							int NewServerCurCap = InitialFullSolution->CurLists.ServerCapArray[NFtoChangeNewSvr];
							auto  it51 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewServerCurCap);
							it51->second.ServerIndex.erase(NFtoChangeNewSvr);

							if(it51->second.ServerIndex.size() <= 0){
								InitialFullSolution->CurLists.ServerCapHashTable.erase(NewServerCurCap);
								InitialFullSolution->CurLists.ServerCapMap.erase(NewServerCurCap);
							}

							int NewCapOfNewServer = InitialFullSolution->CurLists.ServerCapArray[NFtoChangeNewSvr] - NFtoChangeNewCapacity;
							InitialFullSolution->CurLists.ServerCapArray[NFtoChangeNewSvr] = NewCapOfNewServer;
							InitialFullSolution->CurLists.ServerUsedList[NFtoChangeNewSvr] = InitialFullSolution->CurLists.ServerUsedList[NFtoChangeNewSvr] + 1;

							std::map<short int,short int>::iterator  it3 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfNewServer);
							if( it3 != InitialFullSolution->CurLists.ServerCapMap.end()){
								auto it4 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfNewServer);
								it4->second.ServerIndex.insert(NFtoChangeNewSvr);
							}
								else{
								InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfNewServer,1));
								set<int> ValsNew;
								ValsNew.insert(NFtoChangeNewSvr);
								VecForHashTable NewVecForHashTable;
								NewVecForHashTable.ServerIndex = ValsNew;
								InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfNewServer,NewVecForHashTable));
							}

							// update old server
							int OldServerCap = InitialFullSolution->CurLists.ServerCapArray[NFtoChangeCurSvr];
							auto  it5 = InitialFullSolution->CurLists.ServerCapHashTable.find(OldServerCap);
							it5->second.ServerIndex.erase(NFtoChangeCurSvr);
						
							if(it5->second.ServerIndex.size() <= 0){
								InitialFullSolution->CurLists.ServerCapHashTable.erase(OldServerCap);
								InitialFullSolution->CurLists.ServerCapMap.erase(OldServerCap);
							}

							int OldServerNewCap = InitialFullSolution->CurLists.ServerCapArray[NFtoChangeCurSvr] + NFtoChangeCapacity;
							InitialFullSolution->CurLists.ServerCapArray[NFtoChangeCurSvr] = OldServerNewCap;
							InitialFullSolution->CurLists.ServerUsedList[NFtoChangeCurSvr] = InitialFullSolution->CurLists.ServerUsedList[NFtoChangeCurSvr] - 1;
						
							std::map<short int,short int>::iterator  it6 = InitialFullSolution->CurLists.ServerCapMap.find(OldServerNewCap);
							if( it6 != InitialFullSolution->CurLists.ServerCapMap.end()){
								auto it7 = InitialFullSolution->CurLists.ServerCapHashTable.find(OldServerNewCap);
								it7->second.ServerIndex.insert(NFtoChangeCurSvr);
							}
							else{
								InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(OldServerNewCap,1));
								set<int> ValsNew1;
								ValsNew1.insert(NFtoChangeCurSvr);
								VecForHashTable NewVecForHashTable1;
								NewVecForHashTable1.ServerIndex = ValsNew1;
								InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(OldServerNewCap,NewVecForHashTable1));
							}

							// NF to change
							NewNF.ServerID = NFtoChangeNewSvr;
							NewNF.NFID = NID;
							NewNF.PolicyID = NFtoChangePolicyID;
							NewNF.path = NFtoChangeNewPath;
							InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF] = NewNF;

							// Right of the NF to change
							NewNFforRightNF.ServerID = RightOfNFtoChangeSvr;
							NewNFforRightNF.PolicyID = RightOfNFtoChangePolicyID;
							NewNFforRightNF.NFID = RightOfNFtoChangeNFID;
							NewNFforRightNF.path = RightOfNFtoChangeNewPath;
							InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF+1] = NewNFforRightNF;

							SvrSucess = true;

						}
						else{ // there is no path to right NF, so revert back
					
							if(NFtoChangePath.Path.size() > 1){ //revert back to old InPath
								Update_LinksMatrix(InitialFullSolution,  NFtoChangePath, OldBW,"D");
							}
							if(RightOfNFtoChangePath.Path.size() > 1){  // revert back to the old path of right NF
									Update_LinksMatrix(InitialFullSolution,  RightOfNFtoChangePath, OldBW,"D");
							}

							Update_LinksMatrix(InitialFullSolution,  NFtoChangeNewPath, NewBW,"A"); // remove the newly assigned In path
							InitialFullSolution->ObjVal = OldObjVal;
							NoSucess = true;
						}
					}
					else{ //there is no InPath, so revert back
						if(NFtoChangePath.Path.size() > 1){ //revert back to old InPath
							Update_LinksMatrix(InitialFullSolution,  NFtoChangePath, OldBW,"D");
						}
						NoSucess = true;
					}
				}

				if ((PosOfChangingNF != 0) && (PosOfChangingNF != (TotNFs-1))) {
			
					NFAllocation RightOfNFtoChange = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF+1];
					short int RightOfNFtoChangeSvr = RightOfNFtoChange.ServerID;
					short int RightOfNFtoChangePolicyID = RightOfNFtoChange.PolicyID;
					short int RightOfNFtoChangeNFID = RightOfNFtoChange.NFID;
					SinglePath RightOfNFtoChangePath = RightOfNFtoChange.path;

					NFAllocation LeftOfNFtoChange = InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF-1];
					short int LeftOfNFtoChangeSvr = LeftOfNFtoChange.ServerID;

					NFAllocation NewNF;
					NFAllocation NewNFforRightNF;

					if(NFtoChangePath.Path.size() > 1){ // there was a previously assigned path, so remove that path by adding the BW to relavant links, if no path then, NFtoChangePath.size() = 1
						Update_LinksMatrix(InitialFullSolution, NFtoChangePath, OldBW,"A");
					}

					SinglePath NFtoChangeNewPath;
					SinglePath RightOfNFtoChangeNewPath;

					if(NFtoChangeNewSvr != LeftOfNFtoChangeSvr){
						ReturnPath RPath;
						RPath = Find_Shortest_Path(LeftOfNFtoChangeSvr, NFtoChangeNewSvr, CurCommonListNew, &InitialFullSolution->CurLists, NewBW);
						NFtoChangeNewPath = RPath.NewPath;
						NoPathForNF = RPath.NoPathFound;
					}
					else{
						NFtoChangeNewPath.Path.push_back(0);
					}

					if( NoPathForNF == false){ // there is a path from left NF, or they are in same server

						if(NFtoChangeNewPath.Path.size() > 1 ){
							Update_LinksMatrix(InitialFullSolution, NFtoChangeNewPath, NewBW,"D");
						}
					
						if(RightOfNFtoChangePath.Path.size() > 1){ // there was a previously assigned path for right side NF, so remove that path by adding the BW to relavant links, if no path then, NFtoChangePath.size() = 1
							Update_LinksMatrix(InitialFullSolution, RightOfNFtoChangePath, OldBW,"A");
						}
				
						if(NFtoChangeNewSvr != RightOfNFtoChangeSvr){
							ReturnPath RPathNew;
							RPathNew = Find_Shortest_Path(NFtoChangeNewSvr,RightOfNFtoChangeSvr, CurCommonListNew, &InitialFullSolution->CurLists, NewBW);
							RightOfNFtoChangeNewPath = RPathNew.NewPath;
							NoPathForRightNF = RPathNew.NoPathFound;
						}
						else{
							RightOfNFtoChangeNewPath.Path.push_back(0);
						}

						if(NoPathForRightNF == false){ // there is a path to right NF
					
							if(RightOfNFtoChangeNewPath.Path.size() > 1 ){
								Update_LinksMatrix(InitialFullSolution, RightOfNFtoChangeNewPath, NewBW,"D");
							}

							// update new server
					
							int NewServerCurCap = InitialFullSolution->CurLists.ServerCapArray[NFtoChangeNewSvr];
							auto  it51 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewServerCurCap);
							it51->second.ServerIndex.erase(NFtoChangeNewSvr);

							if(it51->second.ServerIndex.size() <= 0){
								InitialFullSolution->CurLists.ServerCapHashTable.erase(NewServerCurCap);
								InitialFullSolution->CurLists.ServerCapMap.erase(NewServerCurCap);
							}

							int NewCapOfNewServer = InitialFullSolution->CurLists.ServerCapArray[NFtoChangeNewSvr] - NFtoChangeNewCapacity;
							InitialFullSolution->CurLists.ServerCapArray[NFtoChangeNewSvr] = NewCapOfNewServer;
							InitialFullSolution->CurLists.ServerUsedList[NFtoChangeNewSvr] = InitialFullSolution->CurLists.ServerUsedList[NFtoChangeNewSvr] + 1;
						
							std::map<short int,short int>::iterator  it3 = InitialFullSolution->CurLists.ServerCapMap.find(NewCapOfNewServer);
							if( it3 != InitialFullSolution->CurLists.ServerCapMap.end()){
								auto it4 = InitialFullSolution->CurLists.ServerCapHashTable.find(NewCapOfNewServer);
								it4->second.ServerIndex.insert(NFtoChangeNewSvr);
							}
							else{
								InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(NewCapOfNewServer,1));
								set<int> ValsNew;
								ValsNew.insert(NFtoChangeNewSvr);
								VecForHashTable NewVecForHashTable;
								NewVecForHashTable.ServerIndex = ValsNew;
								InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(NewCapOfNewServer,NewVecForHashTable));
							}

							// update old server
							int OldServerCap = InitialFullSolution->CurLists.ServerCapArray[NFtoChangeCurSvr];
							auto  it5 = InitialFullSolution->CurLists.ServerCapHashTable.find(OldServerCap);
							it5->second.ServerIndex.erase(NFtoChangeCurSvr);
					
							if(it5->second.ServerIndex.size() <= 0){
								InitialFullSolution->CurLists.ServerCapHashTable.erase(OldServerCap);
								InitialFullSolution->CurLists.ServerCapMap.erase(OldServerCap);
							}
					
							int OldServerNewCap = InitialFullSolution->CurLists.ServerCapArray[NFtoChangeCurSvr] + NFtoChangeCapacity;
							InitialFullSolution->CurLists.ServerCapArray[NFtoChangeCurSvr] = OldServerNewCap;
							InitialFullSolution->CurLists.ServerUsedList[NFtoChangeCurSvr] = InitialFullSolution->CurLists.ServerUsedList[NFtoChangeCurSvr] - 1;
					
							std::map<short int,short int>::iterator  it6 = InitialFullSolution->CurLists.ServerCapMap.find(OldServerNewCap);
							if( it6 != InitialFullSolution->CurLists.ServerCapMap.end()){
								auto it7 = InitialFullSolution->CurLists.ServerCapHashTable.find(OldServerNewCap);
								it7->second.ServerIndex.insert(NFtoChangeCurSvr);
							}
							else{
								InitialFullSolution->CurLists.ServerCapMap.insert(std::pair<short int,short int>(OldServerNewCap,1));
								set<int> ValsNew1;
								ValsNew1.insert(NFtoChangeCurSvr);
								VecForHashTable NewVecForHashTable1;
								NewVecForHashTable1.ServerIndex = ValsNew1;
								InitialFullSolution->CurLists.ServerCapHashTable.insert(std::pair<short int,VecForHashTable>(OldServerNewCap,NewVecForHashTable1));
							}

							// NF to change
							NewNF.ServerID = NFtoChangeNewSvr;
							NewNF.NFID = NID;
							NewNF.PolicyID = NFtoChangePolicyID;
							NewNF.path = NFtoChangeNewPath;
							InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF] = NewNF;
						
							// Right of the NF to change
							NewNFforRightNF.ServerID = RightOfNFtoChangeSvr;
							NewNFforRightNF.PolicyID = RightOfNFtoChangePolicyID;
							NewNFforRightNF.NFID = RightOfNFtoChangeNFID;
							NewNFforRightNF.path = RightOfNFtoChangeNewPath;
							InitialFullSolution->VectorOfPartialSols[policyIndex].VMsForAPolicy[PosOfChangingNF+1] = NewNFforRightNF;

							SvrSucess = true;
						}
						else { // no path to right NF, revert back all
							if(RightOfNFtoChangePath.Path.size() > 1){
								Update_LinksMatrix(InitialFullSolution, RightOfNFtoChangePath, OldBW,"D");
							}
							if(NFtoChangePath.Path.size() > 1){
								Update_LinksMatrix(InitialFullSolution, NFtoChangePath, OldBW,"D");
							}
							if(NFtoChangeNewPath.Path.size() > 1){
								Update_LinksMatrix(InitialFullSolution, NFtoChangeNewPath, NewBW,"A");
							}
							InitialFullSolution->ObjVal = OldObjVal;
							NoSucess = true;
						}
					}
					else { // there is no path to left NF, so revert back
						if(NFtoChangePath.Path.size() > 1){
							Update_LinksMatrix(InitialFullSolution, NFtoChangePath, OldBW,"D");
						}
						InitialFullSolution->ObjVal = OldObjVal;
						NoSucess = true;
					}
				}
			}
			cl ++;
		}
	}

	if ( NoSucess == false){

		// check whether it has been already added to original policies
		std::map<short int,PartialSol>::iterator  it3 = TrackChangedPolicies->OriginalPolicies.find(Pol);
		if( it3 == TrackChangedPolicies->OriginalPolicies.end()){
			TrackChangedPolicies->OriginalPolicies.insert(std::pair<short int,PartialSol>(Pol,basesol.VectorOfPartialSols[Pol]));
		}

		// check whether it has been already added to currently changed policies
		std::map<short int,PartialSol>::iterator  it4 = TrackChangedPolicies->CurrentlyChangedPolicies.find(Pol);
		if( it4 != TrackChangedPolicies->CurrentlyChangedPolicies.end()){
			it4->second = InitialFullSolution->VectorOfPartialSols[Pol];
		}
		else{
			TrackChangedPolicies->CurrentlyChangedPolicies.insert(std::pair<short int,PartialSol>(Pol,InitialFullSolution->VectorOfPartialSols[Pol]));
		}

		LocalObjectValueCalculation(InitialFullSolution, CurCommonListNew, NoOfServers, NoOfLinks, MaxLinksCap, w1, w2, w3, w4, w5, TrackChangedPolicies);
	}

	return NoSucess;
}
