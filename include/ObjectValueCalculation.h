/*
 * ObjectValueCalculation.h
 *
 *  Created on: Oct 19, 2015
 *      Author: u99319
 */


void GlobalObjectValueCalculation(FullSol* InitialFullSolution, float totsvrs, float totlinks, short int MaxLinksCap, int w1, int w2, int w3, int w4, int w5){
	float Obj = 0;
	int NoOfServersUsed = 0;
	int NoOfLinksUsed = 0;
	int svrChange = 0;
	int linksChange = 0;

	for(int f=0; f<=totsvrs;f++){
		if(InitialFullSolution->CurLists.ServerUsedList[f] > 0){
			NoOfServersUsed = NoOfServersUsed + 1;
		}
	}

	int a  = sizeof(InitialFullSolution->CurLists.LinksLists.LinksUse)/sizeof(InitialFullSolution->CurLists.LinksLists.LinksUse[0]);
	float LinksUsedPercent = 0;

	for(int i=0; i < a; ++i ){
		for(int j = 0; j < a; ++j){
			if(InitialFullSolution->CurLists.LinksLists.LinksUse[i][j] > 0){
				NoOfLinksUsed = NoOfLinksUsed + 1;
				LinksUsedPercent = LinksUsedPercent + (float) ((float)(MaxLinksCap - InitialFullSolution->CurLists.LinksLists.LinksBW[i][j])/ (float) MaxLinksCap);
			}
		}
	}

	float p1 = (NoOfServersUsed/totsvrs);
	float p2 = (1 - (float)(NoOfLinksUsed/(totlinks * 2)));
	float p3 = (LinksUsedPercent/ (float) (totlinks*2));

	float p4 = svrChange / totsvrs;
	float p5 = linksChange / totlinks;

	Obj = (w1*p1) + (w2*p2) + (w3*p3) + (w4*p4) + (w5*p5);

	if(InitialFullSolution->ObjVal != Obj){
		InitialFullSolution->ObjVal = Obj;
	}

}


float GlobalReturnObjectValue(FullSol* InitialFullSolution, float totsvrs, float totlinks, short int MaxLinksCap, int w1, int w2, int w3, int w4, int w5){
	float Obj = 0;
	int NoOfServersUsed = 0;
	int NoOfLinksUsed = 0;
	int svrChange = 0;
	int linksChange = 0;

	for(int f=0; f<=totsvrs;f++){
		if(InitialFullSolution->CurLists.ServerUsedList[f] > 0){
			NoOfServersUsed = NoOfServersUsed + 1;
		}
	}

	int a  = sizeof(InitialFullSolution->CurLists.LinksLists.LinksUse)/sizeof(InitialFullSolution->CurLists.LinksLists.LinksUse[0]);
	float LinksUsedPercent = 0;

	for(int i=0; i < a; ++i ){
		for(int j = 0; j < a; ++j){
			if(InitialFullSolution->CurLists.LinksLists.LinksUse[i][j] > 0){
				NoOfLinksUsed = NoOfLinksUsed + 1;
				LinksUsedPercent = LinksUsedPercent + (float) ((float)(MaxLinksCap - InitialFullSolution->CurLists.LinksLists.LinksBW[i][j])/ (float) MaxLinksCap);
			}
		}
	}

	float p1 = (NoOfServersUsed/totsvrs);
	float p2 = (1 - (float)(NoOfLinksUsed/(totlinks * 2)));
	float p3 = (LinksUsedPercent/ (float) (totlinks*2));

	float p4 = svrChange / totsvrs;
	float p5 = linksChange / totlinks;

	Obj = (w1*p1) + (w2*p2) + (w3*p3) + (w4*p4) + (w5*p5);

	return Obj;
}

void LocalObjectValueCalculation(FullSol* InitialFullSolution, CommonList* CurCommonLists, float totsvrs, float totlinks, short int MaxLinksCap, int w1, int w2, int w3, int w4, int w5, ChangedPolicies* TrackChangedPolicies){
	float Obj = 0;
	int NoOfServersUsed = 0;
	int NoOfLinksUsed = 0;
	int svrChange = 0;
	int linksChange = 0;
	int bwuse = 0;

	for(int f=0; f<=totsvrs;f++){
		if(InitialFullSolution->CurLists.ServerUsedList[f] > 0){
			NoOfServersUsed = NoOfServersUsed + 1;
		}
	}

	int a  = sizeof(InitialFullSolution->CurLists.LinksLists.LinksUse)/sizeof(InitialFullSolution->CurLists.LinksLists.LinksUse[0]);
	float LinksUsedPercent = 0;

	for(int i=0; i < a; ++i ){
		for(int j = 0; j < a; ++j){
			if(InitialFullSolution->CurLists.LinksLists.LinksUse[i][j] > 0){
				NoOfLinksUsed = NoOfLinksUsed + 1;
				bwuse = bwuse + (MaxLinksCap - InitialFullSolution->CurLists.LinksLists.LinksBW[i][j]);
				LinksUsedPercent = LinksUsedPercent + (float) ((float)(MaxLinksCap - InitialFullSolution->CurLists.LinksLists.LinksBW[i][j])/ (float) MaxLinksCap);
			}
		}
	}

	float p1 = (NoOfServersUsed/totsvrs);
	float p2 = (1 - (float)(NoOfLinksUsed/(totlinks * 2)));
	float p3 = (LinksUsedPercent/ (float) (totlinks*2));

	// count changes 

	// first from CurrentlyChangedPolicies

	for (std::map<short int, PartialSol>::iterator it=TrackChangedPolicies->CurrentlyChangedPolicies.begin(); it!=TrackChangedPolicies->CurrentlyChangedPolicies.end(); ++it){
		short int Pol= it->first;
		PartialSol NewSol = it->second;

		// compare it with original policies
		std::map<short int,PartialSol>::iterator  it4 = TrackChangedPolicies->OriginalPolicies.find(Pol);
		if( it4 != TrackChangedPolicies->OriginalPolicies.end()){
			PartialSol OrgSol = it4->second;

			int TotNFs = OrgSol.VMsForAPolicy.size();

			for(int k=0; k< OrgSol.VMsForAPolicy.size(); k++){
				NFAllocation NFO = OrgSol.VMsForAPolicy[k];
				int NFSvrO = NFO.ServerID;	
				SinglePath PathO = NFO.path;

				NFAllocation NFC = NewSol.VMsForAPolicy[k];
				int NFSvrC = NFC.ServerID;	
				SinglePath PathC = NFC.path;

				if(NFSvrO != NFSvrC){
					svrChange = svrChange + 1;
				}

				if(PathO.Path != PathC.Path){
					if(PathC.Path.size() >1){
						linksChange =  linksChange + (PathC.Path.size()-1);
					}
				}

				if(NFSvrO != NFSvrC){ // new server
					if( k == (TotNFs - 1)){ // last NF of the policy
						SinglePath OutPathNew = Check_OutPath(NFSvrC,CurCommonLists);
						if(OutPathNew.Path.size() >1){
							linksChange =  linksChange + (OutPathNew.Path.size()-1);
						}
					}
				}
			}
		}
	}

	// next from FirstLocalPolicies

	for (std::map<short int, PartialSol>::iterator it1=TrackChangedPolicies->FirstLocalSearchPolicies.begin(); it1!=TrackChangedPolicies->FirstLocalSearchPolicies.end(); ++it1){
		short int Pol= it1->first;
		PartialSol NewSol = it1->second;

		// If it is in the CurrentlyChangedPolicies, then it has been already counted
		std::map<short int,PartialSol>::iterator  it2 = TrackChangedPolicies->CurrentlyChangedPolicies.find(Pol);
		if(it2 == TrackChangedPolicies->CurrentlyChangedPolicies.end()){ // it2 is the end, Policy not found, that means it is not in CurrentlyChangedPolicies
		
			// compare it with original policies
			std::map<short int,PartialSol>::iterator  it5 = TrackChangedPolicies->OriginalPolicies.find(Pol);
			if( it5 != TrackChangedPolicies->OriginalPolicies.end()){
				PartialSol OrgSol = it5->second;

				int TotNFs = OrgSol.VMsForAPolicy.size();

				for(int k=0; k< OrgSol.VMsForAPolicy.size(); k++){
					NFAllocation NFO = OrgSol.VMsForAPolicy[k];
					int NFSvrO = NFO.ServerID;	
					SinglePath CurPathO = NFO.path;

					NFAllocation NFC = NewSol.VMsForAPolicy[k];
					int NFSvrC = NFC.ServerID;	
					SinglePath CurPathC = NFC.path;

					if(NFSvrO != NFSvrC){
						svrChange = svrChange + 1;
					}

					if(CurPathO.Path != CurPathC.Path){
						if(CurPathC.Path.size() >1){
							linksChange =  linksChange + (CurPathC.Path.size()-1);
						}
					}

					if(NFSvrO != NFSvrC){ // new server
						if( k == (TotNFs - 1)){ // last NF of the policy
							SinglePath OutPathNew = Check_OutPath(NFSvrC,CurCommonLists);
							if(OutPathNew.Path.size() >1){
								linksChange =  linksChange + (OutPathNew.Path.size()-1);
							}
						}
					}
				}
			}
		}
	}


	float p4 = svrChange / totsvrs;
	float p5 = linksChange / totlinks;

	Obj = (w1*p1) + (w2*p2) + (w3*p3) + (w4*p4) + (w5*p5);

	if(InitialFullSolution->ObjVal != Obj){
		InitialFullSolution->ObjVal = Obj;
	}

}


float LocalReturnObjectValue(FullSol* InitialFullSolution, CommonList* CurCommonLists,float totsvrs, float totlinks, short int MaxLinksCap, int w1, int w2, int w3, int w4, int w5, ChangedPolicies* TrackChangedPolicies){
	float Obj = 0;
	int NoOfServersUsed = 0;
	int NoOfLinksUsed = 0;
	int svrChange = 0;
	int linksChange = 0;

	for(int f=0; f<=totsvrs;f++){
		if(InitialFullSolution->CurLists.ServerUsedList[f] > 0){
			NoOfServersUsed = NoOfServersUsed + 1;
		}
	}

	int a  = sizeof(InitialFullSolution->CurLists.LinksLists.LinksUse)/sizeof(InitialFullSolution->CurLists.LinksLists.LinksUse[0]);
	float LinksUsedPercent = 0;

	for(int i=0; i < a; ++i ){
		for(int j = 0; j < a; ++j){
			if(InitialFullSolution->CurLists.LinksLists.LinksUse[i][j] > 0){
				NoOfLinksUsed = NoOfLinksUsed + 1;
				LinksUsedPercent = LinksUsedPercent + (float) ((float)(MaxLinksCap - InitialFullSolution->CurLists.LinksLists.LinksBW[i][j])/ (float) MaxLinksCap);
			}
		}
	}

	float p1 = (NoOfServersUsed/totsvrs);
	float p2 = (1 - (float)(NoOfLinksUsed/(totlinks * 2)));
	float p3 = (LinksUsedPercent/ (float) (totlinks*2));

	// count changes 

	// first from CurrentlyChangedPolicies

	for (std::map<short int, PartialSol>::iterator it=TrackChangedPolicies->CurrentlyChangedPolicies.begin(); it!=TrackChangedPolicies->CurrentlyChangedPolicies.end(); ++it){
		short int Pol= it->first;
		PartialSol NewSol = it->second;

		// compare it with original policies
		std::map<short int,PartialSol>::iterator  it4 = TrackChangedPolicies->OriginalPolicies.find(Pol);
		if( it4 != TrackChangedPolicies->OriginalPolicies.end()){
			PartialSol OrgSol = it4->second;

			int TotNFs = OrgSol.VMsForAPolicy.size();

			for(int k=0; k< OrgSol.VMsForAPolicy.size(); k++){
				NFAllocation NFO = OrgSol.VMsForAPolicy[k];
				int NFSvrO = NFO.ServerID;	
				SinglePath PathO = NFO.path;

				NFAllocation NFC = NewSol.VMsForAPolicy[k];
				int NFSvrC = NFC.ServerID;	
				SinglePath PathC = NFC.path;

				if(NFSvrO != NFSvrC){
					svrChange = svrChange + 1;
				}

				if(PathO.Path != PathC.Path){
					if(PathC.Path.size() >1){
						linksChange =  linksChange + (PathC.Path.size()-1);
					}
				}

				if(NFSvrO != NFSvrC){ // new server
					if( k == (TotNFs - 1)){ // last NF of the policy
						SinglePath OutPathNew = Check_OutPath(NFSvrC,CurCommonLists);
						if(OutPathNew.Path.size() >1){
							linksChange =  linksChange + (OutPathNew.Path.size()-1);
						}
					}
				}
			}
		}
	}

	// next from FirstLocalPolicies

	for (std::map<short int, PartialSol>::iterator it1=TrackChangedPolicies->FirstLocalSearchPolicies.begin(); it1!=TrackChangedPolicies->FirstLocalSearchPolicies.end(); ++it1){
		short int Pol= it1->first;
		PartialSol NewSol = it1->second;

		// If it is in the CurrentlyChangedPolicies, then it has been already counted
		std::map<short int,PartialSol>::iterator  it2 = TrackChangedPolicies->CurrentlyChangedPolicies.find(Pol);
		if(it2 == TrackChangedPolicies->CurrentlyChangedPolicies.end()){ // it2 is the end, Policy not found, that means it is not in CurrentlyChangedPolicies
		
			// compare it with original policies
			std::map<short int,PartialSol>::iterator  it5 = TrackChangedPolicies->OriginalPolicies.find(Pol);
			if( it5 != TrackChangedPolicies->OriginalPolicies.end()){
				PartialSol OrgSol = it5->second;

				int TotNFs = OrgSol.VMsForAPolicy.size();

				for(int k=0; k< OrgSol.VMsForAPolicy.size(); k++){
					NFAllocation NFO = OrgSol.VMsForAPolicy[k];
					int NFSvrO = NFO.ServerID;	
					SinglePath CurPathO = NFO.path;

					NFAllocation NFC = NewSol.VMsForAPolicy[k];
					int NFSvrC = NFC.ServerID;	
					SinglePath CurPathC = NFC.path;

					if(NFSvrO != NFSvrC){
						svrChange = svrChange + 1;
					}

					if(CurPathO.Path != CurPathC.Path){
						if(CurPathC.Path.size() >1){
							linksChange =  linksChange + (CurPathC.Path.size()-1);
						}
					}

					if(NFSvrO != NFSvrC){ // new server
						if( k == (TotNFs - 1)){ // last NF of the policy
							SinglePath OutPathNew = Check_OutPath(NFSvrC,CurCommonLists);
							if(OutPathNew.Path.size() >1){
								linksChange =  linksChange + (OutPathNew.Path.size()-1);
							}
						}
					}
				}
			}
		}
	}

	float p4 = svrChange / totsvrs;
	float p5 = linksChange / totlinks;

	Obj = (w1*p1) + (w2*p2) + (w3*p3) + (w4*p4) + (w5*p5);

	return Obj;
}

float LocalTempObjectValue(FullSol* InitialFullSolution, CommonList* CurCommonLists,float totsvrs, float totlinks, short int MaxLinksCap, int w1, int w2, int w3, int w4, int w5, float SvrChanges, float LinkChanges){
	float Obj = 0;
	int NoOfServersUsed = 0;
	int NoOfLinksUsed = 0;
	int bwuse = 0;

	for(int f=0; f<=totsvrs;f++){
		if(InitialFullSolution->CurLists.ServerUsedList[f] > 0){
			NoOfServersUsed = NoOfServersUsed + 1;
		}
	}

	int a  = sizeof(InitialFullSolution->CurLists.LinksLists.LinksUse)/sizeof(InitialFullSolution->CurLists.LinksLists.LinksUse[0]);
	float LinksUsedPercent = 0;

	for(int i=0; i < a; ++i ){
		for(int j = 0; j < a; ++j){
			if(InitialFullSolution->CurLists.LinksLists.LinksUse[i][j] > 0){
				NoOfLinksUsed = NoOfLinksUsed + 1;
				bwuse = bwuse + (MaxLinksCap - InitialFullSolution->CurLists.LinksLists.LinksBW[i][j]);
				LinksUsedPercent = LinksUsedPercent + (float) ((float)(MaxLinksCap - InitialFullSolution->CurLists.LinksLists.LinksBW[i][j])/ (float) MaxLinksCap);
			}
		}
	}

	float p1 = (NoOfServersUsed/totsvrs);
	float p2 = (1 - (float)(NoOfLinksUsed/(totlinks * 2)));
	float p3 = (LinksUsedPercent/ (float) (totlinks*2));

	float p4 = SvrChanges / totsvrs;
	float p5 = LinkChanges / totlinks;

	Obj = (w1*p1) + (w2*p2) + (w3*p3) + (w4*p4) + (w5*p5);

	return Obj;
}

TempChangesCount CheckNumberOfChangesTemp(ChangedPolicies* TrackChangedPolicies, CommonList* CurCommonLists, PartialSol* SelectedPolicyTemp, int polID){

	int svrChange = 0;
	int linksChange = 0;

	// count changes 

	// first from CurrentlyChangedPolicies

	for (std::map<short int, PartialSol>::iterator it=TrackChangedPolicies->CurrentlyChangedPolicies.begin(); it!=TrackChangedPolicies->CurrentlyChangedPolicies.end(); ++it){
		short int Pol= it->first;
		PartialSol NewSol = it->second;

		if(Pol != polID){ // if the policy is not the one that is changing in this round, use the state in TrackChangedPolicies

			// compare it with original policies
			std::map<short int,PartialSol>::iterator  it4 = TrackChangedPolicies->OriginalPolicies.find(Pol);
			if( it4 != TrackChangedPolicies->OriginalPolicies.end()){
				PartialSol OrgSol = it4->second;

				int TotNFs = OrgSol.VMsForAPolicy.size();

				for(int k=0; k< OrgSol.VMsForAPolicy.size(); k++){
					NFAllocation NFO = OrgSol.VMsForAPolicy[k];
					int NFSvrO = NFO.ServerID;	
					SinglePath PathO = NFO.path;

					NFAllocation NFC = NewSol.VMsForAPolicy[k];
					int NFSvrC = NFC.ServerID;	
					SinglePath PathC = NFC.path;

					if(NFSvrO != NFSvrC){
						svrChange = svrChange + 1;
					}

					if(PathO.Path != PathC.Path){
						if(PathC.Path.size() >1){
							linksChange =  linksChange + (PathC.Path.size()-1);
						}
					}

					if(NFSvrO != NFSvrC){ // new server
						if( k == (TotNFs - 1)){ // last NF of the policy
							SinglePath OutPathNew = Check_OutPath(NFSvrC,CurCommonLists);
							if(OutPathNew.Path.size() >1){
								linksChange =  linksChange + (OutPathNew.Path.size()-1);
							}
						}
					}
				}
			}
		}
	}

	// next from FirstLocalPolicies

	for (std::map<short int, PartialSol>::iterator it1=TrackChangedPolicies->FirstLocalSearchPolicies.begin(); it1!=TrackChangedPolicies->FirstLocalSearchPolicies.end(); ++it1){
		short int Pol= it1->first;
		PartialSol NewSol = it1->second;

		if(Pol != polID){ // if the policy is not the one that is changing in this round, use the state in FirstLocalPolicies

			// If it is in the CurrentlyChangedPolicies, then it has been already counted
			std::map<short int,PartialSol>::iterator  it2 = TrackChangedPolicies->CurrentlyChangedPolicies.find(Pol);
			if(it2 == TrackChangedPolicies->CurrentlyChangedPolicies.end()){ // it2 is the end, Policy not found, that means it is not in CurrentlyChangedPolicies
		
				// compare it with original policies
				std::map<short int,PartialSol>::iterator  it5 = TrackChangedPolicies->OriginalPolicies.find(Pol);
				if( it5 != TrackChangedPolicies->OriginalPolicies.end()){
					PartialSol OrgSol = it5->second;

					int TotNFs = OrgSol.VMsForAPolicy.size();

					for(int k=0; k< OrgSol.VMsForAPolicy.size(); k++){
						NFAllocation NFO = OrgSol.VMsForAPolicy[k];
						int NFSvrO = NFO.ServerID;	
						SinglePath CurPathO = NFO.path;

						NFAllocation NFC = NewSol.VMsForAPolicy[k];
						int NFSvrC = NFC.ServerID;	
						SinglePath CurPathC = NFC.path;

						if(NFSvrO != NFSvrC){
							svrChange = svrChange + 1;
						}

						if(CurPathO.Path != CurPathC.Path){
							if(CurPathC.Path.size() >1){
								linksChange =  linksChange + (CurPathC.Path.size()-1);
							}
						}

						if(NFSvrO != NFSvrC){ // new server
							if( k == (TotNFs - 1)){ // last NF of the policy
								SinglePath OutPathNew = Check_OutPath(NFSvrC,CurCommonLists);
								if(OutPathNew.Path.size() >1){
									linksChange =  linksChange + (OutPathNew.Path.size()-1);
								}
							}
						}
					}
				}
			}
		}
	}

	std::map<short int,PartialSol>::iterator  it511 = TrackChangedPolicies->OriginalPolicies.find(polID);
	if( it511 != TrackChangedPolicies->OriginalPolicies.end()){
		PartialSol OrgSol11 = it511->second;

		int TotNFs = OrgSol11.VMsForAPolicy.size();

		for(int k=0; k< OrgSol11.VMsForAPolicy.size(); k++){
		
			NFAllocation NFO1 = OrgSol11.VMsForAPolicy[k];
			int NFSvrO1 = NFO1.ServerID;	
			SinglePath CurPathO1 = NFO1.path;

			NFAllocation NFC1 = SelectedPolicyTemp->VMsForAPolicy[k];
			int NFSvrC1 = NFC1.ServerID;	
			SinglePath CurPathC1 = NFC1.path;

			if(NFSvrO1 != NFSvrC1){
				svrChange = svrChange + 1;
			}

			if(CurPathO1.Path != CurPathC1.Path){
				if(CurPathC1.Path.size() >1){
					linksChange =  linksChange + (CurPathC1.Path.size()-1);
				}
			}

			if(NFSvrO1 != NFSvrC1){ // new server
				if( k == (TotNFs - 1)){ // last NF of the policy
						SinglePath OutPathNew = Check_OutPath(NFSvrC1,CurCommonLists);
						if(OutPathNew.Path.size() >1){
							linksChange =  linksChange + (OutPathNew.Path.size()-1);
						}
					}
				}
		}
	}

	TempChangesCount Count;
	Count.Svr = svrChange;
	Count.Link = linksChange;

	return Count;
}