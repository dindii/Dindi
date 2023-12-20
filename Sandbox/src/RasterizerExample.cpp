#include "RasterizerExample.h"
#include <Utils/Logger.h>
#include "Platform/Window.h"
#include <core/Application.h>
#include <Math/Maths.h>

#include <Event/ApplicationEvent.h>
#include <Event/KeyEvent.h>
#include <Event/MouseEvent.h>
#include <Input/Input.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

Dindi::Model* sponzaModel = nullptr;
Dindi::Model* backpackModel = nullptr;

void RasterizerExample::OnAttach()
{
	//FROM HERE TO BELOW IS ALL DEBUG STUFF
	Dindi::Application& app = Dindi::Application::GetInstance();

	//m_DefaultEditorCamera = new Dindi::Camera(app.GetWindow()->GetAspectRatio(), Dindi::glm::vec3(0.0f, 0.0f, 5.0f));

	//#TEMPORARY CHANGE ME
	float windowAR = Dindi::Application::GetInstance().GetWindow()->GetAspectRatio();
	m_DefaultEditorCamera = new Dindi::Camera(windowAR, glm::vec3(0.0f, 0.0f, 5.0f));
	m_DefaultEditorCameraSpeed = 10.0f;
	m_DefaultEditorCamera->SetCameraLag(true);
	m_DefaultEditorCamera->SetCameraLagValue(0.15f);

	SceneOne = new Dindi::Scene();
	SceneOne->SetActiveCamera(m_DefaultEditorCamera);

	//DEBUG
	m_DefaultEditorCamera->SetCameraYaw(0.0f);
	m_DefaultEditorCamera->SetCameraPitch(0.0f);
	backpackModel = new Dindi::Model(RESOURCES_PATH "Resources\\Models\\Backpack\\backpack.obj", glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
	sponzaModel = new Dindi::Model(RESOURCES_PATH "Resources\\Models\\sponza.obj", glm::vec3(0.0f, 0.0f, 0.0f), 0.025f);
	SceneOne->GetLightManager()->AddPointLight(glm::vec3(0.0f), glm::vec3(1.0f));
	
	SceneOne->AddEntity(sponzaModel);
	SceneOne->AddEntity(backpackModel);

	app.SetActiveScene(SceneOne);
}

void RasterizerExample::OnDetach()
{
	//#TODO: Unload stuff (like scene Models etc...)
}

void RasterizerExample::OnUIRender()
{
}

void RasterizerExample::OnUpdate(const Dindi::DeltaTime& dt)
{
	LookAround();
	MoveCamera(dt);
}

void RasterizerExample::OnEvent(Dindi::Event& event)
{
	Dindi::EventDispatcher dispatcher(event);


	dispatcher.Dispatch<Dindi::InitializeEvent>([&](Dindi::InitializeEvent Event) -> bool
	{
		Dindi::Application& app = Dindi::Application::GetInstance();

		float UIAspectRatio = app.GetUILayer()->GetViewportAspectRatio();
		float actualFov = app.GetActiveScene()->GetActiveCamera()->GetFieldOfView();

		m_DefaultEditorCamera->SetProjection(UIAspectRatio, actualFov);

		return true;
	});

}

void RasterizerExample::MoveCamera(const Dindi::DeltaTime& dt)
{
	    glm::vec3 IntendedCameraPosition(0.0f);
	
		if (Dindi::Input::IsKeyPressed(Dindi::EKeyCode::DND_KEY_W))
			IntendedCameraPosition.z = -m_DefaultEditorCameraSpeed;
	
		if (Dindi::Input::IsKeyPressed(Dindi::EKeyCode::DND_KEY_S))
			IntendedCameraPosition.z = m_DefaultEditorCameraSpeed;
	
		if (Dindi::Input::IsKeyPressed(Dindi::EKeyCode::DND_KEY_D))
			IntendedCameraPosition.x = m_DefaultEditorCameraSpeed;
	
		if (Dindi::Input::IsKeyPressed(Dindi::EKeyCode::DND_KEY_A))
			IntendedCameraPosition.x = -m_DefaultEditorCameraSpeed;
	
		if (Dindi::Input::IsKeyPressed(Dindi::EKeyCode::DND_KEY_SPACE))
			IntendedCameraPosition.y = m_DefaultEditorCameraSpeed;
	
		if (Dindi::Input::IsKeyPressed(Dindi::EKeyCode::DND_KEY_LEFT_CONTROL))
			IntendedCameraPosition.y = -m_DefaultEditorCameraSpeed;
	
		//Add this vector on the Target Position, this is, where we are looking at, thus making a free camera style
		SceneOne->GetActiveCamera()->AddCameraTargetPosition(IntendedCameraPosition, dt);
}

void RasterizerExample::LookAround()
{
	glm::vec2 MousePosition = Dindi::Input::GetMousePosition();
	Dindi::Camera* sceneCamera = SceneOne->GetActiveCamera();

	sceneCamera->SetCameraYaw(Dindi::toRadians(MousePosition.x * m_MouseSensitivity));
	sceneCamera->SetCameraPitch(Dindi::toRadians(MousePosition.y * m_MouseSensitivity));
}

