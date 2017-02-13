void ScaleLocal(FullSol* InitialFullSolution, CommonList* CurCommonLists, float NoOfServers, short int ServerCap, float NoOfLinks, float MaxLinksCap, int w1, int w2, int w3, int w4, int w5, int PercentageOfPoliciesToChangeInPerturbation, int TerminationConditionRounds, ofstream &myfilenew){

	FullSol basesol;
	basesol.CurLists = InitialFullSolution->CurLists;
	basesol.ObjVal = InitialFullSolution->ObjVal;
	basesol.VectorOfPartialSols = InitialFullSolution->VectorOfPartialSols;

	vector<vector <string> > vec;
	for(size_t i = 0; i < 5; i++){			
		vector<string> row;
		vec.push_back(row);
	}

	bool NoEnoughBW = false;
	bool NoSucess = false;

	int f=0;

	while(CurCommonLists->FullChangeListCur.size()>0){

		ChangedPolicies TrackChangedPolicies;

		if( NoEnoughBW == true || NoSucess == true){
			break;
		}

		vector<Policy> OriginalPolicyList = CurCommonLists->PolicyList;

		basesol.CurLists = InitialFullSolution->CurLists;
		basesol.ObjVal = InitialFullSolution->ObjVal;
		basesol.VectorOfPartialSols = InitialFullSolution->VectorOfPartialSols;

		RoundChangeInfo RoundChangeNow;
		RoundChange(CurCommonLists, &RoundChangeNow);

		for(size_t d=0; d<RoundChangeNow.RoundChangeList.size(); d++){
			if( NoEnoughBW == true){
				break;
			}

			int pol = RoundChangeNow.RoundChangeList[d].polToChange;
			int cus = RoundChangeNow.RoundChangeList[d].cus;
			int BWChange = RoundChangeNow.RoundChangeList[d].BWToChange;
			string op = RoundChangeNow.RoundChangeList[d].AR;
                        int changeNF = RoundChangeNow.RoundChangeList[d].NFToChange;

			if( op == "A" ){
				for(int k=0; k<InitialFullSolution->VectorOfPartialSols[pol].VMsForAPolicy.size(); k++){
					if ((k != changeNF) && (k != (changeNF+1))){
						SinglePath P = InitialFullSolution->VectorOfPartialSols[pol].VMsForAPolicy[k].path;
						if( P.Path.size() > 1){
							NoEnoughBW = Link_Check(P, &InitialFullSolution->CurLists, BWChange);
						}

						if( NoEnoughBW == true){
							break;
						}
					}
				}
			}

			if( op == "Q" ){
				for(int k=0; k<InitialFullSolution->VectorOfPartialSols[pol].VMsForAPolicy.size(); k++){
					SinglePath P = InitialFullSolution->VectorOfPartialSols[pol].VMsForAPolicy[k].path;
					if( P.Path.size() > 1){
						NoEnoughBW = Link_Check(P, &InitialFullSolution->CurLists, BWChange);
					}
					
					if( NoEnoughBW == true){
						break;
					}
				}
			}
		}

		if(NoEnoughBW == false && NoSucess == false){

			for(size_t d=0; d<RoundChangeNow.RoundChangeList.size(); d++){
				int pol = RoundChangeNow.RoundChangeList[d].polToChange;
				int cus = RoundChangeNow.RoundChangeList[d].cus;
				int BWChange = RoundChangeNow.RoundChangeList[d].BWToChange;

				string op = RoundChangeNow.RoundChangeList[d].AR;
				int ChangeSize = CurCommonLists->PolicyList[pol].NFIDs.size() - 1;
				int changeAdd = 0;

				if( op == "A"){ 

					changeAdd =  RoundChangeNow.RoundChangeList[d].NFToChange;
					NoSucess = AddInstance(InitialFullSolution, CurCommonLists, pol, changeAdd, BWChange, NoOfServers, NoOfLinks, MaxLinksCap, w1, w2, w3, w4, w5, &TrackChangedPolicies);

					if(NoSucess == true){
						break;
					}
				}

				else if( op == "D"){
					
					changeAdd =  RoundChangeNow.RoundChangeList[d].NFToChange;
					DeleteInstance(InitialFullSolution, CurCommonLists, pol, changeAdd , BWChange, NoOfServers, NoOfLinks, MaxLinksCap,w1, w2, w3, w4, w5, &TrackChangedPolicies);
				}

				// after adding or deleting NF, add/delete BW to the paths used by other NFs of the policy
				// incase of only changing traffic events "P", add/delete BW to the paths used by all NFs of the policy
				if( op == "A" ){
					if(NoSucess == false){ // adding was successful
						for(int k=0; k<InitialFullSolution->VectorOfPartialSols[pol].VMsForAPolicy.size(); k++){ // loop for all NFs in the policy
							// we are not going to add bw for the paths that is used by NF that was changed, and the Right NF of the NF that was changed, because we already did that in AddInstance function
							if (k != changeAdd){ // NF of the loop is not the NF that was changed
								if( k != (changeAdd+1)){ // NF of the loop is not the Right NF of the NF that was changed
									SinglePath P = InitialFullSolution->VectorOfPartialSols[pol].VMsForAPolicy[k].path;
									if( P.Path.size() > 1){
										Update_LinksMatrixForTraffic(InitialFullSolution, P, BWChange,"D"); // add the new BW by deleting BW from relavant links
									}
								}
								if ( k == ChangeSize){ // this is the last NF of the chain, so update the outpath
									int lastNFsvr = InitialFullSolution->VectorOfPartialSols[pol].VMsForAPolicy[k].ServerID;
									SinglePath OutPathLastNF = Check_OutPath(lastNFsvr,CurCommonLists);
									Update_LinksMatrixForTraffic(InitialFullSolution, OutPathLastNF, BWChange,"D"); // add the new BW by deleting BW from relavant links
								}
							}
						}
					}
				}

				if( op == "D" ){
					if(NoSucess == false){ // deleting was successful
						for(int k=0; k<InitialFullSolution->VectorOfPartialSols[pol].VMsForAPolicy.size(); k++){ // loop for all NFs in the policy
							// we are not going to add bw for the paths that is used by NF that was changed, and the Right NF of the NF that was changed, because we already did that in AddInstance function
							if (k != changeAdd){ // NF of the loop is not the NF that was changed
								if( k != (changeAdd+1)){ // NF of the loop is not the Right NF of the NF that was changed
									SinglePath P = InitialFullSolution->VectorOfPartialSols[pol].VMsForAPolicy[k].path;
									if( P.Path.size() > 1){
										Update_LinksMatrixForTraffic(InitialFullSolution, P, BWChange,"A"); // remove the new BW by adding BW to relavant links
									}
								}
								if ( k == ChangeSize){ // this is the last NF of the chain, so update the outpath
									int lastNFsvr = InitialFullSolution->VectorOfPartialSols[pol].VMsForAPolicy[k].ServerID;
									SinglePath OutPathLastNF = Check_OutPath(lastNFsvr,CurCommonLists);
									Update_LinksMatrixForTraffic(InitialFullSolution, OutPathLastNF, BWChange,"A"); // // remove the new BW by adding BW to relavant links
								}
							}
						}
					}
				}

				if( op == "P" ){
					for(int k=0; k<InitialFullSolution->VectorOfPartialSols[pol].VMsForAPolicy.size(); k++){ // loop for all NFs in the policy
						SinglePath P = InitialFullSolution->VectorOfPartialSols[pol].VMsForAPolicy[k].path;
						if( P.Path.size() > 1){
							Update_LinksMatrixForTraffic(InitialFullSolution, P, BWChange,"A"); // // remove the new BW by adding BW to relavant links
						}
						if ( k == ChangeSize){ // this is the last NF of the chain, so update the outpath
							int lastNFsvr = InitialFullSolution->VectorOfPartialSols[pol].VMsForAPolicy[k].ServerID;
							SinglePath OutPathLastNF = Check_OutPath(lastNFsvr,CurCommonLists);
							Update_LinksMatrixForTraffic(InitialFullSolution, OutPathLastNF, BWChange,"A"); // // remove the new BW by adding BW to relavant links
						}
					}
				}

				if( op == "Q" ){
					for(int k=0; k<InitialFullSolution->VectorOfPartialSols[pol].VMsForAPolicy.size(); k++){ // loop for all NFs in the policy
						SinglePath P = InitialFullSolution->VectorOfPartialSols[pol].VMsForAPolicy[k].path;
						if( P.Path.size() > 1){
							Update_LinksMatrixForTraffic(InitialFullSolution, P, BWChange,"D"); // add the new BW by deleting BW from relavant links
						}
						if ( k == ChangeSize){ // this is the last NF of the chain, so update the outpath
							int lastNFsvr = InitialFullSolution->VectorOfPartialSols[pol].VMsForAPolicy[k].ServerID;
							SinglePath OutPathLastNF = Check_OutPath(lastNFsvr,CurCommonLists);
							Update_LinksMatrixForTraffic(InitialFullSolution, OutPathLastNF, BWChange,"D"); // add the new BW by deleting BW from relavant links
						}
					}
				}

				// add / del BW change permenantly to the policy list

				if(op == "A" || op == "Q"){
					CurCommonLists->PolicyList[pol].BW = CurCommonLists->PolicyList[pol].BW + BWChange;
				}
				if(op == "D" || op == "P"){
					CurCommonLists->PolicyList[pol].BW = CurCommonLists->PolicyList[pol].BW - BWChange;
				}

				LocalObjectValueCalculation(InitialFullSolution, CurCommonLists, NoOfServers, NoOfLinks, MaxLinksCap, w1, w2, w3, w4, w5, &TrackChangedPolicies);

			}

			LocalObjectValueCalculation(InitialFullSolution, CurCommonLists, NoOfServers, NoOfLinks, MaxLinksCap, w1, w2, w3, w4, w5, &TrackChangedPolicies);
			InitialFullSolution->InitialObjVal = InitialFullSolution->ObjVal;	

			PrintSummaryLocal(f, InitialFullSolution, myfilenew, NoOfServers, NoOfLinks, MaxLinksCap,'D', w1, w2, w3, w4, w5, &TrackChangedPolicies );

			// very first local search
			ScaleLocalSearch(InitialFullSolution, CurCommonLists, NoOfServers, NoOfLinks, MaxLinksCap, myfilenew, myfilenew, w1, w2, w3, w4, w5, &RoundChangeNow, &TrackChangedPolicies);
			InitialFullSolution->InitialObjVal = InitialFullSolution->ObjVal;	
			//PrintSummaryLocal(f, InitialFullSolution, myfilenew, NoOfServers, NoOfLinks, MaxLinksCap,'I', w1, w2, w3, w4, w5, &TrackChangedPolicies );

			// after the very first local search, move all CurrentlyChangedPolicies to FirstLocalSearchPolicies

			for (std::map<short int, PartialSol>::iterator it6=TrackChangedPolicies.CurrentlyChangedPolicies.begin(); it6!=TrackChangedPolicies.CurrentlyChangedPolicies.end(); ++it6){
				short int Pol= it6->first;

				// check whether it has been already added to FirstLocalSearchPolicies
				std::map<short int,PartialSol>::iterator  it4 = TrackChangedPolicies.FirstLocalSearchPolicies.find(Pol);
				if( it4 != TrackChangedPolicies.FirstLocalSearchPolicies.end()){ // if yes, then update FirstLocalSearchPolicies with the newest current change
					it4->second = it6->second;
				}
				else{ // else add it newly
					TrackChangedPolicies.FirstLocalSearchPolicies.insert(std::pair<short int,PartialSol>(Pol,it6->second));
				}
			}

			// CurrentlyChangedPolicies
			TrackChangedPolicies.CurrentlyChangedPolicies.clear();

			ScaleRepeatProcedure(InitialFullSolution, CurCommonLists, PercentageOfPoliciesToChangeInPerturbation, NoOfLinks, NoOfServers, MaxLinksCap, myfilenew, myfilenew,myfilenew, TerminationConditionRounds, w1, w2, w3, w4, w5, &RoundChangeNow, &TrackChangedPolicies);
			PrintSummaryLocal(f, InitialFullSolution, myfilenew, NoOfServers, NoOfLinks, MaxLinksCap,'A', w1, w2, w3, w4, w5, &TrackChangedPolicies );
			PrintNewLine(myfilenew);
		}
		f++;
	}
}
