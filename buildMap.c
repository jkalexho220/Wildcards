int perlin = 0;

vector startPos = vector(0,0,0);

rule build_map
inactive
highFrequency
{
	trChangeTerrainHeight(0,0,mapSize+1,mapSize+1,0,false);
	perlin = generatePerlinNoise(mapSize, 10);
	float noise = 0;
	trPaintTerrain(0,0,mapSize,mapSize,TERRAIN_PRIMARY, TERRAIN_PRIMARY_SUB,false);
	for(x=0; <= mapSize) {
		for(y=0; <= mapSize) {
			noise = getPerlinNoise(perlin, x, y);
			if (noise > 1.0) {
				trChangeTerrainHeight(x, y, x, y, noise);
				trPaintTerrain(x,y,x,y,TERRAIN_WALL,TERRAIN_WALL_SUB,false);
			} else if (noise > -1.0) {
				trPaintTerrain(x,y,x,y,TERRAIN_WALL_BASE,TERRAIN_WALL_BASE_SUB,false);
			} else if (noise < -2.0 && noise > -3.0) {
				trPaintTerrain(x,y,x,y,TERRAIN_DIRT,TERRAIN_DIRT_SUB,false);
			} else if (noise <= -3.0) {
				trPaintTerrain(x,y,x,y,TERRAIN_ROAD,TERRAIN_ROAD_SUB,false);
			}
		}
	}
	
	xsDisableSelf();

	startPos = perlinRoll(perlin, mapSize / 2, mapSize / 2, 1, -10.0);

	xsEnableRule("gameplay_start");

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
	}

	trPaintTerrain(0,0,0,0,trGetTerrainType(0,0),trGetTerrainSubType(0,0),true);
}