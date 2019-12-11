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
  
  
  GLuint mapTextureID = Util::LoadTexture("tileset.png");
  
  state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 14, 7);
  
  state.player.entityType = PLAYER;
  state.player.isActive = false;

  state.enemies[0].entityType = ENEMY;
  state.enemies[0].isActive = false;
  
  state.money[0].entityType = MONEY;
  state.money[0].isActive = false;
  
  state.money[1].entityType = MONEY;
  state.money[1].isActive = false;
  
  state.money[2].entityType = MONEY;
  state.money[2].isActive = false;
  
  state.money[3].entityType = MONEY;
  state.money[3].isActive = false;
}


void Menu::Update(float deltaTime)
{
  state.start.Update(deltaTime, state.enemies, &state.player, state.enemies, state.money, 0, state.map);
    
}

void Menu::Render(ShaderProgram *program)
{
  state.map->Render(program);
  state.start.Render(program);
}

