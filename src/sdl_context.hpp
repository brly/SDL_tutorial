#ifndef SDL_CONTEXT_HPP
#define SDL_CONTEXT_HPP

#include <SDL.h>
#include <SDL_opengl.h>
#include <lua.hpp>
#include <OpenGL/glu.h>
#include <stdexcept>
#include <cstdio>
#include <cmath>

#include "constants.hpp"

struct SDLContext
{
  explicit SDLContext() : window(nullptr), surface(nullptr) {}

  void init();
  void draw();

  void dispose()
  {
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  bool pollingEvent()
  {
    SDL_Event event;
    SDL_Keycode key;
    while (SDL_PollEvent(&event))
    {
      switch (event.type) {
        case SDL_QUIT:
          return false;
          break;
        case SDL_KEYDOWN: {
          key = event.key.keysym.sym;
          // ESC
          if (key == SDLK_ESCAPE) {
            return false;
          }
          if (key == SDLK_r) {
            luaL_dofile(L, "app.lua");
          }
          break;
        }
      }
    }
    return true;
  }

  void update()
  {
    dt++;
    if (std::fabs(temp_y) > 50) {
      temp_y = 50;
    }
    radius = dt % 100;
    temp_y -= 1;

    lua_getglobal(L, "update");
    lua_pcall(L, 0, 2, 0);
    int first = lua_tointeger(L, -1);
    int second = lua_tointeger(L, -2);
    lua_pop(L, 2);

    int value = (std::fabs(temp_y)) * first;
    temp_x = sin(value * (3.14 / 180)) * second;
    //printf("t=%lld\n", dt);
  }

  // member
  SDL_Window *window;
  SDL_Surface *surface;
  SDL_GLContext context;
  lua_State* L;

  long long dt;
  double temp_x, temp_y;
  double radius;
};


#endif
