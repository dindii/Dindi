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
		ProcessTransformGizmo();
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
		ImGui::Begin("Viewport");
		
		ImGui::Image((ImTextureID)Renderer::GetScreenOutputHandle(), {1366, 768}, { 0,1 }, { 1,0 });

		ImGui::End();
	}

	void UILayer::ProcessTransformGizmo()
	{
		ImGuizmo::SetOrthographic(true);
		
		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		ImGuizmo::SetDrawlist();



		ImGuizmo::OPERATION transformOperation = ImGuizmo::OPERATION::SCALE;
		ImGuizmo::MODE transformMode = ImGuizmo::MODE::LOCAL;

		const mat4& cameraProjection = m_Scene->GetActiveCamera()->GetProjection();
		const mat4& cameraView = m_Scene->GetActiveCamera()->GetViewMatrix();

		//ImGuizmo::Manipulate(cameraView.elements, cameraProjection.elements, transformOperation, transformMode, mat4().elements);
		//ImGuizmo::DrawGrid(cameraView.elements, cameraProjection.elements, mat4().elements, 64 * 64);
		Model* model = nullptr;
		
		if (m_Scene->GetEntities().size() > 0)
			model = m_Scene->GetEntities()[0];

		if (model)
		{
			mat4 modelTransform;
			modelTransform = mat4::Translate(model->GetPosition()) * mat4::Scale({ model->GetScale() });
			ImGuizmo::Manipulate(cameraView.elements, cameraProjection.elements, transformOperation, transformMode, modelTransform.elements);
		}



	}

}