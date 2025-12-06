// CAD_DSA.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <filesystem>
#include "Problem/ProblemFactory.h"

int main()
{
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path InputPath = currentPath / "Input";

    Problem::ProblemFactory factory;
    bool continueLoop = true;
    while (continueLoop)
    {
        std::cout << "Please enter the problem type" << std::endl;
        std::cout << "Press 1 for FillingPolygon" << std::endl;
        std::cout << "Press 2 for MazeSolver" << std::endl;
        std::cout << "Press 3 for PlacementOfGuardProblem" << std::endl;
        std::cout << "Press any other key to exit..." << std::endl;

        std::shared_ptr<Problem::IProblem> problem;
        int input;
        std::cin >> input;
        switch (input)
        {
        case 1:
            problem = factory.GetProblem(Problem::ProblemType::ProblemType_FillingPolygon, InputPath);
            break;
        case 2:
            problem = factory.GetProblem(Problem::ProblemType::ProblemType_MazeSolver, InputPath);
            break;
        case 3:
            problem = factory.GetProblem(Problem::ProblemType::ProblemType_PlacementOfGuardProblem, InputPath);
            break;
        default:
            continueLoop = false;
            break;
        }
        if(continueLoop)
            problem->Solve();
    }
}

