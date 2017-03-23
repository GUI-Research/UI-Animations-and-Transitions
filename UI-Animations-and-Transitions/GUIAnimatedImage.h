#ifndef __GUIANIMATEDIMAGE_H__
#define __GUIANIMATEDIMAGE_H__

#include "GUIElement.h"
#include "GUIImage.h"

class Animation;

class GUIAnimatedImage : public GUIImage
{
public:
	GUIAnimatedImage(Animation* anim);
	virtual ~GUIAnimatedImage();

	void Update(const GUIElement* mouseHover, const GUIElement* focus)override;

	Animation* anim = nullptr;
};

#endif // !__GUIANIMATEDIMAGE_H__