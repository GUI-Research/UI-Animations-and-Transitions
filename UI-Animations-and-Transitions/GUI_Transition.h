#ifndef _GUI_TRANSITION_
#define _GUI_TRANSITION_

#include "App.h"
#include "M_GUI.h"

#include "p2Point.h"
#include "j1Timer.h"
#include "GB_Rectangle.h"

#include "CBezier.h"
enum TRANSITION_START
{
	START_UNKNOWN,
	TOP_LEFT,
	TOP_RIGHT,
	TOP,
	LEFT,
	RIGHT,
	DOWN_LEFT,
	DOWN_RIGHT,
	DOWN

};

enum STATIC_ANIMATIONS
{
	ANIMATION_UNKNOWN,
	FLASH,
	SHAKE,
	PULSE,
	BOUNCE
};

enum TRANSITIONS
{
	TRANSITION_UNKNOWN,
	SCALE,
	FADE,
	DROP,
	FLY,
	SLIDE
};

enum T_STATE
{
	TRANSITION,
	IN,
	OUT
};


class Gui_Transition
{
public:
	Gui_Transition();
	~Gui_Transition();

	// Settings
	void	SetGlovals(iPoint origin, iPoint destiny, uint duration_ms);

	// Set Glovals
	void	SetDuration(uint duration_ms = 500);
	void	SetOrigin(int x = 0, int y = 0);
	void	SetDestiny(int x = 0, int y = 0);
	void	setRect(int x = 0, int y = 0, int w = 0, int h = 0);

	// Set Static Animations
	void	SetOnShow(STATIC_ANIMATIONS static_type);
	void	SetLooping(bool looping = false, int interval_ms = 0);

	// Functionality
	bool	Transition();

	void	DoTransition();
	void	DoAnimation(STATIC_ANIMATIONS type);
	
	// Transitions
	void	scale();
	void	fade();
	void	drop();
	void	fly();
	void	slide();

	// Animations
	void	pulse();
	void	flash();
	void	bounce();
	void	shake();
	
	// Draw
	void	Draw();

	bool	TransitionRunning() { return transition_runing; };
	T_STATE	OnTransition() { return transition_state; };

private:
	//Transitions Functions

private:
	
	int				time_ms;
	int				curr_time_ms;
	Timer			clock_ms;

	iPoint			origin;
	iPoint			destiny;
	
	CBEZIER_TYPE	move_curve;

	//translation active
	bool					transition_actived = false;
	bool					transition_runing = false;

	//static anim
	uint					interval = 0;
	bool					loop = false;
	STATIC_ANIMATIONS		static_anim_type = ANIMATION_UNKNOWN;

	// change translation
	T_STATE					transition_state = OUT;
	TRANSITIONS				default_trans_type = FADE;
	TRANSITIONS				on_show = FADE;
	TRANSITIONS				on_hide = FADE;

	//draw
	SDL_Texture*			texture = nullptr;
	GB_Rectangle<int>		origin_rect = { 0,0,0,0 };

};

Gui_Transition::Gui_Transition() : default_trans_type(FADE), transition_actived(false), loop(false), time_ms(500), curr_time_ms(0), origin({ 0,0 }), destiny{0,0}, move_curve(CBEZIER_TYPE::CB_NO_TYPE)
{

}

Gui_Transition::~Gui_Transition()
{
}

inline void Gui_Transition::Draw()
{
}

inline bool Gui_Transition::Transition()
{
	if (!transition_actived) return true;

	switch (transition_state)
	{
	case TRANSITION: 
		{
		DoTransition();
		}
		break;
	case IN:
		{
			if (static_anim_type != ANIMATION_UNKNOWN);// DoAnimation();
			else
			{
				DoTransition();
			}
		}
		break;
	case OUT:
		break;
	default:
		break;
	}
}

inline void Gui_Transition::DoTransition()
{
	switch (default_trans_type)
	{
	case TRANSITION_UNKNOWN:
		break;
	case SCALE:	scale();
		break;
	case FADE:	fade();
		break;
	case DROP:	drop();
		break;
	case FLY:	fly();
		break;
	case SLIDE:	slide();
		break;
	default:
		break;
	}
}

inline void Gui_Transition::DoAnimation(STATIC_ANIMATIONS type)
{
	transition_actived = true;
	static_anim_type = type;
	transition_runing = true;
	clock_ms.Start();

	switch (static_anim_type)
	{
	case ANIMATION_UNKNOWN: 
		break;
	case FLASH:	 flash();
		break;
	case SHAKE:	 shake();
		break;
	case PULSE:	 pulse();
		break;
	case BOUNCE: bounce();
		break;
	default:
		break;
	}
}

inline void Gui_Transition::scale()
{
}

inline void Gui_Transition::fade()
{

}

inline void Gui_Transition::drop()
{
}

inline void Gui_Transition::fly()
{
}

inline void Gui_Transition::slide()
{
}

inline void Gui_Transition::pulse()
{
	if (transition_runing) curr_time_ms = clock_ms.Read();

	float time = (float)curr_time_ms / time_ms;
	float change_alpha = app->gui->GetBezier()->GetActualX(time_ms, curr_time_ms, CBEZIER_TYPE::CB_SLOW_MIDDLE);

	// Capp alpha var [0,1]
	if (change_alpha < 0.0) change_alpha = 0.0f;
	else if (change_alpha > 1) change_alpha = 1.0f;
	
	// Pulse
	if (time <= 0.5f) app->render->DrawQuad({ 500,500,50,50 }, 255, 150, 150, 255 * (1 - change_alpha));
	else if (time < 1) app->render->DrawQuad({ 500,500,50,50 }, 255, 150, 150, 255 * (change_alpha));
	else {
		if (loop) clock_ms.Start();
		else {
			transition_actived = false;
			transition_runing = false;
		}
	}
}

inline void Gui_Transition::flash()
{
}

inline void Gui_Transition::bounce()
{
}

inline void Gui_Transition::shake()
{
}

inline void Gui_Transition::SetGlovals(iPoint new_origin, iPoint new_destiny, uint duration_ms)
{
	origin = new_origin;
	destiny = new_destiny;
	time_ms = duration_ms;
}

inline void Gui_Transition::SetDuration(uint duration_ms)
{
	time_ms = duration_ms;
}

inline void Gui_Transition::SetOrigin(int x, int y)
{
	origin = { x,y };
}

inline void Gui_Transition::SetDestiny(int x, int y)
{
	destiny = { x,y };
}

inline void Gui_Transition::setRect(int x, int y, int w, int h)
{
	origin_rect.Set(x, y, w, h);
}

inline void Gui_Transition::SetOnShow(STATIC_ANIMATIONS static_type)
{
	static_anim_type = static_type;
}

inline void Gui_Transition::SetLooping(bool looping, int interval_ms)
{
	loop = looping;
	interval = interval_ms;
}

#endif // !_GUI_TRANSITION_
