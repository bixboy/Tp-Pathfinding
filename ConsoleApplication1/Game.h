#pragma once
#include <SDL_render.h>
#include <set>
#include <memory>
#include <functional>
#include "CollisionManager.h"

#ifndef WIN32
#error WIN32 (x86) only due to dll not provided for x64
#endif


class IGameEntity;
class Map;
class Unit;
class PlayerController;

class Game
{
private:

    bool _isRunning;
	SDL_Renderer* _screenRenderer;

	std::set<std::shared_ptr<IGameEntity> > _entities;

	bool _delayEntityRemoval;
	std::set<std::shared_ptr<IGameEntity> > _entitiesToRemove;

	std::shared_ptr<Map> map;

	std::shared_ptr<PlayerController> playerController;
	std::weak_ptr<Game> thisWeakPtr;

	CollisionManager collisionManager;
public:
	Game() = default;
	~Game();
	Game(const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator=(const Game&) = delete;

	bool Init();
	void Release();

	bool Init(SDL_Renderer* screenRenderer);

	void Update(int deltaTime);

	void Render();

	bool IsRunning() const;

	void AddEntity(std::shared_ptr<IGameEntity>);
	void RemoveEntity(std::shared_ptr<IGameEntity>);

	std::shared_ptr<Unit> GetUnitAtPos(int x, int y, bool excludeSelected = false);
	void SelectUnitsInRect(const SDL_Rect& rect, std::set< std::shared_ptr<Unit>>& selectedUnits);
	void GenerateMazeUsingDFS();

	void Exit();

	static std::shared_ptr<Game> Create();

	inline std::shared_ptr<Map> GetMap() { return map; }
};

