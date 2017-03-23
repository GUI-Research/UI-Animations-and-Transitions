#include "GUIAnimatedImage.h"
#include "M_Animation.h"


GUIAnimatedImage::GUIAnimatedImage(Animation* anim) : GUIImage(), anim(anim)
{
	SetType(GUI_ANIMATED_IMAGE);
}


GUIAnimatedImage::~GUIAnimatedImage()
{
}

void GUIAnimatedImage::Update(const GUIElement* mouseHover, const GUIElement* focus)
{
	if (anim != nullptr)
	{
		SetSection(anim->GetCurrentFrame());
	}
}