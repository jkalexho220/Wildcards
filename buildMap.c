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
	
	trPaintTerrain(0,0,0,0,trGetTerrainType(0,0),trGetTerrainSubType(0,0),true);
	
	xsDisableSelf();
	xsEnableRule("gameplay_setup");
}