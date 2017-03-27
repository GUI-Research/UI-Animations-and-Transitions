#ifndef _CURVE_BEZIER_
#define _CURVE_BEZIER

#include "p2Point.h"
#include <vector>
#include <math.h>

#include "App.h"
#include "M_Render.h"

#define MAX_POINTS 100
#define ORIGIN_POINTS 4

enum CBEZIER_TYPE
{
	CB_NO_TYPE,
	CB_EASE_INOUT_BACK,
	CB_SLOW_MIDDLE,
	CB_LINEAL
};

class CBeizier
{
public:
	CBeizier();
	~CBeizier();

	void DrawBezierCurve(CBEZIER_TYPE type);

	float GetActualPoint(const iPoint origin, const iPoint destination, int ms, int current_ms, CBEZIER_TYPE b_type);
	float GetActualX(int ms, int current_ms, CBEZIER_TYPE b_type);

	bool CalculatePivots(std::vector<fPoint>& points);
	void CPoints(fPoint x0, fPoint x1, fPoint x2, fPoint x3);
	void Bezier(std::vector<fPoint> &points, CBEZIER_TYPE b_type);


private:

	std::vector<float>* list = nullptr;
	std::vector<fPoint> pivot_points;

	float speed = 0.0f;
	std::vector<float> ease_inout_back;
	std::vector<float> slow_middle;
	std::vector<float> b_lineal;

	std::vector<float> temp;

};

CBeizier::CBeizier(): list(nullptr), speed(0.0f)
{
	std::vector<fPoint> points;
	//cool efect; stop in the middle;
	points.push_back({ 0.95f,1.f });
	points.push_back({ 1.2f,-0.0f });
	Bezier(points, CBEZIER_TYPE::CB_SLOW_MIDDLE);
	points.clear();
	//bot one
	/*points.push_back({ 0.6f, 0.1f});
	points.push_back({ 0.55f, 0.15f });
	points.push_back({ 0.4f, -0.4f });
	points.push_back({ 0.45f, -0.45f });
	points.push_back({ 0.85f, 1.f });
	points.push_back({ 0.95f, 1.f });
	*/
	Bezier(points, CBEZIER_TYPE::CB_LINEAL);
	points.clear();
	//rebot dalt
	points.push_back({ 0.6f, 1.2f });
	points.push_back({ 0.55f, 1.25f });
	points.push_back({ 0.4f, 0.8f });
	points.push_back({ 0.45f, 0.85f });
	points.push_back({ 0.85f, 1.f });
	points.push_back({ 0.95f, 1.f });

	Bezier(points, CBEZIER_TYPE::CB_EASE_INOUT_BACK);
	points.clear();

	ease_inout_back.reserve(MAX_POINTS);
	slow_middle.reserve(MAX_POINTS);
	b_lineal.reserve(MAX_POINTS);
	temp.reserve(MAX_POINTS);
}

CBeizier::~CBeizier()
{
	ease_inout_back.clear();
	slow_middle.clear();
	b_lineal.clear();
	temp.clear();
}

void CBeizier::DrawBezierCurve(CBEZIER_TYPE type)
{
	int size = 0;
	switch (type)
	{
	case CB_EASE_INOUT_BACK:
	{
		//		for (int i = 0; i+1 < pivot_points.size(); i++)
		//		{
		//			app->render->DrawLine(pivot_points[i].x*200 + 300, -pivot_points[i].y * 100 + 400, pivot_points[i+1].x * 200 + 300, -pivot_points[i+1].y * 100 + 400, 255, 255, 255, 155);
		//		}
		size = ease_inout_back.size();
		for (int i = 0; i + 1 < size; i++)
		{
			app->render->DrawLine((i * 2) + 300, -(ease_inout_back[i] * 100) + 400, (i * 2) + 300, -(ease_inout_back[i + 1] * 100) + 400, 255, 150, 0, 255);
		}
	}
	break;
	case CB_SLOW_MIDDLE:
	{
		size = slow_middle.size();
		for (int i = 0; i + 1 < size; i++)
		{
			app->render->DrawLine((i * 2) + 300, -(slow_middle[i] * 100) + 400, (i * 2) + 300, -(slow_middle[i + 1] * 100) + 400, 255, 150, 0, 255);
		}
	}
	break;
	case CB_LINEAL:
	{
		size = b_lineal.size();
		for (int i = 0; i + 1 < size; i++)
		{
			app->render->DrawLine((i * 2) + 300, -(b_lineal[i] * 100) + 400, (i * 2) + 300, -(b_lineal[i + 1] * 100) + 400, 255, 150, 0, 255);
		}
	}
	break;
	default:
		break;
	}
}

float CBeizier::GetActualPoint(const iPoint origin, const iPoint destination, int ms, int current_ms, CBEZIER_TYPE b_type)
{
	float distance = -(sqrt(pow((destination.x - origin.x), 2) + pow((destination.y - origin.y), 2)));
	return (GetActualX(ms, current_ms, b_type)*distance);
}

float CBeizier::GetActualX(int ms, int current_ms, CBEZIER_TYPE b_type)
{
	int time = (current_ms * 100 / ms);
	float ret = 0.0f;
	switch (b_type)
	{
	case CB_EASE_INOUT_BACK:
	{
		ret = ease_inout_back[time];
	}
	break;
	case CB_SLOW_MIDDLE:
	{
		ret = slow_middle[time];
	}
	break;
	case CB_LINEAL:
	{
		ret = b_lineal[time];
	}
	break;
	default: ret = 0.0f;
		break;
	}
	return ret;
}

bool CBeizier::CalculatePivots(std::vector<fPoint> &points)
{
	//pushback origin
	pivot_points.push_back({ 0.0f,0.0f });

	int size = points.size();
	for (int i = 0; i + 3 < size; i += 2)
	{
		pivot_points.push_back(points[i]);
		pivot_points.push_back(points[i + 1]);
		pivot_points.push_back({ (points[i + 1].x + (points[i + 2].x - points[i + 1].x) / 2),
			(points[i + 1].y + (points[i + 2].y - points[i + 1].y) / 2) });
	}
	//push last 2 points
	pivot_points.push_back(points[points.size() - 2]);
	pivot_points.push_back(points[points.size() - 1]);

	//pushback destiny
	pivot_points.push_back({ 1, 1 });

	return true;
}

void CBeizier::CPoints(fPoint x0, fPoint x1, fPoint x2, fPoint x3)
{
	temp.clear();
	int last = -(x0.x * 100) + (x3.x * 100);
	float delta = (float)100 / last;
	float p = 0;
	for (float ot = 0.f; ot <= 1.f; ot += 0.01f)
	{
		p = (((x0.y)*(1 - ot)*(1 - ot)*(1 - ot)) + 3 * x1.y*ot*(1 - ot)*(1 - ot) + 3 * x2.y*ot*ot*(1 - ot) + x3.y*ot*ot*ot);
		temp.push_back(p);
	}
	for (float count = 0; count < 100; count += delta)
	{
		if (list->size() <= 100)list->push_back(temp[count]);
	}
}

void CBeizier::Bezier(std::vector<fPoint>& points, CBEZIER_TYPE b_type)
{
	//clear previous pivots
	pivot_points.clear();

	switch (b_type)
	{
	case CB_EASE_INOUT_BACK: list = &ease_inout_back;
		break;
	case CB_SLOW_MIDDLE: list = &slow_middle;
		break;
	case CB_LINEAL: list = &b_lineal;
		break;
	default: return;
	}

	speed = 0;
	if (points.size() >= 2)
	{
		if (CalculatePivots(points))
		{
			int psize = pivot_points.size();
			for (int i = 0; i + 3 < psize; i = i + 3)
			{
				CPoints(pivot_points[i], pivot_points[i + 1], pivot_points[i + 2], pivot_points[i + 3]);
			}
			list->push_back(1);
		}
	}
	else if (points.size() == 1)
	{
		float p;
		for (float t = 0.01f; t < 1; t += 0.01f)
		{
			p = 2 * t*(1 - t)*points[0].y + pow(t, 2);
			list->push_back(p);
		}
		list->push_back(1);
	}
	else
	{
		float p;
		for (float t = 0.01f; t < 1; t += 0.01f)
		{
			p = t;
			list->push_back(p);
		}
		list->push_back(1);
	}
}


#endif // !_CURVE_BEZIER_

