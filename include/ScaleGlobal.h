void ScaleGlobal(FullSol* InitialFullSolution, CommonList* CurCommonLists, float NoOfServers, short int ServerCap, float NoOfLinks, float MaxLinksCap, int w1, int w2, int w3, int w4, int w5, int NoOfPoliciesToChangeInPerturbation, int TerminationConditionRounds, ofstream &myfilenew){

	FullSol basesol;
	basesol.CurLists = InitialFullSolution->CurLists;
	basesol.ObjVal = InitialFullSolution->ObjVal;
	basesol.VectorOfPartialSols = InitialFullSolution->VectorOfPartialSols;

	int popsize = 1;

	vector<vector <string> > vec;
	for(size_t i = 0; i < 5; i++){			
		vector<string> row;
		vec.push_back(row);
	}

	int f=0;

	while(CurCommonLists->FullChangeListCur.size()>0){

		RoundChangeInfo RoundChangeNow;
		RoundChange(CurCommonLists, &RoundChangeNow);

		int newpopsize = 1;

		for(size_t d=0; d<RoundChangeNow.RoundChangeList.size(); d++){
			int pol = RoundChangeNow.RoundChangeList[d].polToChange;
			int BWChange = RoundChangeNow.RoundChangeList[d].BWToChange;
			string op = RoundChangeNow.RoundChangeList[d].AR;

			if(op == "A" || op == "Q"){
				CurCommonLists->PolicyList[pol].BW = CurCommonLists->PolicyList[pol].BW + BWChange;
			}
			if(op == "D" || op == "P"){
				CurCommonLists->PolicyList[pol].BW = CurCommonLists->PolicyList[pol].BW - BWChange;
			}
		}

		for(size_t d=0; d<RoundChangeNow.RoundChangeList.size(); d++){
			int pol = RoundChangeNow.RoundChangeList[d].polToChange;
			int cus = RoundChangeNow.RoundChangeList[d].cus;

			string op = RoundChangeNow.RoundChangeList[d].AR;

			int changeIns = 0;
			string changePolIns;
			string delimiter;
			string token;
			string n1, n2;
			int ipol, iinst; 
			size_t posn = 0;
			vector<string> wordArr;

			if( op == "A"){ 
				int changeAdd = RoundChangeNow.RoundChangeList[d].NFToChange;
				CurCommonLists->PolicyList[pol].NFIDs[changeAdd] = 8;  // instance is doubled now

				stringstream ss;
				ss << pol << '-' << changeAdd;
				changePolIns = ss.str();
				vec[cus].push_back(changePolIns);
			}

			else if( op == "D"){
				changePolIns = vec[cus].back();
				vec[cus].pop_back();
				delimiter = "-";

				while ((posn = changePolIns.find(delimiter)) != std::string::npos) {
					 token = changePolIns.substr(0, posn);
					 changePolIns.erase(0, posn + delimiter.length());
				}

				n1 = token;
				n2 = changePolIns;

				istringstream ss1(n1) ;
				ss1 >> ipol;
				istringstream ss2(n2) ;
				ss2 >> iinst;

				CurCommonLists->PolicyList[pol].NFIDs[iinst] = 1; // original instance is assigned
			}
		}

		short int NoOfPolicies = CurCommonLists->PolicyList.size();

		UniqueList NewUniqueList;
		GenUniqueList(&NewUniqueList, NoOfServers, ServerCap, NoOfLinks, MaxLinksCap);

		FullSol NewInitialFullSolution;
		NewInitialFullSolution.CurLists = NewUniqueList;

		NewInitialFullSolution.ObjVal = 0;
		NewInitialFullSolution.InitialObjVal = 0;

		bool sucess = false;

		sucess = InitialPolicyImplementation(&NewInitialFullSolution, CurCommonLists, NoOfLinks, NoOfServers, MaxLinksCap);

	 	if(sucess == true){
			
			GlobalObjectValueCalculation(&NewInitialFullSolution, NoOfServers, NoOfLinks, MaxLinksCap, w1, w2, w3, w4, w5);
			NewInitialFullSolution.InitialObjVal = NewInitialFullSolution.ObjVal;

			PrintSummaryGlobal(f, InitialFullSolution, myfilenew, NoOfServers, NoOfLinks, MaxLinksCap,'D', w1, w2, w3, w4, w5);
			
			InitialLocalSearch(&NewInitialFullSolution, CurCommonLists, NoOfServers, NoOfLinks, MaxLinksCap, myfilenew, myfilenew, w1, w2, w3, w4, w5);
			NewInitialFullSolution.InitialObjVal = NewInitialFullSolution.ObjVal;	
			PrintSummaryGlobal(f, InitialFullSolution, myfilenew, NoOfServers, NoOfLinks, MaxLinksCap,'I', w1, w2, w3, w4, w5);
			InitialRepeatProcedure(&NewInitialFullSolution, CurCommonLists, NoOfPoliciesToChangeInPerturbation, NoOfLinks, NoOfServers, MaxLinksCap, myfilenew, myfilenew,myfilenew, TerminationConditionRounds, w1, w2, w3, w4, w5);
			PrintSummaryGlobal(f, InitialFullSolution, myfilenew, NoOfServers, NoOfLinks, MaxLinksCap,'R', w1, w2, w3, w4, w5);

			int newsvrChanges = 0;
			int newlinkChanges = 0;

			for(int i=0; i<basesol.VectorOfPartialSols.size();i++){
				for(int j=0; j<basesol.VectorOfPartialSols[i].VMsForAPolicy.size();j++){

					NFAllocation NF = basesol.VectorOfPartialSols[i].VMsForAPolicy[j];
					short int NFSvr = NF.ServerID;
					SinglePath NFPath = NF.path;

					NFAllocation NFNew = NewInitialFullSolution.VectorOfPartialSols[i].VMsForAPolicy[j];
					short int NFNewSvr = NFNew.ServerID;
					SinglePath NFNewPath = NFNew.path;

					if(NFSvr != NFNewSvr){
						newsvrChanges = newsvrChanges + 1;		
					}
				
					if(NFPath.Path != NFNewPath.Path){
						if(NFNewPath.Path.size() >1){
							newlinkChanges =  newlinkChanges + (NFNewPath.Path.size()-1);
						}
					}
				}
			}
			//PrintSummaryGlobal(f, CurrentPop.CurPopulaton[0], myfilenew, NoOfServers, NoOfLinks, MaxLinksCap, 0,0,'G',  w1, w2, w3, w4, w5,newsvrChanges,newlinkChanges );
	 		//PrintNewLine(myfilenew);

			basesol = NewInitialFullSolution;
		}
		else{
			break;
		}

		f++;
	}

	myfilenew.close();
}
