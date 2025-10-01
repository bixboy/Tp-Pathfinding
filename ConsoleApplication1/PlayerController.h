#pragma once
#include <SDL_render.h>
#include <set>
#include <memory>
#include "CVect.h"

class Unit;
class Game;
class FlowFieldMap;

enum class MoveMode
{
	aStar,
	FlowField
};

class PlayerController
{
protected:
	//selection
	bool selectionInProgress;
	SDL_Rect selectionRect;
	std::set< std::shared_ptr<Unit>> selectedUnits;
	SDL_Texture* textTexture = nullptr;
	bool textTextureDirty = true;
	MoveMode moveMode = MoveMode::aStar;


	std::shared_ptr<FlowFieldMap> testMap;

	void UnselectAllUnits();
	void SelectUnitAtMousePos(int x, int y);

	void SelectUnitsInRect(const SDL_Rect& rect);
	std::shared_ptr<Game> game;

	void ComputePathUsingAStar(std::shared_ptr<Unit> unit, CVect to);
	void RebuildText(SDL_Renderer* screenRenderer);
public:
	void Init(std::shared_ptr<Game> game);
	void Update(int deltaTime);

	void Render(SDL_Renderer* screenRenderer);

	void SelectUnit(std::shared_ptr<Unit> unit);

	PlayerController() = default;
	~PlayerController();
};

