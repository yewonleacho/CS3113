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

#include "Map.h"


enum EntityType { PLAYER, PLATFORM, ENEMY, LIFE, START };
enum State { IDLE, PATROLLING, JUMPING };
enum Direction { LEFT, RIGHT };
enum Status { NONE, WIN, LOSE, MENU };

class Entity {
public:
    
    EntityType entityType;
    State aiState;
    Status status;
    Direction direction;
    bool isStatic;
    bool isActive;
  
    glm::vec3 initPosition;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    float width;
    float height;
    
    float speed;
  
    int numKill;
    int numLife;
  
    GLuint textureID;
    
    Entity();
    
    bool CheckCollision(Entity other);
    bool CheckCollisionsTop(Entity *obj);
    bool CheckCollisionsBottom(Entity *obj);
  
    void CheckCollisionsX(Entity *objects, int objectCount);
    void CheckCollisionsY(Entity *objects, int objectCount);
  
    void CheckCollisionsX(Map *map);
    void CheckCollisionsY(Map *map);
  
    void AI(Entity *player, Entity *life);
  
    void Update(float deltaTime, Entity *objects, Entity *player, Entity *enemies, Entity *life, int objectCount, Map *map);
  
    void Reset(Entity *player, Entity *enemies, Entity *life, int numKill, int numLife);
    void Render(ShaderProgram *program);
    
    void Jump();
    
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
    
};



