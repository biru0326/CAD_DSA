#pragma once
#include <iostream>
#include "IProblem.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "..\Primitives\Part.h"
#include <array>

namespace Problem
{
	class FillingPolygon : public IProblem
	{
		using constSharedLoopInt = std::shared_ptr<const Primitives::Loop<int>>;
		using sharedPartInt = std::shared_ptr<Primitives::Part<int>>;
		using MatrixChar = std::vector<std::vector<char>>;
		using Point2DInt = Primitives::Point2D<int>;
		using BFSQueue = std::queue<std::pair<int, int>>;
	public:
		FillingPolygon(const std::filesystem::path& InputPath)
		{
			filePath = InputPath / "FillingPolygon.txt";
		}

		void Solve() override
		{
			//I used the structure of part.
			//It is not required
			//Instead directly save into array


			int rowIndex = -1;
			int columnIndex = -1;
			while (1)
			{
				std::cout << "Please enter row index within [0 - 21]" << std::endl;
				std::cin >> rowIndex;
				if (rowIndex < 1 && rowIndex > 22)
				{
					continue;
				}
				break;
			}
			while (1)
			{
				std::cout << "Please enter column index within [0 - 11]" << std::endl;
				std::cin >> columnIndex;
				if (columnIndex < 1 && columnIndex > 12)
				{
					continue;
				}
				break;
			}

			PartReader<int> reader;
			sharedPartInt pPart = reader.CreatePart(filePath);
			MatrixChar result;
			char defaultValue = '$';
			FillPolygon(pPart, rowIndex, columnIndex, defaultValue, result);

			for (int i = 0; i < result.size(); ++i)
			{
				for (int j = 0; j < result[0].size(); ++j)
				{
					if (result[i][j] == defaultValue)
					{
						std::cout << ' ';
					}
					else
					{
						std::cout << result[i][j];
					}
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}

		void FillPolygon(sharedPartInt pPart, int selectedRowIndex, int selectedColumnIndex, char defaultValue, MatrixChar& result)const
		{
			Point2DInt point(selectedColumnIndex, selectedRowIndex);
			bool isPointInside = false;
			constSharedLoopInt selectedLoop = pPart->GetLoop(point, isPointInside);
			if (selectedLoop == nullptr)
			{
				return;
			}

			int columnCount = pPart->GetWidth()+1;
			int rowCount = pPart->GetHeight()+1;
			result.reserve(rowCount);
			for (int i = 0; i < rowCount; ++i)
			{
				result.emplace_back(columnCount, defaultValue);//$ -> means default
			}
			Point2DInt outerLoopCell = pPart->GetOuterLoopPixelCell();
			result[outerLoopCell.GetY()][outerLoopCell.GetX()] = pPart->GetOuterLoopName();

			std::vector<constSharedLoopInt> innerLoops;
			pPart->GetInnerLoops(innerLoops);
			for (const auto& innerLoop : innerLoops)
			{
				std::vector<Point2DInt> orgPoints;
				innerLoop->GetOriginalPoints(orgPoints);
				for (const auto& point : orgPoints)
				{
					result[point.GetY()][point.GetX()] = innerLoop->GetName();
				}
			}

			const int dx[] = { -1, 1, 0, 0, -1, -1, 1, 1 };
			const int dy[] = { 0, 0, -1, 1, -1, 1, 1, -1 };
			if (!isPointInside)
			{
				if (selectedLoop->IsOuter())
				{
					selectedRowIndex = outerLoopCell.GetY();
					selectedColumnIndex = outerLoopCell.GetX();
				}
				else
				{
					for (int i = 0; i < 8; ++i)
					{
						int newRow = selectedRowIndex + dx[i];
						int newColumn = selectedColumnIndex + dy[i];

						if (selectedLoop->IsPointOnOrInsidePolygon(Point2DInt(newColumn, newRow), isPointInside) && isPointInside)
						{
							selectedRowIndex = newRow;
							selectedColumnIndex = newColumn;
							break;
						}
					}
				}
			}

			//Now traverse in 4 direction recursively from [selectedRowIndex, selectedColumnIndex] in result
			char name = selectedLoop->GetName();
			result[selectedRowIndex][selectedColumnIndex] = name;
			FillPolygon(selectedRowIndex, selectedColumnIndex, defaultValue, name, result);
		}

		void FillPolygon(int selectedRowIndex, int selectedColumnIndex, char defaultValue, char name, MatrixChar& result)const
		{
			int rowCount = (int)result.size();
			int columnCount = (int)result[0].size();
			BFSQueue bfs;
			bfs.push({ selectedRowIndex, selectedColumnIndex });

			const int dx[] = { -1, 1, 0, 0 };
			const int dy[] = { 0, 0, -1, 1 };

			while (!bfs.empty())
			{
				auto cell = bfs.front();
				bfs.pop();
				int currentRow = cell.first;
				int currentColumn = cell.second;

				for (int i = 0; i < 4; ++i)
				{
					int newRow = currentRow + dx[i];
					int newColumn = currentColumn + dy[i];

					if (IsValid(result, newRow, newColumn, defaultValue))
					{
						bfs.push({ newRow, newColumn });
						result[newRow][newColumn] = name;
					}
				}
			}
		}

		bool IsValid(const MatrixChar& matrix, int row, int column, char defaultValue)const
		{
			if (row < 0 || column < 0 || row >= matrix.size() || column >= matrix[0].size())
			{
				return false;
			}
			if (matrix[row][column] != defaultValue)
			{
				return false;
			}
			return true;
		}
	};
}