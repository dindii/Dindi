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
			DND_LOG_ERROR("More than one instance of Application is not allowed.");
			return;
		}

		s_Instance = this;
		m_Logger = new Logger;
		m_ApplicationWindow = new Window(windowWidth, windowHeight, appName, aspectRatio);
		m_ApplicationWindow->setEventCallback(BIND_EVENT_FN(Application::OnEvent));
		m_DeltaTime = new DeltaTime();

		Renderer::Init();
		GUI::Init(this->m_ApplicationWindow);
		

		//FROM HERE TO BELOW IS ALL DEBUG STUFF
		m_DefaultEditorCamera = new Camera(this->GetWindow()->GetAspectRatio(), vec3(0.0f, 0.0f, 5.0f));
		m_DefaultEditorCameraSpeed = 10.0f;
		m_DefaultEditorCamera->SetCameraLag(true);
		m_DefaultEditorCamera->SetCameraLagValue(0.15f);

		SceneOne = new Scene();
		m_ActiveScene = SceneOne;
		SceneOne->SetActiveCameraScene(m_DefaultEditorCamera);

//#TODO: Buildar o assimp, separar os dados em Mesh(pos, normals, uv etc) e Material(shader e texturas)
		//achar um jeito legal também de bindar as texturas certas antes de renderizar e de setar pro sampler2d uniform direitinho
		//também derivar alguns primitivos de Model e hardcodar eles (usar os construtores de hardcode do mesh ja basta!)

		//o asset system entra gerindo o armazenamento através de um loader

	//#TODO: Ordem: aprender o cmake > buildar o assimp > criar os containers para os dados > criar um loader que armazene nos containers certos > 
		//criar um asset system para trabalhar em cima desses containers utilizando os loaders
		//Quando um usuario der load num asset, o asset system verifica se já existe em algum container e se sim retorna um shared ptr e se não loada e retorna o shared

		std::vector<vec3> vert;
		vert.emplace_back(vec3(-0.5f, -0.5f, 0.0f));
		vert.emplace_back(vec3( 0.5f, -0.5f, 0.0f));
		vert.emplace_back(vec3( 0.0f, 0.5f, 0.0f ));
		
 		static Mesh* mesh = new Mesh(vert);

		//#TODO: We will have to see how we will handle this material. Like, a default PBR shader with some Tweaks?
		//In that case, only the maps should be placeable or not and its configurations, and not what shader 
		std::string EngineDefaultVertexShader = RESOURCES_PATH + std::string("Resources/DefaultShaderVert.shader");
		std::string EngineDefaultFragmentShader = RESOURCES_PATH + std::string("Resources/DefaultShaderFrag.shader");

 		static Material* mat = new Material(EngineDefaultVertexShader, EngineDefaultFragmentShader);
 		static Model* model = new Model(mesh, mat);

		SceneOne->AddEntity(model);


		Input::HideAndLockCursor(true);
	}

	Application::~Application()
	{
		delete m_ApplicationWindow;
		delete m_Logger;
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent Event) -> bool
		{
			m_ActiveScene->GetSceneActiveCamera()->RemakeProjection((float)Event.GetWidth(), (float)Event.GetHeight());
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
				m_ActiveScene->GetSceneActiveCamera()->LockCamera(!m_MouseLockedAndInvisible);

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
			m_Logger->Flush();

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
		m_ActiveScene->GetSceneActiveCamera()->AddCameraTargetPosition(IntendedCameraPosition, dt);
	}

	void Application::LookAround()
	{
		//#TODO: Set to not do this by default, most of times we do want to pick objects with the cursor to control the scene
		//       and this would change the camera offset making it harder and annoying to edit the scene
		//So just don't start changing every camera based on mouse input, please.
		vec2 MousePosition = Input::GetMousePosition();
		m_ActiveScene->GetSceneActiveCamera()->SetCameraYaw(toRadians(MousePosition.x));
		m_ActiveScene->GetSceneActiveCamera()->SetCameraPitch(toRadians(MousePosition.y));
	}

}