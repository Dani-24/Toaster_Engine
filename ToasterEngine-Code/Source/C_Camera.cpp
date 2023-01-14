#include "C_Camera.h"

#include "Application.h"

#include "ModuleEditor.h"

#include "C_Transform.h"

C_Camera::C_Camera() : Component(nullptr)
{
	name = "Camera";
}

C_Camera::C_Camera(GameObject* _gm) : Component(_gm)
{
	name = "Camera";
	
	GO_camera->camFrustum.pos = gameObject->transform->position;
}

C_Camera::~C_Camera()
{
	GO_camera = nullptr;
}

bool C_Camera::OnEditor() {
	if (GO_camera != nullptr) {
		app->editor->Space();

		ImGui::TextWrapped("Component : Camera");
		ImGui::SameLine();
		if (ImGui::Checkbox("Active", &GO_camera->active)) {
			if (GO_camera->active) {
				app->camera->activeCamera = GO_camera;
				for (int i = 0; i < app->camera->cameras.size(); i++) {
					if (app->camera->cameras[i] != GO_camera) {
						app->camera->cameras[i]->active = false;
					}
				}
			}
		}

		ImGui::NewLine();

		ImGui::Image((ImTextureID)GO_camera->cameraBuffer.GetTexture(), ImVec2(200, 100), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::NewLine();

		ImGui::TextWrapped("Propierties :");

		ImGui::NewLine();

		ImGui::Checkbox("Frustum Culling", &GO_camera->frustumCulling);

		ImGui::NewLine();

		ImGui::TextWrapped("Aspect Ratio : ");
		ImGui::SameLine();
		if (ImGui::DragFloat("AR", &GO_camera->aspectRatio, 0.1f)) {
			GO_camera->SetAspectRatio(GO_camera->aspectRatio);
		}

		ImGui::TextWrapped("FOV :          ");
		ImGui::SameLine();
		float fov = math::RadToDeg(GO_camera->FOV);
		if (ImGui::DragFloat("FOV", &fov, 0.1f)) {
			GO_camera->SetFOV(fov);
		}

		ImGui::TextWrapped("Range :        ");
		ImGui::SameLine();
		if (ImGui::DragFloat("R", &GO_camera->range, 0.1f)) {
			GO_camera->SetRange(GO_camera->range);
		}

		ImGui::TextWrapped("Looking at :        ");
		if (ImGui::DragFloat3("XYZ", &camLookAt[0], 0.1f)) {
			GO_camera->LookAt(camLookAt);
		}
	}
}

Camera* C_Camera::GetCamera() {
	return GO_camera;
}