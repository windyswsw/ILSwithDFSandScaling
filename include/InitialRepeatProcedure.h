void InitialRepeatProcedure(FullSol* FullSolution, CommonList* CurCommonLists, int NoOfNFsToChangeInPerturbation, float NoOfLinks, float NoOfServers, short int MaxLinksCap, ofstream &myfile1, ofstream &myfile2, ofstream &myfile3, int TerminationConditionRounds, int w1, int w2, int w3, int w4, int w5){

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
		InitialPerturbation(FullSolution, CurCommonLists,NoOfNFsToChangeInPerturbation, NoOfServers, NoOfLinks, MaxLinksCap, myfile1, myfile2, w1, w2, w3, w4, w5);

		InitialLocalSearch(FullSolution, CurCommonLists, NoOfServers, NoOfLinks, MaxLinksCap, myfile1, myfile2, w1, w2, w3, w4, w5);
               // cout << a << "," << FullSolution->InitialObjVal << "," << FullSolution->ObjVal << "\n";

		InitialAcceptanceCriteria(FullSolution, CurCommonLists, NoOfServers, NoOfLinks, MaxLinksCap);

		//PrintSummaryOnce(a, FullSolution, myfile3, NoOfServers, NoOfLinks, MaxLinksCap, 0,0,'R');
		//PrintNewLine(myfile3);

	}

	int loopc = FullSolution->VectorOfChangedSols.size();
	for(int c=0; c < loopc; c++){
		FullSolution->VectorOfChangedSols.pop_back();
	}

	int loopd = FullSolution->VectorOfOriginalSols.size();
	for(int c=0; c < loopd; c++){
		FullSolution->VectorOfOriginalSols.pop_back();
	}
}
