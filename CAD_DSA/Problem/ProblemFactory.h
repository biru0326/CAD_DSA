#pragma once
#include<memory>
#include "IProblem.h"
#include "ProblemType.h"

namespace Problem
{
	class PlacementOfGuardProblem;
	class MazeSolver;
	class FillingPolygon;

	class ProblemFactory
	{
	public:
		std::shared_ptr<IProblem> GetProblem(ProblemType type, const std::filesystem::path& filePath);

	private:
		std::shared_ptr<PlacementOfGuardProblem> m_PlacementOfGuardProblem;
		std::shared_ptr<MazeSolver> m_MazeSolver;
		std::shared_ptr<FillingPolygon> m_FillingPolygon;
	};
}