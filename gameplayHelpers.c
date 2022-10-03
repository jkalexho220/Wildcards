
/*
Assumes that the target unit is already selected
*/
void spyEffect(int proto = 0, int anim = 0, vector dest = vector(0,0,0), vector scale = vector(1,1,1), int event = -1) {
	int newest = xAddDatabaseBlock(dSpyRequests);
	xSetInt(dSpyRequests, xSpyRequestProto, proto, newest);
	xSetInt(dSpyRequests, xSpyRequestAnim, anim, newest);
	xSetVector(dSpyRequests, xSpyRequestDest, dest, newest);
	xSetVector(dSpyRequests, xSpyRequestScale, scale, newest);
	xSetInt(dSpyRequests, xSpyRequestEvent, event, newest);
	trTechInvokeGodPower(0, "spy", vector(0,0,0), vector(0,0,0));
}

void spawnPlayer(int p = 0, vector pos = vector(0,0,0)) {
	int old = xGetPointer(dPlayerData);
	xSetPointer(dPlayerData, p);
	xSetInt(dPlayerData, xPlayerUnitName, trGetNextUnitScenarioNameNumber());
	trArmyDispatch(""+p+",0", "Hoplite", 1, xsVectorGetX(pos), 0, xsVectorGetZ(pos), 0, true);
	xSetInt(dPlayerData, xPlayerUnitID, kbGetBlockID(""+xGetInt(dPlayerData, xPlayerUnitName), true));
	xSetBool(dPlayerData, xPlayerAlive, true);
	xSetVector(dPlayerData, xPlayerPos, pos);
	int x = xsVectorGetX(pos) / 4;
	int y = xsVectorGetZ(pos) / 4;
	addFrontier(x * 2, y * 2);
	cleanFrontier();

	xUnitSelectByID(dPlayerData, xPlayerUnitID);
	spyEffect(kbGetProtoUnitID("Cinematic Block"), 0, xsVectorSet(dPlayerData, xPlayerProjSpawner, p));
	xSetPointer(dPlayerData, old);
}


rule spy_find
inactive
highFrequency
{
	int id = 0;
	vector scale = vector(0,0,0);
	vector dest = vector(0,0,0);
	for(i = spysearch; < trGetNextUnitScenarioNameNumber()) {
		id = kbGetBlockID(""+i, true);
		if (kbGetUnitBaseTypeID(id) == kbGetProtoUnitID("Spy Eye")) {
			if (xGetDatabaseCount(dSpyRequests) > 0) {
				scale = xGetVector(dSpyRequests, xSpyRequestScale);
				dest = xGetVector(dSpyRequests, xSpyRequestDest);
				trUnitSelectClear();
				trUnitSelectByID(id);
				trMutateSelected(xGetInt(dSpyRequests, xSpyRequestProto));
				trSetSelectedScale(xsVectorGetX(scale),xsVectorGetY(scale),xsVectorGetZ(scale));
				trUnitOverrideAnimation(xGetInt(dSpyRequests, xSpyRequestAnim),0,true,false,-1);
				trEventFire(xGetInt(dSpyRequests, xSpyRequestEvent));
				if (aiPlanSetUserVariableInt(1*xsVectorGetX(dest),1*xsVectorGetY(dest),1*xsVectorGetZ(dest),i) == false) {
					debugLog("spy error N/A: " + 1*xsVectorGetX(dest) + "," + 1*xsVectorGetY(dest) + "," + 1*xsVectorGetZ(dest));
				}
				xFreeDatabaseBlock(dSpyRequests);
			} else {
				debugLog("Spy Buffer is empty");
			}
		}
	}
	spysearch = trGetNextUnitScenarioNameNumber();
}