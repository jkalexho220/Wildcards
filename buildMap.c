int perlin = 0;

float wallHeight = 2;

vector startPos = vector(0,0,0);

rule build_map
inactive
highFrequency
{
	trChangeTerrainHeight(0,0,mapSize+1,mapSize+1,0.0,false);
	perlin = generatePerlinNoise(mapSize, 10);

	int houseArray = aiPlanCreate("houseArray", 8);
	frostArray = aiPlanCreate("frostArray", 8);
	smokeArray = aiPlanCreate("smokeArray", 8);
	for(x=0; < mapSize) {
		aiPlanAddUserVariableInt(smokeArray, x, "smoke"+x, mapSize);
		aiPlanAddUserVariableInt(frostArray, x, "frost"+x, mapSize);
		aiPlanAddUserVariableInt(houseArray, x, "house"+x, mapSize);
		for(y=0; < mapSize) {
			aiPlanSetUserVariableInt(smokeArray, x, y, 0);
			aiPlanSetUserVariableInt(frostArray, x, y, 0);
			aiPlanSetUserVariableInt(houseArray, x, y, 0);
		}
	}

	int buildingType = 0;
	int overlap = 0;
	float noise = 0;
	float maximumHeight = 0;
	trPaintTerrain(0,0,mapSize,mapSize,TERRAIN_PRIMARY, TERRAIN_PRIMARY_SUB,false);
	for(x=0; <= mapSize) {
		for(y=0; <= mapSize) {
			trQuestVarSetFromRand("rand", 0, 1, false);
			noise = getPerlinNoise(perlin, x, y);
			maximumHeight = xsMax(maximumHeight, noise);
			if (noise > wallHeight) {
				if (trGetTerrainType(x,y) != TERRAIN_WALL) {
					trPaintTerrain(x,y,x,y,TERRAIN_WALL_BASE,TERRAIN_WALL_BASE_SUB,false);
				}
				overlap = 0;
				for(i = -2; <= 2) {
					for(j = -2; <= 2) {
						if (coordinatesInPerlin(perlin, x + i, y + j)) {
							overlap = overlap + aiPlanGetUserVariableInt(houseArray, x + i, y + j);
						}
					}
				}
				if (overlap < 10) {
					trPaintTerrain(x - 1, y - 1, x + 1, y + 1, TERRAIN_WALL, TERRAIN_WALL_SUB, false);
					trQuestVarSetFromRand("sound", 0, 2, true);
					buildingType = trQuestVarGet("sound");
					trQuestVarSetFromRand("sound", 0, 2, true);
					if (buildingType > trQuestVarGet("sound")) {
						buildingType = trQuestVarGet("sound");
					}
					trArmyDispatch("0,0","Dwarf",1,x * 2 + 1, 0, y * 2 + 1, 180, true);
					trArmySelect("0,0");
					switch(buildingType)
					{
						case 0:
						{
							trMutateSelected(kbGetProtoUnitID("Manor"));
						}
						case 1:
						{
							trMutateSelected(kbGetProtoUnitID("Dock"));
						}
						case 2:
						{
							trMutateSelected(kbGetProtoUnitID("Guild"));
						}
					}

					for(i = -2; <= 2) {
						for(j = -2; <= 2) {
							if (coordinatesInPerlin(perlin, x + i, y + j)) {
								aiPlanSetUserVariableInt(houseArray, x + i, y + j, 1 + aiPlanGetUserVariableInt(houseArray, x + i, y + j));
							}
						}
					}
				}
				//trChangeTerrainHeight(x, y, x, y, noise);
				//trPaintTerrain(x,y,x,y,TERRAIN_WALL,TERRAIN_WALL_SUB,false);
			} else if (trGetTerrainType(x,y) != TERRAIN_WALL) {
				if (noise > -0.5) {
					trPaintTerrain(x,y,x,y,TERRAIN_WALL_BASE,TERRAIN_WALL_BASE_SUB,false);
					if (trQuestVarGet("rand") < 0.06) {
						trQuestVarSetFromRand("rand", 1, 360, true);
						trArmyDispatch("0,0","Columns",1,1 + 2 * x, 0, 1 + 2 * y, trQuestVarGet("rand"), true);
					}
				} else if (noise < -2.0 && noise > -3.0) {
					trPaintTerrain(x,y,x,y,TERRAIN_DIRT,TERRAIN_DIRT_SUB,false);
				} else if (noise <= -3.0) {
					trPaintTerrain(x,y,x,y,TERRAIN_ROAD,TERRAIN_ROAD_SUB,false);
				} else {
					if (trQuestVarGet("rand") < 0.03) {
						trQuestVarSetFromRand("rand", 1, 360, true);
						trArmyDispatch("0,0","Rock Granite Big",1,1 + 2 * x, 0, 1 + 2 * y, trQuestVarGet("rand"), true);
					} else if (trQuestVarGet("rand") < 0.1) {
						trQuestVarSetFromRand("rand", 1, 360, true);
						trArmyDispatch("0,0","Tundra Tree",1,1 + 2 * x, 0, 1 + 2 * y, trQuestVarGet("rand"), true);
					}
				}
			}
		}
	}

	debugLog("Maximum height: " + maximumHeight);

	aiPlanDestroy(houseArray);

	trRevealEntireMap();

	trPaintTerrain(0,0,0,mapSize,TERRAIN_WALL,TERRAIN_WALL_SUB,false);
	trPaintTerrain(0,0,mapSize,0,TERRAIN_WALL,TERRAIN_WALL_SUB,false);
	trPaintTerrain(0,mapSize,mapSize,mapSize,TERRAIN_WALL,TERRAIN_WALL_SUB,false);
	trPaintTerrain(mapSize,0,mapSize,mapSize,TERRAIN_WALL,TERRAIN_WALL_SUB,true);
	
	//trPaintTerrain(0,0,0,0,trGetTerrainType(0,0),trGetTerrainSubType(0,0),true);
	
	xsDisableSelf();
	xsEnableRule("gameplay_setup");
}