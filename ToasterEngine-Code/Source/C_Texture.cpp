#include "C_Texture.h"

#include "../External/ImGui/imgui.h"

#include "Application.h"

#include "ModuleRenderer3D.h"
//#include "MO_Scene.h"

#include "R_Texture.h"
#include "R_Shader.h"
#include "ResourceManager.h"

//#include "DEJsonSupport.h"
#include "ModuleTexture.h"

#include "R_Material.h"

#include "ModuleImporter.h"
//#include "IM_MaterialImporter.h"

C_Material::C_Material(GameObject* _gm) : Component(_gm), viewWithCheckers(false), matTexture(nullptr),
material(nullptr)
{
	name = "Material";
	material = ( app->scene->defaultMaterial != nullptr) ? dynamic_cast<ResourceMaterial*>(app->resourceManager->RequestResource(app->scene->defaultMaterial->GetUID())) : NULL;
}

C_Material::~C_Material()
{
	if (matTexture != nullptr)
		app->resourceManager->UnloadResource(matTexture->GetUID());

	if (material != nullptr)
		app->resourceManager->UnloadResource(material->GetUID());
}

#ifndef STANDALONE
bool C_Material::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		ImGui::Text("Current material: ");

		ImGui::Text("Drop here to change material");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_MATERIAL"))
			{
				std::string* assetsPath = (std::string*)payload->Data;

				ResourceMaterial* newMaterial = dynamic_cast<ResourceMaterial*>(app->resourceManager->RequestFromAssets(assetsPath->c_str()));

				SetMaterial(newMaterial);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();
		if (material == nullptr)
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "No material");
		else
		{
			material->DrawEditor("##Mat");
		}

		if (material && material->shader)
		{
			ImGui::Dummy(ImVec2(0, 15));
			ImGui::Text("Using shader: %s", material->shader->GetAssetPath());
		}

		return true;
	}
	return false;
}
#endif // !STANDALONE

void C_Material::SetMaterial(ResourceMaterial* newMaterial)
{
	if (material != nullptr)
		app->resourceManager->UnloadResource(material->GetUID());

	material = newMaterial;
}

int C_Material::GetTextureID()
{
	return (viewWithCheckers == false && (matTexture && matTexture->textureID != 0)) ? matTexture->textureID : EngineExternal->moduleRenderer3D->checkersTexture;
}