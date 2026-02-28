#pragma once
struct PreProcessorconfig {
	static bool isDarkNoise;
	static bool isPixelization;
	static int meanWinSize;
	static bool hammingWind;
};
struct FitProcessorConfig {
	static int polynominalOrder;
	static bool distortioncorrect;
};
struct OverSampleProcessorConfig {
	static bool forwardProjection;
};
struct ESFProcessorConfig
{
	static int meanWindow;
	static bool reverse;
};
struct LSFProcessorConfig {
	static int meanWindowSize;
	static bool hammingWindow;
};
struct MTFPostProcessorConfig
{
	static bool anglecorrect;
	static bool smoothcorrect;
	static bool ccdcorrect;
	static bool derivationcorrect;
	static bool mtfsyscorrect;
};
struct DFTProcessorConfig {
    static const bool dftProcess = true;
};