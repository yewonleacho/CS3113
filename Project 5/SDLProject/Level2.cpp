#include "Level2.h"
#include "Entity.h"

#define LEVEL2_WIDTH 20
#define LEVEL2_HEIGHT 8

unsigned int level2_data[] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16,
  17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17,
  17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 17,
  17, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 3, 3, 17,
  17, 3, 3, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 2, 2, 3, 3, 3, 3, 17,
  17, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 17,
  17, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 17
};

void Level2::Initialize()
{
  GLuint mapTextureID = Util::LoadTexture("tilesheet.png");
  GLuint lifeTextureID = Util::LoadTexture("life.png");
  GLuint enemyTextureID = Util::LoadTexture("grinch.png");
  
  state.nextLevel = -1;
  
  state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 14, 7);
  
  state.player.entityType = PLAYER;
  state.player.isActive = true;
  state.player.numKill = 0;
  state.player.width = 1.0f;
  state.player.position = glm::vec3(5, 0, 0);
  state.player.initPosition = glm::vec3(5, 0, 0);
  state.player.acceleration = glm::vec3(0, -9.81f, 0);
  state.player.textureID = Util::LoadTexture("santa.png");
  
  for (int i = 0; i < LIFE_COUNT; i++){
    state.life[i].entityType = LIFE;
    state.life[i].isActive = true;
    state.life[i].position = glm::vec3(state.player.position.x + i + 2.5, -7, 0);
    state.life[i].textureID = lifeTextureID;
  }
  
  state.enemies[0].entityType = ENEMY;
  state.enemies[0].aiState = PATROLLING;
  state.enemies[0].direction = RIGHT;
  state.enemies[0].isActive = true;
  state.enemies[0].initPosition = glm::vec3(1, -3, 0);
  state.enemies[0].position = glm::vec3(1, -3, 0);
  state.enemies[0].textureID = enemyTextureID;
  
  state.enemies[1].entityType = ENEMY;
  state.enemies[1].aiState = JUMPING;
  state.enemies[1].direction = RIGHT;
  state.enemies[1].isActive = true;
  state.enemies[1].initPosition = glm::vec3(10, -4, 0);
  state.enemies[1].position = glm::vec3(10, -4, 0);
  state.enemies[1].textureID = enemyTextureID;
}


void Level2::Update(float deltaTime)
{
  if (state.player.numKill >= 2) {
    state.nextLevel = 3;
  }
  
  if (state.player.numLife <= 0){
    state.player.isActive = false;
  }
  
  state.player.Update(deltaTime, state.enemies, &state.player, state.enemies, state.life, 0, state.map);
  
  state.enemies[0].Update(deltaTime, state.enemies, &state.player, state.enemies, state.life, 0, state.map);
  state.enemies[1].Update(deltaTime, state.enemies, &state.player, state.enemies, state.life, 0, state.map);

  
}


void Level2::Render(ShaderProgram *program)
{
  state.map->Render(program);
  
  if (state.player.isActive){
    state.player.Render(program);
  }
  
  for (int i = 0; i < state.player.numLife; i++){
    if (state.life[i].isActive){
      state.life[i].Render(program);
    }
  }
  
  for (int j = 0; j < ENEMY_COUNT; j++){
    if (state.enemies[j].isActive){
      state.enemies[j].Render(program);
    }
  }
  
  
}
