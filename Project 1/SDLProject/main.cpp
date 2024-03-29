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

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
ShaderProgram program2;

glm::mat4 viewMatrix, modelMatrix, projectionMatrix, triangleMatrix, anotherMatrix;

float player_x = 0;

GLuint playerTextureID;
GLuint anotherTextureID;

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
  displayWindow = SDL_CreateWindow("Project 1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
  SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
  SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
  glewInit();
#endif

  glViewport(0, 0, 640, 480);

  program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
  program2.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

  playerTextureID = LoadTexture("freddie.jpg");
  anotherTextureID = LoadTexture("acdc.png");

  viewMatrix = glm::mat4(1.0f);
  modelMatrix = glm::mat4(1.0f);
  anotherMatrix = glm::mat4(1.0f);
  triangleMatrix = glm::mat4(1.0f);
  projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

  program.SetProjectionMatrix(projectionMatrix);
  program.SetViewMatrix(viewMatrix);
  program.SetColor(0.8f, 0.2f, 0.5f, 1.0f);

  program2.SetProjectionMatrix(projectionMatrix);
  program2.SetViewMatrix(viewMatrix);
  program2.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

  glUseProgram(program.programID);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  glClearColor(0.5f, 0.3f, 0.6f, 1.0f);
}

void DrawTriangle() {
  program2.SetModelMatrix(triangleMatrix);
  program2.SetColor(0.4, 0.2, 0.6, 1.0);

  float vertices[] = { 4.0f, -4.0f, 0.0f, 0.0f, -4.0f, -4.0f };
  glVertexAttribPointer(program2.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
  glEnableVertexAttribArray(program2.positionAttribute);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glDisableVertexAttribArray(program2.positionAttribute);
}

void DrawAnotherTriangle(){
  program2.SetModelMatrix(triangleMatrix);
  program2.SetColor(0.4, 0.2, 0.6, 1.0);

  float vertices[] = { -4.0f, 4.0f, 0.0f, 0.0f, 4.0f, 4.0f };
  glVertexAttribPointer(program2.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
  glEnableVertexAttribArray(program2.positionAttribute);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glDisableVertexAttribArray(program2.positionAttribute);
}


void DrawTexture(){
  program.SetModelMatrix(modelMatrix);

  float vertices[]  = { -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0 };
  float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

  glBindTexture(GL_TEXTURE_2D, playerTextureID);

  glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
  glEnableVertexAttribArray(program.positionAttribute);

  glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
  glEnableVertexAttribArray(program.texCoordAttribute);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  glDisableVertexAttribArray(program.positionAttribute);
  glDisableVertexAttribArray(program.texCoordAttribute);
}

void DrawAnother() {
  program.SetModelMatrix(anotherMatrix);

  float vertices[]  = { -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0 };
  float texCoords[] = { 0.0, 2.0, 2.0, 2.0, 2.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 0.0 };

  glBindTexture(GL_TEXTURE_2D, anotherTextureID);

  glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
  glEnableVertexAttribArray(program.positionAttribute);

  glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
  glEnableVertexAttribArray(program.texCoordAttribute);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  glDisableVertexAttribArray(program.positionAttribute);
  glDisableVertexAttribArray(program.texCoordAttribute);
}

void ProcessInput() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
      gameIsRunning = false;
    }
  }
}

float lastTicks = 0;
float rotate_z = 0;

void Update() {
  float ticks = (float)SDL_GetTicks() / 1000.0f;
  float deltaTime = ticks - lastTicks;
  lastTicks = ticks;

  player_x += 1.0f * deltaTime;

  rotate_z += 45.0 * deltaTime;

  anotherMatrix = glm::mat4(1.0f);
  anotherMatrix = glm::translate(anotherMatrix, glm::vec3(player_x, 0.0f, 0.0f));

//  modelMatrix = glm::rotate(modelMatrix,
//                            glm::radians(rotate_z),
//                            glm::vec3(1.0f, 1.0f, 1.0f));
}

void Render() {
  glClear(GL_COLOR_BUFFER_BIT);

  DrawTriangle();
  DrawAnotherTriangle();
  DrawTexture();
  DrawAnother();

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



