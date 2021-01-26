#ifndef SCENE0_H_
#define SCENE0_H_

#if _XBOX
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "scene.h"

class Scene0 : public Scene {
 public:
  Scene0();
  virtual ~Scene0();
  void event(SDL_Event event);
  void render(SDL_Renderer *renderer);

 private:
  bool findKernelPatchVersion(uint8_t *version);

  uint8_t current_item = 0;
  uint8_t secret_counter = 0;
  bool secret_found = false;

  SDL_Texture *background_texture;
  SDL_Texture *arrow_texture;
  const SDL_Color font_color = {0xFF, 0xFF, 0xFF, 0xFF};

  SDL_Texture *info_line[5] = { NULL };
  SDL_Rect info_line_pos[5] = {
      {80, 420 - (24 * 0), 0, 0},  // Firmware Version
      {80, 420 - (24 * 1), 0, 0},  // BIOS Patch Version
      {80, 420 - (24 * 2), 0, 0},  // Start update / update progress
      {80, 420 - (24 * 3), 0, 0},  // Errors
      {80, 420 - (24 * 4), 0, 0},  // Completion percentage
  };

  const SDL_Rect arrow_pos[4] = {
      {406, 190 + (40 * 0), 24, 16},  // Self Test
      {406, 190 + (40 * 1), 24, 16},  // Advance
      {406, 190 + (40 * 2), 24, 16},  // Firemware Update
      {532, 406, 24, 16}              // Secret Menu
  };

  uint8_t firmware_version[3] = { 0 };
  uint8_t kernel_patch_version[3] = { 0 };

  char text_buffer[100] = { 0 };
};

#endif
