#include "EnginePCH.h"
#include "GeometryGenerator.h"
#include "Assets/Mesh/Mesh.h"

/*
https://learn.microsoft.com/ko-kr/windows/win32/dxtecharts/the-direct3d-transformation-pipeline?redirectedfrom=MSDN
*/
class Mesh GeometryGenerator::Quad(float scale)
{
	const float& size = scale;

	//	  1	+-----+ 2	0, 1, 2
	//		|	  |		2, 3, 0
	//		|	  |		
	//	  0 +-----+ 3	
	//     size	
	const std::vector<uint32> indices =
	{
		0, 1, 2,
		2, 3, 0
	};

	std::vector<SVertex>		vertices(4);

	// vertices - CW
	vertices[0].Position = SVector3(-size, -size, 0.0f);
	vertices[0].Normal = SVector3(0.0f, 0.0f, -1.0f);
	vertices[0].UV = SVector2(0.0f, 1.0f);

	vertices[1].Position = SVector3(-size, +size, 0.0f);
	vertices[1].Normal = SVector3(0.0f, 0.0f, -1.0f);
	vertices[1].UV = SVector2(0.0f, 0.0f);

	vertices[2].Position = SVector3(+size, +size, 0.0f);
	vertices[2].Normal = SVector3(0.0f, 0.0f, -1.0f);
	vertices[2].UV = SVector2(1.0f, 0.0f);

	vertices[3].Position = SVector3(+size, -size, 0.0f);
	vertices[3].Normal = SVector3(0.0f, 0.0f, -1.0f);
	vertices[3].UV = SVector2(1.0f, 1.0f);

	CalcTangentBiTangent(vertices, indices);

	Mesh newQuadMesh("Quad");
	newQuadMesh.Initialize<SVertex>(vertices, indices);

	return newQuadMesh;
}

class Mesh GeometryGenerator::FullScreenQuad()
{
	const float& size = 1.0f;

	//	  1	+-----+ 2	0, 1, 2
	//		|	  |		2, 3, 0
	//		|	  |		
	//	  0 +-----+ 3	
	//     size	
	const std::vector<uint32> indices =
	{
		0, 1, 2,
		2, 3, 0
	};

	std::vector<SVertex_PUV>		vertices(4);

	// vertices - CW
	vertices[0].Position = SVector3(-size, -size, 0.0f);	
	vertices[0].UV = SVector2(0.0f, 1.0f);

	vertices[1].Position = SVector3(-size, +size, 0.0f);	
	vertices[1].UV = SVector2(0.0f, 0.0f);

	vertices[2].Position = SVector3(+size, +size, 0.0f);	
	vertices[2].UV = SVector2(1.0f, 0.0f);

	vertices[3].Position = SVector3(+size, -size, 0.0f);	
	vertices[3].UV = SVector2(1.0f, 1.0f);	

	Mesh newQuadMesh = Mesh("FullScrQuad");
	newQuadMesh.Initialize<SVertex_PUV>(vertices, indices);

	return newQuadMesh;
}

class Mesh GeometryGenerator::Cube()
{
	vector<SVertex> vertices(24);
	const vector<uint32> indices =
	{
		0, 1, 2, 0, 2, 3,		// Top
		4, 5, 6, 4, 6, 7,		// back
		8, 9, 10, 8, 10, 11,	// Right
		12, 13, 14, 12, 14, 15, // Back
		16, 17, 18, 16, 18, 19, // Left
		20, 22, 21, 20, 23, 22, // Bottom
	};

	//------------------------------------------------------------------
	//		ASCII Cube art from: http://www.lonniebest.com/ASCII/Art/?ID=2
	// 
	//			   0 _________________________ 1		0, 1, 2, 0, 2, 3,		// Top
	//		        / _____________________  /|			4, 5, 6, 4, 6, 7,		// Front
	//		       / / ___________________/ / |			8, 9, 10, 8, 10, 11,	// Right
	//		      / / /| |               / /  |			12, 13, 14, 12, 14, 15, // Left
	//		     / / / | |              / / . |			16, 17, 18, 16, 18, 19, // Back
	//		    / / /| | |             / / /| |			20, 22, 21, 20, 23, 22, // Bottom
	//		   / / / | | |            / / / | |			
	//		  / / /  | | |           / / /| | |		   +Y
	//		 / /_/__________________/ / / | | |			|  +Z
	//	4,3 /________________________/5/  | | |			|  /
	//		| ______________________8|2|  | | |			| /
	//		| | |    | | |_________| | |__| | |			|/______+X
	//		| | |    | |___________| | |____| |			
	//		| | |   / / ___________| | |_  / /		
	//		| | |  / / /           | | |/ / /		
	//		| | | / / /            | | | / /		
	//		| | |/ / /             | | |/ /			
	//		| | | / /              | | ' /			
	//		| | |/_/_______________| |  /			
	//		| |____________________| | /			
	//		|________________________|/6			
	//		7
	vertices[0].UV = SVector2(+0.0f, +0.0f);
	vertices[1].UV = SVector2(+1.0f, +0.0f);
	vertices[2].UV = SVector2(+1.0f, +1.0f);

	vertices[3].UV = SVector2(+0.0f, +1.0f);
	vertices[4].UV = SVector2(+0.0f, +0.0f);
	vertices[5].UV = SVector2(+1.0f, +0.0f);

	vertices[6].UV = SVector2(+1.0f, +1.0f);
	vertices[7].UV = SVector2(+0.0f, +1.0f);
	vertices[8].UV = SVector2(+0.0f, +0.0f);

	vertices[9].UV = SVector2(+1.0f, +0.0f);
	vertices[10].UV = SVector2(+1.0f, +1.0f);
	vertices[11].UV = SVector2(+0.0f, +1.0f);

	vertices[12].UV = SVector2(+0.0f, +0.0f);
	vertices[13].UV = SVector2(+1.0f, +0.0f);
	vertices[14].UV = SVector2(+1.0f, +1.0f);
	vertices[15].UV = SVector2(+0.0f, +1.0f);
	vertices[16].UV = SVector2(+0.0f, +0.0f);
	vertices[17].UV = SVector2(+1.0f, +0.0f);
	vertices[18].UV = SVector2(+1.0f, +1.0f);
	vertices[19].UV = SVector2(+0.0f, +1.0f);

	vertices[20].UV = SVector2(+1.0f, +0.0f);
	vertices[21].UV = SVector2(+0.0f, +0.0f);
	vertices[22].UV = SVector2(+0.0f, +1.0f);
	vertices[23].UV = SVector2(+1.0f, +1.0f);

	// vertices - CW 
	// TOP
	vertices[0].Position = SVector3(-1.0f, +1.0f, +1.0f);
	vertices[0].Normal = SVector3(+0.0f, +1.0f, +0.0f);
	vertices[0].Tangent = SVector3(+1.0f, +0.0f, +0.0f);

	vertices[1].Position = SVector3(+1.0f, +1.0f, +1.0f);
	vertices[1].Normal = SVector3(+0.0f, +1.0f, +0.0f);
	vertices[1].Tangent = SVector3(+1.0f, +0.0f, +0.0f);

	vertices[2].Position = SVector3(+1.0f, +1.0f, -1.0f);
	vertices[2].Normal = SVector3(+0.0f, +1.0f, +0.0f);
	vertices[2].Tangent = SVector3(+1.0f, +0.0f, +0.0f);

	vertices[3].Position = SVector3(-1.0f, +1.0f, -1.0f);
	vertices[3].Normal = SVector3(+0.0f, +1.0f, +0.0f);
	vertices[3].Tangent = SVector3(+1.0f, +0.0f, +0.0f);

	// FRONT
	vertices[4].Position = SVector3(-1.0f, +1.0f, -1.0f);
	vertices[4].Normal = SVector3(+0.0f, +0.0f, -1.0f);
	vertices[4].Tangent = SVector3(+1.0f, +0.0f, +0.0f);

	vertices[5].Position = SVector3(+1.0f, +1.0f, -1.0f);
	vertices[5].Normal = SVector3(+0.0f, +0.0f, -1.0f);
	vertices[5].Tangent = SVector3(+1.0f, +0.0f, +0.0f);

	vertices[6].Position = SVector3(+1.0f, -1.0f, -1.0f);
	vertices[6].Normal = SVector3(+0.0f, +0.0f, -1.0f);
	vertices[6].Tangent = SVector3(+1.0f, +0.0f, +0.0f);

	vertices[7].Position = SVector3(-1.0f, -1.0f, -1.0f);
	vertices[7].Normal = SVector3(+0.0f, +0.0f, -1.0f);
	vertices[7].Tangent = SVector3(+1.0f, +0.0f, +0.0f);

	// RIGHT
	vertices[8].Position = SVector3(+1.0f, +1.0f, -1.0f);
	vertices[8].Normal = SVector3(+1.0f, +0.0f, +0.0f);
	vertices[8].Tangent = SVector3(+0.0f, +0.0f, +1.0f);

	vertices[9].Position = SVector3(+1.0f, +1.0f, +1.0f);
	vertices[9].Normal = SVector3(+1.0f, +0.0f, +0.0f);
	vertices[9].Tangent = SVector3(+0.0f, +0.0f, +1.0f);

	vertices[10].Position = SVector3(+1.0f, -1.0f, +1.0f);
	vertices[10].Normal = SVector3(+1.0f, +0.0f, +0.0f);
	vertices[10].Tangent = SVector3(+0.0f, +0.0f, +1.0f);

	vertices[11].Position = SVector3(+1.0f, -1.0f, -1.0f);
	vertices[11].Normal = SVector3(+1.0f, +0.0f, +0.0f);
	vertices[11].Tangent = SVector3(+0.0f, +0.0f, +1.0f);

	// BACK
	vertices[12].Position = SVector3(+1.0f, +1.0f, +1.0f);
	vertices[12].Normal = SVector3(+0.0f, +0.0f, +1.0f);
	vertices[12].Tangent = SVector3(+1.0f, +0.0f, +0.0f);

	vertices[13].Position = SVector3(-1.0f, +1.0f, +1.0f);
	vertices[13].Normal = SVector3(+0.0f, +0.0f, +1.0f);
	vertices[13].Tangent = SVector3(+1.0f, +0.0f, +0.0f);

	vertices[14].Position = SVector3(-1.0f, -1.0f, +1.0f);
	vertices[14].Normal = SVector3(+0.0f, +0.0f, +1.0f);
	vertices[14].Tangent = SVector3(+1.0f, +0.0f, +0.0f);

	vertices[15].Position = SVector3(+1.0f, -1.0f, +1.0f);
	vertices[15].Normal = SVector3(+0.0f, +0.0f, +1.0f);
	vertices[15].Tangent = SVector3(+1.0f, +0.0f, +0.0f);

	// LEFT
	vertices[16].Position = SVector3(-1.0f, +1.0f, +1.0f);
	vertices[16].Normal = SVector3(-1.0f, +0.0f, +0.0f);
	vertices[16].Tangent = SVector3(+0.0f, +0.0f, -1.0f);

	vertices[17].Position = SVector3(-1.0f, +1.0f, -1.0f);
	vertices[17].Normal = SVector3(-1.0f, +0.0f, +0.0f);
	vertices[17].Tangent = SVector3(+0.0f, +0.0f, -1.0f);

	vertices[18].Position = SVector3(-1.0f, -1.0f, -1.0f);
	vertices[18].Normal = SVector3(-1.0f, +0.0f, +0.0f);
	vertices[18].Tangent = SVector3(+0.0f, +0.0f, -1.0f);

	vertices[19].Position = SVector3(-1.0f, -1.0f, +1.0f);
	vertices[19].Normal = SVector3(-1.0f, +0.0f, +0.0f);
	vertices[19].Tangent = SVector3(+0.0f, +0.0f, -1.0f);


	// BOTTOM
	vertices[20].Position = SVector3(+1.0f, -1.0f, -1.0f);
	vertices[20].Normal = SVector3(+0.0f, -1.0f, +0.0f);
	vertices[20].Tangent = SVector3(+1.0f, +0.0f, +0.0f);

	vertices[21].Position = SVector3(-1.0f, -1.0f, -1.0f);
	vertices[21].Normal = SVector3(+0.0f, -1.0f, +0.0f);
	vertices[21].Tangent = SVector3(+1.0f, +0.0f, +0.0f);

	vertices[22].Position = SVector3(-1.0f, -1.0f, +1.0f);
	vertices[22].Normal = SVector3(+0.0f, -1.0f, +0.0f);
	vertices[22].Tangent = SVector3(+1.0f, +0.0f, +0.0f);

	vertices[23].Position = SVector3(+1.0f, -1.0f, +1.0f);
	vertices[23].Normal = SVector3(+0.0f, -1.0f, +0.0f);
	vertices[23].Tangent = SVector3(+1.0f, +0.0f, +0.0f);

	//--------------------------------------------------------------

	// make edges unit length
	//for (int i = 0; i < 24; ++i)
	//{
	//	vertices[i].position.x() *= 0.5f;
	//	vertices[i].position.y() *= 0.5f;
	//	vertices[i].position.z() *= 0.5f;
	//}

	CalcTangentBiTangent(vertices, indices);
	Mesh cubeMesh("BuiltInCube");
	cubeMesh.Initialize<SVertex>(vertices,indices);

	return 	cubeMesh;
	
}

class Mesh GeometryGenerator::Sphere(float radius, uint32 ringCount, uint32 sliceCount)
{	
	//BuiltinSphere	
	Mesh newSphere = Mesh("BuiltinSphere");

	// shorthands
	std::vector<SVertex> vertices;
	std::vector<uint32> Indices;

	const uint32 MIN_RING_COUNT = 12;
	const uint32 MIN_SLICE_COUNT = 12;

	if (ringCount <= MIN_RING_COUNT)
		ringCount = MIN_RING_COUNT;

	if (sliceCount <= MIN_SLICE_COUNT)
		sliceCount = MIN_SLICE_COUNT;

	// SPHERE


	// Compute vertices for each stack ring starting at the bottom and moving up.
	float dPhi = XM_PI / (ringCount - 1);
	for (float phi = -XM_PIDIV2; phi <= XM_PIDIV2 + 0.00001f; phi += dPhi)
	{
		float y = radius * sinf(phi);	// horizontal slice center height
		float r = radius * cosf(phi);	// horizontal slice radius

		// vertices of ring
		float dTheta = 2.0f * XM_PI / sliceCount;
		for (uint32 j = 0; j <= sliceCount; ++j)	// for each pice(slice) in horizontal slice
		{
			SVertex vertex;
			float theta = j * dTheta;
			float x = r * cosf(theta);
			float z = r * sinf(theta);
			vertex.Position = SVector3(x, y, z);
			{
				float u = (float)j / sliceCount;
				float v = (y + radius) / (2 * radius);
				//fmod(2 * v, 1.0f);
				vertex.UV = SVector2(u, v);
			}			

			// TangentU us unit length.
			vertex.Tangent = SVector3(-z, 0.0f, x).GetSafeNormal();		
			XMVECTOR N = XMVectorSet(0, 1, 0, 1);
			XMVECTOR ROT = XMQuaternionRotationRollPitchYaw(0.0f, -XM_PI - theta, XM_PIDIV2 - phi);
			N = XMVector3Rotate(N, ROT);

			vertex.Normal = N;
			vertices.push_back(vertex);
		}


		uint32 ringVertexCount = sliceCount + 1;

		// Compute indices for each stack.
		for (uint32 i = 0; i < ringCount; ++i)
		{
			for (uint32 j = 0; j < sliceCount; ++j)
			{
				Indices.push_back(i * ringVertexCount + j);
				Indices.push_back((i + 1) * ringVertexCount + j);
				Indices.push_back((i + 1) * ringVertexCount + j + 1);
				Indices.push_back(i * ringVertexCount + j);
				Indices.push_back((i + 1) * ringVertexCount + j + 1);
				Indices.push_back(i * ringVertexCount + j + 1);
			}
		}
	}
	//------------------------------------------------
	newSphere.Initialize<SVertex>(vertices, Indices);

	return newSphere;
}



class Mesh GeometryGenerator::Grid(float width, float depth, uint32 horizontalTessellation, uint32 verticalTessellation)
{
	//MeshLODData< DefaultVertexBufferData> meshData(numLODLevels, "BuiltinGrid");

	const uint32 MIN_HSLICE_COUNT = 8;
	const uint32 MIN_VSLICE_COUNT = 8;

	if(horizontalTessellation < MIN_HSLICE_COUNT)
	{
		horizontalTessellation = MIN_HSLICE_COUNT;
	}

	if (verticalTessellation < MIN_VSLICE_COUNT)
	{
		verticalTessellation = MIN_VSLICE_COUNT;
	}

	//		Grid of m x n vertices
	//		-----------------------------------------------------------
	//		+	: Vertex
	//		d	: depth
	//		w	: width
	//		dx	: horizontal cell spacing = width / (m-1)
	//		dz	: z-axis	 cell spacing = depth / (n-1)
	// 
	//		  V(0,0)		  V(m-1,0)	^ Z
	//		^	+-------+-------+ ^		|
	//		|	|		|		| |		|
	//		|	|		|		| dz	|
	//		|	|		|		| |		|
	//		d	+-------+-------+ v		+--------> X
	//		|	|		|		|		
	//		|	|		|		|
	//		|	|		|		|
	//		v	+-------+-------+		
	//			<--dx--->		  V(m-1, n-1)
	//			<------ w ------>

	const uint32 m = horizontalTessellation;
	const uint32 n = verticalTessellation;

	unsigned numQuads = (m - 1) * (n - 1);
	unsigned faceCount = numQuads * 2; // 2 faces per quad = triangle count
	unsigned vertCount = m * n;
	float dx = width / (n - 1);
	float dz = depth / (m - 1);	// m & n mixed up??

	// offsets for centering the grid : V(0,0) = (-halfWidth, halfDepth)
	float halfDepth = depth / 2;
	float halfWidth = width / 2;

	// texture coord increments
	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	std::vector<SVertex> Vertices;
	std::vector<uint32> Indices;

	Vertices.resize(vertCount);
	Indices.resize(faceCount * 3);

	// position the Vertices
	for (uint32 i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (uint32 j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;
			float u = j * du;
			float v = i * dv;
			Vertices[i * n + j].Position = SVector3(x, 0.0f, z);
			Vertices[i * n + j].Normal = SVector3(0.0f, 0.0f, 0.0f);
			Vertices[i * n + j].UV = SVector2(u, v);
			Vertices[i * n + j].Tangent = SVector3(1.0f, 0.0f, 0.0f);
		}
	}

	//	generate Indices
	//
	//	  A	+------+ B
	//		|	 / |
	//		|	/  |
	//		|  /   |
	//		| /	   |
	//		|/	   |
	//	  C	+------+ D
	//
	//	A	: V(i  , j  )
	//	B	: V(i  , j+1)
	//	C	: V(i+1, j  )
	//	D	: V(i+1, j+1)
	//
	//	ABC	: (i*n +j    , i*n + j+1, (i+1)*n + j  )
	//	CBD : ((i+1)*n +j, i*n + j+1, (i+1)*n + j+1)

	uint32 k = 0;
	for (uint32 i = 0; i < m - 1; ++i)
	{
		for (uint32 j = 0; j < n - 1; ++j)
		{
			Indices[k] = i * n + j;
			Indices[k + 1] = i * n + j + 1;
			Indices[k + 2] = (i + 1) * n + j;
			Indices[k + 3] = (i + 1) * n + j;
			Indices[k + 4] = i * n + j + 1;
			Indices[k + 5] = (i + 1) * n + j + 1;
			k += 6;
		}
	}

	// apply height function
	for (size_t i = 0; i < Vertices.size(); ++i)
	{
		SVector3& pos = Vertices[i].Position;
		pos.y = 0.2f * (pos.z * sinf(20.0f * pos.x) + pos.x * cosf(10.0f * pos.z));
	}

	CalcTangentBiTangent(Vertices, Indices);

	Mesh grid("BuiltinGrid");
	grid.Initialize(Vertices, Indices);

	return grid;
}

class Mesh GeometryGenerator::Cylinder(float height, float topRadius, float bottomRadius, uint32 sliceCount, uint32 stackCount)
{

	const uint32 MIN_STACK_COUNT = 4;
	const uint32 MIN_SLICE_COUNT = 8;

	if (sliceCount < MIN_SLICE_COUNT)
		sliceCount = MIN_SLICE_COUNT;

	if (stackCount < MIN_STACK_COUNT)
		stackCount = MIN_STACK_COUNT;


	// slice count	: horizontal resolution
	// stack count	: height resolution
	float stackHeight = height / stackCount;
	float radiusStep = (topRadius - bottomRadius) / stackCount;
	uint32 ringCount = stackCount + 1;

	// CYLINDER BODY
	//-----------------------------------------------------------
	std::vector<SVertex> Vertices;
	std::vector<uint32> Indices;

	// Compute vertices for each stack ring starting at the bottom and moving up.
	for (uint32 i = 0; i < ringCount; ++i)
	{
		float y = -0.5f * height + i * stackHeight;
		float r = bottomRadius + i * radiusStep;

		// vertices of ring
		float dTheta = 2.0f * XM_PI / sliceCount;
		for (uint32 j = 0; j <= sliceCount; ++j)
		{
			SVertex vertex;
			float c = cosf(j * dTheta);
			float s = sinf(j * dTheta);
			vertex.Position = SVector3(r * c, y, r * s);
			{
				float u = (float)j / sliceCount;
				float v = 1.0f - (float)i / stackCount;
				vertex.UV = SVector2(u, v);
			}
			// Cylinder can be parameterized as follows, where we
			// introduce v parameter that goes in the same direction
			// as the v tex-coord so that the bitangent goes in the
			// same direction as the v tex-coord.
			// Let r0 be the bottom radius and let r1 be the
			// top radius.
			// y(v) = h - hv for v in [0,1].
			// r(v) = r1 + (r0-r1)v
			//
			// x(t, v) = r(v)*cos(t)
			// y(t, v) = h - hv
			// z(t, v) = r(v)*sin(t)
			//
			// dx/dt = -r(v)*sin(t)
			// dy/dt = 0
			// dz/dt = +r(v)*cos(t)
			//
			// dx/dv = (r0-r1)*cos(t)
			// dy/dv = -h
			// dz/dv = (r0-r1)*sin(t)

			// TangentU us unit length.
			vertex.Tangent = SVector3(-s, 0.0f, c);
			float dr = bottomRadius - topRadius;
			SVector3 bitangent(dr * c, -height, dr * s);
			SVector3 T = vertex.Tangent;
			SVector3 B = bitangent;
			vertex.Normal = SVector3::CrossProduct(T, B).GetSafeNormal();
			Vertices.push_back(vertex);
		}
	}

	// Add one because we duplicate the first and last vertex per ring since the texture coordinates are different.
	uint32 ringVertexCount = sliceCount + 1;

	// Compute indices for each stack.
	for (uint32 i = 0; i < stackCount; ++i)
	{
		for (uint32 j = 0; j < sliceCount; ++j)
		{
			Indices.push_back(i * ringVertexCount + j);
			Indices.push_back((i + 1) * ringVertexCount + j);
			Indices.push_back((i + 1) * ringVertexCount + j + 1);
			Indices.push_back(i * ringVertexCount + j);
			Indices.push_back((i + 1) * ringVertexCount + j + 1);
			Indices.push_back(i * ringVertexCount + j + 1);
		}
	}

	// CYLINDER TOP
	//-----------------------------------------------------------
	{
		uint32 baseIndex = (uint32)Vertices.size();
		float y = 0.5f * height;
		float dTheta = 2.0f * XM_PI / sliceCount;

		// Duplicate cap ring vertices because the texture coordinates and normals differ.
		for (uint32 i = 0; i <= sliceCount; ++i)
		{
			float x = topRadius * cosf(i * dTheta);
			float z = topRadius * sinf(i * dTheta);

			// Scale down by the height to try and make top cap texture coord area proportional to base.
			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			SVertex Vert;
			Vert.Position = SVector3(x, y, z);
			Vert.Normal = SVector3(0.0f, 1.0f, 0.0f);
			Vert.Tangent = SVector3(1.0f, 0.0f, 0.0f);	// ?
			Vert.UV = SVector2(u, v);
			Vertices.push_back(Vert);
		}

		// Cap center vertex.
		SVertex capCenter;
		capCenter.Position = SVector3(0.0f, y, 0.0f);
		capCenter.Normal = SVector3(0.0f, 1.0f, 0.0f);
		capCenter.Tangent = SVector3(1.0f, 0.0f, 0.0f);
		capCenter.UV = SVector2(0.5f, 0.5f);
		Vertices.push_back(capCenter);

		// Index of center vertex.
		uint32 centerIndex = (uint32)Vertices.size() - 1;
		for (uint32 i = 0; i < sliceCount; ++i)
		{
			Indices.push_back(centerIndex);
			Indices.push_back(baseIndex + i + 1);
			Indices.push_back(baseIndex + i);
		}
	}


	// CYLINDER BOTTOM
	//-----------------------------------------------------------
	{
		uint32 baseIndex = (uint32)Vertices.size();
		float y = -0.5f * height;
		float dTheta = 2.0f * XM_PI / sliceCount;

		// Duplicate cap ring vertices because the texture coordinates and normals differ.
		for (uint32 i = 0; i <= sliceCount; ++i)
		{
			float x = bottomRadius * cosf(i * dTheta);
			float z = bottomRadius * sinf(i * dTheta);

			// Scale down by the height to try and make top cap texture coord area proportional to base.
			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			SVertex Vert;
			Vert.Position = SVector3(x, y, z);
			Vert.Normal = SVector3(0.0f, -1.0f, 0.0f);
			Vert.Tangent = SVector3(-1.0f, 0.0f, 0.0f);	// ?
			Vert.UV = SVector2(u, v);
			Vertices.push_back(Vert);
		}
		// Cap center vertex.
		SVertex capCenter;
		capCenter.Position = SVector3(0.0f, y, 0.0f);
		capCenter.Normal = SVector3(0.0f, -1.0f, 0.0f);
		capCenter.Tangent = SVector3(-1.0f, 0.0f, 0.0f);
		capCenter.UV = SVector2(0.5f, 0.5f);
		Vertices.push_back(capCenter);

		// Index of center vertex.
		uint32 centerIndex = (uint32)Vertices.size() - 1;
		for (uint32 i = 0; i < sliceCount; ++i)
		{
			Indices.push_back(centerIndex);
			Indices.push_back(baseIndex + i);
			Indices.push_back(baseIndex + i + 1);
		}
	}

	
	//------------------------------------------------

	Mesh cyilender;
	cyilender.Initialize(Vertices, Indices);
	return cyilender;
}

void GeometryGenerator::CalcTangentBiTangent(std::vector<SVertex>& OutVertices, const vector<uint32> indices)
{
	const size_t countVerts = OutVertices.size();
	const size_t countIndices = indices.size();
	assert(countIndices % 3 == 0);

	//const SVector3 N = SVector3::ForwardVector;	//  (0, 0, 1)

	for (size_t i = 0; i < countIndices; i += 3)
	{
		SVertex& v0 = OutVertices[indices[i]];
		SVertex& v1 = OutVertices[indices[i + 1]];
		SVertex& v2 = OutVertices[indices[i + 2]];

		const SVector3 E1 = v1.Position - v0.Position;
		const SVector3 E2 = v2.Position - v0.Position;

		const SVector2 dUV1 = SVector2(v1.UV - v0.UV);
		const SVector2 dUV2 = SVector2(v2.UV - v0.UV);

		const float f = 1.0f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);
		assert(!std::isinf(f));

		SVector3 T(f * (dUV2.y * E1.x - dUV1.y * E2.x),
			   f * (dUV2.y * E1.y - dUV1.y* E2.y),
			   f * (dUV2.y * E1.z - dUV1.y * E2.z));
		T.Normalize();

		SVector3 B(f * (-dUV2.x * E1.x + dUV1.x * E2.x),
			   f * (-dUV2.x * E1.y + dUV1.x * E2.y),
			   f * (-dUV2.x * E1.z + dUV1.x * E2.z));
		B.Normalize();

		v0.Tangent = T;
		v1.Tangent = T;
		v2.Tangent = T;

		// calculated in shader
		//v0.bitangent = B;
		//v1.bitangent = B;
		//v2.bitangent = B;

		//Make Normal if Zero

		if (v0.Normal == SVector3::ZeroVector)
		{			
			v0.Normal = SVector3::CrossProduct(T, B).GetSafeNormal();
		}
		if (v1.Normal == SVector3::ZeroVector)
		{
			v1.Normal = SVector3::CrossProduct(T, B).GetSafeNormal();
		}
		if (v2.Normal == SVector3::ZeroVector)
		{
			v2.Normal = SVector3::CrossProduct(T, B).GetSafeNormal();
		}
	}

}

void GeometryGenerator::CalcTangent(std::vector<SVertex>& OutVertices, const vector<uint32> indices)
{
	const size_t countVerts = OutVertices.size();
	const size_t countIndices = indices.size();
	assert(countIndices % 3 == 0);
	

	for (size_t i = 0; i < countIndices; i += 3)
	{
		uint32	idx0 = indices[i];
		uint32	idx1 = indices[i + 1];
		uint32	idx2 = indices[i + 2];

		SVertex& v0 = OutVertices[idx0];
		SVertex& v1 = OutVertices[idx1];
		SVertex& v2 = OutVertices[idx2];

	

		float	fVtx1[3], fVtx2[3];
		fVtx1[0] = OutVertices[idx1].Position.x - OutVertices[idx0].Position.x;
		fVtx1[1] = OutVertices[idx1].Position.y - OutVertices[idx0].Position.y;
		fVtx1[2] = OutVertices[idx1].Position.z - OutVertices[idx0].Position.z;

		fVtx2[0] = OutVertices[idx2].Position.x - OutVertices[idx0].Position.x;
		fVtx2[1] = OutVertices[idx2].Position.y - OutVertices[idx0].Position.y;
		fVtx2[2] = OutVertices[idx2].Position.z - OutVertices[idx0].Position.z;

		float	ftu[2], ftv[2];
		ftu[0] = OutVertices[idx1].UV.x - OutVertices[idx0].UV.x;
		ftv[0] = OutVertices[idx1].UV.y - OutVertices[idx0].UV.y;

		ftu[1] = OutVertices[idx2].UV.x - OutVertices[idx0].UV.x;
		ftv[1] = OutVertices[idx2].UV.y - OutVertices[idx0].UV.y;

		float	fDen = 1.f / (ftu[0] * ftv[1] - ftu[1] * ftv[0]);

		SVector3	Tangent;
		Tangent.x = (ftv[1] * fVtx1[0] - ftv[0] * fVtx2[0]) * fDen;
		Tangent.y = (ftv[1] * fVtx1[1] - ftv[0] * fVtx2[1]) * fDen;
		Tangent.z = (ftv[1] * fVtx1[2] - ftv[0] * fVtx2[2]) * fDen;

		Tangent.Normalize();

		OutVertices[idx0].Tangent = Tangent;
		OutVertices[idx1].Tangent = Tangent;
		OutVertices[idx2].Tangent = Tangent;
	}
}
