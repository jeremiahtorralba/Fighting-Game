#include "GameState.h"
#include "Game.h"
#include "PauseMenuPopupState.h"
#include "GameManager.h"
void GameState::Enter()
{
//	bgSpriteTex = Game::Instance()->LoadTexture("Img/Levels/SF_Level_1.jpg");
	mainSpriteTex = Game::Instance()->LoadTexture("Img/Players/PlayerKenSprite2.png");
	mainSpriteTex2 = Game::Instance()->LoadTexture("Img/Players/EnemyKenSprite.png");

	m_pFont = TTF_OpenFont("Font/LTYPE.TTF", 30);

	SDL_Rect bgSrcRect;
	bgSrcRect.x = bgSrcRect.y = 0;
	SDL_QueryTexture(bgSpriteTex, NULL, NULL, &bgSrcRect.w, &bgSrcRect.h);

	SDL_Rect bgDestRect;
	bgDestRect.x = bgDestRect.y = 0;
	SDL_GetWindowSize(Game::Instance()->GetWindow(), &bgDestRect.w, &bgDestRect.h);

	bg = new SpriteEx(bgSpriteTex, bgSrcRect, bgDestRect);
	
	player = new Player(mainSpriteTex, bgDestRect.w*0.5, bgDestRect.h - 100);
	enemy = new Enemy(mainSpriteTex2, bgDestRect.w*0.6, bgDestRect.h - 100);

	player->spriteSrcRect = { 0,0,70,80 };	
	player->spriteDestRect = { (int)(player->m_X - 50),(int)(player->m_Y - 50)  ,70,80 };

	enemy->spriteSrcRect = { 0,0,70,80 };
	enemy->spriteDestRect = { (int)(enemy->m_X - 50),(int)(enemy->m_Y - 50)  ,70,80 };
}

void GameState::Update()
{
	if (Game::Instance()->KeyDown(SDL_SCANCODE_ESCAPE) == 1)
	{
		Game::Instance()->GetFSM()->PushState(new PauseMenuPopupState());
		return; //to make sure we don't run the game in background while pause menu shows
	}

	if (player) player->Update();

	if (player) this->CheckCollision(player->spriteDestRect, enemy->spriteDestRect);

	if (player) player->Render();

	if (enemy) enemy->Update();

	if (enemy) this->CheckCollision(enemy->spriteDestRect, player->spriteDestRect);	

	this->Render();	

	if (player->spriteDestRect.x < enemy->spriteDestRect.x) { // checks if flipped or not.
		// a. The enemy should be able to attack the player if player is within X distance of the enemy.
		if (enemy->spriteDestRect.x - 30 <= player->spriteDestRect.x + 30) {
			if (enemy) enemy->EnemyAttack();
			if (enemy) enemy->EnemyDefense();
		}
		if (enemy) enemy->EnemyHadouken();
		if (enemy) enemy->EnemyHadouDefense();
	}
	else {
		if (player->spriteDestRect.x - 30 <= enemy->spriteDestRect.x + 30) {
			if (enemy) enemy->EnemyAttack();
			if (enemy) enemy->EnemyDefense();
		}
		if (enemy) enemy->EnemyHadouken();
		if (enemy) enemy->EnemyHadouDefense();
	}

	EnemyHitByPlayer();
	PlayerHitByEnemy();
}

void GameState::Render()
{
	SDL_SetRenderDrawColor(Game::Instance()->GetRenderer(), 0, 0, 0, 255); //set background window color
	SDL_RenderClear(Game::Instance()->GetRenderer()); //clear the screen

	bg->Render();

	if (player->spriteDestRect.x > enemy->spriteDestRect.x) { // checks if flipped or not.
		if (enemy) enemy->FlipEnemy();
		if (enemy) enemy->FlippedMoveToPlayer();
		if (player) player->FlipPlayer();
	}
	else {
		if (enemy) enemy->Render();
		if (enemy) enemy->MoveToPlayer();
		if (player) player->Render();
	}

	ScreenState::Render();
}

bool GameState::CheckCollision(SDL_Rect A, SDL_Rect B) {
	int leftA, leftB; // sides of rectangles
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	leftA = A.x; // calculate sides of A
	rightA = A.x + A.w;
	topA = A.y;
	bottomA = A.y + A.h;

	leftB = B.x; // calculate sides of B
	rightB = B.x + B.w;
	topB = B.y;
	bottomB = B.y + B.h;

	if (bottomA <= topB) { // if any of the sides from A are outside of B
		return false;
	}
	if (topA >= bottomB) {
		return false;
	}
	if (rightA <= leftB) {
		return false;
	}
	if (leftA >= rightB) {
		return false;
	}

	return true; // if none of the sides from A are outside B
}

void GameState::EnemyHitByPlayer() {
	if (CheckCollision(enemy->spriteDestRect, player->spriteDestRect)) {
		GameState* DamageEnemy();
	}
}

void GameState::PlayerHitByEnemy() {
	if (CheckCollision(player->spriteDestRect, enemy->spriteDestRect)) {
		GameState* DamagePlayer();
	}
}

void GameState::Exit()
{
	SDL_Quit(); // shutdown SDL, any clearing of properties should be placed here
	TTF_Quit();//to-do:: de-allocate memory, texture, surface etc, sound 
}