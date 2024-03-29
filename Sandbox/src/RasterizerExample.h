#pragma once
#include <Core/Layer.h>
#include <Utils/DeltaTime.h>
#include <Event/Event.h>
#include <Rendering/Scene/Scene.h>
#include <Rendering/Camera/Camera.h>

class RasterizerExample : public Dindi::Layer
{
public:
	virtual void OnUIRender() override;
	virtual void OnAttach()   override;
	virtual void OnDetach()   override;
	virtual void OnUpdate(const Dindi::DeltaTime& dt) override;
	virtual void OnEvent(Dindi::Event& event) override;

private:
	Dindi::Scene* SceneOne;
	Dindi::Camera* m_DefaultEditorCamera;
	
	float m_MouseSensitivity = 0.05f;
	float m_DefaultEditorCameraSpeed;

	void MoveCamera(const Dindi::DeltaTime& dt);
	void LookAround();
};