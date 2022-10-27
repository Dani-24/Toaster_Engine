
#include "Globals.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Primitive.h"

#include "glmath.h"
#include <cmath>

#include "ModuleMesh3D.h"

// ------------------------------------------------------------
Primitive::Primitive() : transform(IdentityMatrix), color(White), wire(false), axis(true), type(PrimitiveTypes::Primitive_Point)
{}

// ------------------------------------------------------------
PrimitiveTypes Primitive::GetType() const
{
	return type;
}

Mesh* Primitive::CreateCube()
{
	float s = 0.5f;

	Mesh* mesh = new Mesh();

	//VERTICES
	mesh->num_vertices = 8;
	mesh->vertices = new float[mesh->num_vertices * 3]; //3 floats per vertex

	//generate all vertices
	for (int i = 0; i < (mesh->num_vertices * 3); i++) {
		int p = 1;	//positivity(sign)
		int count = i / 3;
		switch (i % 3) {
			//x
		case 0:
			if (count == 0 || count == 3 || count == 4 || count == 7) p = -1;
			break;
			//y
		case 1:
			if (count == 2 || count == 3 || count == 6 || count == 7) p = -1;
			break;
			//z
		case 2:
			if (count <= 3) p = -1;
			break;
		}

		mesh->vertices[i] = s * p;
	}

	//INDICES
	mesh->num_indices = 36;	//6*2 triangle faces, 3 index per triangle face
	mesh->indices = new uint[mesh->num_indices];

	uint index[36] =
	{ 0,3,1,
	3,2,1,
	1,2,5,
	2,6,5,
	5,6,4,
	6,7,4,
	4,7,0,
	7,3,0,
	4,0,5,
	0,1,5,
	3,7,2,
	7,6,2 };

	for (int i = 0; i < mesh->num_indices; i++) {
		mesh->indices[i] = index[i];
	}

	return mesh;
}

Mesh* Primitive::CreateSphere()
{
	Mesh* mesh = new Mesh();


	return mesh;
}

Mesh* Primitive::CreateCylinder()
{
	Mesh* mesh = new Mesh();


	return mesh;
}

Mesh* Primitive::CreatePlane()
{
	float s = 0.5;

	Mesh* mesh = new Mesh();

	//VERTICES
	mesh->num_vertices = 4;
	mesh->vertices = new float[mesh->num_vertices * VERTEX_ARG]; //3 floats per vertex

	//Vertex 0
	//x y z
	mesh->vertices[0] = -s;
	mesh->vertices[1] = 0;
	mesh->vertices[2] = -s;
	//uv
	mesh->vertices[3] = -1;
	mesh->vertices[4] = -1;

	//Vertex 1
	mesh->vertices[5] = s;
	mesh->vertices[6] = 0;
	mesh->vertices[7] = -s;
	mesh->vertices[8] = 1;
	mesh->vertices[9] = -1;

	//Vertex 2
	mesh->vertices[10] = -s;
	mesh->vertices[11] = 0;
	mesh->vertices[12] = s;
	mesh->vertices[13] = -1;
	mesh->vertices[14] = 1;

	//Vertex 3
	mesh->vertices[15] = s;
	mesh->vertices[16] = 0;
	mesh->vertices[17] = s;
	mesh->vertices[18] = 1;
	mesh->vertices[19] = 1;


	//INDICES
	mesh->num_indices = 6;	//3* 2 faces
	mesh->indices = new uint[mesh->num_indices];

	mesh->indices[0] = 0;
	mesh->indices[1] = 2;
	mesh->indices[2] = 1;

	mesh->indices[3] = 2;
	mesh->indices[4] = 3;
	mesh->indices[5] = 1;


	return mesh;
}

void Primitive::Render() const
{

	glPushMatrix();
	glMultMatrixf(transform.M);

	if(axis == true)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);

	if(wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	InnerRender();

	glPopMatrix();
}

void Primitive::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

void Primitive::SetPos(float x, float y, float z)
{
	transform.translate(x, y, z);
}

vec3 Primitive::GetPos() {
	return transform.translation();
}

void Primitive::SetRotation(float angle, const vec3 &u)
{
	transform.rotate(angle, u);
}

//void Primitive::SetRotationEuler(btQuaternion q)
//{
//	float angle = 2 * acos(q.getW());
//	float x = q.getX() / sqrt(1 - q.getW() * q.getW());
//	float y = q.getY() / sqrt(1 - q.getW() * q.getW());
//	float z = q.getZ() / sqrt(1 - q.getW() * q.getW());
//
//	transform.rotate(angle, {x,y,z});
//}

void Primitive::Scale(float x, float y, float z)
{
	transform.scale(x, y, z);
}

// CUBE ============================================
CubePrimitive::CubePrimitive() : Primitive(), size(1.0f, 1.0f, 1.0f)
{
	type = PrimitiveTypes::Primitive_Cube;
}

CubePrimitive::CubePrimitive(float sizeX, float sizeY, float sizeZ) : Primitive(), size(sizeX, sizeY, sizeZ)
{
	type = PrimitiveTypes::Primitive_Cube;
}

void CubePrimitive::InnerRender() const
{	
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	glBegin(GL_QUADS);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-sx, -sy, sz);
	glVertex3f( sx, -sy, sz);
	glVertex3f( sx,  sy, sz);
	glVertex3f(-sx,  sy, sz);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f( sx, -sy, -sz);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx,  sy, -sz);
	glVertex3f( sx,  sy, -sz);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(sx, -sy,  sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx,  sy, -sz);
	glVertex3f(sx,  sy,  sz);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, -sy,  sz);
	glVertex3f(-sx,  sy,  sz);
	glVertex3f(-sx,  sy, -sz);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-sx, sy,  sz);
	glVertex3f( sx, sy,  sz);
	glVertex3f( sx, sy, -sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f( sx, -sy, -sz);
	glVertex3f( sx, -sy,  sz);
	glVertex3f(-sx, -sy,  sz);

	glEnd();
}

// SPHERE ============================================
SpherePrimitive::SpherePrimitive() : Primitive(), radius(1.0f)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

SpherePrimitive::SpherePrimitive(float radius) : Primitive(), radius(radius)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

void SpherePrimitive::InnerRender() const
{
	//glutSolidSphere(radius, 25, 25);
}


// CYLINDER ============================================
CylinderPrimitive::CylinderPrimitive() : Primitive(), radius(1.0f), height(1.0f)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

CylinderPrimitive::CylinderPrimitive(float radius, float height) : Primitive(), radius(radius), height(height)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

void CylinderPrimitive::InnerRender() const
{
	int n = 30;

	// Cylinder Bottom
	glBegin(GL_POLYGON);
	
	for(int i = 360; i >= 0; i -= (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder Top
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for(int i = 0; i <= 360; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(height * 0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder "Cover"
	glBegin(GL_QUAD_STRIP);
	for(int i = 0; i < 480; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians

		glVertex3f(height*0.5f,  radius * cos(a), radius * sin(a) );
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a) );
	}
	glEnd();
}

// LINE ==================================================
LinePrimitive::LinePrimitive() : Primitive(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = PrimitiveTypes::Primitive_Line;
}

LinePrimitive::LinePrimitive(float x, float y, float z) : Primitive(), origin(0, 0, 0), destination(x, y, z)
{
	type = PrimitiveTypes::Primitive_Line;
}

void LinePrimitive::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();

	glLineWidth(1.0f);
}

// PLANE ==================================================
PlanePrimitive::PlanePrimitive() : Primitive(), normal(0, 1, 0), constant(1)
{
	type = PrimitiveTypes::Primitive_Plane;
}

PlanePrimitive::PlanePrimitive(float x, float y, float z, float d) : Primitive(), normal(x, y, z), constant(d)
{
	type = PrimitiveTypes::Primitive_Plane;
}

void PlanePrimitive::InnerRender() const
{
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	float d = 200.0f;

	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}