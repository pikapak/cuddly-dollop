#include "mathutils.h"
#include "transform.h"

static const float StepSize = 0.2f;
Camera* Camera::_currentCam = NULL;
//int Camera::Target = 1;
//Vector3f Camera::Mapsize = Vector3f();
//Vector3f Camera::_translate = Vector3f();
//Vector3f Camera::_scale = Vector3f(1, 1, 1);

Camera::Camera() : Target(1), _followSpeed(0.005f), _followConfiguration(FT_Exponential), _scale(Vector3f(1)), _style(CAMSTYLE_FollowDad),
_scaleTarget(Vector3f(1))
{
	_transform = std::make_unique<Transformation>(Transformation());
}

Vector3f Camera::MapCenter()
{
	float left = OrthoProjInfo::GetRegularInstance().Left;
	float right = OrthoProjInfo::GetRegularInstance().Right;
	float top = OrthoProjInfo::GetRegularInstance().Top;
	float size = OrthoProjInfo::GetRegularInstance().Size;
	// (Maplength - viewWidth) / 2
	Vector3f result;
	result.x = (_mapsize.x - ((abs(left) + abs(right)) / size)) / 2.f;
	result.y = (_mapsize.y - ((top * 2.f) / size)) / 2.f;
	result.x += right / size;
	result.y += top / size;
	result.z = _transform->GetTranslation().z;

	return result;
}

void Camera::SetCameraFollowSpeed(CameraSpeeds cs)
{
	switch (cs)
	{
	case CAMSPEED_Slow:
		_followSpeed = 0.000005f;
		break;
	case CAMSPEED_Normal:
		_followSpeed = 0.0005f;
		break;
	case CAMSPEED_Fast:
		_followSpeed = 0.005f;
		break;
	}
}


bool Camera::IsOnCamera(Vector3f& position, Vector3f& size)
{
	float right = OrthoProjInfo::GetRegularInstance().Right;
	float top = OrthoProjInfo::GetRegularInstance().Top;
	float orthoSize = OrthoProjInfo::GetRegularInstance().Size;

	float targetRightLimit = position.x + 0.5f * size.x;
	float targetLeftLimit = position.x - 0.5f * size.x;
	float targetTopLimit = position.y + 0.5f * size.y;
	float targetBottomLimit = position.y - 0.5f * size.y;

	// 3 times - once because this translation is done once in the negative to center the screen, then twice more for the whole width
	float cameraRightLimit = -_translate.x + (right / orthoSize);
	float cameraLeftLimit = -_translate.x - (right / orthoSize);
	float cameraTopLimit = -_translate.y + (top / orthoSize);
	float cameraBottomLimit = -_translate.y - (top / orthoSize);

	// Needs to be in left-right limits AND top-down limits
	if (((targetRightLimit > cameraLeftLimit && targetRightLimit < cameraRightLimit) ||
		(targetLeftLimit < cameraRightLimit && targetLeftLimit > cameraLeftLimit)) &&
		((targetTopLimit > cameraBottomLimit && targetTopLimit < cameraTopLimit) ||
		(targetBottomLimit < cameraTopLimit && targetBottomLimit > cameraBottomLimit)))
		return true;

	return false;
}

void Camera::Update()
{
	switch (_style)
	{
	case CAMSTYLE_FollowDad:
		// If the translate is rlly close to target, start targeting random nearby places
		// The lack of break will make the execute activate as well
	case CAMSTYLE_Follow:
		ExecuteScale();
		ExecuteFollow();
		break;
	default:
		break;
	}
}

void Camera::ExecuteScale()
{
	// Set the scale slowly as the follow takes effect
	_scale.x += (_scaleTarget.x - _scale.x) * 0.015f;
	_scale.y += (_scaleTarget.y - _scale.y) * 0.015f;
	_transform->SetScale(_scale);
}

void Camera::ExecuteFollow(bool useDadFollow)
{
	Vector3f target = useDadFollow ? _followTargetDad : _followTarget;

	_translate = _transform->GetTranslation();
	//_scale = _transform->GetScale();
	Vector3f scale = _scale;

	float size = OrthoProjInfo::GetRegularInstance().Size;

	float distanceX = -(target.x * scale.x) - _translate.x;//find the distance between the 2, -target so that the movement of the world will be proper
	float distanceY = -(target.y * scale.y) - _translate.y;

	//float percentagex = abs(distanceX) / 15.f / scale.x / 2.f;
	//float percentagey = abs(distanceY) / 10.f / scale.x / 2.f;
	float percentagex = 1;
	float percentagey = 1;

	if (_followConfiguration == FT_Exponential)
	{
		percentagex = (_followSpeed * pow(distanceX, 2)) * scale.x + 0.02f;
		percentagey = (_followSpeed * pow(distanceY, 2)) * scale.y + 0.02f;
	}
	else if (_followConfiguration == FT_Stable)
	{
		percentagex = (_followSpeed * 300 * (abs(distanceX) / abs(distanceY))) * scale.x + 0.02f;
		percentagey = (_followSpeed * 300 * (abs(distanceY) / abs(distanceX))) * scale.y + 0.02f;
	}

	percentagex = fmin(percentagex, 1.0f);
	percentagey = fmin(percentagey, 1.0f);

	// Sets how much the camera will move in this frame
	Vector3f lerp = _lerper.Lerp(-_translate, _followTarget * scale);
	_translate.x = -lerp.x;
	_translate.y = -lerp.y;
	//_translate.x += distanceX * percentagex;
	//_translate.y += distanceY * percentagey;

	// If the pan would bring you left further than the left limit OR the map is too small to fit the screen width,
	//  just pan at the left limit
	if (_translate.x - ((OrthoProjInfo::GetRegularInstance().Left) / size) > 0 ||
		_mapsize.x < ((OrthoProjInfo::GetRegularInstance().Right) / size) * 2)
		_translate.x = ((OrthoProjInfo::GetRegularInstance().Left) / size);
	// If the map is big enough for the screen to pan it right and you would normally pass the limits,
	//  set the pan to the exact right limit
	else if (abs(_translate.x - ((OrthoProjInfo::GetRegularInstance().Right) / size)) > _mapsize.x * scale.x)
		_translate.x = -(_mapsize.x * scale.x - ((OrthoProjInfo::GetRegularInstance().Right) / size));

	// If the pan would bring you down further than the bottom OR the map isnt high enough to fill the screen,
	//  just stay at the bottom
	if (_translate.y - ((OrthoProjInfo::GetRegularInstance().Bottom) / size) > 0 ||
		_mapsize.y < ((OrthoProjInfo::GetRegularInstance().Top) / size) * 2)
		_translate.y = ((OrthoProjInfo::GetRegularInstance().Bottom) / size);
	// If the map is big enough for the screen to pan it upwards and you would normally pass the limits,
	//  set the pan to the exact top
	else if (abs(_translate.y - ((OrthoProjInfo::GetRegularInstance().Top) / size)) > _mapsize.y * scale.y)
		_translate.y = -(_mapsize.y * scale.y - ((OrthoProjInfo::GetRegularInstance().Top) / size));

	_transform->SetTranslation(_translate);
}

void Camera::SetScale(Vector3f& scale)
{
	_scaleTarget = scale;
}

void Camera::SetFollow(Vector3f& target)
{
	if (target == _followTarget)
		return;
	_followTarget = target;
}

void Camera::SetFollowCenteredY(Vector3f target)
{
	auto center = MapCenter();
	target.y = center.y + ((target.y - center.y) / 2);
	_followTarget = target;
}


namespace MathUtils
{
	// ProgressPercent = between 0 and 1
	// The final result disregards the z value
	Vector3f FindPositionInParabola(float progressPercent, Vector3f startPoint, Vector3f endPoint)
	{
		// The x to be used in the final calculation
		float x = ((endPoint.x - startPoint.x) * progressPercent) + startPoint.x;
		Vector3f result;
		result.x = x;
		float a = -0.08f;
		float y1 = startPoint.y;
		float y2 = endPoint.y;
		float x1 = startPoint.x;
		float x2 = endPoint.x;
		float h = (y2 - a * pow(x2, 2) - y1 + a * pow(x1, 2)) / (2 * a * (x1 - x2));
		float k = y1 - a * pow(x1 - h, 2);

		result.y = a * pow(x - h, 2) + k;

		return result;
	}

	//void CalcNormals(std::vector<GLuint>& indices, std::vector<Vertex>& verts)
	//{
	//	int vertCount = 0;
	//	for(Vertex& v : verts)
	//	{
	//		std::vector<Vector3f> norms = std::vector<Vector3f>();
	//		for (unsigned int i = 0; i < indices.size(); i += 3) {
	//			unsigned int Index0 = indices[i];
	//			unsigned int Index1 = indices[i + 1];
	//			unsigned int Index2 = indices[i + 2];
	//			if (Index0 == vertCount || Index1 == vertCount || Index2 == vertCount)
	//			{
	//				Vector3f v1 = verts[Index1].vertex - verts[Index0].vertex;
	//				Vector3f v2 = verts[Index2].vertex - verts[Index0].vertex;
	//				Vector3f Normal = v1.Cross(v2);
	//				Normal.Normalize();
	//				if (std::find(norms.begin(), norms.end(), Normal) != norms.end())
	//					continue;

	//				norms.push_back(Normal);

	//				v.normal += Normal;
	//			}
	//		}
	//		vertCount++;
	//	}

	//	for(Vertex& v : verts)
	//	{
	//		v.normal.Normalize();
	//	}
	//}
}
