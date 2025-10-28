/* points.c ... */

/*
 * This example creates an SDL window and renderer, and then draws some points
 * to it every frame.
 *
 * This code is public domain. Feel free to use it for any purpose!
 */

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "gameengine.h"
#include "Factory.h"
#include "World.h"
#include "system/JoystickManager.h"
#include "InputsManager.h"
#include "GameState.h"
#include "system/Camera.h"
#include "system/Viewport.h"

 /* We will use this renderer to draw into this window every frame. */
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static Uint64 last_time = 0;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define NUM_POINTS 500
#define MIN_PIXELS_PER_SECOND 30  /* move at least this many pixels per second. */
#define MAX_PIXELS_PER_SECOND 60  /* move this many pixels per second at most. */

/* (track everything as parallel arrays instead of a array of structs,
   so we can pass the coordinates to the renderer in a single function call.) */

   /* Points are plotted as a set of X and Y coordinates.
      (0, 0) is the top left of the window, and larger numbers go down
      and to the right. This isn't how geometry works, but this is pretty
      standard in 2D graphics. */
static SDL_FPoint points[NUM_POINTS];
static float point_speeds[NUM_POINTS];

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    int i;

    SDL_SetAppMetadata("Olympe Game Engine", "2.0", "com.googlesites.olympeengine");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Olympe Engine 2.0", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    /* set up the data for a bunch of points. */
    for (i = 0; i < SDL_arraysize(points); i++) {
        points[i].x = SDL_randf() * ((float)WINDOW_WIDTH);
        points[i].y = SDL_randf() * ((float)WINDOW_HEIGHT);
        point_speeds[i] = MIN_PIXELS_PER_SECOND + (SDL_randf() * (MAX_PIXELS_PER_SECOND - MIN_PIXELS_PER_SECOND));
    }

    last_time = SDL_GetTicks();

	//Olympe Engine Elements Initialization Here
    GameEngine::Get();
    World::Get();
    InputsManager::Get().Initialize();
    Camera::Get().Initialize();
 

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    // Forward to input submanagers (they will post EventManager messages)
    JoystickManager::Get().HandleEvent(event);
    KeyboardManager::Get().HandleEvent(event);
    MouseManager::Get().HandleEvent(event);

    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
    const Uint64 now = SDL_GetTicks();
    const float elapsed = ((float)(now - last_time)) / 1000.0f;  /* seconds since last iteration */
    int i;

    /* let's move all our points a little for a new frame. */
    for (i = 0; i < SDL_arraysize(points); i++) {
        const float distance = elapsed * point_speeds[i];
        points[i].x += distance;
        points[i].y += distance;
        if ((points[i].x >= WINDOW_WIDTH) || (points[i].y >= WINDOW_HEIGHT)) {
            /* off the screen; restart it elsewhere! */
            if (SDL_rand(2)) {
                points[i].x = SDL_randf() * ((float)WINDOW_WIDTH);
                points[i].y = 0.0f;
            }
            else {
                points[i].x = 0.0f;
                points[i].y = SDL_randf() * ((float)WINDOW_HEIGHT);
            }
            point_speeds[i] = MIN_PIXELS_PER_SECOND + (SDL_randf() * (MAX_PIXELS_PER_SECOND - MIN_PIXELS_PER_SECOND));
        }
    }

    last_time = now;

    // If game state requests quit, end the application loop
    if (GameStateManager::GetState() == GameState::GameState_Quit)
    {
        return SDL_APP_SUCCESS;
    }

    // Update world properties (events already processed inside World::Process)
    World::Get().Process(elapsed);

    /* as you can see from this, rendering draws over whatever was drawn before it. */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  /* black, full alpha */
    SDL_RenderClear(renderer);  /* start with a blank canvas. */

    // Multi-viewport rendering: for each viewport, set SDL viewport and draw scene
    const auto& rects = Viewport::Get().GetViewRects();
    const auto& players = Viewport::Get().GetPlayers();

    if (rects.empty())
    {
        // fallback single full render
        SDL_SetRenderViewport(renderer, nullptr);
        Camera::Get().Apply(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderPoints(renderer, points, SDL_arraysize(points));
    }
    else
    {
        // for each viewport: transform points according to the camera assigned to the player (if any)
        for (size_t vi = 0; vi < rects.size(); ++vi)
        {
            const SDL_Rect& vr = rects[vi];
            SDL_SetRenderViewport(renderer, &vr);

            short playerID = 0;
            if (vi < players.size()) playerID = players[vi];

            auto cam = Camera::Get().GetCameraForPlayer(playerID);

            // transform points to camera space (top-left world coordinates stored in cam.x/cam.y)
            static SDL_FPoint transformed[NUM_POINTS];
            for (int p = 0; p < NUM_POINTS; ++p)
            {
                float tx = (points[p].x - cam.x) * cam.zoom; // apply zoom
                float ty = (points[p].y - cam.y) * cam.zoom;
                transformed[p].x = tx;
                transformed[p].y = ty;
            }

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderPoints(renderer, transformed, SDL_arraysize(points));
        }

        // reset viewport to full for any UI overlays
        SDL_SetRenderViewport(renderer, nullptr);
    }

    SDL_RenderPresent(renderer);  /* put it all on the screen! */

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
    InputsManager::Get().Shutdown();
    Camera::Get().Shutdown();
}