#include "Log.h"
#include "App.h"
#include "M_Window.h"
#include "M_Render.h"

#include "M_Map.h"
#include "M_GUI.h"
#include "M_Animation.h"

//TEMP
#include "M_Textures.h"
#include <algorithm>

#define VSYNC true

M_Render::M_Render(bool startEnabled) : Module(startEnabled)
{
	name.assign("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
M_Render::~M_Render()
{}

// Called before render is available
bool M_Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if(config.child("vsync").attribute("value").as_bool(true) == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}

	renderer = SDL_CreateRenderer(app->win->window, -1, flags);

	if(renderer == nullptr)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = app->win->screenSurface->w;
		camera.h = app->win->screenSurface->h;
		camera.x = 0;
		camera.y = 0;
	}

	return ret;
}

// Called before the first frame
bool M_Render::Start()
{
	LOG("render start");
	//Top right viewport
	if (app->editor == true)
	{
		editorViewPort = app->win->GetWindowSize().GetSDLrect();
		gameViewPort = app->win->GetWindowSize().GetSDLrect();

		gameViewPort.y = 0;
		gameViewPort.w = gameViewPort.w / 1.5f;
		gameViewPort.h = gameViewPort.h / 1.5f;
		gameViewPort.x = (editorViewPort.w - gameViewPort.w)/2;
		game_tex_background = app->tex->Load("gui/TestingTexture.png");
		editor_tex_background = app->tex->Load("gui/TestingTexture_Green.png");
	}
	else
	{
		SDL_RenderGetViewport(renderer, &gameViewPort);
	}
	
	
	//Render texture to screen
	
	// back background
	
	return true;
}

// Called each loop iteration
update_status M_Render::PreUpdate(float dt)
{
	SDL_RenderClear(renderer);
	return UPDATE_CONTINUE;
}

update_status M_Render::PostUpdate(float dt)
{
	
	
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool M_Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

// Load Game State
bool M_Render::Load(pugi::xml_node& data)
{
	camera.x = data.child("camera").attribute("x").as_int();
	camera.y = data.child("camera").attribute("y").as_int();

	return true;
}

// Save Game State
bool M_Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;

	return true;
}

void M_Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void M_Render::DrawDebug()
{
}

void M_Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void M_Render::ResetViewPort()
{
	//SDL_RenderSetViewport(renderer, &viewport);
}

iPoint M_Render::ScreenToWorld(int x, int y) const
{
	iPoint ret;
	int scale = app->win->GetScale();

	ret.x = (x - camera.x / scale);
	ret.y = (y - camera.y / scale);

	return ret;
}

// Blit to screen
bool M_Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, float speed, double angle, int pivotX, int pivotY) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * scale;
	rect.y = (int)(camera.y * speed) + y * scale;

	if(section != nullptr)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, nullptr, nullptr, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = nullptr;
	SDL_Point pivot;

	if(pivotX != INT_MAX && pivotY != INT_MAX)
	{
		pivot.x = pivotX;
		pivot.y = pivotY;
		p = &pivot;
	}

	if(SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

/**
	Blit2: Render a texture just like blit but with the capacity of resize the source image.

	Parameters:
		-SDL_Texture* texture: Texture to render.
		-GB_Rectangle<int> destinationRectangle: Rectangle that contains the desired position and size of the resulting texture.
		-GB_Rectangle<int>* section: Rectangle with the section of the source texture you want to blit. If null takes the full size.
		-...
*/
bool M_Render::Blit2(SDL_Texture* texture, GB_Rectangle<int> destinationRectangle, const GB_Rectangle<int>* section = nullptr, float speed = 1.0f, double angle = 0, int pivotX, int pivotY) const
{
	bool ret = true;

	uint scale = app->win->GetScale();

	SDL_Rect dRect;
	dRect.x = (int)(camera.x * speed) + destinationRectangle.x * scale;
	dRect.y = (int)(camera.y * speed) + destinationRectangle.y * scale;

	SDL_Rect sect;

	if (section == nullptr)
	{
		SDL_QueryTexture(texture, nullptr, nullptr, &dRect.w, &dRect.h);
		sect.x = 0; sect.y = 0;
		sect.w = dRect.w; sect.h = dRect.h;
	}
	else
	{
		sect.x = section->x;
		sect.y = section->y;
		sect.w = section->w;
		sect.h = section->h;
	}

	dRect.w *= scale;
	dRect.h *= scale;

	SDL_Point* p = nullptr;
	SDL_Point pivot;

	if (pivotX != INT_MAX && pivotY != INT_MAX)
	{
		pivot.x = pivotX;
		pivot.y = pivotY;
		p = &pivot;
	}

	if (SDL_RenderCopyEx(renderer, texture, &sect, &dRect, angle, p, SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool M_Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool useCamera, bool useGameViewPort) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(useCamera)
	{
		rec.x = (int)(camera.x + rect.x - (useGameViewPort == true ? gameViewPort.x : 0)  * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool M_Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool useCamera, bool useGameViewPort) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	int realPosX1 = x1 - (useGameViewPort == true ? gameViewPort.x : 0);
	int realPosX2 = x2 - (useGameViewPort == true ? gameViewPort.x : 0);

	if(useCamera)
		result = SDL_RenderDrawLine(renderer, camera.x + realPosX1 * scale, camera.y + y1 * scale, camera.x + realPosX2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, realPosX1 * scale, y1 * scale, realPosX2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw line to screen. SDL_RenderDrawLine error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool M_Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool useCamera, bool useGameViewPort) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	int realPosX = x - (useGameViewPort == true ? gameViewPort.x : 0);

	for(uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(realPosX + radius * cos(i * factor));
		points[i].y = (int)(y + radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw circle to screen. SDL_RenderDrawPoints error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

