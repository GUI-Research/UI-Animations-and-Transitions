#ifndef __M_ANIMATION_H__
#define __M_ANIMATION_H__

#include "Module.h"
#include <vector>
#include <list>
#include "GB_Rectangle.h"
#include "p2Point.h"
#include "j1Timer.h"

struct SDL_Texture;

struct Texture
{
public:
	Texture(SDL_Texture* tex) : texture(tex)
	{}
	~Texture() {}
	SDL_Texture* texture;
};

class Animation
{
	friend class M_Animation;
public:
	Animation(std::string name);
	~Animation();

	void SetLoop(bool loop);
	void SetSpeed(float speed);
	bool Finished() const;
	void Reset();
	GB_Rectangle<int>& GetCurrentFrame();

private:
	void CleanUp();

	iPoint& GetCurrentPivot();
	
private:
	std::string name;

	std::vector<GB_Rectangle<int>> frames;
	std::vector<iPoint> pivotPoints;

	float currentFrame;
	bool loop = true;
	int loops = 0;
	float speed = 50.0f;
	Timer animationTimer;

};

class M_Animation : public Module
{
public:
	M_Animation(bool startEnabled = true);
	virtual ~M_Animation();

	bool Awake(pugi::xml_node& none);
	bool Start();
	bool CleanUp();

	SDL_Texture* GetTexture()const;
	Animation* GetAnimation()const;
	void GetFrame(GB_Rectangle<int>& rect, iPoint& pivot);

	void DrawDebug()override;

private:
	std::vector<Texture> textures;
	std::list<Animation*> animations;

};

#endif //__M_ANIMATION_H__
