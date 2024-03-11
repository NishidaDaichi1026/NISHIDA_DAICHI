#include	"CUndulationPlaneMesh.h"

void CUndulationPlaneMesh::MakeUndulation(float min,float max)
{
		std::mt19937 mt;
		std::uniform_real_distribution<float> dist(min, max);

		for (auto& v : m_vertices) {
			v.Position.y = dist(mt);
		}
}
