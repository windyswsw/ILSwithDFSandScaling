/*
 * StructsUse.h
 *
 *  Created on: Oct 19, 2015
 *      Author: u99319
 */

using namespace std;

struct Links{
	int LinksBW[85][85];
	short int LinksUse[85][85];
	void PrintLinksBW();
	void PrintLinksUse();
};

struct SinglePath{
	vector<int> Path;
};

struct ReturnPath{
	SinglePath NewPath;
	bool NoPathFound;
};

struct Paths{
	vector<SinglePath> PathsList[65][65];
	vector<SinglePath> InPaths;
	vector<SinglePath> OutPaths;
};

struct Policy{
	vector<int> NFIDs;
	short int PolicyID;
	char ART;
	short int BW;
};

struct NFAllocation{
	short int ServerID;
	SinglePath path;
	short int PolicyID;
	short int NFID;
};

struct PartialSol{
	vector<NFAllocation> VMsForAPolicy;
	int Splits;
};

struct VecForHashTable{
	set<int> ServerIndex;
};

struct ChangeInfo{
	int cus;
	int polToChange;
	int NFToChange;
	string AR;
	int BWToChange;
};

struct RoundChangeInfo{
	vector<ChangeInfo> RoundChangeList;
};

struct UniqueList{
	map<short int,short int> ServerCapMap;
	unordered_multimap<short int, VecForHashTable> ServerCapHashTable;
	vector<short int> ServerCapArray;
	vector<short int> ServerUsedList;
	Links LinksLists;
};

struct CommonList{
	vector<Policy> PolicyList;
	Paths PathsList;
	vector<ChangeInfo> FullChangeListCur;
	vector<short int> NFCap;
};

struct FullSol{
	vector<PartialSol> VectorOfPartialSols;
	vector<PartialSol> VectorOfOriginalSols;
	vector<PartialSol> VectorOfChangedSols;
	float ObjVal;
	float InitialObjVal;
	UniqueList CurLists;
	void PrintAll();
	void PrintTopologyLinks();
	void PrintTopologyServers();
	void PrintPartialSols();
};


struct SpecialForMoreOptMoves{
	NFAllocation InfoOnNf;
	short int PosOfNF;
};

struct ChangedPolicies{
	map<short int,PartialSol> OriginalPolicies;
	map<short int,PartialSol> FirstLocalSearchPolicies;
	map<short int,PartialSol> CurrentlyChangedPolicies;
};

struct TempChangesCount{
	int Svr;
	int Link;
};


void FullSol::PrintAll(){

	char filename1[256] = {0};
	strcpy(filename1,"Allocation.txt");
	
	ofstream myfile;
	myfile.open (filename1, ios::out | ios::app);

	for(int i=0; i<VectorOfPartialSols.size();i++){
		for(int j=0; j<VectorOfPartialSols[i].VMsForAPolicy.size();j++){
			NFAllocation valnew = VectorOfPartialSols[i].VMsForAPolicy[j];
			myfile << valnew.ServerID << " ";
			for(int g=0; g< valnew.path.Path.size(); g++){
				myfile << valnew.path.Path[g] << "-";
			}
			myfile << ",";
		}

		myfile << "\n";
	}

	myfile << "\n";

	for(int i=1; i<CurLists.ServerCapArray.size();i++){
		myfile << CurLists.ServerCapArray[i] << "," ;
	}

	myfile << "\n";

	int a  = sizeof(CurLists.LinksLists.LinksUse)/sizeof(CurLists.LinksLists.LinksUse[0]);

	for(int i=0; i < a; ++i ){
		for(int j = 0; j < a; ++j){
			if(CurLists.LinksLists.LinksUse[i][j] >= 0){
				float usedBW = CurLists.LinksLists.LinksBW[i][j];
				myfile << i << "-" << j << " " << usedBW;
				myfile << ",";
			}
		}
	}
	myfile << "\n";
	myfile.close();
}

void FullSol::PrintTopologyLinks(){

	char filename1[256] = {0};
	strcpy(filename1,"TopologyLinks.txt");
	
	ofstream myfile;
	myfile.open (filename1, ios::out | ios::app);

	int a  = sizeof(CurLists.LinksLists.LinksUse)/sizeof(CurLists.LinksLists.LinksUse[0]);

	for(int i=0; i < a; ++i ){
		for(int j = 0; j < a; ++j){
			if(CurLists.LinksLists.LinksUse[i][j] >= 0){
				float usedBW = CurLists.LinksLists.LinksBW[i][j];
				myfile << i << "-" << j << " " << usedBW;
				myfile << "\n";
			}
		}
	}
	myfile << "\n";
	myfile.close();
}


void FullSol::PrintTopologyServers(){

	char filename1[256] = {0};
	strcpy(filename1,"TopologyServers.txt");
	
	ofstream myfile;
	myfile.open (filename1, ios::out | ios::app);

	for(int i=1; i<CurLists.ServerCapArray.size();i++){
		myfile << i << "-" << CurLists.ServerCapArray[i] << "," ;
		myfile << "\n";
	}

	myfile << "\n";
	myfile.close();
}

void FullSol::PrintPartialSols(){

	char filename1[256] = {0};
	strcpy(filename1,"PartialSols.txt");
	
	ofstream myfile;
	myfile.open (filename1, ios::out | ios::app);

	for(int i=0; i<VectorOfPartialSols.size();i++){
		for(int j=0; j<VectorOfPartialSols[i].VMsForAPolicy.size();j++){
			NFAllocation valnew = VectorOfPartialSols[i].VMsForAPolicy[j];
			myfile << valnew.PolicyID << " ";
			myfile << valnew.ServerID << " ";
			for(int g=0; g< valnew.path.Path.size(); g++){
				myfile << valnew.path.Path[g] << "-";
			}
			myfile << "\n";
		}
	}

	myfile << "\n";
	myfile.close();
}