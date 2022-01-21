#include "boidspch.h"
#include "Camera.h"
#include "Math/Maths.h"
#include "Event/ApplicationEvent.h"
#include "Math/mat4.h"
#include "Utils/Logger.h"
#include "Rendering/Core/Renderer.h"

//#TODO: Refactor this, it is a little confusing.


namespace Boids
{
	Camera::Camera(const float AR, const vec3& position) : m_Yaw(0.0f), m_Pitch(0.0f), m_FOV(70.0f),
		m_CameraLag(false)
	{
		m_Projection = mat4::Perspective(m_FOV /*zoom*/, AR, 0.1f, 100.0f);

		m_CameraPos = position;

		//This is needed so we don't lerp to another place when (like 0,0,-1) the camera is constructed
		m_DesiredPos = m_CameraPos;
		m_CameraTarget = m_DesiredPos;

		UpdateCameraVectors();
	}

	mat4 Camera::getViewMatrix() const
	{
		return m_ViewMatrix;
	}

	void Camera::UpdateCameraVectors()
	{
		if (!m_CameraLock)
		{
			mat4 m_CameraRotation, m_CameraPosition;

			m_CameraRotation = mat4::Rotate(m_Pitch, vec3(1.0f, 0.0f, 0.0f));
			m_CameraRotation *= mat4::Rotate(m_Yaw, vec3(0.0f, 1.0f, 0.0f));

			m_CameraPosition = mat4::Translate(-m_CameraPos);

			m_ViewMatrix = m_CameraRotation * m_CameraPosition;

			vec3 forward(m_ViewMatrix[2 + 0 * 4], m_ViewMatrix[2 + 1 * 4], m_ViewMatrix[2 + 2 * 4]);
			vec3 strafe(m_ViewMatrix[0 + 0 * 4], m_ViewMatrix[0 + 1 * 4], m_ViewMatrix[0 + 2 * 4]);
			vec3 up(m_ViewMatrix[1 + 0 * 4], m_ViewMatrix[1 + 1 * 4], m_ViewMatrix[1 + 2 * 4]);
			
			m_FacingDirection = forward;
			m_CameraRight = strafe;
			m_CameraUp = up;
		}
	}

	//For Free look fps camera
	void Camera::AddCameraTargetPosition(vec3 pos, DeltaTime& dt)
	{
		pos *= dt;

		vec3 target = m_CameraTarget;
		target += ((m_FacingDirection * pos.z) + (m_CameraRight * pos.x)) + (m_CameraUp * pos.y);

		m_CameraTarget = target;

		SetCameraPosition(m_CameraTarget);
	}

	void Camera::SetCameraPosition(vec3& pos)
	{
		if (m_CameraLag)
		{
			m_DesiredPos = pos;
			vec3 lerped = vec3::lerp(m_CameraPos, m_DesiredPos, m_LagVal);

			UpdateCameraVectors();

			m_CameraPos = lerped;
		}
		else
		{
			m_CameraPos = pos;
			UpdateCameraVectors();
		}
	}

	void Camera::SetProjection(float AR)
	{
		//#TODO: More params
		m_Projection = mat4::Perspective(m_FOV /*zoom*/, AR, 1.0f, 100.0f);
	}

	void Camera::RemakeProjection(float newWidth, float newHeight)
	{//#TODO: More params
		float NewAspectRatio = newWidth / newHeight;
		Renderer::SetViewport(0, 0, uint32_t(newWidth), uint32_t(newHeight));
		SetProjection(NewAspectRatio);
	}
}