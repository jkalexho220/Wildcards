int perlin = 0;

vector startPos = vector(0,0,0);

rule build_map
inactive
highFrequency
{
	trChangeTerrainHeight(0,0,mapSize+1,mapSize+1,0.0,false);
	perlin = generatePerlinNoise(mapSize, 10);

	float noise = 0;
	trPaintTerrain(0,0,mapSize,mapSize,TERRAIN_PRIMARY, TERRAIN_PRIMARY_SUB,false);
	for(x=0; <= mapSize) {
		for(y=0; <= mapSize) {
			trQuestVarSetFromRand("rand", 0, 1, false);
			noise = getPerlinNoise(perlin, x, y);
			if (noise > 1.0) {
				trChangeTerrainHeight(x, y, x, y, noise);
				trPaintTerrain(x,y,x,y,TERRAIN_WALL,TERRAIN_WALL_SUB,false);
			} else if (noise > -1.0) {
				trPaintTerrain(x,y,x,y,TERRAIN_WALL_BASE,TERRAIN_WALL_BASE_SUB,false);
				if (trQuestVarGet("rand") < 0.05) {
					trQuestVarSetFromRand("rand", 1, 360, true);
					trArmyDispatch("0,0","Rock Granite Big",1,1 + 2 * x, 0, 1 + 2 * y, trQuestVarGet("rand"), true);
				}
			} else if (noise < -2.0 && noise > -3.0) {
				trPaintTerrain(x,y,x,y,TERRAIN_DIRT,TERRAIN_DIRT_SUB,false);
			} else if (noise <= -3.0) {
				trPaintTerrain(x,y,x,y,TERRAIN_ROAD,TERRAIN_ROAD_SUB,false);
			} else {
				if (trQuestVarGet("rand") < 0.1) {
					trQuestVarSetFromRand("rand", 1, 360, true);
					trArmyDispatch("0,0","Tundra Tree",1,1 + 2 * x, 0, 1 + 2 * y, trQuestVarGet("rand"), true);
				}
			}
		}
	}

	frostArray = aiPlanCreate("frostArray", 8);
	smokeArray = aiPlanCreate("smokeArray", 8);
	for(x=0; < mapSize) {
		aiPlanAddUserVariableInt(smokeArray, x, "smoke"+x, mapSize);
		aiPlanAddUserVariableInt(frostArray, x, "frost"+x, mapSize);
		for(y=0; < mapSize) {
			aiPlanSetUserVariableInt(smokeArray, x, y, 0);
			aiPlanSetUserVariableInt(frostArray, x, y, 0);
		}
	}

	trPaintTerrain(0,0,0,mapSize,TERRAIN_WALL,TERRAIN_WALL_SUB,false);
	trPaintTerrain(0,0,mapSize,0,TERRAIN_WALL,TERRAIN_WALL_SUB,false);
	trPaintTerrain(0,mapSize,mapSize,mapSize,TERRAIN_WALL,TERRAIN_WALL_SUB,false);
	trPaintTerrain(mapSize,0,mapSize,mapSize,TERRAIN_WALL,TERRAIN_WALL_SUB,true);
	
	//trPaintTerrain(0,0,0,0,trGetTerrainType(0,0),trGetTerrainSubType(0,0),true);
	
	xsDisableSelf();
	xsEnableRule("gameplay_setup");
}