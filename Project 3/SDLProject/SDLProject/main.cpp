// Player should fall with gravity (make it very low so it moves slowly)
// Moving left and right should change ACCELERATION instead of velocity
// If the player touches a wall/rock show text "Mission failed"
// If the player touches the platform show text "Mission Successful"

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#include <GL/glut.h>

#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

#define NUM_PLATFORM 28

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

struct GameState{
  Entity player;
  Entity tile[NUM_PLATFORM];
  Entity msgSuccess;
  Entity msgFail;
  bool success = false;
  bool touched = false;
};

GameState state;


#define FIXED_TIMESTEP 0.166666f
float lastTicks = 0;
float accumulator = 0.0f;

GLuint fontTextureID;

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
  displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
  SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
  SDL_GL_MakeCurrent(displayWindow, context);
  
#ifdef _WINDOWS
  glewInit();
#endif
  
  glViewport(0, 0, 640, 480);
  
  program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
  
  GLuint tileTextureID = LoadTexture("platform.png");
  
  state.player.textureID = LoadTexture("player.png");
  state.player.position = glm::vec3(0.0, 3.3, 0.0);
  state.player.acceleration = glm::vec3(0.0, -9.81f, 0);
  state.player.entityType = PLAYER;
  
  state.msgFail.textureID = LoadTexture("fail.png");
  state.msgSuccess.textureID = LoadTexture("success.png");

  //Positioning tiles on the ground
  for (int i = 0; i < 10; i++){
    if (i == 2 || i == 3){
      state.tile[i].textureID = LoadTexture("tile.png");
      state.tile[i].entityType = PLATFORM;
    }
    else {
      state.tile[i].textureID = tileTextureID;
      state.tile[i].entityType = TILE;
    }
    state.tile[i].position = glm::vec3(-4.5 + i, -3.25f, 0);
  }
  
  //Positioning tiles in the left middle of window
  float leftXPos = -3.5;
  for (int i = 10; i < 12; i++){
    state.tile[i].textureID = tileTextureID;
    state.tile[i].position = glm::vec3(leftXPos, 1.75f, 0);
    state.tile[i].entityType = TILE;
    leftXPos += 1;
  }

  //Positioning tiles in the right middle of window
  float rightXPos = 3.5;
  for (int i = 12; i < 16; i++){
    state.tile[i].textureID = tileTextureID;
    state.tile[i].position = glm::vec3(rightXPos, -0.25f, 0);
    state.tile[i].entityType = TILE;
    rightXPos -= 1;
  }
  
  //Positioning tiles on the left wall
  float leftYPos = 2.75;
  for (int i = 16; i < 22; i++){
    state.tile[i].textureID = tileTextureID;
    state.tile[i].position = glm::vec3(-4.5f, leftYPos, 0);
    state.tile[i].entityType = TILE;
    leftYPos -= 1;
  }
  
  //Positioning tiles on the right wall
  float rightYPos = 2.75;
  for (int i = 22; i < 28; i++){
    state.tile[i].textureID = tileTextureID;
    state.tile[i].position = glm::vec3(4.5f, rightYPos, 0);
    state.tile[i].entityType = TILE;
    rightYPos -= 1;
  }
  
  
  viewMatrix = glm::mat4(1.0f);
  modelMatrix = glm::mat4(1.0f);
  projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
  
  program.SetProjectionMatrix(projectionMatrix);
  program.SetViewMatrix(viewMatrix);
  program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

  
  glUseProgram(program.programID);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}


void ProcessInput() {
  state.player.velocity = glm::vec3(0, 0, 0);
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
      case SDL_WINDOWEVENT_CLOSE:
        gameIsRunning = false;
        break;
        
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym){
          case SDLK_SPACE:
            break;
        }
    }
  }
  
  const Uint8 *keys = SDL_GetKeyboardState(NULL);
  
  if (keys[SDL_SCANCODE_LEFT]){
    state.player.acceleration.x -= 0.5f;
  }
  else if (keys[SDL_SCANCODE_RIGHT]){
    state.player.acceleration.x += 0.5f;
  }
  else if (keys[SDL_SCANCODE_R]){
    state.player.position = glm::vec3(0.0, 3.3, 0.0);
    state.player.acceleration = glm::vec3(0.0, -9.81f, 0);
    state.player.lastCollision = NONE;
    state.success = false;
    state.touched = false;
  }
  else {
    state.player.acceleration.x = 0.0f;
  }
}

void Update() {
  float ticks = (float)SDL_GetTicks() / 1000.0f;
  float deltaTime = ticks - lastTicks;
  lastTicks = ticks;

  if (state.player.lastCollision != NONE){
    state.touched = true;
    state.success = false;
    if (state.player.lastCollision == PLATFORM){
      state.touched = true;
      state.success = true;
    }
    return;
  }

  deltaTime += accumulator;
  if (deltaTime < FIXED_TIMESTEP) {
    accumulator = deltaTime;
    return;
  }
  
  while (deltaTime >= FIXED_TIMESTEP) {
    // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
    state.player.Update(FIXED_TIMESTEP, state.tile, NUM_PLATFORM);
    
    deltaTime -= FIXED_TIMESTEP;
  }
  
  accumulator = deltaTime;
}

void Render() {
  glClear(GL_COLOR_BUFFER_BIT);
  
  state.player.Render(&program);
  
  for (int i = 0; i < NUM_PLATFORM; i++){
    state.tile[i].Render(&program);
  }
  

  if (state.touched){
    if (state.success){
      state.msgSuccess.MessageRender(&program);
    }
    else{
      state.msgFail.MessageRender(&program);
    }
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
