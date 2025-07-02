#include "Dindipch.h"

#include <Rendering/Core/Renderer.h>
#include <Rendering/Core/LowLevelRenderer.h>

#include "UILayer.h"

#include <GUI/GUI.h>

#include <Utils/FileDialog.h>
#include <Core/Application.h>

#include <Event/ApplicationEvent.h>
#include <Event/KeyEvent.h>

#include <Input/Input.h>

#include <Math/Maths.h>
#include <Utils/EntityPicker.h>
#include <glm/gtc/type_ptr.hpp>

namespace Dindi
{
	void UILayer::OnUpdate(const DeltaTime& dt)
	{		
		GUI::Begin();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ProcessViewport();
		ImGui::PopStyleVar();
	
		ProcessMenu();
		ProcessLightInspector();
		ProcessModelInspector();
		ProcessPerformanceStats(dt);
		ProcessGraphicsDefinitions();

#ifdef DINDI_DEBUG
		DrawDebugImageUI();
#endif
		GUI::End();
	}

#ifdef DINDI_DEBUG
	std::vector<uint32_t> DebugImageUIPool;
#endif

	void UILayer::DrawDebugImageUI()
	{
		ImGui::Begin("Custom Debug Image");

		for (int i = 0; i < DebugImageUIPool.size(); i++)
			ImGui::Image((ImTextureID)DebugImageUIPool[i], { 400, 400}, { 0,1 }, { 1,0 });

		ImGui::End();
	}
	void UILayer::DebugImageUI(uint32_t imageHandle)
	{
		DebugImageUIPool.push_back(imageHandle);
	}

	void UILayer::OnEvent(Dindi::Event& event)
	{
		EventDispatcher dispatcher(event);
			
		dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent Event) -> bool
		{
			uint32_t width = Event.GetWidth();
			uint32_t height = Event.GetHeight();
			
			if (width <= 0 || height <= 0)
				return false;

			SetFrameDimensions(Event.GetWidth(), Event.GetHeight());
		
			return false;
		});

		dispatcher.Dispatch<SceneChangeEvent>([&](SceneChangeEvent Event) -> bool
		{
			SetScene(Event.GetScene());

			return false;
		});

		dispatcher.Dispatch<KeyPressedEvent>([&](KeyPressedEvent Event) -> bool
			{
				switch (Event.GetKeyCode())
				{
				case DND_KEY_TAB:
				{
					//Make a selector for the editor mode (translate, rotate and scale)
					uint8_t fooEnum = (uint8_t)m_EditMode;
					fooEnum++;
					fooEnum %= EditMode::END;
					m_EditMode = (EditMode)fooEnum;

					return true;
				}   break;

				default:
					return false;
				}
			});

	}

	void UILayer::OnAttach()
	{

	}

	void UILayer::OnDetach()
	{

	}

	bool UILayer::IsHovering() const
	{
		return ImGuizmo::IsOver() || ImGui::IsAnyItemHovered();
	}

	glm::vec2 UILayer::GetViewportMousePosition() const
	{
		Application& app = Application::GetInstance();
		Window* appWindow = app.GetWindow();

		float mx = (float)appWindow->GetMouseX() - (float)m_ViewportMinX;
		float my = (float)appWindow->GetMouseY() - (float)m_ViewportMinY;

		return { mx, my };
	}

	glm::vec2 UILayer::GetViewportSize() const
	{
		return { (float)m_ViewportSizeX, (float)m_ViewportSizeY };
	}

	void UILayer::ProcessPerformanceStats(const DeltaTime& dt)
	{
		glm::vec2 windowSize = { (float)m_FrameWidth, (float)m_FrameHeight };
	
		ImGui::SetNextWindowPos({ windowSize.x * m_StatsWindowPosX, m_MenuBarHeight });
		ImGui::SetNextWindowSize({ windowSize.x * m_StatsWindowWidth, m_StatsWindowHeight });

		ImGui::Begin("Stats");
		ImGui::Text("%.4f ms", dt.Milliseconds());
		ImGui::NewLine();
		ImGui::Text("%i DrawCalls", DND_INTERNAL::LowLevelRenderer::GetDrawCallNumber());
		ImGui::End();
	
	}

	
	void UILayer::ProcessGraphicsDefinitions()
	{
		ImGui::Begin("ShadowMap");
		std::vector<Texture2D*>& shadowMaps = Renderer::GetShadowMap();
		
		for (uint32_t i = 0; i < shadowMaps.size(); i++)
		{
			ImGui::Image((ImTextureID)shadowMaps[i]->GetID(), { 500, 500 }, { 0, 1 }, { 1,0 });
		}
		
		GraphicsDefinitions& gd = Renderer::GetGraphicsDefinitions();

		ImGui::SliderFloat4("Directional Light Position'", &gd.directionalLightDir.x, 0.0f, 5.0f);

		for (int i = 0; i < 3; i++)
		{
			ImGui::Image((ImTextureID)DND_INTERNAL::LowLevelRenderer::GetGBuffer()[i]->GetID(), { 500, 500 }, { 0,1 }, { 1,0 });
		}

		ImGui::Image((ImTextureID)DND_INTERNAL::LowLevelRenderer::GetGBuffer()[FramebufferRenderTargetSlot::DEPTH]->GetID(), { 500, 500 }, { 0,1 }, { 1,0 });

		ImGui::End();
	}

	void UILayer::CacheViewportMinPos(uint32_t width, uint32_t height)
	{
		m_ViewportMinX = width;
		m_ViewportMinY = height;
	}

	void UILayer::CacheViewportSize(uint32_t width, uint32_t height)
	{
		m_ViewportSizeX = width;
		m_ViewportSizeY = height;
	}

	void UILayer::ProcessMenu()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("New"))
			{
				if (ImGui::MenuItem("Light"))
				{
					LightManager* lights = m_Scene->GetLightManager();
					lights->AddPointLight(glm::vec3(0.0f, 0.0f, 0.0f) , glm::vec3(1.0f, 1.0f, 1.0f));
				}

				if (ImGui::MenuItem("Model..."))
				{
					std::string path = FileDialog::OpenFile(".obj\0\0");
					Model* newModel = new Model(path, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);

					if (newModel)
						m_Scene->AddEntity(newModel);
					else
						DND_LOG_ERROR("Couldn't open model file!");
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
	
	void UILayer::ProcessLightInspector()
	{
		static constexpr int maxLightLabelSize = 128;

		glm::vec2 windowSize = { (float)m_FrameWidth, (float)m_FrameHeight };


		ImGui::SetNextWindowPos({ windowSize.x * m_LightInspectorPosX, m_MenuBarHeight });
		ImGui::SetNextWindowSize({ windowSize.x * m_LightInspectorWidth, windowSize.y });

		ImGui::Begin("Scene Lights");

		LightManager* lights = m_Scene->GetLightManager();
		std::vector<PointLight>& sceneLights = lights->GetLights();

		for (int x = 0; x < sceneLights.size(); x++)
		{
			PointLight& light = sceneLights[x];

			float pos[3] = { light.GetPosition().x, light.GetPosition().y, light.GetPosition().z };
			float color[3] = { light.GetColor().x, light.GetColor().y, light.GetColor().z };

			char lightLabel[maxLightLabelSize] = "Position##";
			char lightColorLabel[maxLightLabelSize] = "Color##";
			char lightRemoveLabel[maxLightLabelSize] = "Delete##";

			char number[maxLightLabelSize];

			sprintf(number, "%i", x);

			strcat(lightLabel, number);
			strcat(lightColorLabel, number);
			strcat(lightRemoveLabel, number);

			if (ImGui::DragFloat3(lightLabel, pos, m_PositionSliderSpeed))
				light.SetPosition({ pos[0], pos[1], pos[2] });

			if (ImGui::ColorEdit3(lightColorLabel, color, ImGuiColorEditFlags_HDR))
				light.SetColor({ color[0], color[1], color[2], 0.0f });

			if (ImGui::Button(lightRemoveLabel))
			{
				lights->RemoveLight(x);
			}

			ImGui::NewLine();
		}
		ImGui::End();
	}
	
	void UILayer::ProcessModelInspector()
	{
		static constexpr int maxModelLabelSize = 128;

		glm::vec2 windowSize = { (float)m_FrameWidth, (float)m_FrameHeight };

		ImGui::SetNextWindowPos({ m_ModelInspectorWindowPosX, m_MenuBarHeight });
		ImGui::SetNextWindowSize({ windowSize.x * m_ModelInspectorWindowWidth, windowSize.y });

		ImGui::Begin("Scene Models");

		std::vector<Model*>& sceneEntities = m_Scene->GetEntities();

		for (int x = 0; x < sceneEntities.size(); x++)
		{
			Model* model = sceneEntities[x];

			glm::vec3 pos = model->GetPosition();
			float scale = model->GetScale();

			char ModelPosLabel[maxModelLabelSize] = "Position##";
			char ModelScaleLabel[maxModelLabelSize] = "Scale##";
			char ModelDeleteLabel[maxModelLabelSize] = "Delete##";

			char number[maxModelLabelSize];

			sprintf(number, "%i", x);

			strcat(ModelPosLabel, number);
			strcat(ModelScaleLabel, number);
			strcat(ModelDeleteLabel, number);

			ImGui::Text("%s",model->GetName().data());

			if (ImGui::DragFloat3(ModelPosLabel, &pos[0], m_PositionSliderSpeed))
			{
				model->SetPickableDirty(true);
				model->SetPosition({ pos[0], pos[1], pos[2] });
			}

			if (ImGui::DragFloat(ModelScaleLabel, &scale, m_ScaleSliderSpeed))
			{
				model->SetPickableDirty(true);
				model->SetScale(scale);
			}

			if (ImGui::Button(ModelDeleteLabel))
			{
				model->SetPickableDirty(true);
				sceneEntities.erase(sceneEntities.begin() + x);
			}

			ImGui::NewLine();
		}

		ImGui::End();

	}

	void UILayer::ProcessViewport()
	{		
		glm::vec2 windowSize = { (float)m_FrameWidth, (float)m_FrameHeight };

		//#TODO - Turn those values into variables.
		ImGui::SetNextWindowPos({ windowSize.x * m_ModelInspectorWindowWidth, m_MenuBarHeight });
		ImGui::SetNextWindowSize({ windowSize.x * m_ViewportPosX, windowSize.y });

		glm::vec2 viewportDims(windowSize.x * m_ViewportPosX, windowSize.y);

		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::Image((ImTextureID)DND_INTERNAL::LowLevelRenderer::GetPostProcessingHandle(), { windowSize.x * m_ViewportPosX, windowSize.y }, { 0,1 }, { 1,0 });
		//ImGui::Image((ImTextureID)Renderer::GetScreenOutputHandle(), { windowSize.x * m_ViewportPosX, windowSize.y }, { 0,1 }, { 1,0 });
	//	ImGui::Image((ImTextureID)Renderer::GetScreenOutputHandle(), { 300, 300 }, { 0,1 }, { 1,0 });
//		ImGui::Image((ImTextureID)DND_INTERNAL::LowLevelRenderer::GetPostProcessingHandle(), { 300, 300 }, { 0,1 }, { 1,0 });
		
		//This must use the same ImGui window to draw the gizmo, so it is necessary to keep it in the same Begin-End call
		ProcessTransformGizmo();
		
		ImGui::End();

	}

	ImGuizmo::OPERATION GetImGuizmoOperation(const EditMode editMode)
	{
		ImGuizmo::OPERATION operation;

		//This could be an index access..
		switch (editMode)
		{
			case EditMode::Translate:
			{
				operation = ImGuizmo::OPERATION::TRANSLATE;
			} break;

			case EditMode::Rotation:
			{
				operation = ImGuizmo::OPERATION::ROTATE;
			} break;

			case EditMode::Scale:
			{
				operation = ImGuizmo::OPERATION::SCALE;
			} break;

			default:
			{
				DND_LOG_WARNING("Invalid Edit Mode!");
				operation = ImGuizmo::OPERATION::TRANSLATE;
			}
		}

		return operation;
	}

	void UILayer::ProcessTransformGizmo()
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		ImVec2 imageSize = ImGui::GetItemRectSize();
		CacheViewportSize(imageSize.x, imageSize.y);
		
		//#NOTE: I'm not sure if this should be here, but since we are already using the camera for another purposes (such as edit projection and view), I guess it will not be a problem to
		//let this here. But we should keep in mind to change the architecture over this next time.
		if ((uint32_t)imageSize.x != m_ViewportSizeX || (uint32_t)imageSize.y != m_ViewportSizeY || !m_ViewportInitializedProperties)
		{
			m_Scene->GetActiveCamera()->RemakeProjection(imageSize.x, imageSize.y);
			m_ViewportInitializedProperties = true;
		}
		

		ImVec2 imagePos = ImGui::GetItemRectMin();
		
		CacheViewportMinPos(imagePos.x, imagePos.y);
		
		ImGuizmo::SetRect(imagePos.x, imagePos.y, imageSize.x, imageSize.y);

		ImGuizmo::OPERATION transformOperation = GetImGuizmoOperation(m_EditMode);
		ImGuizmo::MODE transformMode = ImGuizmo::MODE::LOCAL;

		const glm::mat4& cameraProjection = m_Scene->GetActiveCamera()->GetProjection();
		const glm::mat4& cameraView = m_Scene->GetActiveCamera()->GetViewMatrix();


		EntityPickerContext pickContext = EntityPicker::GetLatestPickedEntity();

		if (pickContext.pickedEntity)
		{
			std::pair<glm::vec3, glm::vec3> entityGizmoContext = pickContext.pickedEntity->GetPickablePosition();

			glm::vec3 position = entityGizmoContext.first;
			glm::vec3 gizmoOffset = entityGizmoContext.second;

			glm::vec3 entityRotation = pickContext.pickedEntity->GetPickableRotation();
			
			float entityScale = pickContext.pickedEntity->GetPickableScale();

			//DND_LOG_TRACE("EntityRotation: ", entityRotation);

			//#TODO #PERF: Transform is also calculated from scratch on the Draw function (LowLevelRenderer.cpp), we could cache that transform there and then retrieve it here.
			glm::mat4 rotationTransform(1.0f);
			
			rotationTransform = glm::rotate(rotationTransform, entityRotation.z, { 0.0f, 0.0f, 1.0f });
			rotationTransform = glm::rotate(rotationTransform, entityRotation.y, { 0.0f, 1.0f, 0.0f });
			rotationTransform = glm::rotate(rotationTransform, entityRotation.x, { 1.0f, 0.0f, 0.0f });

			glm::mat4 modelTransform(1.0f);
			modelTransform = glm::translate(modelTransform, position) * rotationTransform * glm::scale(modelTransform, glm::vec3(entityScale));

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), transformOperation, transformMode, glm::value_ptr(modelTransform));

			glm::vec3 translation, rotation, scale;

			ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelTransform), glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));

			constexpr float epsilon = std::numeric_limits<float>::epsilon();
			bool isScaleDirty = (std::abs(entityScale - scale.x) <= epsilon);

			//if (!(CloseOrEqual(position, translation) && CloseOrEqual(entityRotation, rotation) && CloseOrEqual(glm::vec3(entityScale), { scale })))
			if (!(CloseOrEqual(position, translation) && CloseOrEqual(entityRotation, rotation) && isScaleDirty))
			{
				pickContext.pickedEntity->SetPickableDirty(true);

				translation -= gizmoOffset;

				//DND_LOG_TRACE("Gizmo Rotation: ", rotation);

				pickContext.pickedEntity->SetPickablePosition(translation);
				pickContext.pickedEntity->SetPickableRotation(rotation);
				pickContext.pickedEntity->SetPickableScale(scale.x);
			}
		}
	}
}