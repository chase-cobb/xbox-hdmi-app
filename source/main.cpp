#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef _XBOX
#include <SDL.h>
#include <SDL_image.h>
#include <hal/debug.h>
#include <hal/video.h>
#include <hal/xbox.h>
#include <windows.h>
#include <mutex>
#include "HdmiTools.h"
#define FONT_PATH "D:\\assets\\fonts\\RobotoMono-Regular.ttf"
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#define FONT_PATH "assets/fonts/RobotoMono-Regular.ttf"
#endif

#include "common.h"
#include "scene.h"
#include "scene0.h"
#include "scene1.h"
#include "scene2.h"
#include "scene3.h"
#include "scene4.h"

static void printSDLErrorAndReboot(void) {
  debugPrint("SDL_Error: %s\n", SDL_GetError());
  debugPrint("Rebooting in 5 seconds.\n");
  Sleep(5000);
  XReboot();
}

static void printIMGErrorAndReboot(void) {
  debugPrint("SDL_Image Error: %s\n", IMG_GetError());
  debugPrint("Rebooting in 5 seconds.\n");
  Sleep(5000);
  XReboot();
}

#ifdef _XBOX
// Screen dimension constants
// const extern int SCREEN_WIDTH;
// const extern int SCREEN_HEIGHT;
int SCREEN_WIDTH = 720;
int SCREEN_HEIGHT = 480;
#else
int SCREEN_WIDTH = 720;
int SCREEN_HEIGHT = 480;
#endif

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

SDL_Renderer *gRenderer = NULL;

TTF_Font *gFont = NULL;
TTF_Font *gFontSmall = NULL;
SDL_Window *window = NULL;

uint8_t current_scene = 0;
uint8_t load_scene = 0;

bool running = true;

Conflux::HdmiTools* hdmi_tools;

std::mutex mutex_object;

bool firmware_update_available = false;
bool start_firmware_update = false;
bool firmware_update_started = false;
bool firmware_update_in_progress = false;
bool firmware_update_succeeded = false;

std::string current_update_process;
std::string current_update_error;
int firmware_update_percent;

// These are to provide some thread safety while
// externing variables
std::string current_update_process_copy;
std::string current_update_error_copy;

void progressUpdate(const char* progress_update);
void percentComplete(int percent_complete);
void errorMessage(const char* error_message);
void firmwareUpdateComplete(bool flash_successful);

int main(void) {
#ifdef _XBOX
  XVideoSetMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, REFRESH_DEFAULT);

  // Initialize Conflux and check for firmware update
  // in the working directory.
  hdmi_tools = Conflux::HdmiTools::GetInstance();
  if (hdmi_tools->Initialize()) {
    firmware_update_available = hdmi_tools->IsUpdateAvailable(Conflux::UpdateSource::WORKING_DIRECTORY);
    current_update_process = "";
    current_update_error = "";
    firmware_update_percent = 0;
  }
#endif

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
    debugPrint("SDL_VideoInit failed: %s", SDL_GetError());
    Sleep(2000);
    return 0;
  }

  // Load joystick
  SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

  if (SDL_NumJoysticks() < 1) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Couldn't find any joysticks.\n");
    printSDLErrorAndReboot();
  } else {
    SDL_GameController *controller = NULL;
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
      if (SDL_IsGameController(i)) {
        controller = SDL_GameControllerOpen(i);
        if (controller == nullptr) {
          printSDLErrorAndReboot();
        }
      }
    }
  }

  if (TTF_Init() != 0) {
    debugPrint("TTF_Init failed: %s", TTF_GetError());
    Sleep(2000);
    return 0;
  }

  window = SDL_CreateWindow("XboxHDMI", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                            SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    debugPrint("Window creation failed: %s", SDL_GetError());
    Sleep(2000);
    return 0;
  }

  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Couldn't intialize SDL_image.\n");
    SDL_VideoQuit();
    printIMGErrorAndReboot();
  }

  gRenderer = SDL_CreateRenderer(window, -1, 0);
  if (gRenderer == NULL) {
    debugPrint("CreateRenderer failed: %s\n", SDL_GetError());
    Sleep(2000);
    return 0;
  }

// Load font
  gFont = TTF_OpenFont(FONT_PATH, 26);
  if (gFont == NULL) {
    debugPrint("Couldn't load font: %s", TTF_GetError());
    Sleep(2000);
    return 0;
  }
  gFontSmall = TTF_OpenFont(FONT_PATH, 20);
  if (gFontSmall == NULL) {
    debugPrint("Couldn't load font: %s", TTF_GetError());
    Sleep(2000);
    return 0;
  }

  // Main render loop
  Scene *currentScene = new Scene0();

  while (running) {
#ifdef _XBOX
    XVideoWaitForVBlank();
#endif

    if (load_scene != current_scene) {
      switch (load_scene) {
        case 0:
          delete currentScene;
          currentScene = new Scene0();
          current_scene = load_scene;
          break;
        case 1:
          delete currentScene;
          currentScene = new Scene1();
          current_scene = load_scene;
          break;
        case 2:
          delete currentScene;
          currentScene = new Scene2();
          current_scene = load_scene;
          break;
        case 3:
          delete currentScene;
          currentScene = new Scene3();
          current_scene = load_scene;
          break;
        case 4:
          delete currentScene;
          currentScene = new Scene4();
          current_scene = load_scene;
          break;
        default:
          load_scene = 0;
          break;
      }
    }

    if (firmware_update_available && start_firmware_update)
    {
      start_firmware_update = false;
      firmware_update_started = firmware_update_in_progress = true;
      hdmi_tools->UpdateFirmware(Conflux::UpdateSource::WORKING_DIRECTORY, (*progressUpdate),
                                                                           (*percentComplete),
                                                                           (*errorMessage),
                                                                           (*firmwareUpdateComplete));

    }

    if(firmware_update_in_progress)
    {
      mutex_object.lock();
        current_update_process_copy = std::string(current_update_process);
        current_update_error_copy = std::string(current_update_error);
      mutex_object.unlock();
    }

    // Check for events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          running = false;
          break;
      }

      currentScene->event(event);
    }

    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(gRenderer);

    currentScene->render(gRenderer);

    SDL_RenderPresent(gRenderer);
  }

  return 0;
}


void progressUpdate(const char* progress_update)
{
  mutex_object.lock();
    current_update_process = std::string(progress_update);
  mutex_object.unlock();
}
void percentComplete(int percent_complete)
{
  firmware_update_percent = percent_complete;
}
void errorMessage(const char* error_message)
{
  mutex_object.lock();
    current_update_error = error_message;
  mutex_object.unlock();
}
void firmwareUpdateComplete(bool flash_successful)
{
  firmware_update_succeeded = flash_successful;
  firmware_update_in_progress = false;
  firmware_update_percent = 0;
  mutex_object.lock();
    current_update_process = "";
    current_update_error = "";
    current_update_process_copy = "";
    current_update_error_copy = "";
  mutex_object.unlock();
}