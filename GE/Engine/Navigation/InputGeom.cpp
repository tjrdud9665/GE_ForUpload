//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include "EnginePCH.h"
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <algorithm>
#include "Recast.h"
#include "InputGeom.h"
#include "ChunkyTriMesh.h"
#include "DebugDraw.h"
#include "RecastDebugDraw.h"
#include "DetourNavMesh.h"
#include <Physics/PhysicsSystem.h>
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include <GameFramework/Components/PrimitiveComponent.h>
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletCollision/CollisionShapes/btConvexHullShape.h"
#include <Renderer/D3DBuffer.h>
#include <Renderer/Renderer.h>
#include <GameFramework/Components/StaticMeshComponent.h>



static bool intersectSegmentTriangle(const float* sp, const float* sq,
									 const float* a, const float* b, const float* c,
									 float &t)
{
	float v, w;
	float ab[3], ac[3], qp[3], ap[3], norm[3], e[3];
	rcVsub(ab, b, a);
	rcVsub(ac, c, a);
	rcVsub(qp, sp, sq);
	
	// Compute triangle normal. Can be precalculated or cached if
	// intersecting multiple segments against the same triangle
	rcVcross(norm, ab, ac);
	
	// Compute denominator d. If d <= 0, segment is parallel to or points
	// away from triangle, so exit early
	float d = rcVdot(qp, norm);
	if (d <= 0.0f) return false;
	
	// Compute intersection t value of pq with plane of triangle. A ray
	// intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
	// dividing by d until intersection has been found to pierce triangle
	rcVsub(ap, sp, a);
	t = rcVdot(ap, norm);
	if (t < 0.0f) return false;
	if (t > d) return false; // For segment; exclude this code line for a ray test
	
	// Compute barycentric coordinate components and test if within bounds
	rcVcross(e, qp, ap);
	v = rcVdot(ac, e);
	if (v < 0.0f || v > d) return false;
	w = -rcVdot(ab, e);
	if (w < 0.0f || v + w > d) return false;
	
	// Segment/ray intersects triangle. Perform delayed division
	t /= d;
	
	return true;
}

static char* parseRow(char* buf, char* bufEnd, char* row, int len)
{
	bool start = true;
	bool done = false;
	int n = 0;
	while (!done && buf < bufEnd)
	{
		char c = *buf;
		buf++;
		// multirow
		switch (c)
		{
			case '\n':
				if (start) break;
				done = true;
				break;
			case '\r':
				break;
			case '\t':
			case ' ':
				if (start) break;
				// else falls through
			default:
				start = false;
				row[n++] = c;
				if (n >= len-1)
					done = true;
				break;
		}
	}
	row[n] = '\0';
	return buf;
}



InputGeom::InputGeom() :
	m_chunkyMesh(0),	
	m_hasBuildSettings(false),
	m_offMeshConCount(0),
	m_volumeCount(0)
{
	verts = 0;
	tris = 0;
	nverts =0;
	ntris =0;
}

InputGeom::~InputGeom()
{
	delete m_chunkyMesh;

	SAFE_DELETE_ARRAY(verts);
	SAFE_DELETE_ARRAY(tris);
	SAFE_DELETE_ARRAY(nrms);
	
}
		
bool InputGeom::loadMesh(rcContext* ctx)
{
	SAFE_DELETE_ARRAY(verts);
	SAFE_DELETE_ARRAY(tris);
	SAFE_DELETE_ARRAY(nrms);


	m_offMeshConCount = 0;
	m_volumeCount = 0;

	PhysicsSystem* physicsSystem = g_Engine->Get<PhysicsSystem>();

	btCollisionObjectArray& arr = physicsSystem->GetWorld()->getCollisionObjectArray();
	int32 cnt = physicsSystem->GetWorld()->getNumCollisionObjects();

	nverts = 0;
	ntris = 0;
	nrms = 0;
	int32 mergeMeshCount = 0;
	std::vector<PrimitiveComponent*> meshComps;

	for (int32 i = 0; i < cnt; i++)
	{
		PrimitiveComponent* comp = (PrimitiveComponent*)arr[i]->getUserPointer();
		if (comp->GetClass()->IsA(StaticMeshComponent::StaticClass()))
		{
			if (comp->GetMeshAsset())
			{
				mergeMeshCount++;
				meshComps.push_back(comp);
			}
		}
	}

	size_t* meshVertexCount = new size_t[mergeMeshCount];
	size_t* meshIndexCount = new size_t[mergeMeshCount];

	SVector3** meshVertices = new SVector3 * [mergeMeshCount];
	SVector3** meshNormals = new SVector3 * [mergeMeshCount];

	uint32** meshIndices = new uint32 * [mergeMeshCount];

	//Mesh 데이터에서 정점 및 인덱스를 꺼내서 취합합니다.
	for (int32 i = 0; i < meshComps.size(); i++)
	{
		PrimitiveComponent* comp = meshComps[i];
		if (comp->GetClass()->IsA(StaticMeshComponent::StaticClass()))
		{
			getMeshInformation(comp->GetMeshAsset(), meshVertexCount[i], meshVertices[i], meshNormals[i], meshIndexCount[i], meshIndices[i]);

			nverts += (int32)meshVertexCount[i];
			ntris += (int32)meshIndexCount[i];

		}
	}

	if(nverts == 0)
	{

		for (int32 i = 0; i < mergeMeshCount; i++)
		{
			delete[] meshVertices[i];
			delete[] meshNormals[i];
			delete[] meshIndices[i];
		}

		delete[] meshVertices;
		delete[] meshNormals;
		delete[] meshIndices;


		delete[] meshVertexCount;
		delete[] meshIndexCount;

		return false;
	}
	
	verts = new float[nverts * 3];// *3 as verts holds x,y,&z for each verts in the array
	nrms = new float[nverts * 3];
	tris = new int[ntris];// 

	//convert index count into tri count
	ntris = ntris / 3; //although the tris array are indices the ntris is actual number of triangles, eg. indices/3;

	int vertsIndex = 0;
	int prevVerticiesCount = 0;
	int prevIndexCountTotal = 0;
	for (int32 i = 0; i < meshComps.size(); i++)
	{
		PrimitiveComponent* comp = meshComps[i];
		if (comp->GetClass()->IsA(StaticMeshComponent::StaticClass()))
		{
			SVector3 vertexPos;
			SVector3 vertexNrm;
			for (size_t j = 0; j < meshVertexCount[i]; j++)
			{
				Matrix matrix = comp->GetComponentTransform().ToMatrixWithScale();

				vertexPos = meshVertices[i][j].TransformCoord(matrix);
				vertexNrm = meshNormals[i][j].TransformNormal(matrix);

				verts[vertsIndex] = vertexPos.x;
				verts[vertsIndex + 1] = vertexPos.y;
				verts[vertsIndex + 2] = vertexPos.z;

				nrms[vertsIndex] = vertexNrm.x;
				nrms[vertsIndex + 1] = vertexNrm.y;
				nrms[vertsIndex + 2] = vertexNrm.z;


				vertsIndex += 3;



			}

			for (size_t j = 0; j < meshIndexCount[i]; j++)
			{
				tris[prevIndexCountTotal + j] = meshIndices[i][j] + prevVerticiesCount;
			}
			prevIndexCountTotal += (int32)meshIndexCount[i];
			prevVerticiesCount += (int32)meshVertexCount[i];

		}
	}



	for (int32 i = 0; i < mergeMeshCount; i++)
	{
		delete[] meshVertices[i];
		delete[] meshNormals[i];
		delete[] meshIndices[i];
	}

	delete[] meshVertices;
	delete[] meshNormals;
	delete[] meshIndices;


	delete[] meshVertexCount;
	delete[] meshIndexCount;

	//////////////////////////////////////////////////////////////////////////



	if (m_chunkyMesh)
	{
		SAFE_DELETE(m_chunkyMesh);
	}

	rcCalcBounds(verts, nverts, m_meshBMin, m_meshBMax);
	m_chunkyMesh = new rcChunkyTriMesh;

	if (!rcCreateChunkyTriMesh(verts, tris, ntris, 256, m_chunkyMesh))
	{
		//ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Failed to build chunky mesh.");
		return false;
	}

	return true;
}

bool InputGeom::loadGeomSet(rcContext* ctx, const std::string& filepath)
{
	
	return true;
}



bool InputGeom::saveGeomSet(const BuildSettings* settings)
{	
	
	return true;
}

static bool isectSegAABB(const float* sp, const float* sq,
						 const float* amin, const float* amax,
						 float& tmin, float& tmax)
{
	static const float EPS = 1e-6f;
	
	float d[3];
	d[0] = sq[0] - sp[0];
	d[1] = sq[1] - sp[1];
	d[2] = sq[2] - sp[2];
	tmin = 0.0;
	tmax = 1.0f;
	
	for (int i = 0; i < 3; i++)
	{
		if (fabsf(d[i]) < EPS)
		{
			if (sp[i] < amin[i] || sp[i] > amax[i])
				return false;
		}
		else
		{
			const float ood = 1.0f / d[i];
			float t1 = (amin[i] - sp[i]) * ood;
			float t2 = (amax[i] - sp[i]) * ood;
			if (t1 > t2) { float tmp = t1; t1 = t2; t2 = tmp; }
			if (t1 > tmin) tmin = t1;
			if (t2 < tmax) tmax = t2;
			if (tmin > tmax) return false;
		}
	}
	
	return true;
}


bool InputGeom::raycastMesh(float* src, float* dst, float& tmin)
{
	// Prune hit ray.
	float btmin, btmax;
	if (!isectSegAABB(src, dst, m_meshBMin, m_meshBMax, btmin, btmax))
		return false;
	float p[2], q[2];
	p[0] = src[0] + (dst[0]-src[0])*btmin;
	p[1] = src[2] + (dst[2]-src[2])*btmin;
	q[0] = src[0] + (dst[0]-src[0])*btmax;
	q[1] = src[2] + (dst[2]-src[2])*btmax;
	
	int cid[512];
	const int ncid = rcGetChunksOverlappingSegment(m_chunkyMesh, p, q, cid, 512);
	if (!ncid)
		return false;
	
	tmin = 1.0f;
	bool hit = false;
	const float* verts = 0;
		//m_mesh->getVerts();
	
	for (int i = 0; i < ncid; ++i)
	{
		const rcChunkyTriMeshNode& node = m_chunkyMesh->nodes[cid[i]];
		const int* tris = &m_chunkyMesh->tris[node.i*3];
		const int ntris = node.n;

		for (int j = 0; j < ntris*3; j += 3)
		{
			float t = 1;
			if (intersectSegmentTriangle(src, dst,
										 &verts[tris[j]*3],
										 &verts[tris[j+1]*3],
										 &verts[tris[j+2]*3], t))
			{
				if (t < tmin)
					tmin = t;
				hit = true;
			}
		}
	}
	
	return hit;
}

void InputGeom::addOffMeshConnection(const float* spos, const float* epos, const float rad,
									 unsigned char bidir, unsigned char area, unsigned short flags)
{
	if (m_offMeshConCount >= MAX_OFFMESH_CONNECTIONS) return;
	float* v = &m_offMeshConVerts[m_offMeshConCount*3*2];
	m_offMeshConRads[m_offMeshConCount] = rad;
	m_offMeshConDirs[m_offMeshConCount] = bidir;
	m_offMeshConAreas[m_offMeshConCount] = area;
	m_offMeshConFlags[m_offMeshConCount] = flags;
	m_offMeshConId[m_offMeshConCount] = 1000 + m_offMeshConCount;
	rcVcopy(&v[0], spos);
	rcVcopy(&v[3], epos);
	m_offMeshConCount++;
}

void InputGeom::deleteOffMeshConnection(int i)
{
	m_offMeshConCount--;
	float* src = &m_offMeshConVerts[m_offMeshConCount*3*2];
	float* dst = &m_offMeshConVerts[i*3*2];
	rcVcopy(&dst[0], &src[0]);
	rcVcopy(&dst[3], &src[3]);
	m_offMeshConRads[i] = m_offMeshConRads[m_offMeshConCount];
	m_offMeshConDirs[i] = m_offMeshConDirs[m_offMeshConCount];
	m_offMeshConAreas[i] = m_offMeshConAreas[m_offMeshConCount];
	m_offMeshConFlags[i] = m_offMeshConFlags[m_offMeshConCount];
}

void InputGeom::drawOffMeshConnections(duDebugDraw* dd, bool hilight)
{
	unsigned int conColor = duRGBA(192,0,128,192);
	unsigned int baseColor = duRGBA(0,0,0,64);
	dd->depthMask(false);

	dd->begin(DU_DRAW_LINES, 2.0f);
	for (int i = 0; i < m_offMeshConCount; ++i)
	{
		float* v = &m_offMeshConVerts[i*3*2];

		dd->vertex(v[0],v[1],v[2], baseColor);
		dd->vertex(v[0],v[1]+0.2f,v[2], baseColor);
		
		dd->vertex(v[3],v[4],v[5], baseColor);
		dd->vertex(v[3],v[4]+0.2f,v[5], baseColor);
		
		duAppendCircle(dd, v[0],v[1]+0.1f,v[2], m_offMeshConRads[i], baseColor);
		duAppendCircle(dd, v[3],v[4]+0.1f,v[5], m_offMeshConRads[i], baseColor);

		if (hilight)
		{
			duAppendArc(dd, v[0],v[1],v[2], v[3],v[4],v[5], 0.25f,
						(m_offMeshConDirs[i]&1) ? 0.6f : 0.0f, 0.6f, conColor);
		}
	}	
	dd->end();

	dd->depthMask(true);
}

void InputGeom::addConvexVolume(const float* verts, const int nverts,
								const float minh, const float maxh, unsigned char area)
{
	if (m_volumeCount >= MAX_VOLUMES) return;
	ConvexVolume* vol = &m_volumes[m_volumeCount++];
	memset(vol, 0, sizeof(ConvexVolume));
	memcpy(vol->verts, verts, sizeof(float)*3*nverts);
	vol->hmin = minh;
	vol->hmax = maxh;
	vol->nverts = nverts;
	vol->area = area;
}

void InputGeom::deleteConvexVolume(int i)
{
	m_volumeCount--;
	m_volumes[i] = m_volumes[m_volumeCount];
}

void InputGeom::drawConvexVolumes(struct duDebugDraw* dd, bool /*hilight*/)
{
	dd->depthMask(false);

	dd->begin(DU_DRAW_TRIS);
	
	for (int i = 0; i < m_volumeCount; ++i)
	{
		const ConvexVolume* vol = &m_volumes[i];
		unsigned int col = duTransCol(dd->areaToCol(vol->area), 32);
		for (int j = 0, k = vol->nverts-1; j < vol->nverts; k = j++)
		{
			const float* va = &vol->verts[k*3];
			const float* vb = &vol->verts[j*3];

			dd->vertex(vol->verts[0],vol->hmax,vol->verts[2], col);
			dd->vertex(vb[0],vol->hmax,vb[2], col);
			dd->vertex(va[0],vol->hmax,va[2], col);
			
			dd->vertex(va[0],vol->hmin,va[2], duDarkenCol(col));
			dd->vertex(va[0],vol->hmax,va[2], col);
			dd->vertex(vb[0],vol->hmax,vb[2], col);

			dd->vertex(va[0],vol->hmin,va[2], duDarkenCol(col));
			dd->vertex(vb[0],vol->hmax,vb[2], col);
			dd->vertex(vb[0],vol->hmin,vb[2], duDarkenCol(col));
		}
	}
	
	dd->end();

	dd->begin(DU_DRAW_LINES, 2.0f);
	for (int i = 0; i < m_volumeCount; ++i)
	{
		const ConvexVolume* vol = &m_volumes[i];
		unsigned int col = duTransCol(dd->areaToCol(vol->area), 220);
		for (int j = 0, k = vol->nverts-1; j < vol->nverts; k = j++)
		{
			const float* va = &vol->verts[k*3];
			const float* vb = &vol->verts[j*3];
			dd->vertex(va[0],vol->hmin,va[2], duDarkenCol(col));
			dd->vertex(vb[0],vol->hmin,vb[2], duDarkenCol(col));
			dd->vertex(va[0],vol->hmax,va[2], col);
			dd->vertex(vb[0],vol->hmax,vb[2], col);
			dd->vertex(va[0],vol->hmin,va[2], duDarkenCol(col));
			dd->vertex(va[0],vol->hmax,va[2], col);
		}
	}
	dd->end();

	dd->begin(DU_DRAW_POINTS, 3.0f);
	for (int i = 0; i < m_volumeCount; ++i)
	{
		const ConvexVolume* vol = &m_volumes[i];
		unsigned int col = duDarkenCol(duTransCol(dd->areaToCol(vol->area), 220));
		for (int j = 0; j < vol->nverts; ++j)
		{
			dd->vertex(vol->verts[j*3+0],vol->verts[j*3+1]+0.1f,vol->verts[j*3+2], col);
			dd->vertex(vol->verts[j*3+0],vol->hmin,vol->verts[j*3+2], col);
			dd->vertex(vol->verts[j*3+0],vol->hmax,vol->verts[j*3+2], col);
		}
	}
	dd->end();
	
	
	dd->depthMask(true);
}

void InputGeom::getMeshInformation(class MeshAsset* meshAsset, size_t& vertex_count, SVector3*& vertices, SVector3*& normals,size_t& index_count, uint32*& indices)
{
	bool added_shared = false;
	size_t current_offset = 0;
	size_t shared_offset = 0;
	size_t next_offset = 0;
	size_t index_offset = 0;

	vertex_count = index_count = 0;

	size_t submeshCnt = meshAsset->GetMesh(0).GetIAPairSize();

	const SMeshCollisionData& collisionData = meshAsset->GetMeshCollisionData();
	
	vertex_count = collisionData.vertexData.size();

	index_count = collisionData.indexData.size();

	vertices = new SVector3[vertex_count];
	normals = new SVector3[vertex_count];
	indices = new uint32[index_count];

	for (uint32 v = 0; v < vertex_count; v++)
	{
		vertices[v] = collisionData.vertexData[v];		
	}
	for (uint32 i = 0; i < index_count; i++)
	{
		indices[i] = collisionData.indexData[i];
	}

	//Gen
	for (uint32 i = 0; i < index_count; i+=3)
	{
		// Get the face normal
		SVector3 vector1 = vertices[indices[(size_t)i + 1]] - vertices[indices[i]];
		SVector3 vector2 = vertices[indices[(size_t)i + 2]] - vertices[indices[i]];
		SVector3 faceNormal = vector1 ^ vector2;		
		faceNormal.Normalize();			
		// Add the face normal to the 3 vertices normal touching this face
		normals[indices[i]] += faceNormal;
		normals[indices[(size_t)i + 1]] += faceNormal;
		normals[indices[(size_t)i + 2]] += faceNormal;
	}

	//Generate Face Normal

//	for (i = 0; i < indices.size(); i += 3)
//	{
//		// Get the face normal
//		auto vector1 = verticesPos[indices[(size_t)i + 1]] - verticesPos[indices[i]];
//		auto vector2 = verticesPos[indices[(size_t)i + 2]] - verticesPos[indices[i]];
//		auto faceNormal = sf::VectorCross(vector1, vector2);
//		sf::Normalize(faceNormal);
//
//		// Add the face normal to the 3 vertices normal touching this face
//		verticesNormal[indices[i]] += faceNormal;
//		verticesNormal[indices[(size_t)i + 1]] += faceNormal;
//		verticesNormal[indices[(size_t)i + 2]] += faceNormal;
//	}


	

	//for(int32 i=0;i< submeshCnt;i++)
	//{
	//	const SD3DBuffer& vtxBuffer = g_Engine->Get<Renderer>()->GetVertexBuffer(meshAsset->GetMesh(0).GetIAPair(i).first);
	//	//std::vector<SVertex> vertices;
	//	//vertices.resize(vtxBuffer.Desc.ElementCount);
	//	//::memcpy(vertices.data(), vtxBuffer.CPUData, vtxBuffer.Desc.ElementCount * vtxBuffer.Desc.Stride);
	//	vertex_count += vtxBuffer.Desc.ElementCount;

	//	const SD3DBuffer& idxBuffer = g_Engine->Get<Renderer>()->GetIndexBuffer(meshAsset->GetMesh(0).GetIAPair(i).second);
	//	index_count += idxBuffer.Desc.ElementCount;
	//}

	//vertices = new SVector3[vertex_count];
	//normals = new SVector3[vertex_count];

	//indices = new uint32[index_count];

	//for (int32 i = 0; i < submeshCnt; i++)
	//{
	//	const SD3DBuffer& vtxBuffer = g_Engine->Get<Renderer>()->GetVertexBuffer(meshAsset->GetMesh(0).GetIAPair(i).first);
	//	std::vector<SVertex> ve;
	//	ve.resize(vtxBuffer.Desc.ElementCount);
	//	::memcpy(ve.data(), vtxBuffer.CPUData, vtxBuffer.Desc.ElementCount * vtxBuffer.Desc.Stride);
	//	for (uint32 v = 0; v < vtxBuffer.Desc.ElementCount; v++)
	//	{
	//		vertices[v] = ve[v].Position;
	//		normals[v] = ve[v].Normal;
	//	}

	//	const SD3DBuffer& idxBuffer = g_Engine->Get<Renderer>()->GetIndexBuffer(meshAsset->GetMesh(0).GetIAPair(i).second);
	//	std::vector<uint32>				ie;
	//	ie.resize(idxBuffer.Desc.ElementCount);
	//	::memcpy(ie.data(), idxBuffer.CPUData, idxBuffer.Desc.ElementCount * idxBuffer.Desc.Stride);

	//	for (uint32 v = 0; v < idxBuffer.Desc.ElementCount; v++)
	//	{
	//		indices[v] = ie[v];
	//	}
	//}



}
