/*
Visible = first bit
Frontier = second bit

X = start unit name
Y = array of ints
*/
int losDimension = 0;

rule setup_los
active
highFrequency
{
	xsDisableSelf();
	losDimension = mapSize / 2;
	int dbName = 0;
	for(p=1; < cNumberPlayers) {
		trModifyProtounit("Revealer to Player", p, 2, 999999999999.0);
		trModifyProtounit("Revealer to Player", p, 2, -999999999999.0);
		trModifyProtounit("Revealer to Player", p, 2, 3.0);
		dbName = aiPlanCreate("playerLOS"+p, 9);
		xSetPointer(dPlayerData, p);
		xSetInt(dPlayerData, xPlayerLosStart, trGetNextUnitScenarioNameNumber());
		xSetInt(dPlayerData, xPlayerLosDB, dbName);
		for(x=0; < losDimension) {
			aiPlanAddUserVariableInt(dbName, x, "row"+x, losDimension);
			for(y=0; < losDimension) {
				aiPlanSetUserVariableInt(dbName, x, y, 0);
				trArmyDispatch(""+p+",0","Dwarf",1,1 + x * 4, 0, 1 + y * 4, 0, true);
				trArmySelect(""+p+",0");
				trMutateSelected(kbGetProtoUnitID("Cinematic Block"));
			}
		}
		xSetInt(dPlayerData, xPlayerLosStart, kbGetBlockID(""+xGetInt(dPlayerData, xPlayerLosStart)));
		trArmyDispatch(""+p+",0","Victory Marker",1,1,0,1,0,true);
	}

	// TESTING
	trQuestVarSet("test", trGetNextUnitScenarioNameNumber());
	trArmyDispatch("1,0", "Hoplite", 1, 10, 0, 10, 0, true);
	xsEnableRule("hello");
}

rule hello
inactive
highFrequency
{
	vector pos = kbGetBlockPosition(""+1*trQuestVarGet("test"), true);
	vector test = vector(0,0,0);
	int xPos = xsVectorGetX(pos) / 4;
	int yPos = xsVectorGetZ(pos) / 4;
	xSetPointer(dPlayerData, 1);
	for(x=xsMax(0, xPos - 5); < xsMin(xPos + 5, losDimension)) {
		for(y=xsMax(0, yPos - 5); < xsMin(yPos + 5, losDimension)) {
			test = xsVectorSet(x * 4, 0, y * 4);
			trUnitSelectClear();
			trUnitSelectByID(xGetInt(dPlayerData, xPlayerLosStart) + losDimension * x + y);
			if (distanceBetweenVectors(pos, test) > 144.0) {
				trMutateSelected(kbGetProtoUnitID("Cinematic Block"));
			} else {
				trMutateSelected(kbGetProtoUnitID("Lampades Bolt"));
			}
		}
	}
}