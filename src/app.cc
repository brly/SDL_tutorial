#include <SDL.h>
#include <SDL_opengl.h>
#include <OpenGL/glu.h>
#include <stdexcept>
#include <cstdio>
#include <cmath>
#include <lua.hpp>

enum Constant
{
  kScreenWidth = 640,
  kScreenHeight = 480
};

struct SDLContext
{
  explicit SDLContext() : window(nullptr), surface(nullptr)
  {
  }

  void init()
  {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      printf("SDL could not initialize! %s\n", SDL_GetError());
      throw std::runtime_error("1");
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow("SDL Tutorial",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      kScreenWidth,
      kScreenHeight,
      SDL_WINDOW_OPENGL);

    if (window == nullptr)
    {
      printf("Window could not be created! %s\n", SDL_GetError());
      throw std::runtime_error("2");
    }

    context = SDL_GL_CreateContext(window);

    L = luaL_newstate();
    luaL_dofile(L, "app.lua");

    if (context == nullptr)
    {
      throw std::runtime_error("3");
    }

    glViewport(0, 0, kScreenWidth, kScreenHeight);
    glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    // setup projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLdouble) kScreenWidth / (GLdouble) kScreenHeight, 2.0, 200.0);
    // setup light
    static GLfloat position[] = {-10.0f, 10.0f, 10.0f, 1.0f};
    static GLfloat ambient [] = { 1.0f, 1.0f, 1.0f, 1.0f};
    static GLfloat diffuse [] = { 1.0f, 1.0f, 1.0f, 1.0f};
    static GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
  }

  void draw()
  {
    // clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // setup view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt( 0.0f,  0.0f,-150.0f,
               0.0f,  0.0f, 0.0f,
               0.0f, -1.0f, 0.0f);
    //
    glPushMatrix();
    glBegin(GL_LINES);
    glVertex3f(-100, 0, 0);
    glVertex3f(100, 0, 0);
    glEnd();
    glPopMatrix();

    // setup material
    glPushMatrix();
    GLfloat ambient  [] = { 0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat diffuse  [] = { 1.0f, 0.0f, 0.0f, 1.0f};
    GLfloat specular [] = { 1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat shininess[] = { 0.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

    // draw sphere
    GLUquadric* quadric = gluNewQuadric();
    glTranslatef(temp_x, temp_y, 0.f);
    gluSphere(quadric, 1.0, 30, 30);
    glPopMatrix();

    gluDeleteQuadric(quadric);
    glPopMatrix();

    // draw fireworks
    int N = 20;
    int unitTheta = 360.0 / N;
    for(int d=0; d<20; d++)
    {
      for(int i=0;i<N;i++)
      {
        glPushMatrix();
        glRotatef(i * unitTheta, 0, 0, 1);
        glTranslatef(radius + d, 0, 0);
        //gluSphere(quadric, 1.0, 30, 30);
        glBegin(GL_TRIANGLES);
        glColor3ub(0xFF , 0 , 0);
      		glVertex2f(0 , 0);
      		glColor3f(0 , 0 , 1);
      		glVertex2f(-1 , 0.9);
      		glVertex2f(1 , 0.9);
        glEnd();
        glPopMatrix();
      }
    }

    SDL_GL_SwapWindow(window);
  }

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
    printf("t=%lld\n", dt);
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

int main()
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
  return 0;
}
