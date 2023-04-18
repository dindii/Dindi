#pragma once
#include "Dindipch.h"
#include "Trace.h"

#include <Core/Application.h>

namespace Dindi
{

	Trace::Trace(const float x, const float y, const Camera& camera)
	{
		m_Coords = Trace::CastRay(x, y, camera);
	}

	vec3 Trace::CastRay(const float xCoord, const float yCoord, const Camera& camera)
	{	
		Application& app = Application::GetInstance();

		//Screen Coords |   0:width, 0:height
		float width = (float)app.GetWindow()->GetWidth();
		float height = (float)app.GetWindow()->GetHeight();

		return CastRay(xCoord, yCoord, width, height, camera.GetProjection(), camera.GetViewMatrix());
	}

	Dindi::vec3 Trace::CastRay(const float xCoord, const float yCoord, const float width, const float height, const mat4& projection, const mat4& view)
	{
		//Convert to NDC | -1:1, -1:1, -1:1
		float x = (2.0f * xCoord) / width - 1.0f;
		float y = 1.0f - (2.0f * yCoord) / height;
		float z = 1.0f;

		vec3 ray_NormalizedDeviceCoords(x, y, z);
	
		//Convert to Homogeneous Clip Coordinates | -1:1, -1:1, -1:1
		vec4 ray_ClipCoords(ray_NormalizedDeviceCoords.x, ray_NormalizedDeviceCoords.y, -1.0f, 1.0f);
		//OBS: Test without needing to reverse perspective division. Keep in mind we may need reverse it in some point.
	
		//Convert to View (camera/eye) Space | -x:x, -y:y, -z:z
		vec4 ray_ViewSpace = mat4::Inverse(projection) * ray_ClipCoords;
	
		//Reverse perspective division
		//ray_ViewSpace /= ray_ViewSpace.w;
	
		//We only need the x, y. Depth values will be set further
		ray_ViewSpace = vec4(ray_ViewSpace.x, ray_ViewSpace.y, -1.0f, 0.0f);
	
		//Convert to World Space -x:x, -y:y, -z:z
		vec4 ray_WorldSpace4 = (mat4::Inverse(view) * ray_ViewSpace);
		vec3 ray_WorldSpace = { ray_WorldSpace4.x, ray_WorldSpace4.y, ray_WorldSpace4.z };
	
		ray_WorldSpace = vec3::Normalize(ray_WorldSpace);


		Application& app = Application::GetInstance();
		float fov = app.GetActiveScene()->GetActiveCamera()->GetFieldOfView();
		ray_WorldSpace *= cos(fov);

		return ray_WorldSpace;
	}

}