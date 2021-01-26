#include "scene0.h"

#include <SDL.h>
#include <SDL_image.h>
#include <hal/debug.h>
#include <hal/video.h>
#include <hal/xbox.h>
#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>

#include "common.h"
#include "i2c_map.h"
#include "HdmiTools.h"

extern SDL_Renderer *gRenderer;
extern TTF_Font *gFontSmall;
extern uint8_t load_scene;
extern bool running;
extern Conflux::HdmiTools* hdmi_tools;

Scene0::Scene0() {
  background_texture =
      loadTexture(gRenderer, "D:\\assets\\images\\screen0_background.png");
  arrow_texture = loadTexture(gRenderer, "D:\\assets\\images\\arrow.png");

  current_item = 0;

  // Get firmware version
  snprintf(text_buffer, sizeof(text_buffer), "Firmware Version: %s",
      hdmi_tools->GetFirmwareVersion().GetVersionCodeAsCString());

  info_line[0] = drawText(gFontSmall, font_color, text_buffer);
  SDL_QueryTexture(info_line[0], NULL, NULL,
                   &info_line_pos[0].w,
                   &info_line_pos[0].h);

  // Get kernel version
  int kernel_patch_major = hdmi_tools->GetKernelPatchVersion().GetMajor();
  int kernel_patch_minor = hdmi_tools->GetKernelPatchVersion().GetMinor();
  int kernel_patch_patch = hdmi_tools->GetKernelPatchVersion().GetPatch();
  if (kernel_patch_major || kernel_patch_minor || kernel_patch_patch) {
    snprintf(text_buffer, sizeof(text_buffer), "Kernel Patch Version: %u.%u.%u",
      kernel_patch_major, kernel_patch_minor, kernel_patch_patch);
  } else {
    snprintf(text_buffer, sizeof(text_buffer), "Kernel patch not detected!");
  }

  info_line[1] = drawText(gFontSmall, font_color, text_buffer);
  SDL_QueryTexture(info_line[1], NULL, NULL,
                   &info_line_pos[1].w,
                   &info_line_pos[1].h);
}

Scene0::~Scene0(void) {
  SDL_DestroyTexture(background_texture);
  SDL_DestroyTexture(arrow_texture);
  SDL_DestroyTexture(info_line[0]);
}

void Scene0::event(SDL_Event event) {
  int8_t button_press = proccess_event(event);

  ULONG temp_read = 0;

  switch (button_press) {
    case SDL_CONTROLLER_BUTTON_A:
      load_scene = current_item + 1;
      break;
    case SDL_CONTROLLER_BUTTON_B:
      running = false;
      break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
      current_item = MAX(current_item - 1, 0);
      secret_counter = 0;
      break;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
      if (secret_found) {
        current_item = MIN(current_item + 1, 3);
      } else {
        current_item = MIN(current_item + 1, 2);
        secret_counter++;

        if (secret_counter > 20) {
          secret_found = true;
        }
      }
      break;
    default:
      break;
  }
}

void Scene0::render(SDL_Renderer *renderer) {
  SDL_RenderCopy(gRenderer, background_texture, NULL, NULL);
  SDL_RenderCopy(gRenderer, arrow_texture, NULL, &arrow_pos[current_item]);
  SDL_RenderCopy(gRenderer, info_line[0], NULL, &info_line_pos[0]);
  SDL_RenderCopy(gRenderer, info_line[1], NULL, &info_line_pos[1]);
}
