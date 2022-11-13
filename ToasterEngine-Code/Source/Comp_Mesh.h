#pragma once

#include "Component.h"

#include "ModuleMesh3D.h"

class Comp_Mesh : public Component {
public:
	Comp_Mesh(GameObject* gameObject, Mesh* m);
	virtual ~Comp_Mesh() override;

	void AddMesh(Mesh* m);

	void RenderMesh() override;

	void ShowMesh(bool show);

	void ParentPositionUpdate(float3 position) override;
	void ParentScaleUpdate(float3 scale) override;
	void ParentRotationUpdate(float3 rotation) override;
	void ParentTransformUpdate(float3 position, float3 scale, float3 rotation) override;

	Mesh* GetMesh() { return compMesh; }

	void UpdateMatrix(float3 position, float3 scale, float3 rotation);

	void OnEditor();

private:
	Mesh* compMesh = nullptr;
};