void ScaleLocalSearch(FullSol* FullSolution, CommonList* CurCommonLists, float NoOfServers, float NoOfLinks, short int MaxLinksCap, ofstream &myfile1, ofstream &myfile3, int w1, int w2, int w3, int w4, int w5, RoundChangeInfo* RoundChangeNow, ChangedPolicies* TrackChangedPolicies){
	
	for(short int LoopIndex = 0; LoopIndex < RoundChangeNow->RoundChangeList.size(); LoopIndex ++){// for each policy, full space search

		int PolicySelected = RoundChangeNow->RoundChangeList[LoopIndex].polToChange;
		int NFSelected = RoundChangeNow->RoundChangeList[LoopIndex].NFToChange;
		string op = RoundChangeNow->RoundChangeList[LoopIndex].AR;

		bool sucess = false;

		if (op == "A" || op == "D"){

			PartialSol SelectedPolicyOriginal = FullSolution->VectorOfPartialSols[PolicySelected];
			PartialSol SelectedPolicyTemp = FullSolution->VectorOfPartialSols[PolicySelected];

			RemoveNF(FullSolution, CurCommonLists, PolicySelected, NFSelected, NoOfServers, NoOfLinks, MaxLinksCap);
			sucess = ScaleLocalSearchWithOneNFMove(FullSolution, CurCommonLists, PolicySelected, NFSelected, NoOfServers, NoOfLinks, MaxLinksCap, &SelectedPolicyOriginal, w1, w2, w3, w4, w5, RoundChangeNow, TrackChangedPolicies);

			//if (sucess == true){
			//	break;
			//}
			if (sucess == false){
				FullSolution->VectorOfPartialSols[PolicySelected] = SelectedPolicyOriginal;
				AddNF(FullSolution, CurCommonLists, PolicySelected, NFSelected, NoOfServers, NoOfLinks, MaxLinksCap,  w1, w2, w3, w4, w5, TrackChangedPolicies);
			}
		}

	}
}
