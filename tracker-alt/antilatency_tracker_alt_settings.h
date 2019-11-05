#pragma once

#include "options/options.hpp"

using namespace options;

struct AltTrackerSettings : opts {
	value<bool> UsePlacementCorrection;
	value<double> ExtrapolationTime;
	
    AltTrackerSettings() :
        opts("tracker-alt"),
        UsePlacementCorrection(b, "use-placement-correction", true),
        ExtrapolationTime(b, "extrapolation-time", 0.0f)
    {}
};
