#include "Dindipch.h"
#include "Platform/Window.h"
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

#include <Physics/AABB.h>
#include <Utils/EntityPicker.h>

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

		dispatcher.Dispatch<MouseMovedEvent>([&](MouseMovedEvent Event) -> bool
		{
			return false;
		});

		dispatcher.Dispatch<InitializeEvent>([&](InitializeEvent Event) -> bool
		{
				Renderer::OnAppInitialized();
				return true;
		});

			
		dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent Event) -> bool
		{
			uint32_t newWidth = Event.GetWidth();
			uint32_t newHeight = Event.GetHeight();
			
			if (newWidth <= 0 || newHeight <= 0)
				return false;

			Renderer::OnContextResize(0, 0, newWidth, newHeight);
			m_ActiveScene->GetActiveCamera()->RemakeProjection((float)newWidth, (float)newHeight);

			return false;
		});


		dispatcher.Dispatch<MouseButtonPressedEvent>([&](MouseButtonPressedEvent Event) -> bool
		{
				switch (Event.GetMouseButton())
				{
				case DND_MOUSE_BUTTON_1:
				{
					if (!m_UILayer->IsHovering())
					{
						EntityPickerMode mode = EntityPickerMode::MeshOnly;

						if (Input::IsKeyPressed(DND_KEY_LEFT_SHIFT))
							mode = EntityPickerMode::ModelOnly;

						EntityPicker::PickEntity(mode);
					}
					
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
					Camera* camera = m_ActiveScene->GetActiveCamera();

					m_MouseLockedAndInvisible = !m_MouseLockedAndInvisible;
					
					Input::HideAndLockCursor(m_MouseLockedAndInvisible);
					camera->LockCamera(!m_MouseLockedAndInvisible);
					
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
		{
			if (!m_AppInitialized)
			{
				m_AppInitialized = true;
				
				InitializeEvent initEvent;
				OnEvent(initEvent);
			}
			
			OnUpdate(m_DeltaTime);
		}
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