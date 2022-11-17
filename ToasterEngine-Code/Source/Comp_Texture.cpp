#include "Comp_Texture.h"

#include "GameObject.h"

#include "Application.h"

Comp_Texture::Comp_Texture(GameObject* gameObject, uint t) : Component(gameObject, Comp_Type::Texture)
{
	texture = new Texture();
	AddTexture(t);
	originalTexture = t;
}

Comp_Texture::~Comp_Texture() {

}

void Comp_Texture::AddTexture(uint t) {
	texture->OpenGLID = t;
}

uint Comp_Texture::GetTexture() {
	if (texture->bind) {
		return texture->OpenGLID;
	}
	else {
		return NULL;
	}
}

void Comp_Texture::OnEditor() {
	// TEXTURE COMPONENT
	if (texture != NULL) {
		ImGui::TextWrapped("Component : TEXTURES");

		if (ImGui::BeginCombo("Texture", "Select", ImGuiComboFlags_HeightSmall))
		{
			bool is_selected = (texture->OpenGLID == originalTexture);
			if (ImGui::Selectable("Default", is_selected))
			{
				texture->OpenGLID = originalTexture;
			}
			is_selected = (texture->OpenGLID == app->textures->checkers_texture);
			if (ImGui::Selectable("Checkers", is_selected))
			{
				texture->OpenGLID = app->textures->checkers_texture;
			}
			ImGui::EndCombo();
		}
		ImGui::TextWrapped("Show Texture: ");
		ImGui::SameLine();
		ImGui::Selectable("Visible : ", &texture->bind);
		ImGui::SameLine();
		if (texture->bind) {
			ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.0f, 1.0f), "True");
		}
		else {
			ImGui::TextColored(ImVec4(1.f, 0.0f, 0.0f, 1.0f), "False");
		}

		// Delete Texture

		ImGui::Selectable("Delete Component ", &deleteTexture);

		if (deleteTexture) {
			go->RemoveComponent(this);
		}
	}
}