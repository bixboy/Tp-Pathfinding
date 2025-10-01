#pragma once
#include "Sprite.h"
#include <memory>
#include <map>
#include <cassert>

#define TILE_SIZE 32

enum Direction
{
    None = 0,
    North = 1,
    South = 2,
    East = 4,
    West = 8,
    All = 15
};

struct TileConfig
{
    int direction;
    SDL_Rect renderRect;

    TileConfig(int _direction, int _posXOnSheet, int _posYOnSheet, int _tileWidth, int _tileHeight)
    {
        direction = _direction;
        renderRect = SDL_Rect{ _posXOnSheet * _tileWidth , _posYOnSheet * _tileHeight , _tileWidth , _tileHeight };
    }

    void Render(SDL_Renderer* screenRenderer, Sprite* tileset, int posX, int posY)
    {
        tileset->Render(screenRenderer, posX, posY, renderRect);
    }
};

class Tile 
{
protected:

    inline static std::map<int, std::shared_ptr<TileConfig>> tileConfigs
    {
        //No wall
        { Direction::None, std::make_shared<TileConfig>(Direction::None, 1,2,TILE_SIZE,TILE_SIZE)},
        //1 wall
        { Direction::North, std::make_shared<TileConfig>(Direction::North, 1,1,TILE_SIZE,TILE_SIZE) },
        { Direction::South, std::make_shared<TileConfig>(Direction::South, 1,3,TILE_SIZE,TILE_SIZE) },
        { Direction::East, std::make_shared<TileConfig>(Direction::East, 2,2,TILE_SIZE,TILE_SIZE) },
        { Direction::West, std::make_shared<TileConfig>(Direction::West, 0,2,TILE_SIZE,TILE_SIZE) },
        //2 walls
        { Direction::North | Direction::South, std::make_shared<TileConfig>(Direction::North | Direction::South, 1,4,TILE_SIZE,TILE_SIZE) },
        { Direction::North | Direction::East, std::make_shared<TileConfig>(Direction::North | Direction::East, 2,1,TILE_SIZE,TILE_SIZE) },
        { Direction::North | Direction::West, std::make_shared<TileConfig>(Direction::North | Direction::West, 0,1,TILE_SIZE,TILE_SIZE) },
        { Direction::South | Direction::East, std::make_shared<TileConfig>(Direction::South | Direction::East, 2,3,TILE_SIZE,TILE_SIZE) },
        { Direction::South | Direction::West, std::make_shared<TileConfig>(Direction::South | Direction::West, 0,3,TILE_SIZE,TILE_SIZE) },
        { Direction::East | Direction::West, std::make_shared<TileConfig>(Direction::East | Direction::West, 2,4,TILE_SIZE,TILE_SIZE) },
        //3 walls
        { Direction::North | Direction::South | Direction::East, std::make_shared<TileConfig>(Direction::North | Direction::South | Direction::East, 3,3,TILE_SIZE,TILE_SIZE) },
        { Direction::North | Direction::South | Direction::West, std::make_shared<TileConfig>(Direction::North | Direction::South | Direction::East, 3,2,TILE_SIZE,TILE_SIZE) },
        { Direction::West | Direction::South | Direction::East, std::make_shared<TileConfig>(Direction::West | Direction::South | Direction::East, 3,1,TILE_SIZE,TILE_SIZE) },
        { Direction::North | Direction::West | Direction::East, std::make_shared<TileConfig>(Direction::North | Direction::South | Direction::East, 3,0,TILE_SIZE,TILE_SIZE) },
        //All
        { Direction::All, std::make_shared<TileConfig>(Direction::All, 0,4,TILE_SIZE,TILE_SIZE) },
    };

    std::shared_ptr<TileConfig> type;
    int walls = Direction::All;
public:
	Tile() = delete;
    Tile(int posX, int posY) : type(tileConfigs[(int)Direction::All]) {};
	Tile(const Sprite&) = delete;
	Tile(Tile&&) = delete;
	Tile& operator=(const Tile&) = delete;
	~Tile() = default;

    void Render(SDL_Renderer* screenRenderer, Sprite* tileset, int posX, int posY)
    {
        type->Render(screenRenderer, tileset, posX, posY);
        posX -= TILE_SIZE / 2;
        posY -= TILE_SIZE / 2;
    };

    inline bool CanMove(Direction direction)
    {
        return (direction & walls) == 0;
    }

    inline void AddWall(Direction direction)
    {
        walls |= direction;
        assert(tileConfigs.find(walls) != tileConfigs.end());
        type = tileConfigs[walls];
    }

    inline void RemoveWall(Direction direction)
    {
        walls &= ~direction;
        assert(tileConfigs.find(walls) != tileConfigs.end());
        type = tileConfigs[walls];
    }
};

