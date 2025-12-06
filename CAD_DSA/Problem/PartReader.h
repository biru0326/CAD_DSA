#pragma once
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "..\Primitives\Part.h"
#include <tuple>

namespace Problem
{
	template<typename T>
	class PartReader
	{
	public:
		std::shared_ptr<Primitives::Part<T>> CreatePart(const std::filesystem::path& filePath)
		{
			std::ifstream inputFile(filePath);
			if (!inputFile.is_open())
			{
				throw std::runtime_error("Failed to open file: " + filePath.string());
			}

			bool isPart = false;
			bool isOuter = false;
			char name = '$';
			Primitives::LoopDirection direction = Primitives::LoopDirection::CCW;
			std::vector<Primitives::Point2D<T>> vtPoints;
			std::shared_ptr<Primitives::Loop<T>> outerLoop = nullptr;;
			std::vector<std::shared_ptr<Primitives::Loop<T>>> innerLoops;
			Primitives::Point2D<int> outerLoopPixelPoint = Primitives::Point2D<int>(-1, -1);

			std::string line;
			while (std::getline(inputFile, line))
			{
				if (line == "PART")
				{
					isPart = true;
				}
				else if (line == "OUTERLOOP")
				{
					isOuter = true;
				}
				else if (line == "INNERLOOP")
				{
					if (!vtPoints.empty())
					{
						if (isOuter)
						{
							outerLoop = std::make_shared<Primitives::Loop<T>>(isOuter, direction, name, vtPoints);
							vtPoints.clear();
						}
						else
						{
							std::shared_ptr<Primitives::Loop<T>> innerLoop = std::make_shared<Primitives::Loop<T>>(isOuter, direction, name, vtPoints);
							innerLoops.push_back(innerLoop);
							vtPoints.clear();
						}
					}
					isOuter = false;
				}
				else if (line.find("DIRECTION") != std::string::npos)
				{
					size_t equalsPos = line.find('=');
					std::string str = line.substr(equalsPos + 1);

					direction = (str == "CCW") ? Primitives::LoopDirection::CCW : Primitives::LoopDirection::CW;
				}
				else if (line.find("NAME") != std::string::npos)
				{
					size_t equalsPos = line.find('=');
					name = line[equalsPos + 1];
				}
				else if (line.find("OUTERLOOPPIXELPOINT") != std::string::npos)
				{
					Primitives::Point2D<int> point = ParsePoint<int>(line);
					outerLoopPixelPoint.Change(point.GetX(), point.GetY());
				}
				else if (line.find("POINT") != std::string::npos)
				{
					vtPoints.push_back(ParsePoint<T>(line));
				}
			}

			inputFile.close();

			if (!isPart)
			{
				throw std::runtime_error("Could not find PART in the file.");
			}
			else
			{
				if (!vtPoints.empty())
				{
					if (isOuter)
					{
						outerLoop = std::make_shared<Primitives::Loop<T>>(isOuter, direction, name, vtPoints);
						vtPoints.clear();
					}
					else
					{
						std::shared_ptr<Primitives::Loop<T>> innerLoop = std::make_shared<Primitives::Loop<T>>(isOuter, direction, name, vtPoints);
						innerLoops.push_back(innerLoop);
						vtPoints.clear();
					}
				}

				return std::make_shared<Primitives::Part<T>>(outerLoop, outerLoopPixelPoint, innerLoops);
			}
		}

	private:
		template<typename M>
		Primitives::Point2D<M> ParsePoint(const std::string& line)
		{
			size_t equalsPos = line.find('=');
			std::string valuesSubstring = line.substr(equalsPos + 1);
			size_t commaPosInValues = valuesSubstring.find(',');

			std::string xStr = valuesSubstring.substr(0, commaPosInValues);
			std::string yStr = valuesSubstring.substr(commaPosInValues + 1);

			std::istringstream issX(xStr);
			std::istringstream issY(yStr);
			M x, y;
			issX >> x;
			issY >> y;

			return Primitives::Point2D<M>(x, y);
		}
	};
}
