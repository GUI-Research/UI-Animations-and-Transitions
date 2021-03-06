#include "M_Animation.h"
#include "M_Textures.h"
#include "App.h"
#include "M_FileSystem.h"
#include "M_Textures.h"

#include "Log.h"

M_Animation::M_Animation(bool startEnabled) : Module(startEnabled)
{
	LOG("Animation: Creation.");
	name.assign("animation");
}

M_Animation::~M_Animation() {
	LOG("Animation: Destroying.");
}

bool M_Animation::Awake(pugi::xml_node& none)
{
	LOG("Animation: Awake.");
	bool ret = true;

	std::string animationInfo = "animation/Units_data.xml"; //TODO: Folder with xml info on animations

	char* buff = nullptr;
	int size = app->fs->Load(animationInfo.c_str(), &buff);
	pugi::xml_document animationData;
	pugi::xml_parse_result result = animationData.load_buffer(buff, size);
	RELEASE(buff);

	if (result == NULL) 
	{
		LOG("Animation: Failed loading at awake: %s", result.description());
		return false;
	}

	//LOAD UNIT ANIMATIONS FROM XML

	pugi::xml_node node = animationData.child("Sprites").first_child();
	while (node != NULL)
	{
		pugi::xml_node actionNode = node.first_child();
		while (actionNode != NULL)
		{
			pugi::xml_node directionNode = actionNode.first_child();
			while (directionNode != NULL)
			{
				Animation* newAnim = new Animation(node.name());
				pugi::xml_node spriteNode = directionNode.first_child();
				while (spriteNode != NULL)
				{
					newAnim->frames.push_back({spriteNode.attribute("x").as_int(), spriteNode.attribute("y").as_int(), spriteNode.attribute("w").as_int(), spriteNode.attribute("h").as_int()});

					float pX = spriteNode.attribute("w").as_int() * spriteNode.attribute("pX").as_float();
					float pY = spriteNode.attribute("h").as_int() * spriteNode.attribute("pY").as_float();
					pX = (pX > (floor(pX) + 0.5f)) ? ceil(pX) : floor(pX);
					pY = (pY > (floor(pY) + 0.5f)) ? ceil(pY) : floor(pY);
					newAnim->pivotPoints.push_back({(int)pX, (int)pY});

					spriteNode = spriteNode.next_sibling();
				}

				std::string animName = node.name();
				newAnim->name = animName + "_" + actionNode.name() + "_" + directionNode.name();

				std::string action = actionNode.name();
				if (!action.compare("disappear"))
				{
					newAnim->speed = 1000.0f;
					newAnim->loop = false;
				}
				animations.push_back(newAnim);

				directionNode = directionNode.next_sibling();
			}
			actionNode = actionNode.next_sibling();
		}
		node = node.next_sibling();
	}
	return ret;
}

bool M_Animation::Start()
{
	LOG("Animation: Start.");
	bool ret = true;

	//IMPORTANT: NEW UNITS PNGS GO HERE
	textures.push_back(Texture(app->tex->Load("animation/CavalryArcher.png")));

	return ret;
}

bool M_Animation::CleanUp()
{
	bool ret = true;

	for (std::list<Animation*>::iterator it = animations.begin(); it != animations.end(); it++)
	{
		(*it)->CleanUp();
		delete (*it);
	}

	animations.clear();

	return ret;
}

Animation* M_Animation::GetAnimation()const
{
	

	return nullptr;
}

SDL_Texture* M_Animation::GetTexture() const{
	

	LOG("Could not find texture for that unit.");
	return nullptr;
}

void M_Animation::GetFrame(GB_Rectangle<int>& rect, iPoint& pivot)
{
	Animation* tmp = GetAnimation();

	if (tmp == nullptr) 
	{
		LOG("No unit found");
		return;
	}
	if (tmp->Finished() == false)
	{
		rect = tmp->GetCurrentFrame();
		pivot = tmp->GetCurrentPivot();
	}
}

void M_Animation::DrawDebug()
{
}


//-------------------------------------------ANIMATION-------------------------

Animation::Animation(std::string name) : name(name)
{
}

Animation::~Animation()
{
}

void Animation::CleanUp()
{
	frames.clear();
	pivotPoints.clear();

}

void Animation::SetLoop(bool loop)
{
	this->loop = loop;
}

void Animation::SetSpeed(float speed) 
{
	this->speed = speed;
}

bool Animation::Finished()const
{
	return (loops > 0 && loop == false);
}

void Animation::Reset()
{
	currentFrame = 0;
	animationTimer.Start();
	loops = 0;
}


GB_Rectangle<int>& Animation::GetCurrentFrame()
{
	if (currentFrame == -1)
	{
		return GB_Rectangle<int>{0, 0, 0, 0};
	}

	currentFrame = (float)floor(animationTimer.Read() / speed);

	if (currentFrame >= frames.size())
	{
		if (loop = true)
		{
			animationTimer.Start();
			currentFrame = 0;
			loops++;
		}
		else
		{
			currentFrame = -1;
			loops = 0;
			return GB_Rectangle<int>{0, 0, 0, 0};
		}
	}

	return frames[(int)currentFrame];
}

iPoint& Animation::GetCurrentPivot()
{
	return pivotPoints[(int)currentFrame];
}

