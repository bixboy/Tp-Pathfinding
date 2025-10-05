#include <array>
#include <cstdlib>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

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
    auto currentMap = GetMap();
    if (!currentMap)
        return;
	
    const int width = currentMap->GetWidth();
    const int height = currentMap->GetHeight();
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

    std::vector visited(static_cast<size_t>(height), std::vector<bool>(static_cast<size_t>(width), false));

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
        for (int i = static_cast<int>(directions.size()) - 1; i > 0; --i)
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

            currentMap->RemoveWall(current.x, current.y, step.direction);

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
    if (unit == nullptr || game == nullptr)
        return;

    auto map = game->GetMap();
    if (map == nullptr)
        return;

    const int width = map->GetWidth();
    const int height = map->GetHeight();
    if (width <= 0 || height <= 0)
        return;

    const int targetX = static_cast<int>(to.x);
    const int targetY = static_cast<int>(to.y);
    if (targetX < 0 || targetX >= width || targetY < 0 || targetY >= height)
    {
        unit->SetPath(std::queue<CVect>());
        return;
    }

    const CVect unitTile = unit->GetTile();
    const int startX = static_cast<int>(unitTile.x);
    const int startY = static_cast<int>(unitTile.y);

    if (startX < 0 || startX >= width || startY < 0 || startY >= height)
    {
        unit->SetPath(std::queue<CVect>());
        return;
    }

    if (startX == targetX && startY == targetY)
    {
        unit->SetPath(std::queue<CVect>());
        return;
    }

    const unsigned int kInfinity = std::numeric_limits<unsigned int>::max();
    std::vector gScore(static_cast<size_t>(height), std::vector(static_cast<size_t>(width), kInfinity));
    std::vector closed(static_cast<size_t>(height), std::vector(static_cast<size_t>(width), false));
    
    struct Parent
    {
        int x = -1;
        int y = -1;
    };
    std::vector parents(static_cast<size_t>(height), std::vector<Parent>(static_cast<size_t>(width)));
    
    struct Node
    {
        unsigned int fScore;
        int x;
        int y;
    };

    struct NodeCompare
    {
        bool operator()(const Node& lhs, const Node& rhs) const
        {
            return lhs.fScore > rhs.fScore;
        }
    };

    auto heuristic = [targetX, targetY](int x, int y) -> unsigned int
    {
        return static_cast<unsigned int>(std::abs(x - targetX) + std::abs(y - targetY));
    };

    std::priority_queue<Node, std::vector<Node>, NodeCompare> openSet;

    gScore[static_cast<size_t>(startY)][static_cast<size_t>(startX)] = 0;
    openSet.push(Node{ heuristic(startX, startY), startX, startY });

    const std::array<std::pair<int, int>, 4> neighborOffsets{ {
        { 0, -1 },
        { 0, 1 },
        { 1, 0 },
        { -1, 0 }
    } };

    bool found = false;

    while (!openSet.empty())
    {
        Node current = openSet.top();
        openSet.pop();

        if (closed[static_cast<size_t>(current.y)][static_cast<size_t>(current.x)])
            continue;

        closed[static_cast<size_t>(current.y)][static_cast<size_t>(current.x)] = true;

        if (current.x == targetX && current.y == targetY)
        {
            found = true;
            break;
        }

        for (const auto& offset : neighborOffsets)
        {
            const int neighborX = current.x + offset.first;
            const int neighborY = current.y + offset.second;

            if (neighborX < 0 || neighborX >= width || neighborY < 0 || neighborY >= height)
                continue;

            if (!map->CanMoveBetween(current.x, current.y, neighborX, neighborY))
                continue;

            if (closed[static_cast<size_t>(neighborY)][static_cast<size_t>(neighborX)])
                continue;

            const unsigned int tentativeG = gScore[static_cast<size_t>(current.y)][static_cast<size_t>(current.x)] + 1u;
            if (tentativeG >= gScore[static_cast<size_t>(neighborY)][static_cast<size_t>(neighborX)])
                continue;

            gScore[static_cast<size_t>(neighborY)][static_cast<size_t>(neighborX)] = tentativeG;
            parents[static_cast<size_t>(neighborY)][static_cast<size_t>(neighborX)] = Parent{ current.x, current.y };
            const unsigned int fScore = tentativeG + heuristic(neighborX, neighborY);
            openSet.push(Node{ fScore, neighborX, neighborY });
        }
    }

    if (found)
    {
        int currentX = targetX;
        int currentY = targetY;

        std::vector<CVect> reversePath;
        unsigned int distanceToGoal = gScore[static_cast<size_t>(targetY)][static_cast<size_t>(targetX)];
        if (distanceToGoal == kInfinity)
            distanceToGoal = 0;
        
        reversePath.reserve(distanceToGoal + 1u);

        bool rebuilt = true;
        while (!(currentX == startX && currentY == startY))
        {
            reversePath.emplace_back(static_cast<float>(currentX), static_cast<float>(currentY), 0.0f);
            const Parent& parent = parents[static_cast<size_t>(currentY)][static_cast<size_t>(currentX)];
            if (parent.x == -1 || parent.y == -1)
            {
                rebuilt = false;
                break;
            }
            
            currentX = parent.x;
            currentY = parent.y;
        }

        if (rebuilt)
        {
            std::queue<CVect> orderedPath;
            for (auto it = reversePath.rbegin(); it != reversePath.rend(); ++it)
            {
                orderedPath.push(*it);
            }
            
            unit->SetPath(orderedPath);
            return;
        }
    }

    unit->SetPath(std::queue<CVect>());
}

void FlowFieldMap::GenerateFlowField(std::shared_ptr<Map> sourceMap, CVect dest)
{
        if (sourceMap == nullptr)
            return;

        const int width = sourceMap->GetWidth();
        const int height = sourceMap->GetHeight();
        if (width <= 0 || height <= 0)
            return;

        ResetMap(width, height);

        const int destX = static_cast<int>(dest.x);
        const int destY = static_cast<int>(dest.y);
        if (destX < 0 || destX >= width || destY < 0 || destY >= height)
            return;

        const unsigned int kInfinity = std::numeric_limits<unsigned int>::max();

        struct QueueNode
        {
            unsigned int distance;
            int x;
            int y;
        };

        struct Compare
        {
            bool operator()(const QueueNode& lhs, const QueueNode& rhs) const
            {
                return lhs.distance > rhs.distance;
            }
        };

        std::priority_queue<QueueNode, std::vector<QueueNode>, Compare> queue;

        distanceMap[static_cast<size_t>(destY)][static_cast<size_t>(destX)] = 0;
        queue.push(QueueNode{ 0u, destX, destY });

        const std::array<std::pair<int, int>, 4> neighborOffsets{ {
            { 0, -1 },
            { 0, 1 },
            { 1, 0 },
            { -1, 0 }
        } };

        while (!queue.empty())
        {
            QueueNode current = queue.top();
            queue.pop();

            if (current.distance != distanceMap[static_cast<size_t>(current.y)][static_cast<size_t>(current.x)])
                continue;

            for (const auto& offset : neighborOffsets)
            {
                const int neighborX = current.x + offset.first;
                const int neighborY = current.y + offset.second;

                if (neighborX < 0 || neighborX >= width || neighborY < 0 || neighborY >= height)
                    continue;

                if (!sourceMap->CanMoveBetween(current.x, current.y, neighborX, neighborY))
                    continue;

                const unsigned int tentativeDistance = current.distance + 1u;
                if (tentativeDistance < distanceMap[static_cast<size_t>(neighborY)][static_cast<size_t>(neighborX)])
                {
                    distanceMap[static_cast<size_t>(neighborY)][static_cast<size_t>(neighborX)] = tentativeDistance;
                    queue.push(QueueNode{ tentativeDistance, neighborX, neighborY });
                }
            }
        }

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                if (distanceMap[static_cast<size_t>(y)][static_cast<size_t>(x)] == kInfinity)
                {
                    flowField[static_cast<size_t>(y)][static_cast<size_t>(x)] = CVect(0.0f, 0.0f, 0.0f);
                    continue;
                }

                if (x == destX && y == destY)
                {
                    flowField[static_cast<size_t>(y)][static_cast<size_t>(x)] = CVect(0.0f, 0.0f, 0.0f);
                    continue;
                }

                unsigned int bestDistance = distanceMap[static_cast<size_t>(y)][static_cast<size_t>(x)];
                CVect bestDirection(0.0f, 0.0f, 0.0f);

                for (const auto& offset : neighborOffsets)
                {
                    const int neighborX = x + offset.first;
                    const int neighborY = y + offset.second;

                    if (neighborX < 0 || neighborX >= width || neighborY < 0 || neighborY >= height)
                        continue;

                    if (!sourceMap->CanMoveBetween(x, y, neighborX, neighborY))
                        continue;

                    const unsigned int neighborDistance = distanceMap[static_cast<size_t>(neighborY)][static_cast<size_t>(neighborX)];
                    if (neighborDistance < bestDistance)
                    {
                        bestDistance = neighborDistance;
                        bestDirection = CVect(static_cast<float>(neighborX - x), static_cast<float>(neighborY - y), 0.0f);
                    }
                }

                flowField[static_cast<size_t>(y)][static_cast<size_t>(x)] = bestDirection;
            }
        }
}
