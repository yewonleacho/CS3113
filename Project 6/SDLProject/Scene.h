#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Util.h"
#include "Entity.h"
#include "Map.h"

#define ENEMY_COUNT 8
#define MONEY_COUNT 7


struct GameState {
  Entity player;
  Entity enemies[ENEMY_COUNT];
  Entity start;
  Entity money[MONEY_COUNT];
  Entity message;
  Map *map;
  int nextLevel;
};

class Scene {
public:
  GameState state;
  virtual void Initialize() = 0;
  virtual void Update(float deltaTime) = 0;
  virtual void Render(ShaderProgram *program) = 0;
  
};
