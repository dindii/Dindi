#include "RasterizerExample.h"
#include <Utils/Logger.h>
#include <core/Application.h>
#include <input/Input.h>
#include <Math/Maths.h>

void RasterizerExample::OnAttach()
{
	//FROM HERE TO BELOW IS ALL DEBUG STUFF
	Dindi::Application& app = Dindi::Application::GetInstance();

	m_DefaultEditorCamera = new Dindi::Camera(app.GetWindow()->GetAspectRatio(), Dindi::vec3(0.0f, 0.0f, 5.0f));
	m_DefaultEditorCameraSpeed = 10.0f;
	m_DefaultEditorCamera->SetCameraLag(true);
	m_DefaultEditorCamera->SetCameraLagValue(0.15f);

	SceneOne = new Dindi::Scene();
	SceneOne->SetActiveCamera(m_DefaultEditorCamera);

	Dindi::Model* model = new Dindi::Model(RESOURCES_PATH "Resources/Models/backpack.obj");
	SceneOne->AddEntity(model);

	app.SetActiveScene(SceneOne);
}

void RasterizerExample::OnDetach()
{
	//#TODO: Unload stuff (like scene Models etc...)
}

void RasterizerExample::OnUIRender()
{

}

void RasterizerExample::OnUpdate(Dindi::DeltaTime& dt)
{
	LookAround();
	MoveCamera(dt);
}

void RasterizerExample::OnEvent(Dindi::Event& event)
{
	DND_LOG_TRACE(event);
}

void RasterizerExample::MoveCamera(Dindi::DeltaTime& dt)
{
	Dindi::vec3 IntendedCameraPosition;

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
	Dindi::vec2 MousePosition = Dindi::Input::GetMousePosition();
	Dindi::Camera* sceneCamera = SceneOne->GetActiveCamera();

	sceneCamera->SetCameraYaw(  Dindi::toRadians(MousePosition.x));
	sceneCamera->SetCameraPitch(Dindi::toRadians(MousePosition.y));
}

