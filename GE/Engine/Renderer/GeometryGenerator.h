#pragma once



class GeometryGenerator
{
public:

	static class Mesh Quad(float scale);
	static class Mesh FullScreenQuad();
	static class Mesh Cube();
	static class Mesh Sphere(float radius, uint32 ringCount, uint32 sliceCount);
	static class Mesh Grid(float width, float depth, uint32 m, uint32 n);
	static class Mesh Cylinder(float height, float topRadius, float bottomRadius, uint32 sliceCount, uint32 stackCount);
	static class Mesh Cone(float height, float radius, uint32 sliceCount);

	static void CalcTangentBiTangent(std::vector<SVertex>& OutVertices, const vector<uint32> indices);

	static void CalcTangent(std::vector<SVertex>& OutVertices, const vector<uint32> indices);
};

