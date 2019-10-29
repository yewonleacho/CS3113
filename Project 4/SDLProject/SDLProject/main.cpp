//3 (or more) enemies with basic AI
//Each should have different behavior such as: walking, patrolling, jumping, shooting, etc
//Player must be able to get rid of the enemies such as jumping on them, shooting, etc
//If the player touches an enemy, gets shot, etc, show text "GAME OVER"
//If the player gets rid of all the enemies, show text "YOU WIN"


#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

#define PLATFORM_COUNT 21
#define ENEMY_COUNT 3
#define FIRE_COUNT 25


struct GameState {
  Entity player;
  Entity enemy[ENEMY_COUNT];
  Entity platforms[PLATFORM_COUNT];
  Entity fire[FIRE_COUNT];
  Entity msgWin;
  Entity msgLose;
};

GameState gState;

GLuint LoadTexture(const char* filePath) {
  int w, h, n;
  unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
  
  if (image == NULL) {
    std::cout << "Unable to load image. Make sure the path is correct\n";
    assert(false);
  }
  
  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  stbi_image_free(image);
  return textureID;
}

void Initialize() {
  SDL_Init(SDL_INIT_VIDEO);
  displayWindow = SDL_CreateWindow("AI!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
  SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
  SDL_GL_MakeCurrent(displayWindow, context);
  
#ifdef _WINDOWS
  glewInit();
#endif
  
  glViewport(0, 0, 640, 480);
  
  program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
  
  gState.player.entityType = PLAYER;
  gState.player.status = NONE;
  gState.player.isActive = true;
  gState.player.kill = 0;
  gState.player.width = 1.0f;
  gState.player.position = glm::vec3(-4.50f, -0.5f, 0);
  gState.player.acceleration = glm::vec3(0, -9.81f, 0);
  gState.player.textureID = LoadTexture("player.png");
  
  GLuint fireTextureID = LoadTexture("fire.png");
  GLuint dirtTextureID = LoadTexture("dirt.png");
  GLuint grassTextureID = LoadTexture("grass.png");
  
  gState.msgWin.textureID = LoadTexture("youwin.png");
  gState.msgWin.isActive = false;
  gState.msgLose.textureID = LoadTexture("gameover.png");
  gState.msgLose.isActive = false;
  
  for (int i = 0; i < 11; i++)
  {
    gState.platforms[i].textureID = dirtTextureID;
    gState.platforms[i].position = glm::vec3(i - 5.0f, -3.25f, 0);
  }

  for (int i = 11; i < 15; i++){
    gState.platforms[i].textureID = grassTextureID;
    gState.platforms[i].position = glm::vec3((i - 11) - 5.0f, -2.25f, 0);
  }
  
  gState.platforms[15].textureID = grassTextureID;
  gState.platforms[15].position = glm::vec3(3.0f, -2.25f, 0);
  
  for (int i = 16; i < 19; i++){
    gState.platforms[i].textureID = dirtTextureID;
    gState.platforms[i].position = glm::vec3(i - 12, -2.25f, 0);
  }
  
  for (int i = 19; i < PLATFORM_COUNT; i++){
    gState.platforms[i].textureID = grassTextureID;
    gState.platforms[i].position = glm::vec3(i - 15, -1.25f, 0);
  }
  
  for (int i = 0; i < FIRE_COUNT; i++){
    gState.fire[i].entityType = FIRE;
    gState.fire[i].width = 0.5f;
    gState.fire[i].height = 0.5f;
    gState.fire[i].textureID = fireTextureID;
    gState.fire[i].isActive = false;
  }
  
  gState.enemy[0].entityType = ENEMY;
  gState.enemy[0].aiState = WALKING;
  gState.enemy[0].direction = LEFT;
  gState.enemy[0].isActive = true;
  gState.enemy[0].width = 1.0f;
  gState.enemy[0].position = glm::vec3(-2, -1.25, 0);
  gState.enemy[0].acceleration = glm::vec3(0, -9.81f, 0);
  gState.enemy[0].textureID = LoadTexture("evil.png");

  gState.enemy[1].entityType = ENEMY;
  gState.enemy[1].aiState = PATROLLING;
  gState.enemy[1].direction = RIGHT;
  gState.enemy[1].isActive = true;
  gState.enemy[1].width = 1.0f;
  gState.enemy[1].position = glm::vec3(0, -2.25, 0);
  gState.enemy[1].acceleration = glm::vec3(0, -9.81f, 0);
  gState.enemy[1].textureID = LoadTexture("evil.png");

  gState.enemy[2].entityType = ENEMY;
  gState.enemy[2].aiState = JUMPING;
  gState.enemy[2].isActive = true;
  gState.enemy[2].width = 1.0f;
  gState.enemy[2].position = glm::vec3(4.5, -0.25, 0);
  gState.enemy[2].acceleration = glm::vec3(0, -9.81f, 0);
  gState.enemy[2].textureID = LoadTexture("evil.png");


  viewMatrix = glm::mat4(1.0f);
  modelMatrix = glm::mat4(1.0f);
  projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
  
  program.SetProjectionMatrix(projectionMatrix);
  program.SetViewMatrix(viewMatrix);
  program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
  
  glUseProgram(program.programID);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}


void ProcessInput() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
      case SDL_WINDOWEVENT_CLOSE:
        gameIsRunning = false;
        break;
        
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_SPACE:
            gState.player.Jump();
            break;
          case SDLK_s:
            gState.player.Fire(gState.fire, FIRE_COUNT);
            break;
        break;

           
    }
  }
  
  gState.player.velocity.x = 0;
  
  // Check for pressed/held keys below
  const Uint8 *keys = SDL_GetKeyboardState(NULL);
  if (keys[SDL_SCANCODE_LEFT])
  {
    gState.player.velocity.x = -1.0f;
    gState.player.direction = LEFT;
  }
  else if  (keys[SDL_SCANCODE_RIGHT])
  {
    gState.player.velocity.x = 1.0f;
    gState.player.direction = RIGHT;
  }
  }}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
  
  if (gState.player.kill >= ENEMY_COUNT) {
    gState.msgWin.isActive = true;
  }
  else if (gState.player.isActive == false) {
    gState.msgLose.isActive = true;
  }
  
  
  float ticks = (float)SDL_GetTicks() / 1000.0f;
  float deltaTime = ticks - lastTicks;
  lastTicks = ticks;
  
  deltaTime += accumulator;
  if (deltaTime < FIXED_TIMESTEP) {
    accumulator = deltaTime;
    return;
  }
  
  while (deltaTime >= FIXED_TIMESTEP) {
    // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
    gState.player.Update(FIXED_TIMESTEP, gState.platforms, &gState.player, gState.enemy, gState.fire, FIRE_COUNT, PLATFORM_COUNT);
    
    for (int i = 0; i < ENEMY_COUNT; i++){
      gState.enemy[i].Update(FIXED_TIMESTEP, gState.platforms, &gState.player, gState.enemy, gState.fire, FIRE_COUNT, PLATFORM_COUNT);
    }
    
    for (int i = 0; i < FIRE_COUNT; i++){
      gState.fire[i].Update(FIXED_TIMESTEP, gState.platforms, &gState.player, gState.enemy, gState.fire, FIRE_COUNT, PLATFORM_COUNT);
    }
    
    deltaTime -= FIXED_TIMESTEP;
  }
  
  accumulator = deltaTime;
  

}


void Render() {
  glClear(GL_COLOR_BUFFER_BIT);
  
  gState.player.Render(&program);
  
  for (int i = 0; i < ENEMY_COUNT; i++){
    gState.enemy[i].Render(&program);
  }

  for (int i = 0; i < PLATFORM_COUNT; i++){
    gState.platforms[i].Render(&program);
  }
  
  for (int i = 0; i < FIRE_COUNT; i++){
    gState.fire[i].Render(&program);
  }
  
  if (gState.player.kill >= 3){
    gState.msgWin.MessageRender(&program);
  }
  else if (gState.player.isActive == false){
    gState.msgLose.MessageRender(&program);
  }
  
  SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
  SDL_Quit();
}

int main(int argc, char* argv[]) {
  Initialize();
  
  while (gameIsRunning) {
    ProcessInput();
    Update();
    Render();
  }
  
  Shutdown();
  return 0;
}
