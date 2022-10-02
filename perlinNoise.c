float interpolatePerlin(float start = 0, float end = 1, float percentage = 0) {
	// return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
	return((end - start) * (3.0 - percentage * 2.0) * xsPow(percentage, 2) + start);
	//return((end - start) * percentage + start);
}

int generatePerlinNoise(int size = 50, int granularity = 5) {
	int db = aiPlanCreate("perlinNoise", 8);
	int meta = zNewArray(mInt, 3, "perlinMeta");
	zSetInt(meta, 0, db);
	zSetInt(meta, 1, granularity);
	zSetInt(meta, 2, zNewArray(mFloat, 4, "perlinInterpolation"));
	int dimension = 1 + size / granularity;
	for(i=0; < dimension) {
		aiPlanAddUserVariableVector(db,i,"perlin"+i,dimension);
		for(j=0; < dimension) {
			trQuestVarSetFromRand("temp", 0, 6.283185, false);
			aiPlanSetUserVariableVector(db,i,j,vectorSetFromAngle(trQuestVarGet("temp")));
		}
	}
	return(meta);
}

float getPerlinNoise(int meta = 0, int x = 0, int y = 0, bool debug = false) {
	int db = zGetInt(meta, 0);
	int granularity = zGetInt(meta, 1);
	int interpolation = zGetInt(meta, 2);
	int baseX = x / granularity;
	int baseY = y / granularity;
	vector pos = xsVectorSet(x - baseX * granularity, 0, y - baseY * granularity);
	vector dir = vector(0,0,0);
	float total = 0;
	float interpolateX = xsVectorGetX(pos) / granularity;
	float interpolateY = xsVectorGetZ(pos) / granularity;
	for(i=0; <= 1) {
		for(j=0; <= 1) {
			dir = pos - xsVectorSet(1.0 * granularity * i, 0, 1.0 * granularity * j);
			zSetFloat(interpolation, 2 * i + j, dotProduct(dir, aiPlanGetUserVariableVector(db, baseX + i, baseY + j)));
		}
	}
	total = interpolatePerlin(interpolatePerlin(zGetFloat(interpolation, 0), zGetFloat(interpolation, 1), interpolateY),interpolatePerlin(zGetFloat(interpolation, 2), zGetFloat(interpolation, 3), interpolateY), interpolateX);
	return(total);
}