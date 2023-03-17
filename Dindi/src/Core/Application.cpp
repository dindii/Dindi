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

#include <Physics/AABB.h>


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
			int dx = 204;
			int dy = 44;

			float mx = (float)m_ApplicationWindow->GetMouseX() - dx;
			float my = (float)m_ApplicationWindow->GetMouseY() - dy;

		//	DND_LOG_TRACE("\nMX: ", (uint32_t)mx, " MY: ", (uint32_t)my);


			float yaw = m_ActiveScene->GetActiveCamera()->GetCameraYaw();
			float pitch = m_ActiveScene->GetActiveCamera()->GetCameraYaw();
			

			return false;
		});

		dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent Event) -> bool
		{
			uint32_t newWidth = Event.GetWidth();
			uint32_t newHeight = Event.GetHeight();

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
						bool meshOnly = Input::IsKeyPressed(DND_KEY_LEFT_SHIFT);
						PickObject(meshOnly);
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

				case DND_KEY_F4:
				{
					m_SelectedEntity.selectedMesh = nullptr;
					m_SelectedEntity.selectedModel = nullptr;
				}

				default:
					return false;
			}
		});

		for (Layer* layer : m_LayerStack)
			layer->OnEvent(e);
	}

	void Application::PickObject(bool meshOnly)
	{
		Camera* camera = m_ActiveScene->GetActiveCamera();

		//Viewport dimensions
		vec2 viewportDims = m_UILayer->GetViewportSize();

		//Window coords translated to viewport coords (windowCoord - viewportMin)
		vec2 viewportMouseCoords = m_UILayer->GetViewportMousePosition();

		const mat4& proj = camera->GetProjection();
		const mat4& view = camera->GetViewMatrix();

		//The easiest fix is to simply divide by cos(angle of ray - viewAngle) and use that as the distance
		float   distancePerStep = 1.0f;

		uint32_t steps = 1;
		while (steps < 128)
		{
			//#TODO: adjust to have a little offset always behind the camera.
			vec3 direction = Trace::CastRay(viewportMouseCoords.x, viewportMouseCoords.y, viewportDims.x, viewportDims.y, proj, view);
			vec3 finalTrace = camera->GetCameraPos() + direction * distancePerStep;

			Debug::DebugShapeContext sc;
			sc.firstPosition = finalTrace;
			sc.shapeColor = { 0.4f, 0.1f, 0.4f };
			sc.shapeLifetime = 5000;
			sc.shapeRenderFlags = Debug::EDebugRenderFlags::NO_DEPTH_TESTING | Debug::EDebugRenderFlags::WIREFRAME;
			sc.shapeSize = 0.1f;
			sc.shapeType = Debug::EDebugShape::CUBE;

			Debug::DebugRenderer::DrawShape(sc);

			std::vector<Model*>& entities = m_ActiveScene->GetEntities();

			bool shouldExit = false;

			for (uint32_t i = 0; i < entities.size(); i++)
				for(uint32_t m = 0; m < entities[i]->GetMeshes().size(); m++)
				{
					AABB toCheckCollisionAABB;

					meshOnly ? toCheckCollisionAABB = entities[i]->GetMeshes()[m]->GetAABB() : toCheckCollisionAABB = entities[i]->GetAABB();
					
					if (toCheckCollisionAABB.CheckCollision(m_ActiveScene->GetActiveCamera()->GetCameraPos()))
						continue;

					if (toCheckCollisionAABB.CheckCollision(finalTrace))
					{
						m_SelectedEntity.selectedModel = entities[i];
						m_SelectedEntity.selectedMesh = entities[i]->GetMeshes()[m];
						m_SelectedEntity.ignoreMesh = !meshOnly;

						shouldExit = true;
						break;
					}

					if (shouldExit)
						break;
				}

			if (shouldExit)
				break;

			distancePerStep += 0.7f;
			steps++;
		}

	//	Debug::DebugRenderer::ClearQueue();

		std::vector<Model*>& entities = m_ActiveScene->GetEntities();
		for (uint32_t i = 0; i < entities.size(); i++)
			for (uint32_t m = 0; m < entities[i]->GetMeshes().size(); m++)
			{
				Debug::DebugShapeContext sc;
				//sc.firstPosition = entities[i]->GetMeshes()[m]->GetOffsetAABB(entities[i]->GetPosition()).GetMin();
				sc.firstPosition = entities[i]->GetMeshes()[m]->GetAABB().GetMin();
				sc.shapeColor = entities[i]->GetMeshes()[m]->GetAABB().GetAABBSize() / 256;
				sc.shapeLifetime = 5000;
				sc.shapeRenderFlags = Debug::EDebugRenderFlags::NO_DEPTH_TESTING | Debug::EDebugRenderFlags::WIREFRAME;
				sc.shapeSize = 0.1f;
				sc.shapeType = Debug::EDebugShape::CUBE;

				Debug::DebugRenderer::DrawShape(sc);

				Debug::DebugShapeContext sc2;
			//	sc2.firstPosition = entities[i]->GetMeshes()[m]->GetOffsetAABB(entities[i]->GetPosition()).GetMax();
				sc2.firstPosition = entities[i]->GetMeshes()[m]->GetAABB().GetMax();
				sc2.shapeColor = entities[i]->GetMeshes()[m]->GetAABB().GetAABBSize() / 256;
				sc2.shapeLifetime = 5000;
				sc2.shapeRenderFlags = Debug::EDebugRenderFlags::NO_DEPTH_TESTING | Debug::EDebugRenderFlags::WIREFRAME;
				sc2.shapeSize = 0.1f;
				sc2.shapeType = Debug::EDebugShape::CUBE;

				Debug::DebugRenderer::DrawShape(sc2);
			}


	}

	void Application::Run()
	{
		while (m_Running)
		{
			OnUpdate(m_DeltaTime);

			if (!m_AppInitialized)
			{
				m_AppInitialized = true;
				
				InitializeEvent initEvent;
				OnEvent(initEvent);
			}
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