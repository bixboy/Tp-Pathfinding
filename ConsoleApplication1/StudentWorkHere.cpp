#include <array>

#include "FlowFieldMap.h"
#include "PlayerController.h"
#include "Map.h"
#include "Game.h"
#include "Unit.h"

#include <memory>

//Helper function that force a render followed by a small delay. You can use it to display a step by step run of your maze algorithm
void DebugRender();


void Game::GenerateMazeUsingDFS() 
{
    auto map = GetMap();
    if (!map)
        return;
	
    const int width = map->GetWidth();
    const int height = map->GetHeight();
    if (width <= 0 || height <= 0)
            return;

    struct DirectionStep
    {
        Direction direction;
        int deltaX;
        int deltaY;
    };

    const std::array<DirectionStep, 4> baseDirections{ {
        { Direction::North, 0, -1 },
        { Direction::South, 0, 1 },
        { Direction::East, 1, 0 },
        { Direction::West, -1, 0 },
    } };

    std::vector visited(static_cast<size_t>(height), std::vector(static_cast<size_t>(width), false));

    struct Cell
    {
        int x;
        int y;
    };

    std::vector<Cell> stack;
    stack.reserve(static_cast<size_t>(width * height));

    const int startX = rand() % width;
    const int startY = rand() % height;
    
    stack.push_back({ startX, startY });
    visited[static_cast<size_t>(startY)][static_cast<size_t>(startX)] = true;

    while (!stack.empty())
    {
        Cell current = stack.back();

        auto directions = baseDirections;
        for (int i = directions.size() - 1; i > 0; --i)
        {
            int j = rand() % (i + 1);
            std::swap(directions[i], directions[j]);
        }

        bool carved = false;
        for (const auto& step : directions)
        {
            const int nextX = current.x + step.deltaX;
            const int nextY = current.y + step.deltaY;

            if (nextX < 0 || nextX >= width || nextY < 0 || nextY >= height)
                continue;

            if (visited[static_cast<size_t>(nextY)][static_cast<size_t>(nextX)])
                continue;

            map->RemoveWall(current.x, current.y, step.direction);

            visited[static_cast<size_t>(nextY)][static_cast<size_t>(nextX)] = true;
            stack.push_back({ nextX, nextY });

            carved = true;

            DebugRender();
            break;
        }

        if (!carved)
            stack.pop_back();
    }
}

void PlayerController::ComputePathUsingAStar(std::shared_ptr<Unit> unit, CVect to)
{
	//This function computes the path from a unit to a destination using A*

	//Temp code, to be replaced by your own code
	std::queue<CVect> path;
	path.push(to);
	unit->SetPath(path);
}

void FlowFieldMap::GenerateFlowField(std::shared_ptr<Map> sourceMap, CVect dest)
{

	ResetMap(sourceMap->GetWidth(), sourceMap->GetHeight());

	//1st step: generate a distance map
	//uses a dijkstra to build the distance map (except we do it until all tiles are visited) => distanceMap variable
	//2nd step: generate the flow field => flowField variable
	//for each tile, find the lowest distance neighbor
	
}
