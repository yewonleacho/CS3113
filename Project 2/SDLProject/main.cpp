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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
ShaderProgram program2;
ShaderProgram program3;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix, anotherMatrix, ballMatrix;

float lastTicks = 0;

Entity player, player2, ball;


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
  displayWindow = SDL_CreateWindow("Project 2 - Ping Pong Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
  SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
  SDL_GL_MakeCurrent(displayWindow, context);
  
#ifdef _WINDOWS
  glewInit();
#endif
  
  glViewport(0, 0, 640, 480);
  
  program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
  program2.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
  program3.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
  
  
  player.textureID = LoadTexture("red.png");
  player.position = glm::vec3(-4.5f, 0.0f, 0.0f);
  player.movement = glm::vec3(0.0f, 0.0f, 0.0f);
  player.speed = 4;
  player.width = 1.0;
  player.height = 3.0;
  
  player2.textureID = LoadTexture("black.png");
  player2.position = glm::vec3(4.5f, 0.0f, 0.0f);
  player2.movement = glm::vec3(0.0f, 0.0f, 0.0f);
  player2.speed = 4;
  player2.width = 1.0;
  player2.height = 3.0;
  
  ball.textureID = LoadTexture("ball.png");
  ball.position = glm::vec3(0.0f, 0.0f, 0.0f);
  ball.movement = glm::vec3(1.0f, 1.0f, 0.0f);
  ball.speed = 4;
  ball.width = 0.5;
  ball.height = 0.5;
  

  viewMatrix = glm::mat4(1.0f);
  modelMatrix = glm::mat4(1.0f);
  projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
  
  program.SetProjectionMatrix(projectionMatrix);
  program.SetViewMatrix(viewMatrix);
  program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
  
  program2.SetProjectionMatrix(projectionMatrix);
  program2.SetViewMatrix(viewMatrix);
  program2.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
  
  program3.SetProjectionMatrix(projectionMatrix);
  program3.SetViewMatrix(viewMatrix);
  program3.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
  
  
  glUseProgram(program.programID);
  glUseProgram(program2.programID);
  glUseProgram(program3.programID);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

bool TopCollisionDetect(Entity* obj){
  return (obj->position.y + (obj->height / 2.0f)) > 3.75;
}

bool BottomCollisionDetect(Entity* obj){
  return (obj->position.y - (obj->height / 2.0f)) < -3.75;
}

bool LeftCollisionDetect(Entity* obj){
  return (obj->position.x + (obj->width / 2.0f)) > 5.0;
}

bool RightCollisionDetect(Entity* obj){
  return (obj->position.x - (obj->width / 2.0f)) < -5.0;
}

bool ObjCollisionDetect(Entity* obj, Entity* obj2){
  float xdist = fabs(obj2->position.x - obj->position.x) - ((obj->width + obj2->width) / 2.0f);
  float ydist = fabs(obj2->position.y - obj->position.y) - ((obj->height + obj2->height) / 2.0f);
  return (xdist < 0 && ydist < 0);
}

void ProcessInput() {
  player.movement = glm::vec3(0, 0, 0);
  player2.movement = glm::vec3(0, 0, 0);
  
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
  
  if (keys[SDL_SCANCODE_W]){
    if (TopCollisionDetect(&player) == false){
      player.movement.y = 1;
    }
  }
  else if (keys[SDL_SCANCODE_S]){
    if (BottomCollisionDetect(&player) == false){
      player.movement.y = -1;
    }
  }
  
  
  if (keys[SDL_SCANCODE_UP]){
    if (TopCollisionDetect(&player2) == false){
      player2.movement.y = 1;
    }
  }
  else if (keys[SDL_SCANCODE_DOWN]){
    if (BottomCollisionDetect(&player2) == false){
      player2.movement.y = -1;
    }
  }
  
}

void Update() {
  float ticks = (float)SDL_GetTicks() / 1000.0f;
  float deltaTime = ticks - lastTicks;
  lastTicks = ticks;
  
  if (TopCollisionDetect(&ball)){
    ball.movement.y *= -1;
  }
  else if (BottomCollisionDetect(&ball)){
    ball.movement.y *= -1;
  }
  
  // If the ball touches right or left wall, the window color changes to the color of winner's paddle
  // Player(left): red, player(right): black
  else if (RightCollisionDetect(&ball)){
    ball.position = glm::vec3(0);
    ball.movement.x *= -1;
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
  }
  else if (LeftCollisionDetect(&ball)){
    ball.position = glm::vec3(0);
    ball.movement.x *= -1;
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
  }
  
  if (ObjCollisionDetect(&ball, &player)){
    ball.movement.x *= -1;
  }
  if (ObjCollisionDetect(&ball, &player2)){
    ball.movement.x *= -1;
  }
  
  player.Update(deltaTime);
  player2.Update(deltaTime);
  ball.Update(deltaTime);
}

void Render() {
  glClear(GL_COLOR_BUFFER_BIT);
  
  player.PlayerRender(&program);
  player2.Player2Render(&program2);
  ball.BallRender(&program3);
  
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
