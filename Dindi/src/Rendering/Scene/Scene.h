#pragma once
#include <vector>
#include "Rendering/Object/Renderable.h"
#include "Rendering/Camera/Camera.h"
#include "Visual/Model/Model.h"
#include "Visual/Light/Light.h"
namespace Dindi
{
	class Scene
	{
	public:
		Scene();
	
		//We will have this fixed type "Model" because i want to keep it simple, we don't have stuff like ECS because this is
		//meant to be just a program with some models with fancy graphics applied to it and simple stuff as this should be enough.
		void AddEntity(Model* Entity) { m_Entities.emplace_back(Entity); }
		void AddPointLight(const PointLight& light) { m_Lights.emplace_back(light); }
		
		std::vector<PointLight>& GetLights() { return m_Lights; }

		void SetActiveCamera(Camera* camera) { m_ActiveCamera = camera; }
		
		Camera* GetActiveCamera() { return m_ActiveCamera; }
		std::vector<Model*>& GetEntities() { return m_Entities; }

	private:
		//Since i want to objects be shared between scenes (as i don't have a file system right now), this should be 
		//a shared pointer.
		//#TODO: Convert it to shared pointer
		std::vector<Model*> m_Entities;
		std::vector<PointLight> m_Lights;
		Camera* m_ActiveCamera;
	};
}