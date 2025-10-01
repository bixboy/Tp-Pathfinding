#include "Map.h"
#include <random>

Map::~Map()
{
	Release();
}

std::shared_ptr<Map> Map::Create()
{
	std::shared_ptr<Map> newShared = std::make_shared<Map>();
	newShared->thisWeakPtr = newShared;
	return newShared;
}

std::shared_ptr<IGameEntity> Map::GetSharedPtr()
{
	return thisWeakPtr.lock();
}

void Map::Init(const IGameEntityConfig* config)
{
	spriteSheet = SpriteFactory::GetSprite("Basis.png");
	width = 1280 / TILE_SIZE;
	height = 720 / TILE_SIZE;
	tileset = new std::shared_ptr<Tile>*[height];
	//generate a random map
	for (int i = 0; i < height; ++i)
	{
		tileset[i] = new std::shared_ptr<Tile>[width];
		for (int j = 0; j < width; ++j)
		{
			tileset[i][j] = std::make_shared<Tile>(j, i);
		}
	}

}

void Map::Release()
{
	if (tileset != nullptr)
	{
		for (int i = 0; i < height; ++i)
		{
			delete[] tileset[i];
		}
		delete[] tileset;
		tileset = nullptr;
	}
}

void Map::Update(int deltaTime)
{
}

void Map::Render(SDL_Renderer* screenRenderer)
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			tileset[i][j]->Render(screenRenderer, spriteSheet.get(), j * TILE_SIZE + (TILE_SIZE >> 1), i * TILE_SIZE + (TILE_SIZE >> 1));
		}
	}
}

bool Map::CanMoveInDirection(int x, int y, Direction direction)
{
	if (x < 0 || y < 0 || x >= width || y >= height)
	{
		return false;
	}
	return tileset[y][x]->CanMove(direction);
}

bool Map::CanMoveBetween(int x1, int y1, int x2, int y2)
{
	if (x1 < 0 || y1 < 0 || x1 >= width || y1 >= height)
		return false;
	if (x2 < 0 || y2 < 0 || x2 >= width || y2 >= height)
		return false;
	if(x1 < x2 && !CanMoveInDirection(x1, y1, Direction::East))
		return false;
	if (x1 > x2 && !CanMoveInDirection(x1, y1, Direction::West))
		return false;
	if (y1 < y2 && !CanMoveInDirection(x1, y1, Direction::South))
		return false;
	if (y1 > y2 && !CanMoveInDirection(x1, y1, Direction::North))
		return false;
	return true;
}

bool Map::CanMoveBetween(float x1, float y1, float x2, float y2)
{
	return CanMoveBetween((int)x1, (int)y1, (int)x2, (int)y2);
}

bool Map::AddWall(int x, int y, Direction direction)
{
	if (x < 0 || y < 0 || x >= width || y >= height)
	{
		return false;
	}
	if(x == 0 && direction & Direction::West) 
		return false;
	if (x == width - 1 && direction & Direction::East)
		return false;
	if (y == 0 && direction & Direction::North)
		return false;
	if (y == height - 1 && direction & Direction::South)
		return false;

	tileset[y][x]->AddWall(direction);
	if(direction & Direction::North)
		tileset[y - 1][x]->AddWall(Direction::South);
	if (direction & Direction::South)
		tileset[y + 1][x]->AddWall(Direction::North);
	if (direction & Direction::East)
		tileset[y][x + 1]->AddWall(Direction::West);
	if (direction & Direction::West)
		tileset[y][x - 1]->AddWall(Direction::East);
	return true;
}

bool Map::RemoveWall(int x, int y, Direction direction)
{
	if (x < 0 || y < 0 || x >= width || y >= height)
	{
		return false;
	}
	if (x == 0 && direction & Direction::West)
		return false;
	if (x == width - 1 && direction & Direction::East)
		return false;
	if (y == 0 && direction & Direction::North)
		return false;
	if (y == height - 1 && direction & Direction::South)
		return false;

	tileset[y][x]->RemoveWall(direction);
	if (direction & Direction::North)
		tileset[y - 1][x]->RemoveWall(Direction::South);
	if (direction & Direction::South)
		tileset[y + 1][x]->RemoveWall(Direction::North);
	if (direction & Direction::East)
		tileset[y][x + 1]->RemoveWall(Direction::West);
	if (direction & Direction::West)
		tileset[y][x - 1]->RemoveWall(Direction::East);
	return true;
}
