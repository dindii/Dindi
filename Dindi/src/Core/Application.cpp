#include "Dindipch.h"
#include "Application.h"
#include "Rendering/Core/Renderer.h"
#include "Rendering/Shader/ShaderHotReloader.h"
#include "Rendering/Debug/DebugRenderer.h"
#include "Input/Input.h"

#include "Event/ApplicationEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
#include "Math/Maths.h"
#include "Visual/Model/Model.h"
#include "Visual/Light/Light.h"

#include <Physics/Trace.h>

#include <GUI/GUI.h>

namespace Dindi
{

	Application::Application()
	{
		Application(800.0f, 600.0f);
	}

	Application::Application(float windowWidth, float windowHeight, const char* appName, float aspectRatio)
	{
		if (s_Instance)
			DND_LOG_FATAL("More than one instance of Application is not allowed.");

		s_Instance = this;

#ifdef DINDI_DEBUG
		ShaderHotReloader::Init();
#endif
		Logger::Init();

		m_ApplicationWindow = new Window(windowWidth, windowHeight, appName, aspectRatio);
		m_ApplicationWindow->setEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();
		GUI::Init(this->m_ApplicationWindow);

		m_UILayer = new UILayer(m_ApplicationWindow->GetWidth(), m_ApplicationWindow->GetHeight());
		PushLayer(m_UILayer);

		m_AppState = EApplicationState::EDITOR;
	}

	Application::~Application()
	{
		for (Layer* layer : m_LayerStack)
			layer->OnDetach();

		delete m_ApplicationWindow;
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent Event) -> bool
		{
			uint32_t newWidth = Event.GetWidth();
			uint32_t newHeight = Event.GetHeight();

			Renderer::SetViewport(0, 0, newWidth, newHeight);
			m_ActiveScene->GetActiveCamera()->RemakeProjection((float)newWidth, (float)newHeight);

			return false;
		});


		dispatcher.Dispatch<MouseButtonPressedEvent>([&](MouseButtonPressedEvent Event) -> bool
		{
				switch (Event.GetMouseButton())
				{
				case DND_MOUSE_BUTTON_1:
				{
					vec3 mouseToWorld = Trace::CastRay((float)m_ApplicationWindow->GetMouseX(), (float)m_ApplicationWindow->GetMouseY(), *m_ActiveScene->GetActiveCamera());

					//#TODO - Replace this for a WHILE-STEP ray caster.
					mouseToWorld.z *= 5.0f;

					Debug::DebugShapeContext sc;
					sc.firstPosition = m_ActiveScene->GetActiveCamera()->GetCameraPos();
					sc.secondPosition = mouseToWorld;
					sc.shapeColor = { 1.0f, 0.0f, 1.0f };
					sc.shapeLifetime = 5000.0f;
					sc.shapeType = Debug::EDebugShape::LINE;

					Debug::DebugRenderer::DrawShape(sc);
					
					return false;
				} break;

				default:
					return false;
				}
		});

		dispatcher.Dispatch<KeyPressedEvent>([&](KeyPressedEvent Event) -> bool
		{
			switch (Event.GetKeyCode())
			{
				case DND_KEY_ESCAPE:
				{
					TerminateProgram();
					return true;
				}   break;

				case DND_KEY_F2:
				{
					m_MouseLockedAndInvisible = !m_MouseLockedAndInvisible;

					Input::HideAndLockCursor(m_MouseLockedAndInvisible);
					m_ActiveScene->GetActiveCamera()->LockCamera(!m_MouseLockedAndInvisible);

					return true;
				}   break;

				case DND_KEY_F3:
				{
					if (m_AppState == EApplicationState::SIMULATION)
						m_AppState = EApplicationState::EDITOR;
					else if(m_AppState == EApplicationState::EDITOR)
						m_AppState = EApplicationState::SIMULATION;

					return true;
				}   break;

				default:
					return false;
			}
		});

		for (Layer* layer : m_LayerStack)
			layer->OnEvent(e);
	}

	void Application::Run()
	{
		while (m_Running)
			OnUpdate(m_DeltaTime);
	}

	void Application::OnUpdate(DeltaTime& dt)
	{
		m_DeltaTime.Tick();
		
		m_ApplicationWindow->OnUpdate();
		Renderer::Clear();
		Logger::Flush();

		//This is just for when developing the shaders, removing this would save us some frame budget.

		ShaderHotReloader::OnUpdate();

		Renderer::Draw(m_ActiveScene);

		for (Layer* layer : m_LayerStack)
			layer->OnUpdate(dt);
	}

	float Application::GetTime() const
	{
		//#TODO - Move to a Timer function class.
		return m_ApplicationWindow->GetTime();
	}
	
	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.emplace_back(layer);
		layer->OnAttach();
	}

	void Application::SetActiveScene(Scene* scene)
	{
		m_ActiveScene = scene;
		
		SceneChangeEvent changeSceneEvent(scene);
		OnEvent(changeSceneEvent);
	}
}