#pragma once
#include "IGameEntity.h"
#include "Tile.h"
#include <map>
#include "SpriteFactory.h"

class Map :
	public IGameEntity
{
protected:

	std::weak_ptr<Map> thisWeakPtr;

	std::shared_ptr<Tile>** tileset = nullptr;
	std::shared_ptr<Sprite> spriteSheet = nullptr;

	int width;
	int height;

public:
	Map() = default;
	virtual ~Map();
	Map(const Map&) = delete;
	Map(Map&&) = delete;
	Map& operator=(const Map&) = delete;

	static std::shared_ptr<Map> Create();

	virtual std::shared_ptr<IGameEntity> GetSharedPtr();

	virtual void Init(const IGameEntityConfig* config);

	virtual void Release();

	virtual void Update(int deltaTime);

	virtual void Render(SDL_Renderer* screenRenderer);

	bool CanMoveInDirection(int x, int y, Direction direction);
	bool CanMoveBetween(int x1, int y1, int x2, int y2);
	bool CanMoveBetween(float x1, float y1, float x2, float y2);
	bool AddWall(int x, int y, Direction direction);
	bool RemoveWall(int x, int y, Direction direction);

	inline int GetWidth() { return width; }
	inline int GetHeight() { return height; }
	inline std::shared_ptr<Tile> GetTile(int x, int y) {
		if (x < 0 || x >= width || y < 0 || y >= height)
			return nullptr;
		return tileset[y][x]; 
	}
};

