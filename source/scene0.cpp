#include "scene0.h"

#include <SDL.h>
#include <SDL_image.h>
#include <hal/debug.h>
#include <hal/video.h>
#include <hal/xbox.h>
#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>
#include <string>

#include "common.h"
#include "i2c_map.h"
#include "HdmiTools.h"

extern SDL_Renderer *gRenderer;
extern TTF_Font *gFontSmall;
extern uint8_t load_scene;
extern bool running;
extern Conflux::HdmiTools* hdmi_tools;
extern bool start_firmware_update;
extern bool firmware_update_available;
extern bool firmware_update_started;
extern bool firmware_update_in_progress;
extern bool firmware_update_succeeded;
extern std::string current_update_process_copy;
extern std::string current_update_error_copy;
extern int firmware_update_percent;

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

  if(firmware_update_available)
  {
    snprintf(text_buffer, sizeof(text_buffer), "Press X to start FW update");
  }
  else
  {
    snprintf(text_buffer, sizeof(text_buffer), "No FW update available.");
  }
  info_line[2] = drawText(gFontSmall, font_color, text_buffer);
  SDL_QueryTexture(info_line[2], NULL, NULL,
                   &info_line_pos[2].w,
                   &info_line_pos[2].h);

  snprintf(text_buffer, sizeof(text_buffer), "None                      ");
  info_line[3] = drawText(gFontSmall, font_color, text_buffer);
  SDL_QueryTexture(info_line[3], NULL, NULL,
                   &info_line_pos[3].w,
                   &info_line_pos[3].h);
  
  snprintf(text_buffer, sizeof(text_buffer), "None                      ");
  info_line[4] = drawText(gFontSmall, font_color, text_buffer);
  SDL_QueryTexture(info_line[4], NULL, NULL,
                   &info_line_pos[4].w,
                   &info_line_pos[4].h);
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
      if(!firmware_update_in_progress)
      {
        load_scene = current_item + 1;
      }
      break;
    case SDL_CONTROLLER_BUTTON_B:
      if(!firmware_update_in_progress)
      {
        running = false;
      }
      break;
    case SDL_CONTROLLER_BUTTON_X:
      if(!firmware_update_in_progress)
      {
        start_firmware_update = true;
      }
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
  // This is not an ideal place for this 
  if(firmware_update_in_progress)
  {
    SDL_DestroyTexture(info_line[2]);
    SDL_DestroyTexture(info_line[3]);
    SDL_DestroyTexture(info_line[4]);

    snprintf(text_buffer, sizeof(text_buffer), "Process : %s", current_update_process_copy.c_str());
    info_line[2] = drawText(gFontSmall, font_color, text_buffer);
    SDL_QueryTexture(info_line[2], NULL, NULL,
                    &info_line_pos[2].w,
                    &info_line_pos[2].h);

    snprintf(text_buffer, sizeof(text_buffer), "Error : %s", current_update_error_copy.c_str());
    info_line[3] = drawText(gFontSmall, font_color, text_buffer);
    SDL_QueryTexture(info_line[3], NULL, NULL,
                    &info_line_pos[3].w,
                    &info_line_pos[3].h);

    snprintf(text_buffer, sizeof(text_buffer), "Progess : %d percent", firmware_update_percent);
    info_line[4] = drawText(gFontSmall, font_color, text_buffer);
    SDL_QueryTexture(info_line[4], NULL, NULL,
                    &info_line_pos[4].w,
                    &info_line_pos[4].h);
  }
  else if(firmware_update_succeeded)
  {
    SDL_DestroyTexture(info_line[2]);

    snprintf(text_buffer, sizeof(text_buffer), "FW update successful!!");
    info_line[2] = drawText(gFontSmall, font_color, text_buffer);
    SDL_QueryTexture(info_line[2], NULL, NULL,
                    &info_line_pos[2].w,
                    &info_line_pos[2].h);
  }

  SDL_RenderCopy(gRenderer, background_texture, NULL, NULL);
  SDL_RenderCopy(gRenderer, arrow_texture, NULL, &arrow_pos[current_item]);
  SDL_RenderCopy(gRenderer, info_line[0], NULL, &info_line_pos[0]);
  SDL_RenderCopy(gRenderer, info_line[1], NULL, &info_line_pos[1]);

  if((firmware_update_available && !firmware_update_in_progress) || firmware_update_succeeded)
  {
    SDL_RenderCopy(gRenderer, info_line[2], NULL, &info_line_pos[2]);
  }
  else if(firmware_update_in_progress)
  {
    SDL_RenderCopy(gRenderer, info_line[2], NULL, &info_line_pos[2]);
    SDL_RenderCopy(gRenderer, info_line[3], NULL, &info_line_pos[3]);
    SDL_RenderCopy(gRenderer, info_line[4], NULL, &info_line_pos[4]);
  }
}
