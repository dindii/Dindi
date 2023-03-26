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

	const EntityPickerContext EntityPicker::PickEntity(const EntityPickerMode mode)
	{
		Application& app = Application::GetInstance();

		Scene* scene = app.GetActiveScene();

		Camera* camera = scene->GetActiveCamera();

		//Viewport dimensions
		UILayer* uiLayer = app.GetUILayer();
		vec2 viewportDims = uiLayer->GetViewportSize();

		//Window coords translated to viewport coords (windowCoord - viewportMin)
		vec2 viewportMouseCoords = uiLayer->GetViewportMousePosition();

		const mat4& proj = camera->GetProjection();
		const mat4& view = camera->GetViewMatrix();

		//The easiest fix is to simply divide by cos(angle of ray - viewAngle) and use that as the distance
		float   distancePerStep = 1.0f;

		EntityPickerContext selectedEntity = {};

		uint32_t steps = 1;
		while (steps < 128)
		{
			//#TODO: adjust to have a little offset always behind the camera.
			vec3 direction = Trace::CastRay(viewportMouseCoords.x, viewportMouseCoords.y, viewportDims.x, viewportDims.y, proj, view);
			vec3 finalTrace = camera->GetCameraPos() + direction * distancePerStep;

			std::vector<Model*>& entities = scene->GetEntities();

			for (uint32_t i = 0; i < entities.size(); i++)
				for (uint32_t m = 0; m < entities[i]->GetMeshes().size(); m++)
				{
					//#TODO We are wasting iterations when we only want the model. Fix this with a branch or so
					AABB toCheckCollisionAABB;

					mode == EntityPickerMode::MeshOnly ? toCheckCollisionAABB = entities[i]->GetMeshes()[m]->GetAABB() : toCheckCollisionAABB = entities[i]->GetAABB();

					if (toCheckCollisionAABB.CheckCollision(camera->GetCameraPos()))
						continue;

					if (toCheckCollisionAABB.CheckCollision(finalTrace))
					{
						selectedEntity.selectedModel = entities[i];
						selectedEntity.selectedMesh = entities[i]->GetMeshes()[m];
						selectedEntity.ignoreMesh = (mode == EntityPickerMode::ModelOnly);

						m_CachedEntity = selectedEntity;

						return selectedEntity;
					}
				}


			distancePerStep += 0.7f;
			steps++;
		}

		return selectedEntity;
	}
}