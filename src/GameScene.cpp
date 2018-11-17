#pragma warning(disable:4996)

#include <GameScene.h>
#include <cstdio>

void GameScene::Enter()
{
	Font = GetEngine()->ResourceManager.RetrieveFont("Boxy-Bold.ttf");

	CurrentDirection = MovementDirection::NONE;
	bGameOver = false;
	CurrentBodyPartsCount = 1;

	TileSize = Vector3(WINDOW_WIDTH / TILE_MAP_COLLUMNS, GAME_HEIGHT / TILE_MAP_ROWS, 0);
	for (int Column = 0; Column < TILE_MAP_COLLUMNS; ++Column)
	{
		for (int Row = 0; Row < TILE_MAP_ROWS; ++Row)
		{
			if (Row == 0 || Column == 0 || Row == TILE_MAP_ROWS - 1 || Column == TILE_MAP_COLLUMNS - 1)
				TileMap[Column][Row] = TileMapValue::WALL;
			else
				TileMap[Column][Row] = TileMapValue::FLOOR;
		}
	}

	ResetGame(true, TileMap, CurrentDirection, SnakeBodyParts, CurrentBodyPartsCount);
}

void GameScene::Exit()
{
}

SceneIdentifier GameScene::Update(float ElapsedTime)
{
	CInput& InputManager = GetEngine()->Input;

	if (bGameOver)
		return SceneIdentifier::INTRO;

	//Get Direction
	Vector3 NewPosition = Vector3();
	if (InputManager.Get(EInputKeys::LEFT) && CurrentDirection != MovementDirection::RIGHT)
		CurrentDirection = MovementDirection::LEFT;
	else if (InputManager.Get(EInputKeys::RIGHT) && CurrentDirection != MovementDirection::LEFT)
		CurrentDirection = MovementDirection::RIGHT;

	if (InputManager.Get(EInputKeys::DOWN) && CurrentDirection != MovementDirection::UP)
		CurrentDirection = MovementDirection::DOWN;
	else if (InputManager.Get(EInputKeys::UP) && CurrentDirection != MovementDirection::DOWN)
		CurrentDirection = MovementDirection::UP;

	//Execute movement at certain spped
	MovementCounter += ElapsedTime;
	if (CurrentDirection != MovementDirection::NONE && MovementCounter >= MovementDelay)
	{
		TileMapCoordinate TargetPosition = SnakeBodyParts[0];
		switch (CurrentDirection)
		{

		case MovementDirection::UP:
			TargetPosition.Row -= 1;
			break;
		case MovementDirection::DOWN:
			TargetPosition.Row += 1;
			break;
		case MovementDirection::LEFT:
			TargetPosition.Column -= 1;
			break;
		case MovementDirection::RIGHT:
			TargetPosition.Column += 1;
			break;
		}

		TileMapValue TargetPositionValue = TileMap[TargetPosition.Column][TargetPosition.Row];
		if (TargetPositionValue == TileMapValue::WALL)
		{
			ResetGame(false, TileMap, CurrentDirection, SnakeBodyParts, CurrentBodyPartsCount);
		}
		else if (TargetPositionValue == TileMapValue::APPLE)
		{

			if (CurrentBodyPartsCount < SNAKE_BODY_TOTAL)//Increase snake size
			{
				SnakeBodyParts[CurrentBodyPartsCount] = SnakeBodyParts[CurrentBodyPartsCount - 1];
				++CurrentBodyPartsCount;
			}

			MoveSnake(TileMap, SnakeBodyParts, CurrentBodyPartsCount, TargetPosition);

			Score = min(MAX_SCORE, Score + (APPLE_BASE_POINTS * CurrentBodyPartsCount));
			MovementDelay = max(MIN_MOVEMENT_DELAY, (MovementDelay - MOVEMENT_STEP));
			TileMapCoordinate ApplePosition = GetRandomPositionForApple(TileMap);
			TileMap[ApplePosition.Column][ApplePosition.Row] = TileMapValue::APPLE;

		}
		else if (TargetPositionValue == TileMapValue::FLOOR)
		{
			MoveSnake(TileMap, SnakeBodyParts, CurrentBodyPartsCount, TargetPosition);
		}
		else//BODY
		{
			ResetGame(false, TileMap, CurrentDirection, SnakeBodyParts, CurrentBodyPartsCount);
		}

		MovementCounter = 0.0f;
	}

	RenderGame(GetEngine()->Renderer, TileSize, TileMap);

	return SceneIdentifier::GAME;
}

TileMapCoordinate GameScene::GetRandomPositionForApple(TileMapValue TileMap[TILE_MAP_COLLUMNS][TILE_MAP_ROWS])
{
	TileMapCoordinate CandidateCoordinates[TILE_MAP_ROWS*TILE_MAP_COLLUMNS];
	unsigned int CandidateCount = 0;

	for (int Column = 0; Column < TILE_MAP_COLLUMNS; ++Column)
	{
		for (int Row = 0; Row < TILE_MAP_ROWS; ++Row)
		{
			if (TileMap[Column][Row] == TileMapValue::FLOOR)
			{
				CandidateCoordinates[CandidateCount] = TileMapCoordinate(Row, Column);
				++CandidateCount;
			}
		}
	}

	unsigned int RandomValue = rand() % (CandidateCount - 1);
	return CandidateCoordinates[RandomValue];
}

void GameScene::ResetGame(bool NewGame, TileMapValue TileMap[TILE_MAP_COLLUMNS][TILE_MAP_ROWS], MovementDirection& CurrentDirection, TileMapCoordinate SnakeBodyParts[SNAKE_BODY_TOTAL], unsigned int& CurrentBodyPartsCount)
{
	for (int Column = 0; Column < TILE_MAP_COLLUMNS; ++Column)
	{
		for (int Row = 0; Row < TILE_MAP_ROWS; ++Row)
		{
			if (Row == 0 || Column == 0 || Row == TILE_MAP_ROWS - 1 || Column == TILE_MAP_COLLUMNS - 1)
				TileMap[Column][Row] = TileMapValue::WALL;
			else
				TileMap[Column][Row] = TileMapValue::FLOOR;
		}
	}
	CurrentDirection = MovementDirection::NONE;
	TileMapCoordinate InitialSnakePosition;
	InitialSnakePosition.Row = TILE_MAP_ROWS / 2;
	InitialSnakePosition.Column = TILE_MAP_COLLUMNS / 2;
	SnakeBodyParts[0] = InitialSnakePosition;
	TileMap[InitialSnakePosition.Column][InitialSnakePosition.Row] = TileMapValue::HEAD;

	TileMapCoordinate ApplePosition = GetRandomPositionForApple(TileMap);
	TileMap[ApplePosition.Column][ApplePosition.Row] = TileMapValue::APPLE;

	CurrentBodyPartsCount = 1;
	MovementDelay = START_MOVEMENT_DELAY;
	MovementCounter = 0.0f;

	if (NewGame)
	{
		Score = 0;
		Lives = LIVES;
	}
	else
	{
		if (Lives > 0)
			Lives--;
		else
			bGameOver = true;
	}
}

void GameScene::RenderGame(CRenderer& Renderer, Vector3& TileSize, TileMapValue TileMap[TILE_MAP_COLLUMNS][TILE_MAP_ROWS])
{

	for (int Column = 0; Column < TILE_MAP_COLLUMNS; ++Column)
	{
		for (int Row = 0; Row < TILE_MAP_ROWS; ++Row)
		{
			TileMapValue TileValue = TileMap[Column][Row];
			Vector3 TilePosition(Column*TileSize.X, GAME_HEIGHT - TileSize.Y - Row * TileSize.Y, 0);
			Vector4 Color;
			Vector3 Offset(0, 0, 0);
			switch (TileValue)
			{
			case TileMapValue::FLOOR:
				Color = Vector4(0.0, 0.0, 0.0, 1.0);
				break;
			case TileMapValue::WALL:
				Color = Vector4(0.23, 0.15, 0.95, 1.0);
				break;
			case TileMapValue::HEAD:
				Color = Vector4(0, 1, 0, 1.0);
				break;
			case TileMapValue::BODY:
				Color = Vector4(0, 1, 0, 1.0);
				Offset = TileSize * 0.2f;
				break;
			case TileMapValue::APPLE:
				Color = Vector4(1, 0, 0, 1.0);
				Offset = TileSize * 0.3f;
				break;
			default:
				Color = Vector4(1.0, 0.0, 1.0, 1.0);
			}

			Renderer.DrawSprite(TilePosition, TileSize, Offset, Color);
		}
	}

	Renderer.DrawSprite(Vector3(0, GAME_HEIGHT, 0), Vector3(WINDOW_WIDTH, 50, 0), Vector3(0, 0, 0), Vector4(0.25, 0.25, 0.25, 1));

	char Buffer[256];
	sprintf(Buffer, "Score:%d", Score);
	int t = strlen(Buffer);
	Renderer.DrawTextExt(Font, Buffer, Vector3(WINDOW_WIDTH - 40 * t, GAME_HEIGHT, 0), Vector3(40, 50, 0), Vector3(0, 0, 0), Vector4(0, 1, 0, 1));
	sprintf(Buffer, "Lives:%d", Lives);
	t = strlen(Buffer);
	Renderer.DrawTextExt(Font, Buffer, Vector3(0, GAME_HEIGHT, 0), Vector3(40, 50, 0), Vector3(0, 0, 0), Vector4(0, 1, 0, 1));

}

void GameScene::MoveSnake(TileMapValue TileMap[TILE_MAP_COLLUMNS][TILE_MAP_ROWS], TileMapCoordinate SnakeBodyParts[SNAKE_BODY_TOTAL], unsigned int& CurrentBodyPartsCount, TileMapCoordinate TargetPosition)
{
	TileMapCoordinate PreviousCoordinate = SnakeBodyParts[0];
	for (int Index = 0; Index < CurrentBodyPartsCount; ++Index)
	{
		if (Index == 0)
		{
			TileMap[SnakeBodyParts[Index].Column][SnakeBodyParts[Index].Row] = TileMapValue::FLOOR;
			SnakeBodyParts[Index] = TargetPosition;
			TileMap[SnakeBodyParts[Index].Column][SnakeBodyParts[Index].Row] = TileMapValue::HEAD;
		}
		else
		{
			TileMap[SnakeBodyParts[Index].Column][SnakeBodyParts[Index].Row] = TileMapValue::FLOOR;
			TileMapCoordinate TmpCoordinate = SnakeBodyParts[Index];
			SnakeBodyParts[Index] = PreviousCoordinate;
			PreviousCoordinate = TmpCoordinate;
			TileMap[SnakeBodyParts[Index].Column][SnakeBodyParts[Index].Row] = TileMapValue::BODY;
		}
	}
}