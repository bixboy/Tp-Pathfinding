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
	std::shared_ptr<Map> NewMap = GetMap();
	
	int StartX = rand() % NewMap->GetWidth();
	int StartY = rand() % NewMap->GetHeight();

	std::shared_ptr<Tile> Tile = NewMap->GetTile(StartX, StartY);
	
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
