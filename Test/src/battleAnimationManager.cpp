#include "battleAnimationManager.h"
#include "fontFloat.h"
#include "fontManager.h"
#include "animMoveTo.h"

BattleAnimationManager::BattleAnimationManager()
{

}

void BattleAnimationManager::UpdateColors(int fighterid, bool selected, bool dead)
{
	m_actors.at(fighterid)->UpdateColor(selected, dead);
}

void BattleAnimationManager::MoveUp(int fighterid, bool foreward)
{
	Actor_ptr owner = m_actors.at(fighterid);
	if (foreward)
	{
		Anim_ptr move1 = Anim_ptr(new AnimMoveTo(owner->GetPos() + Vector3f(fighterid < 4 ? 1.f : -1, 0, 0), owner));
		//if (m_singleFileAttacks) // We dont want async movement when there's too many attacks gonna happen
			move1->_async = false;
		m_animations.push_back(move1);
	}
	else
	{
		Anim_ptr move2 = Anim_ptr(new AnimMoveTo(owner->BasePosition, owner));
		m_animations.push_back(move2);
	}
}



Anim_ptr BattleAnimationManager::CreateAnimation(AnimationOperation ao)
{
	Anim_ptr result;

	switch (ao)
	{
	case AS_JumpTo:
		break;
	case AS_ColorFlash:
		break;
	case AS_ScreenShake:
		break;
	case AS_BonusEffect:
		break;
	case AS_MoveTo:
		break;
	case AS_Wait:
		break;
	case AC_CameraFollow:
		break;
	case AC_CameraScale:
		break;
	case AC_CameraCenter:
		break;
	case AA_DealDamage:
		break;
	default:
		break;
	}

	return result;
}

void BattleAnimationManager::UpdateAnimations()
{
	if (m_animations.size() > 0)
	{
		bool nonAsyncHit = false;
		for (int i = 0; i < m_animations.size(); i++)
		{
			if (m_animations.at(i)->_async || i == 0 || !nonAsyncHit)
				m_animations.at(i)->Update();
			if (!m_animations.at(i)->_async)
				nonAsyncHit = true;
			if (m_animations.at(i)->_done)
			{
				m_animations.erase(m_animations.begin() + i);
				i--;
			}
		}
	}
}

void BattleAnimationManager::CreateFloatingText(int fighterid, std::string text)
{
	FontManager::GetInstance().CreateFloatingText(m_actors.at(fighterid)->GetPosRef(), text);
}

void BattleAnimationManager::Push_Back_Animation(Anim_ptr anim)
{
	m_animations.push_back(anim);
}


void BattleAnimationManager::SpawnStatusText(Actor_ptr target, std::string statusText)
{
	Vector3f pos;
	Font_ptr font;

	// Setup font
	pos = target->GetPos() + Vector3f(0.5f, 1.f, 0);
	pos.z = 0;

	// create font
	font = Font_ptr(new FontFloat(0.7));
	font->SetText(statusText, pos, true);

	// color
	dynamic_cast<FontFloat*>(font.get())->Color = Vector3f(0, 0.4f, 0.9f);

	FontManager::GetInstance().AddFont(font);
}

void BattleAnimationManager::SpawnDamageText(Actor_ptr target, Damage dmg, Skill_ptr skill)
{
	Vector3f pos;
	Font_ptr font;

	// Setup font
	pos = target->GetPos() + Vector3f(0.5f, dmg._type == ST_Bonus ? 0.75f : 1.f, 0);
	pos.z = 0;

	// create font
	font = Font_ptr(new FontFloat(0.7));
	if (skill->_critting)
		font->SetScale(0.75f, 0.75f);
	else if (dmg._type == ST_Bonus)
		font->SetScale(1.0f, 1.0f);
	font->SetText((dmg._value < 0 || skill->_skillType == ST_Healing ? "+" : "") + std::to_string(dmg._value), pos, true);

	// color
	Vector3f color;
	if (skill->_critting)
		// purpleish
		color = skill->_ac._success ? Vector3f(0.85f, 0.23f, 0.54f) : Vector3f(0.35f, 0.31f, 0.87f);
	else if (dmg._value >= 0 && skill->_skillType != ST_Healing && dmg._type != ST_Bonus)
		// redish / yellowish
		color = skill->_ac._success ? Vector3f(1, 0.8f, 0) : Vector3f(1, 0, 0);
	else if (dmg._value > 0 && skill->_skillType == ST_Healing || dmg._value < 0 && dmg._type == ST_Bonus)
		// Greenish
		color = skill->_ac._success ? Vector3f(0, 0.95f, 0.6f) : Vector3f(0, 1, 0);
	else if (dmg._type == ST_Bonus)
		color = Vector3f(1, 1.f, 1.f);
	dynamic_cast<FontFloat*>(font.get())->Color = color;

	FontManager::GetInstance().AddFont(font);
}