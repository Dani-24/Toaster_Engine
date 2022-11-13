#include "Comp_Transform.h"

#include "GameObject.h"

Comp_Transform::Comp_Transform(GameObject* go) : Component(go, Comp_Type::Transform) {
	/*if (this->GetGameObject()->GetParent() != NULL) {
		Comp_Transform* parentTrans = dynamic_cast<Comp_Transform*>(go->GetParent()->GetComponent(Component::Comp_Type::Transform));
		componentParentTrans = parentTrans->GetGlobalTransform();
	}*/
}

void Comp_Transform::SetPos(float3 pos) {
	this->componentTrans.position = pos;
	UpdatePosition();
}

void Comp_Transform::SetRot(float3 rot) {
	this->componentTrans.rotation = rot;
	UpdateRotation();
}

void Comp_Transform::SetScale(float3 scale) {
	this->componentTrans.scale = scale;
	UpdateScale();
}

void Comp_Transform::SetTransform(float3 pos, float3 rot, float3 scale) {
	this->componentTrans.position = pos;
	this->componentTrans.rotation = rot;
	this->componentTrans.scale = scale;
	UpdateTransform();
}

void Comp_Transform::Translate(float3 translation) {
	this->componentTrans.position += translation;
	UpdatePosition();
}

void Comp_Transform::Rotate(float3 rot) {
	this->componentTrans.rotation += rot;
	UpdateRotation();
}

void Comp_Transform::Scale(float3 scale) {
	this->componentTrans.scale += scale;
	UpdateScale();
}

void Comp_Transform::UpdatePosition() {
	float3 globalPosition = componentParentTrans.position + componentTrans.position;
	
	for (size_t i = 0; i < this->GetGameObject()->childs.size(); i++)
	{
		Comp_Transform* child_transform = dynamic_cast<Comp_Transform*>(this->GetGameObject()->childs.at(i)->GetComponent(Component::Comp_Type::Transform));
		child_transform->ParentPositionUpdate(globalPosition);
	}

	for (size_t i = 1; i < this->GetGameObject()->components.size(); i++)
	{
		this->GetGameObject()->components.at(i)->ParentPositionUpdate(globalPosition);
	}
}

void Comp_Transform::UpdateRotation() {
	float3 globalRot = componentParentTrans.rotation + componentTrans.rotation;

	for (size_t i = 0; i < this->GetGameObject()->childs.size(); i++)
	{
		Comp_Transform* child_transform = dynamic_cast<Comp_Transform*>(this->GetGameObject()->childs.at(i)->GetComponent(Component::Comp_Type::Transform));
		child_transform->ParentRotationUpdate(globalRot);
	}

	for (size_t i = 1; i < this->GetGameObject()->components.size(); i++)
	{
		this->GetGameObject()->components.at(i)->ParentRotationUpdate(globalRot);
	}
}

void Comp_Transform::UpdateScale() {
	float3 globalScale = componentParentTrans.scale + componentTrans.scale;

	for (size_t i = 0; i < this->GetGameObject()->childs.size(); i++)
	{
		Comp_Transform* child_transform = dynamic_cast<Comp_Transform*>(this->GetGameObject()->childs.at(i)->GetComponent(Component::Comp_Type::Transform));
		child_transform->ParentScaleUpdate(globalScale);
	}

	for (size_t i = 1; i < this->GetGameObject()->components.size(); i++)
	{
		this->GetGameObject()->components.at(i)->ParentScaleUpdate(globalScale);
	}
}

void Comp_Transform::UpdateTransform() {
	Transform globalTransform = GetGlobalTransform();
	
	for (size_t i = 0; i < this->GetGameObject()->childs.size(); i++)
	{
		Comp_Transform* child_transform = dynamic_cast<Comp_Transform*>(this->GetGameObject()->childs.at(i)->GetComponent(Component::Comp_Type::Transform));
		child_transform->ParentTransformUpdate(globalTransform.position, globalTransform.scale, globalTransform.rotation);
	}

	for (size_t i = 1; i < this->GetGameObject()->components.size(); i++)
	{
		this->GetGameObject()->components.at(i)->ParentTransformUpdate(globalTransform.position, globalTransform.scale, globalTransform.rotation);
	}
}

void Comp_Transform::SetTransformMatrix(float3 _position, float3 _rotation, float3 _scale) {
	Transform globalTransform = GetGlobalTransform();

	math::Quat rotation = Quat::FromEulerXYZ(math::DegToRad(globalTransform.rotation.x), math::DegToRad(globalTransform.rotation.y), math::DegToRad(globalTransform.rotation.z));

	goTransform = float4x4::FromTRS(globalTransform.position, rotation, float3(1, 1, 1));

}

Transform Comp_Transform::GetGlobalTransform() {
	if (this->GetGameObject()->GetParent() == NULL) return componentTrans;

	Transform global_transform;
	global_transform.position = componentParentTrans.position + componentTrans.position;
	global_transform.rotation = componentParentTrans.rotation + componentTrans.rotation;
	global_transform.scale = componentParentTrans.scale + componentTrans.scale;

	return global_transform;
}

void Comp_Transform::ParentPositionUpdate(float3 pos) {
	componentParentTrans.position = pos;
	UpdatePosition();
}

void Comp_Transform::ParentRotationUpdate(float3 rot) {
	componentParentTrans.rotation = rot;
	UpdateRotation();
}

void Comp_Transform::ParentScaleUpdate(float3 scale) {
	componentParentTrans.scale = scale;
	UpdateScale();
}

void Comp_Transform::ParentTransformUpdate(float3 pos, float3 rot, float3 scale) {
	componentParentTrans.position = pos;
	componentParentTrans.rotation = rot;
	componentParentTrans.scale = scale;

	UpdateTransform();
}

void Comp_Transform::OnEditor() {
	// Transform Component
	ImGui::TextWrapped("Component : TRANSFORM"); ImGui::NewLine();

	ImGui::TextWrapped("Position : ");
	ImGui::SameLine();

	float3 pos = float3(componentTrans.position.x, componentTrans.position.y, componentTrans.position.z);
	if (ImGui::DragFloat3("pos", &pos[0], 0.1f)) {
		SetPos(pos);
	}

	ImGui::TextWrapped("Rotation : ");
	ImGui::SameLine();
	float3 rot = float3(componentTrans.rotation.x, componentTrans.rotation.y, componentTrans.rotation.z);
	if (ImGui::DragFloat3("rot", &rot[0], 0.1f)) {
		SetRot(rot);
	}

	ImGui::TextWrapped("Scale :    ");
	ImGui::SameLine();
	float3 scale = float3(componentTrans.scale.x, componentTrans.scale.y, componentTrans.scale.z);
	if (ImGui::DragFloat3("scl", &scale[0], 0.1f)) {
		SetScale(scale);
	}

	SetTransformMatrix(pos, rot, scale);
}