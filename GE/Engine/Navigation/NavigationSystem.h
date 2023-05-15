#pragma once
#include "../XObject/XObject.h"
#include <Recast.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>
#include <DetourTileCache.h>
#include <DetourTileCacheBuilder.h>
#include <DetourNavMeshBuilder.h>
#include "LinearMath/btTransform.h"
#include <Renderer/RenderCommands.h>


enum SamplePartitionType
{
	SAMPLE_PARTITION_WATERSHED,
	SAMPLE_PARTITION_MONOTONE,
	SAMPLE_PARTITION_LAYERS
};

/// These are just sample areas to use consistent values across the samples.
/// The use should specify these base on his needs.
enum SamplePolyAreas
{
	SAMPLE_POLYAREA_GROUND,
	SAMPLE_POLYAREA_WATER,
	SAMPLE_POLYAREA_ROAD,
	SAMPLE_POLYAREA_DOOR,
	SAMPLE_POLYAREA_GRASS,
	SAMPLE_POLYAREA_JUMP
};
enum SamplePolyFlags
{
	SAMPLE_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	SAMPLE_POLYFLAGS_SWIM = 0x02,		// Ability to swim (water).
	SAMPLE_POLYFLAGS_DOOR = 0x04,		// Ability to move through doors.
	SAMPLE_POLYFLAGS_JUMP = 0x08,		// Ability to jump.
	SAMPLE_POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
	SAMPLE_POLYFLAGS_ALL = 0xffff	// All abilities.
};





class NavigationSystem :public XObject
{
	CLASS(NavigationSystem, XObject)
		friend class Engine;
	friend class NavMeshBoundsVolume;
	friend class NavMovementComponent;
	friend class Globals;


private:
	NavigationSystem();
	virtual ~NavigationSystem();

	void Initialize(class PhysicsSystem* physics);

	virtual void Update(float DeltaSeconds) override;

	virtual void PostUpdate(float DeltaSeconds) override;

	int rasterizeTileLayers(const int tx, const int ty, const rcConfig& cfg, struct TileCacheData* tiles, const int maxTiles);

public:
	bool BuildTest();

	void drawTiles(struct duDebugDraw* dd, dtTileCache* tc);



public:
	void BeginPlay();

	void EndPlay();


	void InitCrowdAgent();

	int32 AddAgent(const SVector3& InPos);

	void SetAgentLocation(const int32 Id, const SVector3& InAgentLocation);

	void SetAgentVelocity(const int32 Id,  float maxSpeed);	

	void SetAgentRadius(const int32 Id, float Radius);

	void SetAgentHeight(const int32 Id, float Height);

	void GetAgentLocation(const int32 Id,SVector3& OutLoc);

	void ResetMoveTarget(const int32 Id);

	void SetAgentActivate(const int32 Id,bool bActivate);

	void GetAgentVelocity(const int32 Id, SVector3& OutVel);

	void SetMoveTarget(const int32 Id, const SVector3& InTargetPos);

	void RemoveAgent(int32& Id);


	




	
private:
	PROP(int32, MaxAgent, DetailsEdit, "Agent")
	int32 MaxAgent;

	PROP(bool, DrawNav, DetailsEdit)
	bool DrawNav;
	
	//PROP(float, AgentRadius, DetailsEdit, "Agent")
	//float AgentRadius;

	//PROP(float, AgentHeight, DetailsEdit, "Agent")
	//float AgentHeight;	
	//
	//PROP(float, AgentMaxClimb, DetailsEdit, "Agent")
	//float AgentMaxClimb;


private:
	class PhysicsDebugDraw* m_debugDraw;
	class PhysicsSystem* m_physics;

	class InputGeom* m_InputGeomety;

private:
	class dtNavMesh* m_navMesh;
	class dtNavMeshQuery* m_navQuery;
	class dtCrowd* m_crowd;

	struct LinearAllocator* m_talloc;
	struct FastLZCompressor* m_tcomp;	
	struct MeshProcess* m_tmproc;

	class dtTileCache* m_tileCache;

	unsigned char m_navMeshDrawFlags;

	PROP(float, CellSize, DetailsEdit, "NavSettings")
	float CellSize = 19.0f;		

	PROP(float, CellHeight, DetailsEdit, "NavSettings")
	float CellHeight = 10.0f;

	PROP(float, AgentHeight, DetailsEdit, "Agent")
		float AgentHeight = 10.0f;

	PROP(float, AgentRadius, DetailsEdit, "Agent")
		float AgentRadius = 50;

	PROP(float, AgentMaxClimb, DetailsEdit, "Agent")
		float AgentMaxClimb = 80.f;

	PROP(float, AgentMaxSlope, DetailsEdit, "Agent")
		float AgentMaxSlope = 200.0f;

	//float CellSize = 0.3f;
	//float CellHeight = 0.2f;
	//float AgentHeight = 2.0f;
	//float AgentRadius = 0.6f;

	//float AgentMaxClimb = 0.9f;
	//float AgentMaxSlope = 60.0f;

	PROP(float, m_regionMinSize, DetailsEdit, "NavSettings")
	float m_regionMinSize = 8;
	PROP(float, m_regionMergeSize, DetailsEdit, "NavSettings")
	float m_regionMergeSize = 20;

	PROP(float, m_edgeMaxLen, DetailsEdit, "NavSettings")
	float m_edgeMaxLen = 12.0f;

	PROP(float, m_edgeMaxError, DetailsEdit, "NavSettings")
	float m_edgeMaxError = 1.3f;

	PROP(float, m_vertsPerPoly, DetailsEdit, "NavSettings")
	float m_vertsPerPoly = 6.0f;

	PROP(float, m_detailSampleDist, DetailsEdit, "NavSettings")
	float m_detailSampleDist = 6.0f;

	PROP(float, m_detailSampleMaxError, DetailsEdit, "NavSettings")
	float m_detailSampleMaxError = 1.0f;

	int m_partitionType = SAMPLE_PARTITION_WATERSHED;

	bool m_filterLowHangingObstacles = true;
	bool m_filterLedgeSpans = true;
	bool m_filterWalkableLowHeightSpans  = true;



	float m_cacheBuildTimeMs;
	int m_cacheCompressedSize;
	int m_cacheRawSize;
	int m_cacheLayerCount;
	unsigned int m_cacheBuildMemUsage;

	int m_maxTiles;
	int m_maxPolysPerTile;
	float m_tileSize =48;

private:


	SVector3															m_debug_tile_origin;	


	class rcContext* m_ctx;

	bool		m_Begin;

	bool		m_NavInit;
};


