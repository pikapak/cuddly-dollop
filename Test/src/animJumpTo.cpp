#include "animJumpTo.h"

AnimJumpTo::AnimJumpTo(Vector3f position, Actor_ptr target)
{
	_destination = position;
	_destination.y -= 0.1f;
	_target = target;
	_initialPos = target->_Graphics->GetPosRef();
	_speed = (_destination - _initialPos) / 30.0f;
	_speed.z = 0;
	_progress = 0;// -0.40;
	_duration = 1;

	_target->_Graphics->_specialAnimation = true;
	_target->_Graphics->_forceAnimation = true;
	auto& data = Animation::GetMetaData(_target->_Graphics->GetTexture()).data;
	_target->_Graphics->_row = data.at(AE_Jump)._position;
	_length = abs(data.at(AE_Jump)._end - data.at(AE_Jump)._start) + 1;
	_start = data.at(AE_Jump)._start;
}

void AnimJumpTo::Update()
{
	// Dont update if done
	if (_done)
		return;

	if (_progress < 0 || _progress > 1)
	{
		// Set sprite
		_target->_Graphics->_sprite = _start;
	}
	else
	{
		// Set position
		Vector3f result = MathUtils::FindPositionInParabola(_progress / _duration, _initialPos, _destination);
		_target->_Graphics->SetPhysics(result, Vector3f());

		// Set sprite
		int sprite;
		if (_speed.x > 0)
			sprite = 1 + (int)(((_progress) / _duration) * (double)_length / 2);
		else
			sprite = 1 + (int)((1.0 - ((_progress) / _duration)) * (double)_length / 2);

		_target->_Graphics->_sprite = sprite;
	}


	// Update percent
	_progress += 0.025;
	if (_progress >= _duration)
	{
		_done = true;
		_target->_Graphics->SetPhysics(_destination, Vector3f());
		_target->_Graphics->_specialAnimation = false;
		_target->_Graphics->_forceAnimation = false;
		_target->_Graphics->_sprite = 0;
	}
}