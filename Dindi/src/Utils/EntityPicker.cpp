#include "Dindipch.h"
#include <Visual/Model/Model.h>

#include "EntityPicker.h"
#include <Core/Application.h>
#include <Physics/Trace.h>


//TO REMOVE
#include <Rendering/Debug/DebugRenderer.h>

namespace Dindi
{
	EntityPickerContext EntityPicker::m_CachedEntity = {};

	uint32_t EntityPicker::m_RayMaxSteps = 128;
	float EntityPicker::m_AdditionalDistancePerStep = 0.7f;

	const EntityPickerContext EntityPicker::PickEntity(const EntityPickerMode mode)
	{
		Application& app = Application::GetInstance();

		Scene* scene = app.GetActiveScene();

		Camera* camera = scene->GetActiveCamera();

		//Viewport dimensions
		UILayer* uiLayer = app.GetUILayer();
		glm::vec2 viewportDims = uiLayer->GetViewportSize();

		//Window coords translated to viewport coords (windowCoord - viewportMin)
		glm::vec2 viewportMouseCoords = uiLayer->GetViewportMousePosition();

		const glm::mat4& proj = camera->GetProjection();
		const glm::mat4& view = camera->GetViewMatrix();

		//The easiest fix is to simply divide by cos(angle of ray - viewAngle) and use that as the distance
		float   distancePerStep = 1.0f;

		EntityPickerContext selectedEntity = {};

		glm::vec3 cameraPos = camera->GetCameraPos();

		uint32_t steps = 1;
		while (steps < m_RayMaxSteps)
		{
			//#TODO: adjust to have a little offset always behind the camera.
			glm::vec3 direction = Trace::CastRay(viewportMouseCoords.x, viewportMouseCoords.y, viewportDims.x, viewportDims.y, proj, view);
			glm::vec3 finalTrace = cameraPos + direction * distancePerStep;
			
			if (Debug::DebugRenderer::CheckDebugModeFlag(Debug::MOUSE_RAYCAST))
			{
				Debug::DebugShapeContext shapeContext;
				shapeContext.firstPosition = finalTrace;
				shapeContext.shapeColor = glm::vec3(1.0f, 0.0f, 1.0f);
				shapeContext.shapeSize = 0.1f;
				shapeContext.shapeRenderFlags = Debug::EDebugRenderFlags::NO_DEPTH_TESTING;
				shapeContext.shapeType = Debug::EDebugShape::CUBE;
				shapeContext.shapeLifetime = 5000;
				Debug::DebugRenderer::DrawShape(shapeContext);
			}


			//Check for lights
			std::vector<PointLight>& lights = scene->GetLightManager()->GetLights();

			for (uint32_t l = 0; l < lights.size(); l++)
			{
				AABB lightAABB = lights[l].GetPickableAABB();

				if (lightAABB.CheckCollision(finalTrace))
				{
					selectedEntity.pickedEntity = &lights[l];
					m_CachedEntity = selectedEntity;

					return selectedEntity;
				}
			}

			//Check for models as a whole
			std::vector<Model*>& entities = scene->GetEntities();
			if(mode == EntityPickerMode::ModelOnly)
			{
				for (uint32_t i = 0; i < entities.size(); i++)
				{
					AABB toCheckCollisionAABB = entities[i]->GetPickableAABB();

					if (toCheckCollisionAABB.CheckCollision(cameraPos))
						continue;

					if (toCheckCollisionAABB.CheckCollision(finalTrace))
					{
						selectedEntity.pickedEntity = entities[i];
						m_CachedEntity = selectedEntity;

						return selectedEntity;
					}
				}
			}
			else
			{
				//otherwise check for each mesh on each model
				for (uint32_t i = 0; i < entities.size(); i++)
					for (uint32_t m = 0; m < entities[i]->GetMeshes().size(); m++)
					{
						AABB toCheckCollisionAABB = entities[i]->GetMeshes()[m]->GetPickableAABB();

						if (toCheckCollisionAABB.CheckCollision(cameraPos))
							continue;

						if (toCheckCollisionAABB.CheckCollision(finalTrace))
						{
							selectedEntity.pickedEntity = entities[i]->GetMeshes()[m];
							m_CachedEntity = selectedEntity;

							return selectedEntity;
						}
					}
			}

			distancePerStep += m_AdditionalDistancePerStep;
			steps++;
		}

		return selectedEntity;
	}
}