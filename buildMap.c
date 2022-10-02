rule terrainRandom
active
highFrequency
{
	trChangeTerrainHeight(0,0,mapSize+1,mapSize+1,0,false);
	int perlin = generatePerlinNoise(mapSize, 10);
	float noise = 0;
	for(x=0; <= mapSize) {
		for(y=0; <= mapSize) {
			noise = getPerlinNoise(perlin, x, y);
			if (noise > 1.0) {
				trPaintTerrain(x,y,x,y,TERRAIN_WALL_PRIMARY,TERRAIN_WALL_SECONDARY,false);
				trChangeTerrainHeight(x, y, x, y, noise);
			}
		}
	}
	trSetFogAndBlackmap(true, false);
	xsDisableSelf();


	for(p=1; < cNumberPlayers) {
		spawnPlayer(p, vector(20,0,20));
	}
	enableGameplayRules();
}