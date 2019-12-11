#include "Level1.h"

#define LEVEL1_WIDTH 20
#define LEVEL1_HEIGHT 20

unsigned int level1_data[] =
{
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1,
  1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1,
  1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};


void Level1::Initialize() {
  GLuint mapTextureID = Util::LoadTexture("tileset.png");
  GLuint enemyTextureID = Util::LoadTexture("boss.png");
  GLuint moneyTextureID = Util::LoadTexture("money.png");
  GLuint pinkTextureID = Util::LoadTexture("pinks.png");
  GLuint messageTextureID = Util::LoadTexture("success.png");
  
  state.nextLevel = -1;
  
  state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 5);
  
  state.message.entityType = MESSAGE;
  state.message.isActive = false;
  state.message.position = glm::vec3(state.player.position.x - 1.5f, -4.0f, 0);
  state.message.textureID = messageTextureID;
  
  state.player.entityType = PLAYER;
  state.player.isStatic = false;
  state.player.width = 1.0f;
  state.player.position = glm::vec3(7, -3, 0);
  state.player.initPosition = glm::vec3(7, -3, 0);
  state.player.textureID = Util::LoadTexture("player.png");
  
  state.enemies[0].entityType = ENEMY;
  state.enemies[0].aiState = PATROLLING;
  state.enemies[0].direction = RIGHT;
  state.enemies[0].isActive = true;
  state.enemies[0].initPosition = glm::vec3(14, -5, 0);
  state.enemies[0].position = glm::vec3(14, -5, 0);
  state.enemies[0].textureID = enemyTextureID;
  
  state.enemies[1].entityType = ENEMY;
  state.enemies[1].aiState = PATROLLING_FAST;
  state.enemies[1].direction = RIGHT;
  state.enemies[1].isActive = true;
  state.enemies[1].initPosition = glm::vec3(17, -11, 0);
  state.enemies[1].position = glm::vec3(17, -11, 0);
  state.enemies[1].textureID = pinkTextureID;
  
  state.enemies[2].entityType = ENEMY;
  state.enemies[2].aiState = JUMPING;
  state.enemies[2].direction = RIGHT;
  state.enemies[2].isActive = true;
  state.enemies[2].acceleration = glm::vec3(0, -9.81, 0);
  state.enemies[2].initPosition = glm::vec3(17.5, -18, 0);
  state.enemies[2].position = glm::vec3(17.5, -18, 0);
  state.enemies[2].textureID = enemyTextureID;
  
  state.enemies[3].entityType = ENEMY;
  state.enemies[3].aiState = PATROLLING;
  state.enemies[3].direction = RIGHT;
  state.enemies[3].isActive = true;
  state.enemies[3].initPosition = glm::vec3(4, -1.5, 0);
  state.enemies[3].position = glm::vec3(4, -1.5, 0);
  state.enemies[3].textureID = pinkTextureID;
  
  state.enemies[4].entityType = ENEMY;
  state.enemies[4].aiState = JUMPING_HIGH;
  state.enemies[4].direction = RIGHT;
  state.enemies[4].isActive = true;
  state.enemies[4].initPosition = glm::vec3(7.5, -8, 0);
  state.enemies[4].position = glm::vec3(7.5, -8, 0);
  state.enemies[4].textureID = enemyTextureID;
  
  state.enemies[5].entityType = ENEMY;
  state.enemies[5].aiState = PATROLLING_FAST;
  state.enemies[5].direction = RIGHT;
  state.enemies[5].isActive = true;
  state.enemies[5].initPosition = glm::vec3(14.5, -17.5, 0);
  state.enemies[5].position = glm::vec3(14.5, -17.5, 0);
  state.enemies[5].textureID = pinkTextureID;
  
  state.enemies[6].entityType = ENEMY;
  state.enemies[6].aiState = GOINGUP;
  state.enemies[6].direction = UP;
  state.enemies[6].isActive = true;
  state.enemies[6].initPosition = glm::vec3(1.5, -13, 0);
  state.enemies[6].position = glm::vec3(1.5, -13, 0);
  state.enemies[6].textureID = enemyTextureID;
  
  state.enemies[7].entityType = ENEMY;
  state.enemies[7].aiState = GOINGUP;
  state.enemies[7].direction = UP;
  state.enemies[7].isActive = true;
  state.enemies[7].initPosition = glm::vec3(4.5, -17, 0);
  state.enemies[7].position = glm::vec3(4.5, -17, 0);
  state.enemies[7].textureID = enemyTextureID;
  
  state.money[0].entityType = MONEY;
  state.money[0].isActive = true;
  state.money[0].width = 1.0f;
  state.money[0].initPosition = glm::vec3(13, -5, 0);
  state.money[0].position = glm::vec3(13, -5, 0);
  state.money[0].textureID = moneyTextureID;
  
  state.money[1].entityType = MONEY;
  state.money[1].isActive = true;
  state.money[1].width = 1.0f;
  state.money[1].initPosition = glm::vec3(17.5, -17, 0);
  state.money[1].position = glm::vec3(17.5, -17, 0);
  state.money[1].textureID = moneyTextureID;
  
  state.money[2].entityType = MONEY;
  state.money[2].isActive = true;
  state.money[2].width = 1.0f;
  state.money[2].initPosition = glm::vec3(4.5, -4, 0);
  state.money[2].position = glm::vec3(4.5, -4, 0);
  state.money[2].textureID = moneyTextureID;
  
  state.money[3].entityType = MONEY;
  state.money[3].isActive = true;
  state.money[3].width = 1.0f;
  state.money[3].initPosition = glm::vec3(14.5, -14, 0);
  state.money[3].position = glm::vec3(14.5, -14, 0);
  state.money[3].textureID = moneyTextureID;
  
  state.money[4].entityType = MONEY;
  state.money[4].isActive = true;
  state.money[4].width = 1.0f;
  state.money[4].initPosition = glm::vec3(6, -1.5, 0);
  state.money[4].position = glm::vec3(6, -1.5, 0);
  state.money[4].textureID = moneyTextureID;
  
  state.money[5].entityType = MONEY;
  state.money[5].isActive = true;
  state.money[5].width = 1.0f;
  state.money[5].initPosition = glm::vec3(1.5, -1, 0);
  state.money[5].position = glm::vec3(1.5, -1, 0);
  state.money[5].textureID = moneyTextureID;
  
  state.money[6].entityType = MONEY;
  state.money[6].isActive = true;
  state.money[6].width = 1.0f;
  state.money[6].initPosition = glm::vec3(1.5, -15, 0);
  state.money[6].position = glm::vec3(1.5, -15, 0);
  state.money[6].textureID = moneyTextureID;
  
  
}

void Level1::Update(float deltaTime) {
  state.player.Update(deltaTime, state.enemies, &state.player, state.enemies, state.money, 0, state.map);
  
  for (int i = 0; i < ENEMY_COUNT; i++){
    state.enemies[i].Update(deltaTime, state.enemies, &state.player, state.enemies, state.money, 0, state.map);
  }
  
  for (int i = 0; i < MONEY_COUNT; i++){
    state.money[i].Update(deltaTime, state.enemies, &state.player, state.enemies, state.money, 0, state.map);
  }

}

void Level1::Render(ShaderProgram *program) {
  state.map->Render(program);
  state.player.Render(program);
  
  for (int i = 0; i < ENEMY_COUNT; i++){
    state.enemies[i].Render(program);
  }
  
  for (int j = 0; j < MONEY_COUNT; j++){
    if (state.money[j].isActive){
      state.money[j].Render(program);
    }
  }
  
  if (state.message.isActive){
    state.message.Render(program);
  }
}
