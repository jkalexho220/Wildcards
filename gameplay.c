rule gameplay_start
inactive
highFrequency
{
	xsDisableSelf();
	enableGameplayRules();
	trSetFogAndBlackmap(true, false);
	trCameraCut(vector(-46.464447,70.710701,-46.464447) + startPos, vector(0.5,-0.707107,0.5), vector(0.5,0.707107,0.5), vector(0.707107,0,-0.707107));
}