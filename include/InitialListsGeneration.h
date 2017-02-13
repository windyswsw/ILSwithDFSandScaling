/*
 * InitialListsGeneration.h
 *
 *  Created on: Oct 19, 2015
 *      Author: u99319
 */

void GenUniqueList(UniqueList* NewAllList, float NoOfServers, short int ServerCap, float NoOfLinks, short int BW){

	gen_ServerCapacityList(NewAllList, NoOfServers, ServerCap);
	gen_LinksList(NewAllList,"Links.txt", BW);

	for(int i=0; i<=NoOfServers;i++){
		NewAllList->ServerUsedList.push_back(0);
	}
}


void GenCommonList(CommonList* NewAllList, float NoOfServers, short int ServerCap, float NoOfLinks, short int BW){

	gen_PolicyList(NewAllList,"Policy.txt");
	GenAllPathsLists(NewAllList,"Paths.txt", "InPaths.txt", "OutPaths.txt");
	gen_NFCapList(NewAllList,"NFs.txt");
	gen_PolicyChangeList(NewAllList, "ChangeInfo.txt");

}
