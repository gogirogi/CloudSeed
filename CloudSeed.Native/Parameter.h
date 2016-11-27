#ifndef PARAMETER
#define PARAMETER

enum class Parameter
{
	// Input

	InputMix = 0,
	PreDelay,

	HighPass,
	LowPass,

	// Early

	TapCount,
	TapLength,
	TapGain,
	TapDecay,

	DiffusionEnabled,
	DiffusionStages,
	DiffusionDelay,
	DiffusionFeedback,

	// Late

	LateMode,
	LineCount,
	LineDelay,
	LineDecay,
	LineDelayR,
	LineDecayR,


	LateDiffusionEnabled,
	LateDiffusionStages,
	LateDiffusionDelay,
	LateDiffusionFeedback,

	// Grain
	GrainCount,
	GrainLength,
	PitchShift,
	PanSpread,
	PitchRandom,
	TimeRandom,

	// Frequency Response

	PostLowShelfGain,
	PostLowShelfFrequency,
	PostHighShelfGain,
	PostHighShelfFrequency,
	PostCutoffFrequency,

	// Modulation

	EarlyDiffusionModAmount,
	EarlyDiffusionModRate,

	LineModAmount,
	LineModRate,

	LateDiffusionModAmount,
	LateDiffusionModRate,

	// Seeds

	TapSeed,
	DiffusionSeed,
	DelaySeed,
	PostDiffusionSeed,

	// Seed Mix

	CrossSeed,

	DryOut,
	PredelayOut,
	EarlyOut,
	MainOut,

	// Switches
	HiPassEnabled,
	LowPassEnabled,
	LowShelfEnabled,
	HighShelfEnabled,
	CutoffEnabled,
	LateStageTap,

	// Effects
	Interpolation,

	Count,

	Unused = 999
};

#endif
