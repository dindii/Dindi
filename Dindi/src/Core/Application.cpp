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
#include "Visual/Light/Light.h"

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

		m_DeltaTime = new DeltaTime();

		Renderer::Init();
		GUI::Init(this->m_ApplicationWindow);
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
			m_ActiveScene->GetActiveCamera()->RemakeProjection((float)Event.GetWidth(), (float)Event.GetHeight());
			return true;
		});


		dispatcher.Dispatch<KeyPressedEvent>([&](KeyPressedEvent Event) -> bool
		{

#ifdef DINDI_DEBUG
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
#endif
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

		//This is just for when developing the shaders, removing this would save us some frame budget.
#ifdef DINDI_DEBUG
		ShaderHotReloader::OnUpdate();
#endif
		Renderer::Draw(m_ActiveScene);


		for (Layer* layer : m_LayerStack)
			layer->OnUpdate(dt);
	}

	float Application::GetTime() const
	{
		return m_ApplicationWindow->GetTime();
	}

	void Application::ProcessEngineInterface()
	{
		//This is printing something of the engine (the active scene) and not the scene itself from the layers
		//so this will be an engine GUI part thus it is fine to let this to be here.

		static constexpr int maxLightLabelSize = 50;

		GUI::Begin();
			ImGui::Begin("Scene Lights");

			ImGui::NewLine();
			if (ImGui::Button("Add Light", { 128.0f, 32.0f }))
				m_ActiveScene->AddPointLight({ { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } });
			ImGui::NewLine();

			for (int x = 0; x < m_ActiveScene->GetLights().size(); x++)
			{
				PointLight& light0 = m_ActiveScene->GetLights()[x];

				float pos[3] = { light0.GetPosition().x, light0.GetPosition().y, light0.GetPosition().z };
				float color[3] = { light0.GetColor().x, light0.GetColor().y, light0.GetColor().z };

				char lightLabel[maxLightLabelSize] = "Light Position ";
				char lightColorLabel[maxLightLabelSize] = "Light Color ";
				char lightRemoveLabel[maxLightLabelSize] = "Remove Light ";

				char number[maxLightLabelSize];

				sprintf(number, "%i", x);

				//#NOTE: I don't think we will ever get a bottleneck from strcat not being fast enough to found \0, but I will keep and eye on this when profiling later.
				//They're not safe by the way, but we are in a controlled environment.
				//Kinda annoying to have to make every label unique
				strcat(lightLabel, number);
				strcat(lightColorLabel, number);
				strcat(lightRemoveLabel, number);

				if (ImGui::SliderFloat3(lightLabel, pos, -50.0f, 50.0f))
					light0.SetPosition({ pos[0], pos[1], pos[2], 0.0f });
				
				ImGui::SameLine();
				if (ImGui::Button(lightRemoveLabel))
					m_ActiveScene->GetLights().erase(m_ActiveScene->GetLights().begin() + x);

				if (ImGui::ColorEdit3(lightColorLabel, color))
					light0.SetColor({ color[0], color[1], color[2], 0.0f });

				ImGui::NewLine();
			}
			ImGui::End();

			ImGui::Begin("Stats");
			ImGui::Text("%.4f", m_DeltaTime);
			ImGui::End();

		GUI::End();

		for (Layer* layer : m_LayerStack)
			layer->OnUIRender();
	}
	
	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.emplace_back(layer);
		layer->OnAttach();
	}
}