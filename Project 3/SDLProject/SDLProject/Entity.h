#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

enum EntityType {NONE, PLAYER, TILE, PLATFORM};

class Entity {
public:
  
  EntityType entityType;
  EntityType lastCollision;
  
  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec3 acceleration;
  
  float width;
  float height;
  
  GLuint textureID;
  
  Entity();
  
  void Update(float deltaTime, Entity* tile, int numTile);
  void Render(ShaderProgram *program);
  void MessageRender(ShaderProgram *program);
  void CheckCollisionsY(Entity* tile, int numTile);
  void CheckCollisionsX(Entity* tile, int numTile);
  bool CheckCollision(Entity other);

};

