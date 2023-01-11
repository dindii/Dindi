#include "Dindipch.h"
#include "UILayer.h"

#include <GUI/GUI.h>

#include <Utils/FileDialog.h>
#include <Core/Application.h>

#include <Rendering/Core/Renderer.h>

namespace Dindi
{
	void UILayer::Update(const DeltaTime& dt)
	{		
		GUI::Begin();

		ProcessViewport();
		ProcessMenu();
		ProcessLightInspector();
		ProcessModelInspector();
		ProcessPerformanceStats(dt);

		GUI::End();
	}

	void UILayer::ProcessPerformanceStats(const DeltaTime& dt)
	{
		vec2 windowSize = { (float)m_FrameWidth, (float)m_FrameHeight };
	
		ImGui::SetNextWindowPos({ windowSize.x * m_StatsWindowPosX, m_MenuBarHeight });
		ImGui::SetNextWindowSize({ windowSize.x * m_StatsWindowWidth, m_StatsWindowHeight });

		ImGui::Begin("Stats");
		ImGui::Text("%.4f ms", dt.Milliseconds());
		ImGui::End();
	}

	
	void UILayer::ProcessMenu()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("New"))
			{
				if (ImGui::MenuItem("Light"))
				{
					m_Scene->AddPointLight({ { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } });
				}

				if (ImGui::MenuItem("Model..."))
				{
					std::string path = FileDialog::OpenFile(".obj\0\0");
					Model* newModel = new Model(path, vec3(0.0f, 0.0f, 0.0f), 1.0f);

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

		vec2 windowSize = { (float)m_FrameWidth, (float)m_FrameHeight };


		ImGui::SetNextWindowPos({ windowSize.x * m_LightInspectorPosX, m_MenuBarHeight });
		ImGui::SetNextWindowSize({ windowSize.x * m_LightInspectorWidth, windowSize.y });

		ImGui::Begin("Scene Lights");

		std::vector<PointLight>& sceneLights = m_Scene->GetLights();

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
				light.SetPosition({ pos[0], pos[1], pos[2], 0.0f });

			if (ImGui::ColorEdit3(lightColorLabel, color))
				light.SetColor({ color[0], color[1], color[2], 0.0f });

			if (ImGui::Button(lightRemoveLabel))
				sceneLights.erase(sceneLights.begin() + x);

			ImGui::NewLine();
		}
		ImGui::End();
	}
	
	void UILayer::ProcessModelInspector()
	{
		static constexpr int maxModelLabelSize = 128;

		vec2 windowSize = { (float)m_FrameWidth, (float)m_FrameHeight };

		ImGui::SetNextWindowPos({ m_ModelInspectorWindowPosX, m_MenuBarHeight });
		ImGui::SetNextWindowSize({ windowSize.x * m_ModelInspectorWindowWidth, windowSize.y });

		ImGui::Begin("Scene Models");

		std::vector<Model*>& sceneEntities = m_Scene->GetEntities();

		for (int x = 0; x < sceneEntities.size(); x++)
		{
			Model* model = sceneEntities[x];

			vec3 pos = model->GetPosition();
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
				model->SetPosition({ pos[0], pos[1], pos[2] });

			if (ImGui::DragFloat(ModelScaleLabel, &scale, m_ScaleSliderSpeed))
				model->SetScale(scale);

			if (ImGui::Button(ModelDeleteLabel))
				sceneEntities.erase(sceneEntities.begin() + x);

			ImGui::NewLine();
		}

		ImGui::End();

	}

	void UILayer::ProcessViewport()
	{
	

		vec2 windowSize = { (float)m_FrameWidth, (float)m_FrameHeight };

		//#TODO - Turn those values into variables.
		ImGui::SetNextWindowPos({ windowSize.x * m_ModelInspectorWindowWidth, m_MenuBarHeight });
		ImGui::SetNextWindowSize({ windowSize.x * m_ViewportPosX, windowSize.y });

		vec2 viewportDims(windowSize.x * m_ViewportPosX, windowSize.y);

		Application& app = Application::GetInstance();
		Camera* sceneCamera = app.GetActiveScene()->GetActiveCamera();

		sceneCamera->RemakeProjection(viewportDims.x, viewportDims.y);

	//	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::Image((ImTextureID)Renderer::GetScreenOutputHandle(), { windowSize.x * m_ViewportPosX, windowSize.y }, { 0,1 }, { 1,0 });
		
		//This must use the same ImGui window to draw the gizmo, so it is necessary to keep it in the same Begin-End call
		ProcessTransformGizmo();
		
		//ImGui::PopStyleVar(ImGuiStyleVar_WindowPadding);
		ImGui::End();

	}

	void UILayer::ProcessTransformGizmo()
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		
		ImVec2 parentWindowPos = ImGui::GetWindowPos();
		ImVec2 parentWindowSize = { (float)ImGui::GetWindowWidth(), (float)ImGui::GetWindowHeight() };
		ImGuizmo::SetRect(parentWindowPos.x, parentWindowPos.y, parentWindowSize.x, parentWindowSize.y);

		ImGuizmo::OPERATION transformOperation = ImGuizmo::OPERATION::TRANSLATE;
		ImGuizmo::MODE transformMode = ImGuizmo::MODE::WORLD;

		const mat4& cameraProjection = m_Scene->GetActiveCamera()->GetProjection();
		const mat4& cameraView = m_Scene->GetActiveCamera()->GetViewMatrix();

		Model* model = nullptr;
		
		//DEBUG
		if (m_Scene->GetEntities().size() > 0)
			model = m_Scene->GetEntities()[0];

		if (model)
		{
			mat4 modelTransform;
			modelTransform = mat4::Translate(model->GetPosition());// *mat4::Scale(model->GetScale());

			ImGuizmo::Manipulate(cameraView.elements, cameraProjection.elements, transformOperation, transformMode, modelTransform.elements);
			
			vec3 translation, rotation, scale;

			ImGuizmo::DecomposeMatrixToComponents(modelTransform.elements, translation.elements, rotation.elements, scale.elements);

			//DND_LOG_TRACE("Guizmo Translation: ", translation);
			//DND_LOG_TRACE("Guizmo Rotation: ", rotation);
			//DND_LOG_TRACE("Guizmo Scale: ", scale);

			model->SetPosition(translation);
		//	model->SetScale({ scale[0] });
		}
		

	}
}