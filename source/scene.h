#ifndef SCENE_H_
#define SCENE_H_

#if _XBOX
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

class Scene {
 public:
  Scene(){} = 0; // Keep class pure virtual
  virtual ~Scene(){};
  virtual void event(SDL_Event event){};
  virtual void render(SDL_Renderer *renderer){};

 protected:
 private:
};

#endif
