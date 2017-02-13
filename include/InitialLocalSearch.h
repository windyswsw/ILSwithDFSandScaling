void InitialLocalSearch(FullSol* FullSolution, CommonList* CurCommonLists, float NoOfServers, float NoOfLinks, short int MaxLinksCap, ofstream &myfile1, ofstream &myfile3, int w1, int w2, int w3, int w4, int w5){
	
	for(short int PolicySelected = 0; PolicySelected < FullSolution->VectorOfPartialSols.size(); PolicySelected ++){// for each policy, full space search
		bool sucess = false;

		int startNF = 0; // OnePlicyMove start from 1st NF of policy

		PartialSol SelectedPolicyOriginal = FullSolution->VectorOfPartialSols[PolicySelected];
		PartialSol SelectedPolicyTemp = FullSolution->VectorOfPartialSols[PolicySelected];

		RemovePolicy(FullSolution, CurCommonLists, PolicySelected, NoOfServers, NoOfLinks, MaxLinksCap);
		sucess = InitialLocalSearchWithOnePolicyMove(FullSolution, CurCommonLists, PolicySelected, startNF, NoOfServers, NoOfLinks, MaxLinksCap, &SelectedPolicyTemp, &SelectedPolicyOriginal, w1, w2, w3, w4, w5);

		//if (sucess == true){
		//	break;
		//}
		if (sucess == false){
			FullSolution->VectorOfPartialSols[PolicySelected] = SelectedPolicyOriginal;
			AddPolicy(FullSolution, CurCommonLists, PolicySelected, NoOfServers, NoOfLinks, MaxLinksCap,  w1, w2, w3, w4, w5);
		}
		
	}
}
