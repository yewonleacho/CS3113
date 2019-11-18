#include "Entity.h"
#include "Scene.h"

Entity::Entity()
{
    entityType = PLATFORM;
    aiState = IDLE;
    status = NONE;
    direction = RIGHT;
    isActive = true;
    position = glm::vec3(0);
    acceleration = glm::vec3(0, -9.81f, 0);
    speed = 0;
    width = 1;
    height = 1;
    numKill = 0;
    numLife = 2;
}

bool Entity::CheckCollision(Entity other)
{
    if (isActive == false || other.isActive == false) return false;
    
    float xdist = fabs(position.x - other.position.x) - ((width + other.width) / 2.0f);
    float ydist = fabs(position.y - other.position.y) - ((height + other.height) / 2.0f);

    if (xdist < 0 && ydist < 0)
    {
      return true;
    }
    return false;
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity object = objects[i];
        
        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object.position.y);
            float penetrationY = fabs(ydist - (height / 2) - (object.height / 2));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity object = objects[i];
        
        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object.position.x);
            float penetrationX = fabs(xdist - (width / 2) - (object.width / 2));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
        }
    }
}


void Entity::CheckCollisionsY(Map *map)
{
  // Probes for tiles
  glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
  glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
  glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
  
  glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
  glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
  glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);
  
  float penetration_x = 0;
  float penetration_y = 0;
  
  if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
    position.y -= penetration_y;
    velocity.y = 0;
    collidedTop = true;
  }
  else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
    position.y -= penetration_y;
    velocity.y = 0;
    collidedTop = true;
  }
  else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
    position.y -= penetration_y;
    velocity.y = 0;
    collidedTop = true;
  }
  
  if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
    position.y += penetration_y;
    velocity.y = 0;
    collidedBottom = true;
  }
  else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
    position.y += penetration_y;
    velocity.y = 0;
    collidedBottom = true;
  }
  else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
    position.y += penetration_y;
    velocity.y = 0;
    collidedBottom = true;
  }
}


void Entity::CheckCollisionsX(Map *map)
{
  // Probes for tiles
  glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
  glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
  
  float penetration_x = 0;
  float penetration_y = 0;
  
  if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
    position.x += penetration_x;
    velocity.x = 0;
    collidedLeft = true;
  }
  if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
    position.x -= penetration_x;
    velocity.x = 0;
    collidedRight = true;
  }
}


void Entity::AI(Entity* player, Entity* life, Entity *enemies){
  switch(aiState){
    case IDLE:
      velocity = glm::vec3(0);
      break;
      
    case PATROLLING:
      if (CheckCollision(*player)){
        if (player->position.y + 0.2f - player->height / 2 >= position.y) {
          isActive = false;
          player->numKill++;
        }
        else{
          player->numLife--;
          life[player->numLife].isActive = false;
          player->position = player->initPosition;
          for (int i = 0; i < ENEMY_COUNT; i++){
            enemies[i].isActive = true;
            enemies[i].position = enemies[i].initPosition;
          }
        }
      }
      if (direction == LEFT){
        velocity.x = -1.0f;
      }
      else if (direction == RIGHT){
        velocity.x = 1.0f;
      }
      if (collidedLeft){
        direction = RIGHT;
        velocity.x = 1.0f;
      }
      if (collidedRight){
        direction = LEFT;
        velocity.x = -1.0f;
      }
      break;
      
    case JUMPING:
      if (CheckCollision(*player)){
        if (player->position.y + 0.2f - player->height / 2 >= position.y) {
          isActive = false;
          player->numKill++;
        }
        else{
          player->numLife--;
          life[player->numLife].isActive = false;
          player->position = player->initPosition;
          for (int i = 0; i < ENEMY_COUNT; i++){
            enemies[i].isActive = true;
            enemies[i].position = enemies[i].initPosition;
          }
        }
      }
      if (collidedBottom){
        velocity.y = 2.5f;
      }
      break;
  }
}


void Entity::Jump(){
  if (collidedBottom){
      velocity.y = 5.0f;
  }
}


void Entity::Update(float deltaTime, Entity *objects, Entity *player, Entity *enemies, Entity *life, int objectCount, Map *map)
{
  collidedTop = false;
  collidedBottom = false;
  collidedLeft = false;
  collidedRight = false;
  
  velocity += acceleration * deltaTime;
  
  position.y += velocity.y * deltaTime; // Move on Y
  CheckCollisionsY(map);
  CheckCollisionsY(objects, objectCount); // Fix if needed
  
  position.x += velocity.x * deltaTime; // Move on X
  CheckCollisionsX(map);
  CheckCollisionsX(objects, objectCount); // Fix if needed
  
  if (entityType == PLAYER){    // If fall into the pit player dies
    if (player->position.y < -7){
      player->isActive = false;
    }
  }
  
  if (entityType == ENEMY){
    AI(player, life, enemies);
  }
  
}


void Entity::Render(ShaderProgram *program) {
  if (isActive){
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
  }
  
}

