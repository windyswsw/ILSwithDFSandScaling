void ScalePerturbation(FullSol* FullSolution, CommonList* CurCommonLists, float PercentageOfPoliciesToChangeInPerturbation, float NoOfServers, float NoOfLinks, short int MaxLinksCap, ofstream &myfile1, ofstream &myfile2, int w1, int w2, int w3, int w4, int w5, RoundChangeInfo* RoundChangeNow, ChangedPolicies* TrackChangedPolicies){

	float TotPolicies = RoundChangeNow->RoundChangeList.size();
	float NoOfPoliciesToChangeInPerturbation = (TotPolicies/100) * PercentageOfPoliciesToChangeInPerturbation;

	if(NoOfPoliciesToChangeInPerturbation < 1){
		NoOfPoliciesToChangeInPerturbation = 1;
	}
	
	for (int k = 0; k< NoOfPoliciesToChangeInPerturbation; k++){
		int PolicySelectedForLoop = 0;

		if(TotPolicies > 1){
			int TotPoliciesToSelect = TotPolicies - 1;
			PolicySelectedForLoop = rand() % TotPoliciesToSelect + 1;
		}

		int PolicySelected = RoundChangeNow->RoundChangeList[PolicySelectedForLoop].polToChange;
		int NFSelected = RoundChangeNow->RoundChangeList[PolicySelectedForLoop].NFToChange;

		string op = RoundChangeNow->RoundChangeList[PolicySelectedForLoop].AR;

		bool sucess = false;

		if (op == "A" || op == "D"){

			PartialSol SelectedPolicyOriginal = FullSolution->VectorOfPartialSols[PolicySelected];
			PartialSol SelectedPolicyTemp = FullSolution->VectorOfPartialSols[PolicySelected];

			RemoveNF(FullSolution, CurCommonLists, PolicySelected, NFSelected, NoOfServers, NoOfLinks, MaxLinksCap);
			sucess = ScalePerturbationWithOneNFMove(FullSolution, CurCommonLists, PolicySelected, NFSelected, NoOfServers, NoOfLinks, MaxLinksCap, &SelectedPolicyOriginal, w1, w2, w3, w4, w5, TrackChangedPolicies);

			if (sucess == false){
				FullSolution->VectorOfPartialSols[PolicySelected] = SelectedPolicyOriginal;
				AddNF(FullSolution, CurCommonLists, PolicySelected, NFSelected, NoOfServers, NoOfLinks, MaxLinksCap,  w1, w2, w3, w4, w5, TrackChangedPolicies);
			}
		}
	}
}
