#include "Menu.h"
#include "Util.h"

#define MENU_WIDTH 1
#define MENU_HEIGHT 1

unsigned int menu_data[] =
{
  0
};

void Menu::Initialize()
{
  
  state.nextLevel = -1;
  
  state.start.entityType = START;
  state.start.status = MENU;
  state.start.position = glm::vec3(-1, 0, 0);
  
  
  GLuint mapTextureID = Util::LoadTexture("tilesheet.png");
  
  state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 14, 7);
  
  state.player.entityType = PLAYER;
  state.player.isActive = false;
  
  for (int i = 0; i < LIFE_COUNT; i++){
    state.life[i].entityType = LIFE;
    state.life[i].isActive = false;
  }
  
  state.enemies[0].entityType = ENEMY;
  state.enemies[0].isActive = false;
  
  state.enemies[1].entityType = ENEMY;
  state.enemies[1].isActive = false;
}


void Menu::Update(float deltaTime)
{
  state.start.Update(deltaTime, state.enemies, &state.player, state.enemies, state.life, 0, state.map);
}

void Menu::Render(ShaderProgram *program)
{
  state.map->Render(program);
  state.start.Render(program);
}
