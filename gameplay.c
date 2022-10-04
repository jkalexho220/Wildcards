void removeCamTracks(int eventID = -1) {
	trackRemove();
	trUnblockAllSounds();
}

void hotkeyAbility(int ability = 0) {
	xsSetContextPlayer(0);
	int old = xGetPointer(dPlayerData);
	xSetPointer(dPlayerData, trCurrentPlayer());
	if (xGetBool(dPlayerData, xPlayerAlive)) {
		trackInsert();
		trackAddWaypoint();
		trackPlay(1,EVENT_BUILD_AT_CURSOR);
		trBlockAllSounds(false);
		xUnitSelectByID(dPlayerData, xPlayerUnitID);
		if (trUnitIsSelected() == false) {
			uiFindType(xGetString(dPlayerData, xPlayerProto));
		}
		switch(ability)
		{
			case EVENT_BUILD_HOUSE:
			{
				uiSetProtoCursor("House", true);
			}
			case EVENT_BUILD_GRANARY:
			{
				uiSetProtoCursor("Granary", true);
			}
			case EVENT_BUILD_STOREHOUSE:
			{
				uiSetProtoCursor("Storehouse", true);
			}
		}
	}
	xSetPointer(dPlayerData, old);
}

void buildAtCursor(int eventID = -1) {
	uiBuildAtPointer();
	uiClearCursor();
	trackInsert();
	trackAddWaypoint();
	trackPlay(1,EVENT_REMOVE_CAM_TRACKS);
}

rule gameplay_setup
inactive
highFrequency
{
	xsDisableSelf();
	startPos = perlinRoll(perlin, mapSize / 2, mapSize / 2, 1, -10.0);

	xsEnableRule("gameplay_start");

	// spawn the hotkey units
	for(i=EVENT_BUILD_HOUSE; <= EVENT_BUILD_STOREHOUSE) {
		trEventSetHandler(i, "hotkeyAbility");
	}
	trEventSetHandler(EVENT_REMOVE_CAM_TRACKS, "removeCamTracks");
	trEventSetHandler(EVENT_BUILD_AT_CURSOR, "buildAtCursor");

	map("q", "game", "trackInsert(); trackAddWaypoint();trackPlay(-1,0);");
	map("w", "game", "trackInsert(); trackAddWaypoint();trackPlay(-1,1);");
	map("e", "game", "trackInsert(); trackAddWaypoint();trackPlay(-1,2);");

	vector dir = vector(1,0,0);
	float angle = 6.283185 / (cNumberPlayers - 1);
	float mCos = xsCos(angle);
	float mSin = xsSin(angle);
	for(p=1; < cNumberPlayers) {
		spawnPlayer(p, startPos);
		trUnitSelectClear();
		trUnitSelectByID(xGetInt(dPlayerData, xPlayerUnitID, p));
		trUnitTeleport(xsVectorGetX(startPos) + (xsVectorGetX(dir) * 3.0), 0, xsVectorGetZ(startPos) + (xsVectorGetZ(dir) * 3.0));
		trSetUnitOrientation(dir, vector(0,1,0), true);
		dir = rotationMatrix(dir, mCos, mSin);
		pickUpWeapon(p, WEAPON_KNIFE, 5);
	}
	trQuestVarSetFromRand("sound", 0, 6.283185, false);
	angle = trQuestVarGet("sound");
	mCos = xsCos(angle);
	mSin = xsSin(angle);
	vector pos = perlinRoll(perlin, mapSize / 2 * (1.0 + mCos * 0.8), mapSize / 2 * (1.0 + mSin * 0.8), 1, -5.0);
	spawnCollectible(pos, WEAPON_WILDCARD, 1);
}

rule gameplay_start
inactive
highFrequency
{
	xsDisableSelf();
	spysearch = trGetNextUnitScenarioNameNumber();

	xsEnableRule("track_los");
	xsEnableRule("ysearch");
	xsEnableRule("gameplay_always");

	trSetFogAndBlackmap(true, false);
	trCameraCut(vector(-46.464447,70.710701,-46.464447) + startPos, vector(0.5,-0.707107,0.5), vector(0.5,0.707107,0.5), vector(0.707107,0,-0.707107));

	displayWeapons();
	displayDashCount();
}

rule gameplay_always
inactive
highFrequency
{
	float dist = 0;
	int pointer = 0;
	int p = 0;
	int val = 0;
	int db = 0;
	vector pos = vector(0,0,0);
	vector prev = vector(0,0,0);
	vector dir = vector(0,0,0);
	string proto = "";

	for(p=1; < cNumberPlayers) {
		xSetPointer(dPlayerData, p);
		if (xGetBool(dPlayerData, xPlayerAlive)) {
			xUnitSelectByID(dPlayerData, xPlayerUnitID);
			if (trUnitAlive() == false) {
				// death code goes here
				removeFrontier(0, 0, true);
				xSetBool(dPlayerData, xPlayerAlive, false);
				db = xGetInt(dPlayerData, xPlayerWeaponDatabase);
				xClearDatabase(db);
				xSetInt(dPlayerData, xPlayerDashStep, 0);
				xSetInt(dPlayerData, xPlayerRespawnTime, trTimeMS() + 10000);
				if (trCurrentPlayer() == p) {
					displayWeapons();
					trCounterAbort("dashes");
					trCounterAddTime("respawn", 10, 1, "Respawn", -1);
				}
				if (wildcard == p) {
					trMessageSetText("The Wildcard has been dropped! Treasure locations have been reset!");
					trSoundPlayFN("backtowork.wav","1",-1,"","");
					trOverlayText("",1,-1,-1,-1);
					for(i=xGetDatabaseCount(dGoldMines); >0) {
						xDatabaseNext(dGoldMines);
						removeGoldMine();
					}
					wildcard = 0;
					wildcardStep = 0;
					spawnCollectible(xGetVector(dPlayerData, xPlayerPos), WEAPON_WILDCARD, 1);
				}
			} else {
				switch(xGetInt(dPlayerData, xPlayerDashStep))
				{
					case 0:
					{
						if (xGetInt(dPlayerData, xPlayerDashCount) < 2) {
							xSetInt(dPlayerData, xPlayerDashCooldown, trTimeMS() + 15000);
							if (trCurrentPlayer() == p) {
								displayDashCount();
							}
							xSetInt(dPlayerData, xPlayerDashStep, 1);
						}
					}
					case 1:
					{
						if (trTimeMS() > xGetInt(dPlayerData, xPlayerDashCooldown)) {
							xSetInt(dPlayerData, xPlayerDashStep, 0);
							xSetInt(dPlayerData, xPlayerDashCount, 1 + xGetInt(dPlayerData, xPlayerDashCount));
							if (trCurrentPlayer() == p) {
								displayDashCount();
							}
						}
					}
				}
				if (trPlayerResourceCount(p, "food") > 0) {
					trPlayerGrantResources(p, "food", -9999);
					shootWeapon(p);
				}
				if (trPlayerResourceCount(p, "wood") > 0) {
					trPlayerGrantResources(p, "wood", -9999);
					switchWeapon(p);
				}
				if (trPlayerResourceCount(p, "gold") > 0) {
					trPlayerGrantResources(p, "gold", -9999);
					dash(p);
				}
			}
		} else if (trTimeMS() > xGetInt(dPlayerData, xPlayerRespawnTime)) {
			prev = xsVectorSet(mapSize, 0, mapSize);
			if (wildcard > 0) {
				pos = xGetVector(dPlayerData, xPlayerPos, wildcard);
			} else {
				pos = xGetVector(dPlayerData, xPlayerPos);
			}
			dir = getUnitVector(prev, pos);
			trQuestVarSetFromRand("sound", 1.5708, 4.712388, false);
			dir = rotationMatrix(dir, xsCos(trQuestVarGet("sound")), xsSin(trQuestVarGet("sound")));
			pos = vectorToGrid(prev + dir * 0.8 * mapSize);
			spawnPlayer(p, perlinRoll(perlin, xsVectorGetX(pos), xsVectorGetZ(pos), 2, -2.0));
			pickUpWeapon(p, WEAPON_KNIFE, 5);
			if (trCurrentPlayer() == p) {
				proto = xGetString(dPlayerData, xPlayerProto, p);
				uiFindType(proto);
				trSoundPlayFN("herorevive.wav","1",-1,"","");
				displayWeapons();
				displayDashCount();
			}
		}
	}

	// collision position update
	for(i=xGetDatabaseCount(dUnits); >0) {
		xDatabaseNext(dUnits);
		xUnitSelectByID(dUnits, xUnitID);
		if (trUnitAlive() == false) {
			xFreeDatabaseBlock(dUnits);
		} else {
			xSetVector(dUnits, xUnitPos, kbGetBlockPosition(""+xGetInt(dUnits, xUnitName), true));
		}
	}

	for(i=xGetDatabaseCount(dKnives); >0) {
		xDatabaseNext(dKnives);
		pos = kbGetBlockPosition(""+xGetInt(dKnives, xUnitName), true);
		xSetVector(dKnives, xUnitPos, pos);
		if (terrainIsType(vectorToGrid(pos), TERRAIN_WALL, TERRAIN_WALL_SUB)) {
			spawnCollectible(vectorSnapToGrid(xGetVector(dKnives, xProjPrev) - xGetVector(dKnives, xProjDir) * 2.0), WEAPON_KNIFE);
			trQuestVarSetFromRand("sound", 1, 3, true);
			xUnitSelectByID(dKnives, xUnitID);
			if (trUnitVisToPlayer()) {
				trSoundPlayFN("mine"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
			}
			trUnitChangeProtoUnit("Lightning Sparks ground");
			xFreeDatabaseBlock(dKnives);
		}
	}

	// knife collision detection
	bool hit = false;
	for(i=xGetDatabaseCount(dKnives); >0) {
		xDatabaseNext(dKnives);
		pos = xGetVector(dKnives, xUnitPos);
		prev = xGetVector(dKnives, xProjPrev);
		dist = distanceBetweenVectors(pos, prev);
		if (dist > 1.0) {
			p = xGetInt(dKnives, xUnitOwner);
			dist = xsSqrt(dist);
			dir = xGetVector(dKnives, xProjDir);
			for(j=xGetDatabaseCount(dUnits); >0) {
				xDatabaseNext(dUnits);
				if (xGetInt(dUnits, xUnitOwner) == p) {
					continue;
				} else if (targetEligible(p)) {
					if (rayCollision(dUnits, prev, dir, dist, xGetFloat(dKnives, xProjRadius))) {
						xUnitSelectByID(dUnits, xUnitID);
						trDamageUnit(1);
						xUnitSelectByID(dKnives, xUnitID);
						if (xGetInt(dUnits, xUnitOwner) > 0) {
							trQuestVarSetFromRand("sound", 1, 4, true);
							if (trUnitVisToPlayer()) {
								trSoundPlayFN("arrowonflesh"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
							}
						}
						trUnitChangeProtoUnit("Lightning Sparks ground");
						xFreeDatabaseBlock(dKnives);
						hit = true;
						break;
					}
				}
			}
			if (hit == false) {
				pointer = xGetPointer(dKnives);
				for(j=xGetDatabaseCount(dKnives); >1) {
					xDatabaseNext(dKnives);
					if (xGetInt(dKnives, xUnitOwner) != p) {
						if (rayCollision(dKnives, prev, dir, dist, xGetFloat(dKnives, xProjRadius))) {
							xUnitSelectByID(dKnives, xUnitID);
							trUnitChangeProtoUnit("Lightning Sparks ground");
							spawnCollectible(vectorSnapToGrid(xGetVector(dKnives, xProjPrev) - xGetVector(dKnives, xProjDir) * 2.0), WEAPON_KNIFE);
							xFreeDatabaseBlock(dKnives);
							hit = true;
							break;
						}
					}
				}
				xSetPointer(dKnives, pointer);
				if (hit) {
					trQuestVarSetFromRand("sound", 1, 6, true);
					xUnitSelectByID(dKnives, xUnitID);
					if (trUnitVisToPlayer()) {
						trSoundPlayFN("metalslice"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
					}
					trUnitChangeProtoUnit("Lightning Sparks ground");
					spawnCollectible(vectorSnapToGrid(xGetVector(dKnives, xProjPrev) - xGetVector(dKnives, xProjDir) * 2.0), WEAPON_KNIFE);
					xFreeDatabaseBlock(dKnives);
				}
			}
			if (hit == false) {
				xSetVector(dKnives, xProjPrev, pos);
			}
		}
	}

	// Collectibles
	for(i=xsMin(5, xGetDatabaseCount(dCollectibles)); >0) {
		xDatabaseNext(dCollectibles);
		xUnitSelect(dCollectibles, xCollectibleObject);
		if (trUnitIsSelected()) {
			displayWeaponDetails(xGetInt(dCollectibles, xCollectibleType));
			reselectMyself();
		}
		for(p=1; < cNumberPlayers) {
			xSetPointer(dPlayerData, p);
			if (xGetBool(dPlayerData, xPlayerAlive)) {
				if (distanceBetweenVectors(xGetVector(dPlayerData, xPlayerPos), xGetVector(dCollectibles, xUnitPos)) < 10.0) {
					if (pickUpWeapon(p, xGetInt(dCollectibles, xCollectibleType), xGetInt(dCollectibles, xCollectibleCount))) { 
						xUnitSelect(dCollectibles, xCollectibleObject);
						trUnitChangeProtoUnit("Fireball Launch Damage Effect");
						xUnitSelect(dCollectibles, xCollectiblePad);
						trUnitChangeProtoUnit("Dust Small");
						xFreeDatabaseBlock(dCollectibles);
						trQuestVarSetFromRand("sound", 1, 2, true);
						if (trCurrentPlayer() == p) {
							trSoundPlayFN("vortexland"+1*trQuestVarGet("sound")+".wav","1",-1,"","");
						}
						break;
					}
				}
			}
		}
	}

	// WILDCARD
	switch(wildcardStep)
	{
		case 0: // waiting for someone to find the Wildcard
		{
			if (wildcard > 0) {
				trSoundPlayFN("spybirth.wav","1",-1,"","");
				trSoundPlayFN("ageadvance.wav","1",-1,"","");
				trChatSend(0, "<color={Playercolor("+wildcard+")}>{Playername("+wildcard+")}</color> has found the Wildcard!");
				overlayTextColor(wildcard);
				displayWildcardDetails();
				wildcardStep = 1;
				wildcardNext = trTimeMS() + 3000;
			}
		}
		case 1:
		{
			if (trTimeMS() > wildcardNext) {
				wildcardStep = 2;
				trMessageSetText("The treasure locations have appeared!", -1);
				if (trCurrentPlayer() == wildcard) {
					trChatSend(0, "<color=1,1,1>Your goal is to reach the treasure to earn points!</color>");
					trChatSend(0, "<color=1,1,1>Other players can now damage you!</color>");
				} else {
					trChatSend(0, "<color=1,1,1>Stop the Wildcard from reaching the treasure!</color>");
					trChatSend(0, "<color=1,1,1>You can now damage the Wildcard.</color>");
				}
			}
		}
		case 2:
		{
			if (xGetDatabaseCount(dGoldMines) == 0) {
				pos = xGetVector(dPlayerData, xPlayerPos, wildcard);
				prev = xsVectorSet(mapSize, 0, mapSize);
				dir = getUnitVector(prev, pos, 0.8 * mapSize);
				for(i=0; <3) {
					dir = rotationMatrix(dir, 0.0, 1.0);
					if (i == 1) {
						val = 3;
					} else {
						val = 2;
					}
					spawnGoldMine(prev + dir, val);
				}
				trSoundPlayFN("plentybirth.wav","1",-1,"","");
			}
		}
	}

	if (xGetDatabaseCount(dGoldMines) > 0) {
		xDatabaseNext(dGoldMines);
		pos = xGetVector(dGoldMines, xUnitPos);
		if (distanceBetweenVectors(pos, xGetVector(dPlayerData, xPlayerPos, wildcard)) < 25.0) {
			earnPoints(wildcard, xGetInt(dGoldMines, xGoldMineValue));
			displayWildcardDetails();
			trSoundPlayFN("plentyvaultstolen.wav","1",-1,"","");
			trMessageSetText("The Wildcard has earned " + xGetInt(dGoldMines, xGoldMineValue) + " points!");
			removeGoldMine();
		}
	}


}