#include "Entity.h"


Entity::Entity()
{
  entityType = PLATFORM;
  aiState = IDLE;
  direction = RIGHT;
  status = NONE;
  isActive = true;
  position = glm::vec3(0);
  speed = 0;
  width = 1;
  height = 1;
  kill = 0;
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

void Entity::CheckCollisionWall(){
  if ((position.x + (width / 2.0f)) > 5.0f) {
    position.x = 5.0f - (width / 2.0f);
    velocity.x = 0;
    collidedRightWall = true;
  }
  else if ((position.x - (width / 2.0f)) < -5.0f) {
    position.x = -5.0f + (width / 2.0f);
    velocity.x = 0;
    collidedLeftWall = true;
  }
}

void Entity::AI(Entity* player, Entity* platform){
  switch(aiState){
    case IDLE:
      velocity = glm::vec3(0);
      break;

    case PATROLLING:
      if (glm::distance(position, player->position) < 0.5f) {
        player->isActive = false;
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

    case WALKING:
      if (glm::distance(position, player->position) < 0.5f) {
        player->isActive = false;
      }
      velocity.x = -0.5f;
      break;

    case JUMPING:
      if (glm::distance(position, player->position) < 0.5f) {
        player->isActive = false;
      }
      if (collidedBottom){
        velocity.y = 2.5f;
      }
      break;
  }

}

void Entity::Jump()
{
  if (collidedBottom)
  {
    velocity.y = 5.0f;
  }
}

void Entity::Fire(Entity *fire, int fireCount){
  if (isActive == false) return;
  for (int i = 0; i < fireCount; i++) {
    if (fire[i].isActive == false) {
      fire[i].isActive = true;
      fire[i].position = position;
      if (direction == LEFT) {
        fire[i].position.x -= 1.0;
        fire[i].velocity.x = -20.0f;
      }
      else {
        fire[i].position.x += 1.0;
        fire[i].velocity.x = 20.0f;
      }
      break;
    }
  }
}

void Entity::Update(float deltaTime, Entity *objects, Entity *player, Entity *enemy, Entity *fire, int fireCount, int objectCount)
{
  collidedTop = false;
  collidedBottom = false;
  collidedLeft = false;
  collidedRight = false;
  collidedLeftWall = false;
  collidedRightWall = false;

  if (isActive) {
    velocity += acceleration * deltaTime;
    
    if (entityType == FIRE) {
      position.x += velocity.x * deltaTime;
      if (position.x > 6 || position.x < -6) {
        isActive = false;
      }
      else {
        for (int i = 0; i < 3; i++)
        {
          if (CheckCollision(enemy[i])) {
            enemy[i].isActive = false;
            isActive = false;
            player->kill += 1;
          }
        }
        
        for (int i = 0; i < objectCount; i++)
        {
          if (CheckCollision(objects[i])) {
            isActive = false;
          }
        }
      }
    }
    else {
      position.y += velocity.y * deltaTime;
      CheckCollisionsY(objects, objectCount);
      
      position.x += velocity.x * deltaTime;
      CheckCollisionsX(objects, objectCount);
      
      CheckCollisionWall();
      
      if (entityType == ENEMY) {
        AI(player, objects);
      }
      
      
    }
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

void Entity::MessageRender(ShaderProgram *program) {
  glm::mat4 modelMatrix = glm::mat4(1.0f);
  modelMatrix = glm::translate(modelMatrix, position);
  program->SetModelMatrix(modelMatrix);
  
  float vertices[]  = { -1.5, -2.0, 1.5, -2.0, 1.5, 2.0, -1.5, -2.0, 1.5, 2.0, -1.5, 2.0 };
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
