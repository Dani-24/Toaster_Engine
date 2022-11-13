#include "Comp_Mesh.h"

#include "GameObject.h"

Comp_Mesh::Comp_Mesh(GameObject* gameObject, Mesh* m) : Component(gameObject, Comp_Type::Mesh)
{
	AddMesh(m);
}

Comp_Mesh::~Comp_Mesh()
{
	compMesh = nullptr;
}

void Comp_Mesh::AddMesh(Mesh* m) {
	compMesh = m;
}

void Comp_Mesh::ParentPositionUpdate(float3 position) {
	UpdateMatrix(position, float3(0,0,0), float3(0, 0, 0));
}

void Comp_Mesh::ParentScaleUpdate(float3 scale) {
	UpdateMatrix(float3(0, 0, 0), scale, float3(0, 0, 0));
}

void Comp_Mesh::ParentRotationUpdate(float3 rotation) {
	UpdateMatrix(float3(0, 0, 0), float3(0, 0, 0),rotation);
}

void Comp_Mesh::ParentTransformUpdate(float3 position, float3 scale, float3 rotation) {
	UpdateMatrix(position, scale, rotation);
}

void Comp_Mesh::UpdateMatrix(float3 _position, float3 _scale, float3 _rotation) {
	float x = _rotation.x * DEGTORAD;
	float y = _rotation.y * DEGTORAD;
	float z = _rotation.z * DEGTORAD;

	compMesh->matrix[0] = cos(y) * cos(z);
	compMesh->matrix[1] = -cos(x) * sin(z) + sin(y) * cos(z) * sin(x);
	compMesh->matrix[2] = sin(x) * sin(z) + sin(y) * cos(z) * cos(x);
	compMesh->matrix[3] = _position.x;

	compMesh->matrix[4] = cos(y) * sin(z);
	compMesh->matrix[5] = cos(x) * cos(z) + sin(y) * sin(z) * sin(z);
	compMesh->matrix[6] = -sin(x) * cos(z) + sin(y) * sin(z) * cos(x);
	compMesh->matrix[7] = _position.y;

	compMesh->matrix[8] = -sin(y);
	compMesh->matrix[9] = cos(y) * sin(x);
	compMesh->matrix[10] = cos(x) * cos(y);
	compMesh->matrix[11] = _position.z;

	compMesh->matrix[12] = 0;
	compMesh->matrix[13] = 0;
	compMesh->matrix[14] = 0;
	compMesh->matrix[15] = 1;

	compMesh->matrix[0] *= _scale.x;
	compMesh->matrix[5] *= _scale.y;
	compMesh->matrix[10] *= _scale.z;

	compMesh->matrix = transpose(compMesh->matrix);
}

void Comp_Mesh::RenderMesh() {
	if (compMesh->shouldRender) {
		compMesh->Render(go->GetComponent(Component::Comp_Type::Texture)->GetTexture());
	}
}

void Comp_Mesh::ShowMesh(bool show) {
	compMesh->shouldRender = show;
}

void Comp_Mesh::OnEditor() {
	// MESH COMPONENT
	if (compMesh != nullptr) {

		ImGui::TextWrapped("Component : MESH"); ImGui::NewLine();
		ImGui::TextWrapped("Path : %s", compMesh->path.c_str());

		ImGui::TextWrapped("Show Mesh: ");
		ImGui::SameLine();

		ImGui::Selectable("Visible : ", &compMesh->shouldRender);
		ImGui::SameLine();
		if (compMesh->shouldRender) {
			ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.0f, 1.0f), "True");
		}
		else {
			ImGui::TextColored(ImVec4(1.f, 0.0f, 0.0f, 1.0f), "False");
		}

		// Delete Mesh

		bool deleteMesh = false;
		ImGui::Selectable("Delete Component", &deleteMesh);

		if (deleteMesh) {
			go->RemoveComponent(this);
		}
	}
}