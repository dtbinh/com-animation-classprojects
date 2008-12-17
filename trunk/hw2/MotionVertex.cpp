#include "MotionVertex.h"

MotionVertex::MotionVertex()
{
	m_InSCC = false;
	m_NumSCCAdj = 0;
}

void MotionVertex::computeNumSCCAdj()
{
	int i;
	int size = m_AdjVertices.size();

	for (i = 0; i < size; i++)
	{
		if (m_AdjVertices[i]->m_InSCC)
			m_NumSCCAdj++;
	}
}