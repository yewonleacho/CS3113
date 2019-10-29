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

enum  EntityType { PLAYER, PLATFORM, ENEMY, FIRE };
enum State { IDLE, WALKING, PATROLLING, JUMPING };
enum Direction { LEFT, RIGHT };
enum Status { NONE, WIN, LOSE };

class Entity {
public:
  
  EntityType entityType;
  State aiState;
  Direction direction;
  Status status;
  bool isActive;
  
  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec3 acceleration;
  
  float width;
  float height;
  
  float speed;
  
  int kill;
  
  GLuint textureID;
  
  Entity();
  
  bool CheckCollision(Entity other);
  bool CheckCollisionsTop(Entity *obj);
  bool CheckCollisionsBottom(Entity *obj);
  
  void CheckCollisionsX(Entity *objects, int objectCount);
  void CheckCollisionsY(Entity *objects, int objectCount);

  void CheckCollisionWall();
  
  void AI(Entity *objects, Entity *platform);
  
  void Update(float deltaTime, Entity *objects, Entity *player, Entity *enemy, Entity *fire, int fireCount, int objectCount);
  void Render(ShaderProgram *program);
  void MessageRender(ShaderProgram *program);
  
  void Jump();
  void Fire(Entity *fire, int fireCount);

  
  bool collidedTop;
  bool collidedBottom;
  bool collidedLeft;
  bool collidedRight;
  bool collidedLeftWall;
  bool collidedRightWall;
  
};



