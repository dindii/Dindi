#include "Dindipch.h"
#include "UILayer.h"

#include <GUI/GUI.h>

#include <Utils/FileDialog.h>
#include <Core/Application.h>

namespace Dindi
{
	void UILayer::Update(const DeltaTime& dt)
	{
		GUI::Begin();

		ProcessMenu();
		ProcessLightInspector();
		ProcessModelInspector();
		ProcessPerformanceStats(dt);
		
		GUI::End();
	}

	void UILayer::ProcessPerformanceStats(const DeltaTime& dt)
	{
		vec2 windowSize = { (float)m_FrameWidth, (float)m_FrameHeight };

		float statsWindowHeight = 65.0f;
		
		//Turn this variable into a member. Several windows would like to know where our menu bar are.
		float menuBarHeight = 22.0f;

		ImGui::SetNextWindowPos({ windowSize.x * 0.78f, menuBarHeight });
		ImGui::SetNextWindowSize({ windowSize.x * 0.07f, statsWindowHeight });

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

		//So we can draw our inspector right after the menu bar
		float menuBarHeight = 22.0f;

		ImGui::SetNextWindowPos({ windowSize.x * 0.85f, menuBarHeight });
		ImGui::SetNextWindowSize({ windowSize.x * 0.15f, windowSize.y });

		ImGui::Begin("Scene Lights");

		for (int x = 0; x < m_Scene->GetLights().size(); x++)
		{
			PointLight& light0 = m_Scene->GetLights()[x];

			float pos[3] = { light0.GetPosition().x, light0.GetPosition().y, light0.GetPosition().z };
			float color[3] = { light0.GetColor().x, light0.GetColor().y, light0.GetColor().z };

			char lightLabel[maxLightLabelSize] = "Position##";
			char lightColorLabel[maxLightLabelSize] = "Color##";
			char lightRemoveLabel[maxLightLabelSize] = "Delete##";

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

			if (ImGui::ColorEdit3(lightColorLabel, color))
				light0.SetColor({ color[0], color[1], color[2], 0.0f });

			if (ImGui::Button(lightRemoveLabel))
				m_Scene->GetLights().erase(m_Scene->GetLights().begin() + x);

			ImGui::NewLine();
		}
		ImGui::End();
	}
	
	void UILayer::ProcessModelInspector()
	{

	}
}