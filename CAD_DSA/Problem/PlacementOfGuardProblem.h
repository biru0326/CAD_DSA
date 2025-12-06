#pragma once
#include "IProblem.h"
#include "PartReader.h"
#include <iostream>

namespace Problem
{
	class PlacementOfGuardProblem : public IProblem
	{
	public:
		PlacementOfGuardProblem(const std::filesystem::path& InputPath)
		{
			filePath = InputPath / "PlacementOfGuardProblem.txt";
		}

		void Solve() override
		{
			PartReader<double> reader;
			std::shared_ptr<Primitives::Part<double>> pPart = reader.CreatePart(filePath);
			int guards = pPart->Triangulate();
			std::cout << "PlacementOfGuardProblem result : " << guards << std::endl;
		}
	};
}