#include "Dindipch.h"
#include "Application.h"
#include "Rendering/Core/Renderer.h"
#include "Rendering/Shader/ShaderHotReloader.h"
#include "Input/Input.h"

#include "Event/ApplicationEvent.h"
#include "Event/KeyEvent.h"
#include "Math/Maths.h"
#include "Visual/Model/Model.h"
#include "GUI/GUI.h"

namespace Dindi
{

	Application::Application()
	{
		Application(800.0f, 600.0f);
	}

	Application::Application(float windowWidth, float windowHeight, const char* appName, float aspectRatio)
	{
		if (s_Instance)
		{
			DND_LOG_FATAL("More than one instance of Application is not allowed.");
			return;
		}

		s_Instance = this;

		ShaderHotReloader::Init();
		Logger::Init();

		m_ApplicationWindow = new Window(windowWidth, windowHeight, appName, aspectRatio);
		m_ApplicationWindow->setEventCallback(BIND_EVENT_FN(Application::OnEvent));

		m_DeltaTime = new DeltaTime();

		Renderer::Init();
		GUI::Init(this->m_ApplicationWindow);
		
		//#NOTE: ModelLoader as a static class as helpers of the program, classes that will help to build the program
		//stuff like DeltaTime are part of the program (the deltatime of the program duh) so they will be inside Application.

		//FROM HERE TO BELOW IS ALL DEBUG STUFF
		m_DefaultEditorCamera = new Camera(this->GetWindow()->GetAspectRatio(), vec3(0.0f, 0.0f, 5.0f));
		m_DefaultEditorCameraSpeed = 10.0f;
		m_DefaultEditorCamera->SetCameraLag(true);
		m_DefaultEditorCamera->SetCameraLagValue(0.15f);

		SceneOne = new Scene();
		//#TODO: SetActiveScene function, please.
		m_ActiveScene = SceneOne;
		SceneOne->SetActiveCamera(m_DefaultEditorCamera);

 		static Model* model = new Model(RESOURCES_PATH "Resources/Models/backpack.obj");
		SceneOne->AddEntity(model);


		Input::HideAndLockCursor(true);
	}

	Application::~Application()
	{
		delete m_ApplicationWindow;
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent Event) -> bool
		{
			m_ActiveScene->GetActiveCamera()->RemakeProjection((float)Event.GetWidth(), (float)Event.GetHeight());
			return true;
		});


		dispatcher.Dispatch<KeyPressedEvent>([&](KeyPressedEvent Event) -> bool
		{

#ifdef DINDI_DEBUG
			//press escape to close program.
			if (Event.GetKeyCode() == DND_KEY_ESCAPE)
			{
				TerminateProgram();
				return true; 
			}
#endif

			if (Event.GetKeyCode() == DND_KEY_F2)
			{
				m_MouseLockedAndInvisible = !m_MouseLockedAndInvisible;

				Input::HideAndLockCursor(m_MouseLockedAndInvisible);
				m_ActiveScene->GetActiveCamera()->LockCamera(!m_MouseLockedAndInvisible);

				return true;
			}

			return false;
		});
	}

	void Application::Run()
	{
		while (m_Running)
		{
			m_ApplicationWindow->OnUpdate();

			Renderer::Clear();

			m_DeltaTime->Tick();
			Logger::Flush();

			OnUpdate(*m_DeltaTime);

			ProcessEngineInterface();
		}
	}

	void Application::OnUpdate(DeltaTime& dt)
	{
		ShaderHotReloader::OnUpdate();

		Renderer::Draw(m_ActiveScene);

		LookAround();
		MoveCamera(dt);
	}

	float Application::GetTime() const
	{
		return m_ApplicationWindow->GetTime();
	}

	void Application::ProcessEngineInterface()
	{
		//#TODO: this should get huge, so take this to a better place.
		GUI::Begin();
		ImGui::Begin("Iha!");
		ImGui::End();
		GUI::End();
	}
	
	void Application::MoveCamera(DeltaTime& dt)
	{
		vec3 IntendedCameraPosition;

		if (Input::IsKeyPressed(EKeyCode::DND_KEY_W))
			IntendedCameraPosition.z = -m_DefaultEditorCameraSpeed;

		if (Input::IsKeyPressed(EKeyCode::DND_KEY_S))
			IntendedCameraPosition.z = m_DefaultEditorCameraSpeed;

		if (Input::IsKeyPressed(EKeyCode::DND_KEY_D))
			IntendedCameraPosition.x = m_DefaultEditorCameraSpeed;

		if (Input::IsKeyPressed(EKeyCode::DND_KEY_A))
			IntendedCameraPosition.x = -m_DefaultEditorCameraSpeed;

		if (Input::IsKeyPressed(EKeyCode::DND_KEY_SPACE))
			IntendedCameraPosition.y = m_DefaultEditorCameraSpeed;

		if (Input::IsKeyPressed(EKeyCode::DND_KEY_LEFT_CONTROL))
			IntendedCameraPosition.y = -m_DefaultEditorCameraSpeed;

		//Add this vector on the Target Position, this is, where we are looking at, thus making a free camera style
		m_ActiveScene->GetActiveCamera()->AddCameraTargetPosition(IntendedCameraPosition, dt);
	}

	void Application::LookAround()
	{
		//#TODO: Set to not do this by default, most of times we do want to pick objects with the cursor to control the scene
		//       and this would change the camera offset making it harder and annoying to edit the scene
		//So just don't start changing every camera based on mouse input, please.
		vec2 MousePosition = Input::GetMousePosition();
		m_ActiveScene->GetActiveCamera()->SetCameraYaw(toRadians(MousePosition.x));
		m_ActiveScene->GetActiveCamera()->SetCameraPitch(toRadians(MousePosition.y));
	}

}