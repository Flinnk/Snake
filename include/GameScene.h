#pragma once

#include "Scene.h"
#include "Constants.h"
#include "Transition.h"

enum class GameButton : unsigned int
{
	PLAY,
	EXIT
};

enum class TileMapValue
{
	FLOOR,
	WALL,
	HEAD,
	BODY,
	APPLE
};

enum class MovementDirection
{
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct TileMapCoordinate
{
	TileMapCoordinate() :Row(0), Column(0) {}

	TileMapCoordinate(unsigned int RowParam, unsigned int ColumnParam) :Row(RowParam), Column(ColumnParam) {}
	unsigned int Row;
	unsigned int Column;
};


class GameScene : public Scene
{
public:
	GameScene()
	{
		MovementDelay = 0.1f;
		MovementCounter = 0.0f;
		Score = 0;
		CurrentDirection = MovementDirection::NONE;
		CurrentBodyPartsCount = 1;
		TileSize = Vector3(WINDOW_WIDTH / TILE_MAP_COLLUMNS, WINDOW_HEIGHT / TILE_MAP_ROWS, 0);
	}

	void Enter();
	void Exit();
	SceneIdentifier Update(float ElapsedTime);
private:

	SceneIdentifier UpdateGame(float ElapsedTime);
	SceneIdentifier UpdateGameOver(float ElapsedTime);

	void ResetGame(bool NewGame, TileMapValue TileMap[TILE_MAP_COLLUMNS][TILE_MAP_ROWS], MovementDirection& CurrentDirection, TileMapCoordinate SnakeBodyParts[SNAKE_BODY_TOTAL], unsigned int& CurrentBodyPartsCount);
	TileMapCoordinate GetRandomPositionForApple(TileMapValue TileMap[TILE_MAP_COLLUMNS][TILE_MAP_ROWS]);
	void RenderGame(CRenderer& Renderer, Vector3& TileSize, TileMapValue TileMap[TILE_MAP_COLLUMNS][TILE_MAP_ROWS]);
	void MoveSnake(TileMapValue TileMap[TILE_MAP_COLLUMNS][TILE_MAP_ROWS], TileMapCoordinate SnakeBodyParts[SNAKE_BODY_TOTAL], unsigned int& CurrentBodyPartsCount, TileMapCoordinate TargetPosition);

	float MovementDelay;
	float MovementCounter;
	unsigned int Score;
	unsigned int Lives;
	bool bGameOver;
	MovementDirection CurrentDirection;
	Vector3 Direction = Vector3(0, 0, 0);
	TileMapCoordinate SnakeBodyParts[SNAKE_BODY_TOTAL];
	unsigned int CurrentBodyPartsCount;
	Vector3 TileSize;
	TileMapValue TileMap[TILE_MAP_COLLUMNS][TILE_MAP_ROWS];
	CAudioClip* PickUpSound;
	CAudioClip* DeathSound;
	CAudioClip* Music;
	Transition<float> AlphaAnimation;
	GameButton CurrentButton;
};