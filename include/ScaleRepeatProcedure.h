void ScaleRepeatProcedure(FullSol* FullSolution, CommonList* CurCommonLists, int PercentageOfNFsToChangeInPerturbation, float NoOfLinks, float NoOfServers, short int MaxLinksCap, ofstream &myfile1, ofstream &myfile2, ofstream &myfile3, int TerminationConditionRounds, int w1, int w2, int w3, int w4, int w5, RoundChangeInfo* RoundChangeNow, ChangedPolicies* TrackChangedPolicies){

	for(int a=0; a<TerminationConditionRounds; a++){// termination condition

	//while((FullSolution->ObjVal) > 0.933333){

		FullSolution->InitialObjVal = FullSolution->ObjVal;

		int loopc = FullSolution->VectorOfChangedSols.size();
		for(int c=0; c < loopc; c++){
			FullSolution->VectorOfChangedSols.pop_back();
		}

		int loopd = FullSolution->VectorOfOriginalSols.size();
		for(int c=0; c < loopd; c++){
			FullSolution->VectorOfOriginalSols.pop_back();
		}

		// select n policies randomly and then do Pertubation for each policy, regardless improvement or not == Perturbation
		ScalePerturbation(FullSolution, CurCommonLists,PercentageOfNFsToChangeInPerturbation, NoOfServers, NoOfLinks, MaxLinksCap, myfile1, myfile2, w1, w2, w3, w4, w5, RoundChangeNow, TrackChangedPolicies);

		//PrintSummaryLocal(2000, FullSolution, myfile3, NoOfServers, NoOfLinks, MaxLinksCap,'P', w1, w2, w3, w4, w5, TrackChangedPolicies );

		ScaleLocalSearch(FullSolution, CurCommonLists, NoOfServers, NoOfLinks, MaxLinksCap, myfile1, myfile3, w1, w2, w3, w4, w5, RoundChangeNow, TrackChangedPolicies);

		//PrintSummaryLocal(3000, FullSolution, myfile3, NoOfServers, NoOfLinks, MaxLinksCap,'L', w1, w2, w3, w4, w5, TrackChangedPolicies );

		ScaleAcceptanceCriteria(FullSolution, CurCommonLists, NoOfServers, NoOfLinks, MaxLinksCap, TrackChangedPolicies);

		//PrintSummaryLocal(4000, FullSolution, myfile3, NoOfServers, NoOfLinks, MaxLinksCap,'A', w1, w2, w3, w4, w5, TrackChangedPolicies );

	}

	int loopc = FullSolution->VectorOfChangedSols.size();
	for(int c=0; c < loopc; c++){
		FullSolution->VectorOfChangedSols.pop_back();
	}

	int loopd = FullSolution->VectorOfOriginalSols.size();
	for(int c=0; c < loopd; c++){
		FullSolution->VectorOfOriginalSols.pop_back();
	}

	//PrintSummaryLocal(4000, FullSolution, myfile3, NoOfServers, NoOfLinks, MaxLinksCap,'A', w1, w2, w3, w4, w5, TrackChangedPolicies );
}
