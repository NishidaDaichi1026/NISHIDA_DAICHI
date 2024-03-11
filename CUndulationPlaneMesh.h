#pragma once
#include	<random>
#include	"CPlaneMesh.h"

class CUndulationPlaneMesh : public CPlaneMesh {
public:
	void MakeUndulation(float min, float max);
};
