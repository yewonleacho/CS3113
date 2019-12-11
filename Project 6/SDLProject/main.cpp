#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Entity.h"
#include "Menu.h"
#include "Map.h"
#include "Util.h"
#include "Scene.h"
#include "Level1.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID;


Scene *currentScene;
Scene *sceneList[3];
Level1 *level1;
Mix_Music *music;
Mix_Chunk *bounce;
Mix_Chunk *kaching;

void SwitchToScene(Scene *scene)
{
    currentScene = scene;
    currentScene->Initialize();
}


void Initialize() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    music = Mix_LoadMUS("music.mp3");
    bounce = Mix_LoadWAV("running.wav");
    kaching = Mix_LoadWAV("ka-ching.wav");

    Mix_PlayMusic(music, -1);
    
    
    
  displayWindow = SDL_CreateWindow("The Office Jungle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
  SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
  SDL_GL_MakeCurrent(displayWindow, context);
  
#ifdef _WINDOWS
    glewInit();
#endif
    
  glViewport(0, 0, 640, 480);
  
  program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
  
  fontTextureID = Util::LoadTexture("font.png");
  
  viewMatrix = glm::mat4(1.0f);
  modelMatrix = glm::mat4(1.0f);
  projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
  
  program.SetProjectionMatrix(projectionMatrix);
  program.SetViewMatrix(viewMatrix);
  program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
  
  sceneList[0] = new Menu();
  sceneList[1] = new Level1();
  SwitchToScene(sceneList[0]);
  
  glUseProgram(program.programID);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
                        break;
                        
               
            case SDLK_RETURN:
                if (currentScene == sceneList[0]){
                    SwitchToScene(sceneList[1]);
                }
                break;
            }
            break;
            
          case SDLK_LEFT:
            Mix_PlayChannel(-1, bounce, 0);
            break;
          case SDLK_RIGHT:
            Mix_PlayChannel(-1, bounce, 0);
            break;
          case SDLK_UP:
            Mix_PlayChannel(-1, bounce, 0);
            break;
          case SDLK_DOWN:
            Mix_PlayChannel(-1, bounce, 0);
            break;
          
        }
    }
    
    currentScene->state.player.velocity.x = 0;
    currentScene->state.player.velocity.y = 0;
  
    // Check for pressed/held keys below
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_LEFT])
    {
        currentScene->state.player.velocity.x = -3.0f;
        Mix_PlayChannel(-1, bounce, 0);
    }
    else if  (keys[SDL_SCANCODE_RIGHT])
    {
        currentScene->state.player.velocity.x = 3.0f;
        Mix_PlayChannel(-1, bounce, 0);
    }
    else if  (keys[SDL_SCANCODE_UP])
    {
      currentScene->state.player.velocity.y = 3.0f;
      Mix_PlayChannel(-1, bounce, 0);
    }
    else if  (keys[SDL_SCANCODE_DOWN])
    {
      currentScene->state.player.velocity.y = -3.0f;
      Mix_PlayChannel(-1, bounce, 0);
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    currentScene->Update(FIXED_TIMESTEP);
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        
      currentScene->state.player.Update(FIXED_TIMESTEP, currentScene->state.enemies, &currentScene->state.player, currentScene->state.enemies, currentScene->state.money, ENEMY_COUNT, currentScene->state.map);
      
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
  
    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player.position.x, -currentScene->state.player.position.y, 0));
  
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
  
    program.SetViewMatrix(viewMatrix);
  
    if (currentScene->state.start.status == MENU){
      Util::DrawText(&program, fontTextureID, "The Office Jungle", 1.0f, -0.5f, glm::vec3(-4.0, 2, 0));
      Util::DrawText(&program, fontTextureID, "Press ENTER", 1.0f, -0.5f, glm::vec3(-2.6, -2, 0));
      Util::DrawText(&program, fontTextureID, "to start", 1.0f, -0.5f, glm::vec3(-1.8, -3, 0));
    }
  
  if (currentScene->state.player.numEat == MONEY_COUNT){
    Util::DrawText(&program, fontTextureID, "Mission", 1.0f, -0.5f, glm::vec3(currentScene->state.player.position.x - 1.5, -4, 0));
    Util::DrawText(&program, fontTextureID, "Accomplished", 1.0f, -0.5f, glm::vec3(currentScene->state.player.position.x - 2.5, -5, 0));
  }
  
    currentScene->state.player.Render(&program);
    currentScene->state.map->Render(&program);
  
  for (int i = 0; i < ENEMY_COUNT; i++){
    currentScene->state.enemies[i].Render(&program);
  }
  
  for (int i = 0; i < MONEY_COUNT; i++){
    if (currentScene->state.money[i].isActive){
      currentScene->state.money[i].Render(&program);
    }
    if (currentScene->state.money[i].eat){
      Mix_PlayChannel(-1, kaching, 0);
      currentScene->state.money[i].eat = false;
    }
  }
  
    currentScene->Render(&program);
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    Mix_FreeMusic(music);
    Mix_FreeChunk(bounce);
    Mix_FreeChunk(kaching);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        if (currentScene->state.nextLevel >= 0) SwitchToScene(sceneList[currentScene->state.nextLevel]);
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
