
void GenCombination(vector<int> data, vector<vector <int>>* AllCombinations, int Remainder, int MaxPossible){

    if (Remainder == 0){
		vector<int> subset;
        for (int j=0; j<data.size(); j++){
			subset.push_back(data[j]);
		}
		AllCombinations->push_back(subset);		
		printf("\n");
        return;
    }

    for (int k = MaxPossible; k>0; k--){
		if( k <= Remainder){
			data.push_back(k);
			GenCombination(data, AllCombinations,(Remainder-k),k);
			data.pop_back();
		}
    }
}

void GenNFsCombinationsToSplit(vector<vector <int>>* FilteredCombinations, int TotNFs, int OriginalSplits){
    vector<int> data; 
	vector<vector <int>> AllCombinations;
	vector<int> subset;
	subset.push_back(TotNFs);
	//AllCombinations.push_back(subset);

    GenCombination(data, &AllCombinations, TotNFs, (TotNFs-1));

	for(int i=0; i< AllCombinations.size(); i++){
		if(AllCombinations[i].size() <= OriginalSplits){
			FilteredCombinations->push_back(AllCombinations[i]);
		}
	}
}