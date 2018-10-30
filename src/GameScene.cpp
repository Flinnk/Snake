#include <GameScene.h>

void GameScene::Enter()
{
	CurrentDirection = MovementDirection::NONE;

	CurrentBodyPartsCount = 1;

	Score = 0;

	TileSize = Vector3(WINDOW_WIDTH / TILE_MAP_COLLUMNS, WINDOW_HEIGHT / TILE_MAP_ROWS, 0);
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

	ResetGame(TileMap, CurrentDirection, SnakeBodyParts, CurrentBodyPartsCount);

	MovementDelay = 0.1f;
	MovementCounter = 0.0f;
}

void GameScene::Exit()
{

}

SceneIdentifier GameScene::Update(float ElapsedTime, Input& InputManager, Renderer& Render)
{
	//Get Direction
	Vector3 NewPosition = Vector3();
	if (InputManager.Get(InputKeys::LEFT) && CurrentDirection != MovementDirection::RIGHT)
		CurrentDirection = MovementDirection::LEFT;
	else if (InputManager.Get(InputKeys::RIGHT) && CurrentDirection != MovementDirection::LEFT)
		CurrentDirection = MovementDirection::RIGHT;

	if (InputManager.Get(InputKeys::DOWN) && CurrentDirection != MovementDirection::UP)
		CurrentDirection = MovementDirection::DOWN;
	else if (InputManager.Get(InputKeys::UP) && CurrentDirection != MovementDirection::DOWN)
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
			ResetGame(TileMap, CurrentDirection, SnakeBodyParts, CurrentBodyPartsCount);
		}
		else if (TargetPositionValue == TileMapValue::APPLE)
		{

			if (CurrentBodyPartsCount < SNAKE_BODY_TOTAL)//Increase snake size
			{
				SnakeBodyParts[CurrentBodyPartsCount] = SnakeBodyParts[CurrentBodyPartsCount - 1];
				++CurrentBodyPartsCount;
			}

			MoveSnake(TileMap, SnakeBodyParts, CurrentBodyPartsCount, TargetPosition);

			Score += 100;

			TileMapCoordinate ApplePosition = GetRandomPositionForApple(TileMap);
			TileMap[ApplePosition.Column][ApplePosition.Row] = TileMapValue::APPLE;

		}
		else if (TargetPositionValue == TileMapValue::FLOOR)
		{
			MoveSnake(TileMap, SnakeBodyParts, CurrentBodyPartsCount, TargetPosition);
		}
		else//BODY
		{
			ResetGame(TileMap, CurrentDirection, SnakeBodyParts, CurrentBodyPartsCount);
		}

		MovementCounter = 0.0f;
	}

	RenderGame(Render, TileSize, TileMap);

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

void GameScene::ResetGame(TileMapValue TileMap[TILE_MAP_COLLUMNS][TILE_MAP_ROWS], MovementDirection& CurrentDirection, TileMapCoordinate SnakeBodyParts[SNAKE_BODY_TOTAL], unsigned int& CurrentBodyPartsCount)
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
}

void GameScene::RenderGame(Renderer& Renderer, Vector3& TileSize, TileMapValue TileMap[TILE_MAP_COLLUMNS][TILE_MAP_ROWS])
{

	for (int Column = 0; Column < TILE_MAP_COLLUMNS; ++Column)
	{
		for (int Row = 0; Row < TILE_MAP_ROWS; ++Row)
		{
			TileMapValue TileValue = TileMap[Column][Row];
			Vector3 TilePosition(Column*TileSize.X, WINDOW_HEIGHT - TileSize.Y - Row * TileSize.Y, 0);
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

			Renderer.DrawRectangle(TilePosition, TileSize, Offset, Color);
		}
	}


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