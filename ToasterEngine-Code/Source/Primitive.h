#pragma once
#include "glmath.h"
#include "Color.h"

class btQuaternion;
class Mesh;

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder
};

class Primitive
{
public:

	Primitive();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	vec3			GetPos();
	void			SetRotation(float angle, const vec3 &u);
	void			SetRotationEuler(btQuaternion q);
	void			Scale(float x, float y, float z);
	PrimitiveTypes	GetType() const;

public:
	
	Color color;
	mat4x4 transform;
	bool axis,wire;

protected:
	PrimitiveTypes type;
};

// ============================================
class CubePrimitive : public Primitive
{
public :
	CubePrimitive();
	CubePrimitive(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	vec3 size;
};

// ============================================
class SpherePrimitive : public Primitive
{
public:
	SpherePrimitive();
	SpherePrimitive(float radius);
	void InnerRender() const;
public:
	float radius;
};

// ============================================
class CylinderPrimitive : public Primitive
{
public:
	CylinderPrimitive();
	CylinderPrimitive(float radius, float height);
	void InnerRender() const;
public:
	float radius;
	float height;
};

// ============================================
class LinePrimitive : public Primitive
{
public:
	LinePrimitive();
	LinePrimitive(float x, float y, float z);
	void InnerRender() const;
public:
	vec3 origin;
	vec3 destination;
};

// ============================================
class PlanePrimitive : public Primitive
{
public:
	PlanePrimitive();
	PlanePrimitive(float x, float y, float z, float d);
	void InnerRender() const;
public:
	vec3 normal;
	float constant;
};