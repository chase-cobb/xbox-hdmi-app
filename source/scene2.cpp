#include <stdio.h>
#include <stdlib.h>
#ifdef _XBOX
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <hal/debug.h>
#include <hal/video.h>
#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

#include "common.h"
#include "i2c_map.h"
#include "scene2.h"
#include "HdmiTools.h"

extern SDL_Window *window;
extern SDL_Renderer *gRenderer;
extern TTF_Font *gFont;
extern Conflux::HdmiTools* hdmi_tools;

extern uint8_t load_scene;

// Advance Settings
Scene2::Scene2() {
#ifdef _XBOX
  background_texture =
      loadTexture(gRenderer, "D:\\assets\\backgrounds\\test_image_0.png");
  menu_texture = loadTexture(gRenderer, "D:\\assets\\images\\video_adjust.png");

  box_unselected_texture =
      loadTexture(gRenderer, "D:\\assets\\images\\box_unselected.png");
  box_selected_texture =
      loadTexture(gRenderer, "D:\\assets\\images\\box_selected.png");
  box_highlight_texture =
      loadTexture(gRenderer, "D:\\assets\\images\\box_highlight.png");

  bar_unselected_texture =
      loadTexture(gRenderer, "D:\\assets\\images\\bar_unselected.png");
  bar_selected_texture =
      loadTexture(gRenderer, "D:\\assets\\images\\bar_selected.png");
#else
  background_texture =
      loadTexture(gRenderer, "assets/backgrounds/test_image_0.png");
  menu_texture = loadTexture(gRenderer, "assets/images/video_adjust.png");

  box_unselected_texture =
      loadTexture(gRenderer, "assets/images/box_unselected.png");
  box_selected_texture =
      loadTexture(gRenderer, "assets/images/box_selected.png");
  box_highlight_texture =
      loadTexture(gRenderer, "assets/images/box_highlight.png");

  bar_unselected_texture =
      loadTexture(gRenderer, "assets/images/box_unselected.png");
  bar_selected_texture =
      loadTexture(gRenderer, "assets/images/box_selected.png");
#endif

  SDL_Surface *surface =
      SDL_CreateRGBSurface(0, menu_pos.w, menu_pos.h, 32, 0, 0, 0, 0);
  menu_backdrop = SDL_CreateTextureFromSurface(gRenderer, surface);
  SDL_FreeSurface(surface);

  SDL_SetTextureBlendMode(menu_backdrop, SDL_BLENDMODE_BLEND);
  SDL_SetTextureAlphaMod(menu_backdrop, 150);

  // Conflux loads feature configuration whenever
  // it is initialized... so no need to do it again.
}

Scene2::~Scene2(void) {
  SDL_DestroyTexture(background_texture);
  SDL_DestroyTexture(menu_backdrop);
}

void Scene2::updateConfig() {
#ifdef _XBOX
  hdmi_tools->UpdateFeatureConfig();
#endif
}

void Scene2::saveConfig() {
#ifdef _XBOX
  hdmi_tools->SaveSettings();
#endif
}

void Scene2::event(SDL_Event event) {
  switch (event.type) {
    case SDL_QUIT:
    case SDL_JOYHATMOTION:
      // D-Pad
      if (event.jhat.hat == 0) {
        if (event.jhat.value == SDL_HAT_UP) {
          if (cursor_pos == 3 || cursor_pos == 4) {
            cursor_pos = 0;
          } else {
            cursor_pos = MAX(cursor_pos - 1, 0);
          }
        }
        if (event.jhat.value == SDL_HAT_DOWN) {
          if (cursor_pos >= 0 && cursor_pos < 3) {
            cursor_pos = 3;
          } else if (cursor_pos == 3 || cursor_pos == 4) {
            cursor_pos = 5;
          } else {
            cursor_pos = MIN(cursor_pos + 1, 7);
          }
        }
        if (event.jhat.value == SDL_HAT_LEFT) {
          if (cursor_pos > 0 && cursor_pos < 5) {
            cursor_pos--;
          } else {
            // Video Luma
            if (cursor_pos == 5) {
              int current_value = hdmi_tools->GetConstFeatureMap().at(Conflux::SupportedFeatures::LUMA_ADJUST)->GetValue();
              hdmi_tools->SetFeatureValue(Conflux::SupportedFeatures::LUMA_ADJUST, --current_value);
              text_dirty = true;
            }
            // Video Cb
            if (cursor_pos == 6) {
              int current_value = hdmi_tools->GetConstFeatureMap().at(Conflux::SupportedFeatures::CB_ADJUST)->GetValue();
              hdmi_tools->SetFeatureValue(Conflux::SupportedFeatures::CB_ADJUST, --current_value);
              text_dirty = true;
            }
            // Video Cr
            if (cursor_pos == 7) {
              int current_value = hdmi_tools->GetConstFeatureMap().at(Conflux::SupportedFeatures::CR_ADJUST)->GetValue();
              hdmi_tools->SetFeatureValue(Conflux::SupportedFeatures::CR_ADJUST, --current_value);
              text_dirty = true;
            }
            if (cursor_pos == 5 || cursor_pos == 6 || cursor_pos == 7) {
              if (video_adjust_luma != 0 || video_adjust_cb != 0 ||
                  video_adjust_cr != 0) {
                hdmi_tools->SetFeatureValue(Conflux::SupportedFeatures::VIDEO_MODE_ADJUST, 1);
              }
              updateConfig();
            }
          }
        }
        if (event.jhat.value == SDL_HAT_RIGHT) {
          if (cursor_pos >= 0 && cursor_pos < 5) {
            cursor_pos++;
          } else {
            // Video Luma
            if (cursor_pos == 5) {
              int current_value = hdmi_tools->GetConstFeatureMap().at(Conflux::SupportedFeatures::LUMA_ADJUST)->GetValue();
              hdmi_tools->SetFeatureValue(Conflux::SupportedFeatures::LUMA_ADJUST, ++current_value);
              text_dirty = true;
            }
            // Video Cb
            if (cursor_pos == 6) {
              int current_value = hdmi_tools->GetConstFeatureMap().at(Conflux::SupportedFeatures::CB_ADJUST)->GetValue();
              hdmi_tools->SetFeatureValue(Conflux::SupportedFeatures::CB_ADJUST, ++current_value);
              text_dirty = true;
            }
            // Video Cr
            if (cursor_pos == 7) {
              int current_value = hdmi_tools->GetConstFeatureMap().at(Conflux::SupportedFeatures::CR_ADJUST)->GetValue();
              hdmi_tools->SetFeatureValue(Conflux::SupportedFeatures::CR_ADJUST, ++current_value);
              text_dirty = true;
            }
            if (cursor_pos == 5 || cursor_pos == 6 || cursor_pos == 7) {
              if (video_adjust_luma != 0 || video_adjust_cb != 0 ||
                  video_adjust_cr != 0) {
                hdmi_tools->SetFeatureValue(Conflux::SupportedFeatures::VIDEO_MODE_ADJUST, 1);
              }
              updateConfig();
            }
          }
        }
      }
      break;
    case SDL_JOYBUTTONDOWN:
      // A Button
      if (event.jbutton.button == 0) {
        if (cursor_pos == 0 || cursor_pos == 1 || cursor_pos == 2) {
          hdmi_tools->SetFeatureValue(Conflux::SupportedFeatures::WIDESCREEN_ADJUST, cursor_pos);
          updateConfig();
        }
        if (cursor_pos == 3) {
          hdmi_tools->SetFeatureValue(Conflux::SupportedFeatures::VIDEO_MODE_ADJUST, 0);
          hdmi_tools->SetFeatureValue(Conflux::SupportedFeatures::LUMA_ADJUST, 0);
          hdmi_tools->SetFeatureValue(Conflux::SupportedFeatures::CB_ADJUST, 0);
          hdmi_tools->SetFeatureValue(Conflux::SupportedFeatures::CR_ADJUST, 0);

          text_dirty = true;
          updateConfig();
        }
        if (cursor_pos == 4) {
          hdmi_tools->SetFeatureValue(Conflux::SupportedFeatures::VIDEO_MODE_ADJUST, 1);
          updateConfig();
        }
      }

      // B Button - Go back to main menu
      if (event.jbutton.button == 1) {
        if (show_help) {
          SDL_DestroyTexture(background_texture);
          background_texture = loadTexture(
              gRenderer, "D:\\assets\\backgrounds\\test_image_0.png");

          show_help = false;
        } else {
          saveConfig();
          load_scene = 0;
        }
      }

      // X Button - Show help
      if (event.jbutton.button == 2) {
        SDL_DestroyTexture(background_texture);
        background_texture = loadTexture(
            gRenderer, "D:\\assets\\images\\screen2_background.png");

        show_help = true;
      }
      break;
    default:
      break;
  }
}

void Scene2::recalcSliders(void) {
  // Delete old textures
  if (texture_video_luma) {
    SDL_DestroyTexture(texture_video_luma);
    SDL_DestroyTexture(texture_video_cb);
    SDL_DestroyTexture(texture_video_cr);
  }

  // Draw new textures for values
  char video_luma_text[12];
  char video_cb_text[12];
  char video_cr_text[12];

  video_adjust_cr = hdmi_tools->GetConstFeatureMap().at(Conflux::SupportedFeatures::CR_ADJUST)->GetValue();
  video_adjust_cb = hdmi_tools->GetConstFeatureMap().at(Conflux::SupportedFeatures::CB_ADJUST)->GetValue();
  video_adjust_luma = hdmi_tools->GetConstFeatureMap().at(Conflux::SupportedFeatures::LUMA_ADJUST)->GetValue();
  video_widescreen = hdmi_tools->GetConstFeatureMap().at(Conflux::SupportedFeatures::WIDESCREEN_ADJUST)->GetValue();
  video_mode_out = hdmi_tools->GetConstFeatureMap().at(Conflux::SupportedFeatures::VIDEO_MODE_ADJUST)->GetValue();

  sprintf(video_luma_text, "%d", video_adjust_luma);
  sprintf(video_cb_text, "%d", video_adjust_cb);
  sprintf(video_cr_text, "%d", video_adjust_cr);

  texture_video_luma = drawText(gFont, font_color, video_luma_text);
  texture_video_cb = drawText(gFont, font_color, video_cb_text);
  texture_video_cr = drawText(gFont, font_color, video_cr_text);

  SDL_QueryTexture(texture_video_luma, NULL, NULL, &video_luma_pos.w,
                   &video_luma_pos.h);
  SDL_QueryTexture(texture_video_cb, NULL, NULL, &video_cb_pos.w,
                   &video_cb_pos.h);
  SDL_QueryTexture(texture_video_cr, NULL, NULL, &video_cr_pos.w,
                   &video_cr_pos.h);

  // Adjust text position
  video_luma_pos.x = 678 - video_luma_pos.w;
  video_cb_pos.x = 678 - video_cb_pos.w;
  video_cr_pos.x = 678 - video_cr_pos.w;

  // Adjust slider bar position
  video_luma_slider_pos.x = 514 + (video_adjust_luma * 8);
  video_cb_slider_pos.x = 514 + (video_adjust_cb * 8);
  video_cr_slider_pos.x = 514 + (video_adjust_cr * 8);
}

void Scene2::render(SDL_Renderer *renderer) {
  if (text_dirty) {
    recalcSliders();
  }

  SDL_RenderCopy(gRenderer, background_texture, NULL, NULL);

  if (!show_help) {
    SDL_RenderCopy(gRenderer, menu_backdrop, NULL, &menu_pos);
    SDL_RenderCopy(gRenderer, menu_texture, NULL, &menu_pos);

    // Draw 4:3 Widescreen boxes
    SDL_RenderCopy(
        gRenderer,
        video_widescreen == 0 ? box_selected_texture : box_unselected_texture,
        NULL, &box_wide0_pos);
    SDL_RenderCopy(
        gRenderer,
        video_widescreen == 1 ? box_selected_texture : box_unselected_texture,
        NULL, &box_wide1_pos);
    SDL_RenderCopy(
        gRenderer,
        video_widescreen == 2 ? box_selected_texture : box_unselected_texture,
        NULL, &box_wide2_pos);

    // Draw Video Mode boxes
    SDL_RenderCopy(
        gRenderer,
        video_mode_out ? box_unselected_texture : box_selected_texture, NULL,
        &box_video0_pos);
    SDL_RenderCopy(
        gRenderer,
        video_mode_out ? box_selected_texture : box_unselected_texture, NULL,
        &box_video1_pos);

    // Draw slider values
    SDL_RenderCopy(gRenderer, texture_video_luma, NULL, &video_luma_pos);
    SDL_RenderCopy(gRenderer, texture_video_cb, NULL, &video_cb_pos);
    SDL_RenderCopy(gRenderer, texture_video_cr, NULL, &video_cr_pos);

    //
    SDL_RenderCopy(
        gRenderer,
        cursor_pos == 5 ? bar_selected_texture : bar_unselected_texture, NULL,
        &video_luma_slider_pos);
    SDL_RenderCopy(
        gRenderer,
        cursor_pos == 6 ? bar_selected_texture : bar_unselected_texture, NULL,
        &video_cb_slider_pos);
    SDL_RenderCopy(
        gRenderer,
        cursor_pos == 7 ? bar_selected_texture : bar_unselected_texture, NULL,
        &video_cr_slider_pos);

    if (cursor_pos == 0) {
      SDL_RenderCopy(gRenderer, box_highlight_texture, NULL,
                     &box_wide0_selected_pos);
    }
    if (cursor_pos == 1) {
      SDL_RenderCopy(gRenderer, box_highlight_texture, NULL,
                     &box_wide1_selected_pos);
    }
    if (cursor_pos == 2) {
      SDL_RenderCopy(gRenderer, box_highlight_texture, NULL,
                     &box_wide2_selected_pos);
    }

    if (cursor_pos == 3) {
      SDL_RenderCopy(gRenderer, box_highlight_texture, NULL,
                     &box0_selected_pos);
    }
    if (cursor_pos == 4) {
      SDL_RenderCopy(gRenderer, box_highlight_texture, NULL,
                     &box1_selected_pos);
    }
  }
}
