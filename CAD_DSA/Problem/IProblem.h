#pragma once
#include <filesystem>

namespace Problem
{
	class IProblem
	{
	public:
		virtual void Solve() = 0;
		virtual ~IProblem() = default; 
	protected:
		std::filesystem::path filePath;
	};
}