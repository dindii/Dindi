#include "Dindipch.h"
#include "Camera.h"
#include "Math/Maths.h"
#include "Event/ApplicationEvent.h"
#include "Utils/Logger.h"
#include "Rendering/Core/Renderer.h"
#include <Core/Application.h>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
//#TODO: Refactor this, it is a little confusing.


namespace Dindi
{
	Camera::Camera(const float AR, const glm::vec3& position) : m_Yaw(0.0f), m_Pitch(0.0f), m_AspectRatio(AR), m_FOV(70.0f),
		m_CameraLag(false), m_ViewMatrix(1.0f), m_Projection(1.0f)
	{
		m_Projection = glm::perspective(m_FOV /*zoom*/, m_AspectRatio, 1.0f, 200.0f);
		m_CameraPos = position;

		//This is needed so we don't lerp to another place when (like 0,0,-1) the camera is constructed
		m_DesiredPos = m_CameraPos;
		m_CameraTarget = m_DesiredPos;

		UpdateCameraVectors();
	}

	glm::mat4 Camera::GetViewMatrix() const
	{
		return m_ViewMatrix;
	}

	void Camera::UpdateCameraVectors()
	{
		if (!m_CameraLock)
		{
			glm::mat4 m_CameraRotation(1.0f), m_CameraPosition(1.0f);

			m_CameraRotation = glm::rotate(m_CameraRotation, m_Pitch, glm::vec3(1.0f, 0.0f, 0.0f));
			m_CameraRotation = glm::rotate(m_CameraRotation, m_Yaw, glm::vec3(0.0f, 1.0f, 0.0f));

			m_CameraPosition = glm::translate(m_CameraPosition, -m_CameraPos);

			m_ViewMatrix = m_CameraRotation * m_CameraPosition;

			glm::vec3 forward(m_ViewMatrix[0][2], m_ViewMatrix[1][2], m_ViewMatrix[2][2]);
			glm::vec3 strafe(m_ViewMatrix[0][0], m_ViewMatrix[1][0], m_ViewMatrix[2][0]);
			glm::vec3 up(m_ViewMatrix[0][1], m_ViewMatrix[1][1], m_ViewMatrix[2][1]);

			m_FacingDirection = forward;
			m_CameraRight = strafe;
			m_CameraUp = up;
		}
	}

	void Camera::SetCameraYaw(float Yaw)
	{
		if (!m_CameraLock)
		{
			UpdateCameraVectors();
			m_Yaw = Yaw; 
		}
	}
	
	void Camera::SetCameraPitch(float Pitch)
	{
		if (!m_CameraLock)
		{
			UpdateCameraVectors();
			m_Pitch = Pitch;
		}
	}

	//For Free look fps camera
	void Camera::AddCameraTargetPosition(glm::vec3 pos, const DeltaTime& dt)
	{
		if (!m_CameraLock)
		{
			pos *= dt;

			glm::vec3 target = m_CameraTarget;
			target += ((m_FacingDirection * pos.z) + (m_CameraRight * pos.x)) + (m_CameraUp * pos.y);

			m_CameraTarget = target;

			SetCameraPosition(m_CameraTarget);
		}
	}

	void Camera::SetCameraPosition(glm::vec3& pos)
	{
		if (!m_CameraLock)
		{
			if (m_CameraLag)
			{
				m_DesiredPos = pos;
				glm::vec3 lerped = glm::mix(m_CameraPos, m_DesiredPos, m_LagVal);

				UpdateCameraVectors();

				m_CameraPos = lerped;
			}
			else
			{
				m_CameraPos = pos;
				UpdateCameraVectors();
			}
		}
	}

	void Camera::SetProjection(float AR, float FOV)
	{
		//#TODO: More params
		m_Projection = glm::perspective(FOV /*zoom*/, AR, 1.0f, 200.0f);
	}

	void Camera::RemakeProjection(float newWidth, float newHeight)
	{//#TODO: More params
		if (newWidth <= 0 || newHeight <= 0)
		{
			__debugbreak();
			return;
		}

		Application& app = Application::GetInstance();
		float viewportAR = app.GetUILayer()->GetViewportAspectRatio();

		SetProjection(viewportAR, m_FOV);
	}
}