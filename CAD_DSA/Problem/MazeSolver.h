#pragma once
#include "IProblem.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <iostream>
#include <queue>

namespace Problem
{
	class MazeSolver : public IProblem
	{
		using IntMatrix = std::vector<std::vector<int>>;
		using BoolMatrix = std::vector<std::vector<bool>>;
		using BFSQueue = std::queue<std::pair<int, int>>;
	public:
		MazeSolver(const std::filesystem::path& InputPath)
		{
			filePath = InputPath / "MazeSolver.txt";
		}

		void Solve() override
		{
			ResetSourceAndDestinationCell();
			IntMatrix matrix = CreateMazeMatrix(filePath);
			std::vector<std::pair<int, int>> shortestPath = GetShortestPath(matrix);
			std::cout << "MazeSolver shortest path with cell ids (the index starts with 1): " << std::endl;
			for (const auto& pair : shortestPath)
			{
				std::cout << pair.first << " , " << pair.second << std::endl;
			}
		}

	private:
		std::vector<std::pair<int, int>> GetShortestPath(IntMatrix& matrix)
		{
			std::vector<std::pair<int, int>> result;
			if (m_SourceCellRow == m_DestinationCellRow && m_SourceCellColumn == m_DestinationCellColumn)
			{
				result.push_back({ m_SourceCellRow , m_SourceCellColumn });
				return result;
			}
			int rowCount = (int)matrix.size();
			int columnCount = (int)matrix[0].size();
			BoolMatrix visited(rowCount, std::vector<bool>(columnCount, false));
			DoBFS(matrix, m_SourceCellRow, m_SourceCellColumn, visited);

			int currentRow = m_DestinationCellRow;
			int currentColumn = m_DestinationCellColumn;
			result.push_back({ currentRow ,currentColumn });
			int dist = matrix[currentRow][currentColumn];
			while (dist > 0)
			{
				--dist;
				const int dx[] = { -1, 1, 0, 0 };
				const int dy[] = { 0, 0, -1, 1 };
				for (int i = 0; i < 4; ++i)
				{
					int newRow = currentRow + dx[i];
					int newColumn = currentColumn + dy[i];

					if (IsValid(matrix, newRow, newColumn, visited, true))
					{
						if (matrix[newRow][newColumn] == dist)
						{
							result.push_back({ newRow ,newColumn });
							currentRow = newRow;
							currentColumn = newColumn;
							break;
						}
					}
				}
			}

			std::reverse(result.begin(), result.end());
			return result;
		}

		void DoBFS(IntMatrix& matrix, int row, int column, BoolMatrix& visited)
		{
			BFSQueue bfs;
			bfs.push({ row, column });
			visited[row][column] = true;

			const int dx[] = { -1, 1, 0, 0 };
			const int dy[] = { 0, 0, -1, 1 };
			bool destinationFound = false;
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

					if (IsValid(matrix, newRow, newColumn, visited, false))
					{
						bfs.push({ newRow, newColumn });
						visited[newRow][newColumn] = true;
						matrix[newRow][newColumn] = matrix[currentRow][currentColumn] + 1;
						if (m_DestinationCellRow == newRow && m_DestinationCellColumn == newColumn)
						{
							destinationFound = true;
							break;
						}
					}
				}

				if (destinationFound)
					break;
			}
		}

		bool IsValid(const IntMatrix& matrix, int row, int column, BoolMatrix& visited, bool visitedFlagToCheck)
		{
			if (row < 0 || column < 0 || row >= matrix.size() || column >= matrix[0].size())
			{
				return false;
			}
			if (matrix[row][column] == -1)
			{
				return false;
			}
			if (visited[row][column] == visitedFlagToCheck)
			{
				return true;
			}
			return false;
		}

		IntMatrix CreateMazeMatrix(const std::filesystem::path& filePath)
		{
			std::ifstream inputFile(filePath);
			if (!inputFile.is_open())
			{
				throw std::runtime_error("Failed to open file: " + filePath.string());
			}

			IntMatrix result;
			bool isMatrixInitialized = false;
			int rowCount = 0;
			int columnCount = 0;
			std::string line;
			while (std::getline(inputFile, line))
			{
				if (line.find("ROWCOUNT") != std::string::npos)
				{
					rowCount = ParseValue(line);
					isMatrixInitialized = InitializeMazeMatrix(rowCount, columnCount, result);
				}
				else if (line.find("COLUMNCOUNT") != std::string::npos)
				{
					columnCount = ParseValue(line);
					isMatrixInitialized = InitializeMazeMatrix(rowCount, columnCount, result);
				}
				else if (line.find("SOURCECELL") != std::string::npos)
				{
					if (!isMatrixInitialized)
						break;
					std::tuple<int, int> cell = ParseCellIndex(line);
					m_SourceCellRow = std::get<0>(cell);
					m_SourceCellColumn = std::get<1>(cell);
				}
				else if (line.find("DESTINATIONCELL") != std::string::npos)
				{
					if (!isMatrixInitialized)
						break;
					std::tuple<int, int> cell = ParseCellIndex(line);
					m_DestinationCellRow = std::get<0>(cell);
					m_DestinationCellColumn = std::get<1>(cell);
				}
				else if (line.find("BLOCKEDCELL") != std::string::npos)
				{
					if (!isMatrixInitialized)
						break;
					std::tuple<int, int> cell = ParseCellIndex(line);
					result[std::get<0>(cell)][std::get<1>(cell)] = -1;
				}
			}

			return result;
		}

		int ParseValue(const std::string& line)
		{
			size_t equalsPos = line.find('=');
			std::string valuesSubstring = line.substr(equalsPos + 1);
			return std::stoi(valuesSubstring);
		}

		std::tuple<int, int> ParseCellIndex(const std::string& line)
		{
			size_t equalsPos = line.find('=');
			std::string valuesSubstring = line.substr(equalsPos + 1);
			size_t commaPosInValues = valuesSubstring.find(',');

			std::string xStr = valuesSubstring.substr(0, commaPosInValues);
			std::string yStr = valuesSubstring.substr(commaPosInValues + 1);

			int x = std::stoi(xStr);
			int y = std::stoi(yStr);

			return std::make_tuple(--x, --y);
		}

		bool InitializeMazeMatrix(int rowCount, int columnCount, IntMatrix& result)
		{
			if (rowCount == 0 || columnCount == 0)
				return false;
			result.reserve(rowCount);
			for (int i = 0; i < rowCount; ++i)
			{
				result.emplace_back(columnCount, 1);
			}
			return true;
		}

		void ResetSourceAndDestinationCell()
		{
			m_SourceCellRow = -1;
			m_SourceCellColumn = -1;
			m_DestinationCellRow = -1;
			m_DestinationCellColumn = -1;
		}

		int m_SourceCellRow = -1;
		int m_SourceCellColumn = -1;
		int m_DestinationCellRow = -1;
		int m_DestinationCellColumn = -1;
	};
}