// performs perlin roll first
vector spawnGoldMine(vector pos = vector(0,0,0), int val = 0) {
	pos = perlinRoll(perlin, 0 + xsVectorGetX(pos) / 2, 0 + xsVectorGetZ(pos) / 2, 1, -5.0);
	trMinimapFlare(trCurrentPlayer(), 5.0, pos, true);
	xAddDatabaseBlock(dGoldMines, true);
	float x = xsVectorGetX(pos);
	float z = xsVectorGetZ(pos);
	xSetInt(dGoldMines, xGoldMineCircle, trGetNextUnitScenarioNameNumber());
	trArmyDispatch("0,0","Dwarf",1,x,0,z,0,true);
	xUnitSelect(dGoldMines, xGoldMineCircle);
	trUnitChangeProtoUnit("Gaia Forest Effect");

	xSetInt(dGoldMines, xGoldMineRevealer, trGetNextUnitScenarioNameNumber());
	trArmyDispatch("0,0","Revealer",1,x,0,z,0,true);
	
	xSetInt(dGoldMines, xGoldMineHolder, trGetNextUnitScenarioNameNumber());
	trArmyDispatch(""+wildcard+",0","Dwarf",1,x,0,z,225,true);
	trArmySelect(""+wildcard+",0");
	trMutateSelected(kbGetProtoUnitID("Lancer Hero"));

	xSetInt(dGoldMines, xGoldMineObject, trGetNextUnitScenarioNameNumber());
	trArmyDispatch(""+wildcard+",0","Dwarf",1,x,0,z,225,true);
	trArmySelect(""+wildcard+",0");
	trUnitChangeProtoUnit("Gold Mine Dwarven");
	xUnitSelect(dGoldMines, xGoldMineObject);
	trMutateSelected(kbGetProtoUnitID("Relic"));
	trImmediateUnitGarrison(""+xGetInt(dGoldMines, xGoldMineHolder));
	trMutateSelected(kbGetProtoUnitID("Gold Mine Dwarven"));
	trSetSelectedScale(1,1,0.7);

	xSetInt(dGoldMines, xGoldMineFlag, trGetNextUnitScenarioNameNumber());
	trArmyDispatch(""+wildcard+",0","Dwarf",1,x,0,z,225,true);
	trArmySelect(""+wildcard+",0");
	trUnitChangeProtoUnit("Flag Numbered");
	xUnitSelect(dGoldMines, xGoldMineFlag);
	trUnitSetAnimationPath(""+(val - 1)+",0,0,0,0,0,0");
	trMutateSelected(kbGetProtoUnitID("Relic"));
	trImmediateUnitGarrison(""+xGetInt(dGoldMines, xGoldMineHolder));
	trMutateSelected(kbGetProtoUnitID("Flag Numbered"));

	xSetInt(dGoldMines, xGoldMineValue, val);
	xSetVector(dGoldMines, xUnitPos, pos);

	xUnitSelect(dGoldMines, xGoldMineHolder);
	trMutateSelected(kbGetProtoUnitID("Wadjet Spit"));

	return(pos);
}

void removeGoldMine() {
	xUnitSelect(dGoldMines, xGoldMineRevealer);
	trUnitChangeProtoUnit("Qilin Heal");

	xUnitSelect(dGoldMines, xGoldMineFlag);
	trUnitDestroy();

	xUnitSelect(dGoldMines, xGoldMineObject);
	trUnitDestroy();

	xUnitSelect(dGoldMines, xGoldMineHolder);
	trUnitChangeProtoUnit("Hero Birth");

	xUnitSelect(dGoldMines, xGoldMineCircle);
	trUnitChangeProtoUnit("Hero Birth");

	xFreeDatabaseBlock(dGoldMines);
}