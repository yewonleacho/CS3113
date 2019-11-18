#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Entity.h"
#include "Map.h"
#include "Util.h"
#include "Scene.h"
#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID;

Scene *currentScene;
Scene *sceneList[4];

Mix_Music *music;
Mix_Chunk *bounce;

void SwitchToScene(Scene *scene)
{
    if (scene == sceneList[0]){
      scene->state.player.numLife = 3;
    }
    else{
      scene->state.player.numLife = currentScene->state.player.numLife;
    }
    currentScene = scene;
    currentScene->Initialize();
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
  
    music = Mix_LoadMUS("jinglebell.wav");
    bounce = Mix_LoadWAV("jump.wav");
  
    Mix_PlayMusic(music, -1);
  
    displayWindow = SDL_CreateWindow("Platformer!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif
  
    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
  
    fontTextureID = Util::LoadTexture("font1.png");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
  
    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0]);
  
  
    glUseProgram(program.programID);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glClearColor(0.667f, 0.004f, 0.078f, 1.0f);
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
                currentScene->state.player.Jump();
                Mix_PlayChannel(-1, bounce, 0);
                break;
                
              case SDLK_RETURN:
                if (currentScene == sceneList[0]){
                  SwitchToScene(sceneList[1]);
                }
                break;
            }
            break;
        }
    }
    
    currentScene->state.player.velocity.x = 0;
    
    // Check for pressed/held keys below
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
  
    if (keys[SDL_SCANCODE_A])
    {
        currentScene->state.player.velocity.x = -3.0f;
    }
    else if  (keys[SDL_SCANCODE_D])
    {
        currentScene->state.player.velocity.x = 3.0f;
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
        currentScene->state.player.Update(FIXED_TIMESTEP, currentScene->state.enemies, &currentScene->state.player, currentScene->state.enemies, currentScene->state.life, ENEMY_COUNT, currentScene->state.map);
        
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
  
    viewMatrix = glm::mat4(1.0f);
    if (currentScene->state.player.position.x > 5) {
      viewMatrix = glm::translate(viewMatrix,
              glm::vec3(-currentScene->state.player.position.x, 3.75, 0));
    } else {
      viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }
  
}


void Render() {
    program.SetViewMatrix(viewMatrix);
  
    glClear(GL_COLOR_BUFFER_BIT);
  
    if (currentScene->state.start.status == MENU){
      Util::DrawText(&program, fontTextureID, "Catch Mr.Grinch", 1.0f, -0.5f, glm::vec3(1.5, -3, 0));
      Util::DrawText(&program, fontTextureID, "Press ENTER", 1.0f, -0.5f, glm::vec3(2.5, -6, 0));
      Util::DrawText(&program, fontTextureID, "to start", 1.0f, -0.5f, glm::vec3(3.3, -7, 0));
    }

    currentScene->state.player.Render(&program);
  
    currentScene->state.map->Render(&program);
  
    for (int i = 0; i < ENEMY_COUNT; i++){
      if (currentScene->state.enemies[i].isActive){
        currentScene->state.enemies[i].Render(&program);
      }
    }
  
    for (int j = 0; j < LIFE_COUNT; j++){
      if (currentScene->state.life[j].isActive){
        currentScene->state.life[j].Render(&program);
      }
    }

    currentScene->Render(&program);
  
    if (currentScene->state.player.numLife <= 0 || (currentScene != sceneList[0] && currentScene->state.player.isActive == false)){
      Util::DrawText(&program, fontTextureID, "You Lose", 1.0f, -0.5f, glm::vec3(currentScene->state.player.position.x - 1.5, -4, 0));
      Mix_HaltMusic();
      Mix_HaltChannel(-1);
    }
    if (currentScene == sceneList[3] && currentScene->state.player.numKill >= 2){
      Util::DrawText(&program, fontTextureID, "You Win", 1.0f, -0.5f, glm::vec3(currentScene->state.player.position.x - 1.5, -4, 0));
    }
  
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    Mix_FreeMusic(music);
    Mix_FreeChunk(bounce);
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
