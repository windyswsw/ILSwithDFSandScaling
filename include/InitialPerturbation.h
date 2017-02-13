void InitialPerturbation(FullSol* FullSolution, CommonList* CurCommonLists, short int NoOfPoliciesToChangeInPerturbation, float NoOfServers, float NoOfLinks, short int MaxLinksCap, ofstream &myfile1, ofstream &myfile2, int w1, int w2, int w3, int w4, int w5){
	
	for (int k = 0; k< NoOfPoliciesToChangeInPerturbation; k++){
		int TotPolicy = CurCommonLists->PolicyList.size() - 1;
		int PolicySelected = rand() % TotPolicy + 1;

		PartialSol SelectedPolicyOriginal = FullSolution->VectorOfPartialSols[PolicySelected];
		PartialSol SelectedPolicyTemp = FullSolution->VectorOfPartialSols[PolicySelected];

		int CurrentNF = 0;

		RemovePolicy(FullSolution, CurCommonLists, PolicySelected, NoOfServers, NoOfLinks, MaxLinksCap);

		bool success = InitialPerturbationWithOnePolicyMove(FullSolution, CurCommonLists, PolicySelected, CurrentNF, NoOfServers, NoOfLinks, MaxLinksCap, &SelectedPolicyTemp, &SelectedPolicyOriginal, w1, w2, w3, w4, w5);

		if (success == false){
			FullSolution->VectorOfPartialSols[PolicySelected] = SelectedPolicyOriginal;
			AddPolicy(FullSolution, CurCommonLists, PolicySelected, NoOfServers, NoOfLinks, MaxLinksCap,  w1, w2, w3, w4, w5);
		}
	}
}
