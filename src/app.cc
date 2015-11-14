#include "app.hpp"
#include "sdl_context.hpp"

App::App() {}

void App::start()
{
  SDLContext sdl;
  sdl.init();
  while (true)
  {
    if (!sdl.pollingEvent()) break;
    sdl.draw();
    sdl.update();
    SDL_Delay(33);
  }
  sdl.dispose();
}
