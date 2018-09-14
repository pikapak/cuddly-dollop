#include "actor.h"
#include "skill.h"
#include "skillSmack.h"

Actor::Actor()
{
	_ColorState = CS_Normal;
	SetDefault();
}

Actor::Actor(Actor& a)
{
	_ColorState = CS_Normal;
	SetDefault();
}

Actor::~Actor()
{
}

void Actor::SetDefault()
{
	Selected = false;
	ChoosingAction = true;
}

void Actor::UpdateColor(bool dead, bool selected)
{
	_outline = selected;// Always default to no outline unless selected

	if (selected)
	{
		if (_ColorState != CS_Selected)
		{
			//_Graphics->SetColorAll(Vector3f(1, 0.25f, 0.25f), dead ? 0.5f : 1.0f);
			SetColorAll(Vector3f(3.0f, 3.0f, 3.0f), dead ? 0.5f : 1.0f);
			_ColorState = CS_Selected;
			_outline = true;
		}
	}
	else if (dead)
	{
		if (_ColorState != CS_Invis)
		{
			SetColorAll(Vector3f(0.25f, 0.25f, 0.25f), 0.5f);
			_ColorState = CS_Invis;
		}
	}
	//else if (_Fighter->PredictedSkill != NULL && _Fighter->Team == 0 && !ChoosingAction)
	//{
	//	SetColorAll(Vector3f(0.6f, 0.6f, 0.6f), 1.f);
	//	_ColorState = CS_Darker;
	//}
	else if (_ColorState != CS_Normal)
	{
		SetColorAll();
		_ColorState = CS_Normal;
	}
}


void Actor::Update()
{
	if (!Transformation::perspectiveOrtho)
		GetPosRef().z += MathUtils::HeightGivenLengthOfHypotenuseAndAngle(0.5f, -GraphicsComponent::_persRotation);
	Update();
}

void Actor::AdjustHeightForAngle()
{
	if (!Transformation::perspectiveOrtho)
		GetPosRef().z -= MathUtils::HeightGivenLengthOfHypotenuseAndAngle(0.5f, -GraphicsComponent::_persRotation);
}
