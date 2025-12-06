#pragma once
#include "ProblemFactory.h"
#include "PlacementOfGuardProblem.h"
#include "MazeSolver.h"
#include "FillingPolygon.h"
#include <stdexcept>

namespace Problem
{
	std::shared_ptr<IProblem> ProblemFactory::GetProblem(ProblemType type, const std::filesystem::path& InputPath)
	{
		switch (type)
		{
		case ProblemType::ProblemType_FillingPolygon:
		{
			if (m_FillingPolygon == nullptr)
			{
				m_FillingPolygon = std::make_shared<FillingPolygon>(InputPath);
			}
			return m_FillingPolygon;
		}
		case ProblemType::ProblemType_MazeSolver:
		{
			if (m_MazeSolver == nullptr)
			{
				m_MazeSolver = std::make_shared<MazeSolver>(InputPath);
			}
			return m_MazeSolver;
		}
		case ProblemType::ProblemType_PlacementOfGuardProblem:
		{
			if (m_PlacementOfGuardProblem == nullptr)
			{
				m_PlacementOfGuardProblem = std::make_shared<PlacementOfGuardProblem>(InputPath);
			}
			return m_PlacementOfGuardProblem;
		}
		default:
			throw std::runtime_error("Unsupported problem type");
		}
	}
}