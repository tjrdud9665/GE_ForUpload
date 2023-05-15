#include "EnginePCH.h"
#include "NavigationSystem.h"
#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>
#include <DetourCrowd.h>
#include <Recast.h>
#include <DetourNavMeshBuilder.h>
#include <RecastDump.h>

#include <Physics/PhysicsDebugDraw.h>
#include <DetourDebugDraw.h>
#include <DetourCommon.h>
#include <Physics/PhysicsSystem.h>

#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"


#include <SceneManager.h>
#include <Assets/Scene/Scene.h>
#include <DetourTileCache.h>
#include <DetourStatus.h>
#include <GameFramework/GameObject/NavMeshBoundsVolume.h>
#include "BulletCollision/CollisionShapes/btPolyhedralConvexShape.h"
#include <Physics/PhysicsMisc.h>
#include <Physics/BulletPhysicsHelper.h>
#include "BulletCollision/Gimpact/btBoxCollision.h"
#include <DebugDraw.h>
#include <GameFramework/Components/NavMovementComponent.h>
#include <GameFramework/Components/PrimitiveComponent.h>
#include <GameFramework/Components/StaticMeshComponent.h>
#include "InputGeom.h"
#include "fastlz.h"
#include <RecastDebugDraw.h>

static const int MAX_LAYERS = 32;

struct LinearAllocator : public dtTileCacheAlloc
{
	unsigned char* buffer;
	size_t capacity;
	size_t top;
	size_t high;

	LinearAllocator(const size_t cap) : buffer(0), capacity(0), top(0), high(0)
	{
		resize(cap);
	}

	virtual ~LinearAllocator();

	void resize(const size_t cap)
	{
		if (buffer) dtFree(buffer);
		buffer = (unsigned char*)dtAlloc(cap, DT_ALLOC_PERM);
		capacity = cap;
	}

	virtual void reset()
	{
		high = dtMax(high, top);
		top = 0;
	}

	virtual void* alloc(const size_t size)
	{
		if (!buffer)
			return 0;
		if (top + size > capacity)
			return 0;
		unsigned char* mem = &buffer[top];
		top += size;
		return mem;
	}

	virtual void free(void* /*ptr*/)
	{
		// Empty
	}
};

LinearAllocator::~LinearAllocator()
{
	// Defined out of line to fix the weak v-tables warning
	dtFree(buffer);
}

struct TileCacheData
{
	unsigned char* data;
	int dataSize;
};

struct RasterizationContext
{
	RasterizationContext() :
		solid(0),
		triareas(0),
		lset(0),
		chf(0),
		ntiles(0)
	{
		memset(tiles, 0, sizeof(TileCacheData) * MAX_LAYERS);
	}

	~RasterizationContext()
	{
		rcFreeHeightField(solid);
		delete[] triareas;
		rcFreeHeightfieldLayerSet(lset);
		rcFreeCompactHeightfield(chf);
		for (int i = 0; i < MAX_LAYERS; ++i)
		{
			dtFree(tiles[i].data);
			tiles[i].data = 0;
		}
	}

	rcHeightfield* solid;
	unsigned char* triareas;
	rcHeightfieldLayerSet* lset;
	rcCompactHeightfield* chf;
	TileCacheData tiles[MAX_LAYERS];
	int ntiles;
};

struct MeshProcess : public dtTileCacheMeshProcess
{
	class InputGeom* m_geom;

	inline MeshProcess() : m_geom(0)
	{}

	virtual ~MeshProcess();

	inline void init(class InputGeom* geom)
	{
		m_geom = geom;
	}

	virtual void process(struct dtNavMeshCreateParams* params,
						 unsigned char* polyAreas, unsigned short* polyFlags)
	{
		// Update poly flags from areas.
		for (int i = 0; i < params->polyCount; ++i)
		{
			if (polyAreas[i] == DT_TILECACHE_WALKABLE_AREA)
				polyAreas[i] = SAMPLE_POLYAREA_GROUND;

			if (polyAreas[i] == SAMPLE_POLYAREA_GROUND ||
				polyAreas[i] == SAMPLE_POLYAREA_GRASS ||
				polyAreas[i] == SAMPLE_POLYAREA_ROAD)
			{
				polyFlags[i] = SAMPLE_POLYFLAGS_WALK;
			} else if (polyAreas[i] == SAMPLE_POLYAREA_WATER)
			{
				polyFlags[i] = SAMPLE_POLYFLAGS_SWIM;
			} else if (polyAreas[i] == SAMPLE_POLYAREA_DOOR)
			{
				polyFlags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
			}
		}

		// Pass in off-mesh connections.
		if (m_geom)
		{
			params->offMeshConVerts = m_geom->getOffMeshConnectionVerts();
			params->offMeshConRad = m_geom->getOffMeshConnectionRads();
			params->offMeshConDir = m_geom->getOffMeshConnectionDirs();
			params->offMeshConAreas = m_geom->getOffMeshConnectionAreas();
			params->offMeshConFlags = m_geom->getOffMeshConnectionFlags();
			params->offMeshConUserID = m_geom->getOffMeshConnectionId();
			params->offMeshConCount = m_geom->getOffMeshConnectionCount();
		}
	}
};

MeshProcess::~MeshProcess()
{
	// Defined out of line to fix the weak v-tables warning
}

struct FastLZCompressor : public dtTileCacheCompressor
{
	virtual ~FastLZCompressor();

	virtual int maxCompressedSize(const int bufferSize)
	{
		return (int)(bufferSize * 1.05f);
	}

	virtual dtStatus compress(const unsigned char* buffer, const int bufferSize,
							  unsigned char* compressed, const int /*maxCompressedSize*/, int* compressedSize)
	{
		*compressedSize = fastlz_compress((const void* const)buffer, bufferSize, compressed);
		return DT_SUCCESS;
	}

	virtual dtStatus decompress(const unsigned char* compressed, const int compressedSize,
								unsigned char* buffer, const int maxBufferSize, int* bufferSize)
	{
		*bufferSize = fastlz_decompress(compressed, compressedSize, buffer, maxBufferSize);
		return *bufferSize < 0 ? DT_FAILURE : DT_SUCCESS;
	}
};

FastLZCompressor::~FastLZCompressor()
{
	// Defined out of line to fix the weak v-tables warning
}

static int calcLayerBufferSize(const int gridWidth, const int gridHeight)
{
	const int headerSize = dtAlign4(sizeof(dtTileCacheLayerHeader));
	const int gridSize = gridWidth * gridHeight;
	return headerSize + gridSize * 4;
}

static void calcVel(float* vel, const float* pos, const float* tgt, const float speed)
{
	dtVsub(vel, tgt, pos);
	vel[1] = 0.0;
	dtVnormalize(vel);
	dtVscale(vel, vel, speed);
}



static const int EXPECTED_LAYERS_PER_TILE = 4;

uint32 nextPow2(uint32 v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}


NavigationSystem::NavigationSystem()
	:MaxAgent(128)
	, DrawNav(false)
	, m_NavInit(false)
{
	m_ctx = nullptr;
}


NavigationSystem::~NavigationSystem()
{
	dtFreeTileCache(m_tileCache);
	m_tileCache = 0;


	SAFE_DELETE(m_ctx);
	SAFE_DELETE(m_talloc);
	SAFE_DELETE(m_tcomp);
	SAFE_DELETE(m_tmproc);

	


	dtFreeTileCache(m_tileCache);
	dtFreeNavMeshQuery(m_navQuery);
	dtFreeNavMesh(m_navMesh);
	dtFreeCrowd(m_crowd);


	SAFE_DELETE(m_InputGeomety);

	
	
}

void NavigationSystem::Initialize(class PhysicsSystem* physics)
{
	
	m_physics = physics;
	m_debugDraw = physics->GetDebugDraw();
	m_ctx = new rcContext(false); 

	m_talloc = new LinearAllocator(32000);
	m_tcomp = new FastLZCompressor;
	m_tmproc = new MeshProcess;

	m_navQuery = dtAllocNavMeshQuery();
	m_crowd = dtAllocCrowd();
	
	
}



void NavigationSystem::Update(float DeltaSeconds)
{	
	const float texScale = 1.0f / (CellSize * 10.0f);
	
	if(m_InputGeomety)
	{
		if(g_Engine->IsEditMode() && m_InputGeomety->getVerts())
		{
			if (DrawNav)
				duDebugDrawNavMeshWithClosedList(m_debugDraw, *m_navMesh, *m_navQuery, DU_DRAWNAVMESH_CLOSEDLIST/*|DU_DRAWNAVMESH_COLOR_TILES*/);
		}
		
		if (m_Begin)
		{
			if (m_tileCache)
				m_tileCache->update(DeltaSeconds, m_navMesh);


			if (m_crowd->getGrid())
			{
				m_crowd->update(DeltaSeconds, nullptr);

			}
		}
	
	}	


}

void NavigationSystem::PostUpdate(float DeltaSeconds)
{
	if (m_Begin && !m_NavInit)
	{
		if (BuildTest())
		{
			m_NavInit = true;

			InitCrowdAgent();
		}
	}
}

int NavigationSystem::rasterizeTileLayers(const int tx, const int ty, const rcConfig& cfg, struct TileCacheData* tiles, const int maxTiles)
{
	if (!m_InputGeomety  || !m_InputGeomety->getChunkyMesh())
	{
		//m_ctx->log(RC_LOG_ERROR, "buildTile: Input mesh is not specified.");
		return 0;
	}

	FastLZCompressor comp;
	RasterizationContext rc;

	const float* verts = m_InputGeomety->getVerts();
	const int nverts = m_InputGeomety->getVertCount();
	const rcChunkyTriMesh* chunkyMesh = m_InputGeomety->getChunkyMesh();

	// Tile bounds.
	const float tcs = cfg.tileSize * cfg.cs;

	rcConfig tcfg;
	memcpy(&tcfg, &cfg, sizeof(tcfg));

	tcfg.bmin[0] = cfg.bmin[0] + tx * tcs;
	tcfg.bmin[1] = cfg.bmin[1];
	tcfg.bmin[2] = cfg.bmin[2] + ty * tcs;
	tcfg.bmax[0] = cfg.bmin[0] + (tx + 1) * tcs;
	tcfg.bmax[1] = cfg.bmax[1];
	tcfg.bmax[2] = cfg.bmin[2] + (ty + 1) * tcs;
	tcfg.bmin[0] -= tcfg.borderSize * tcfg.cs;
	tcfg.bmin[2] -= tcfg.borderSize * tcfg.cs;
	tcfg.bmax[0] += tcfg.borderSize * tcfg.cs;
	tcfg.bmax[2] += tcfg.borderSize * tcfg.cs;

	// Allocate voxel heightfield where we rasterize our input data to.
	rc.solid = rcAllocHeightfield();
	if (!rc.solid)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
		return 0;
	}
	if (!rcCreateHeightfield(m_ctx, *rc.solid, tcfg.width, tcfg.height, tcfg.bmin, tcfg.bmax, tcfg.cs, tcfg.ch))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
		return 0;
	}
	
	// Allocate array that can hold triangle flags.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	rc.triareas = new unsigned char[chunkyMesh->maxTrisPerChunk];
	if (!rc.triareas)
	{
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", chunkyMesh->maxTrisPerChunk);
		return 0;
	}

	float tbmin[2], tbmax[2];
	tbmin[0] = tcfg.bmin[0];
	tbmin[1] = tcfg.bmin[2];
	tbmax[0] = tcfg.bmax[0];
	tbmax[1] = tcfg.bmax[2];
	int cid[512];// TODO: Make grow when returning too many items.
	const int ncid = rcGetChunksOverlappingRect(chunkyMesh, tbmin, tbmax, cid, 512);
	if (!ncid)
	{
		return 0; // empty
	}

	for (int i = 0; i < ncid; ++i)
	{
		const rcChunkyTriMeshNode& node = chunkyMesh->nodes[cid[i]];
		const int* tris = &chunkyMesh->tris[node.i * 3];
		const int ntris = node.n;

		memset(rc.triareas, 0, ntris * sizeof(unsigned char));
		rcMarkWalkableTriangles(m_ctx, tcfg.walkableSlopeAngle,
								verts, nverts, tris, ntris, rc.triareas);

		if (!rcRasterizeTriangles(m_ctx, verts, nverts, tris, rc.triareas, ntris, *rc.solid, tcfg.walkableClimb))
			return 0;
	}

	// Once all geometry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	if (m_filterLowHangingObstacles)
		rcFilterLowHangingWalkableObstacles(m_ctx, tcfg.walkableClimb, *rc.solid);
	if (m_filterLedgeSpans)
		rcFilterLedgeSpans(m_ctx, tcfg.walkableHeight, tcfg.walkableClimb, *rc.solid);
	if (m_filterWalkableLowHeightSpans)
		rcFilterWalkableLowHeightSpans(m_ctx, tcfg.walkableHeight, *rc.solid);


	rc.chf = rcAllocCompactHeightfield();
	if (!rc.chf)
	{
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
		return 0;
	}
	if (!rcBuildCompactHeightfield(m_ctx, tcfg.walkableHeight, tcfg.walkableClimb, *rc.solid, *rc.chf))
	{
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
		return 0;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(m_ctx, tcfg.walkableRadius, *rc.chf))
	{
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
		return 0;
	}


	// (Optional) Mark areas.
	const ConvexVolume* vols = m_InputGeomety->getConvexVolumes();
	for (int i = 0; i < m_InputGeomety->getConvexVolumeCount(); ++i)
	{
		rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts,
							 vols[i].hmin, vols[i].hmax,
							 (unsigned char)vols[i].area, *rc.chf);
	}

	rc.lset = rcAllocHeightfieldLayerSet();
	if (!rc.lset)
	{
		//m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'lset'.");
		return 0;
	}
	if (!rcBuildHeightfieldLayers(m_ctx, *rc.chf, tcfg.borderSize, tcfg.walkableHeight, *rc.lset))
	{
	//	m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build heighfield layers.");
		return 0;
	}

	rc.ntiles = 0;
	for (int i = 0; i < rcMin(rc.lset->nlayers, MAX_LAYERS); ++i)
	{
		TileCacheData* tile = &rc.tiles[rc.ntiles++];
		const rcHeightfieldLayer* layer = &rc.lset->layers[i];

		// Store header
		dtTileCacheLayerHeader header;
		header.magic = DT_TILECACHE_MAGIC;
		header.version = DT_TILECACHE_VERSION;

		// Tile layer location in the navmesh.
		header.tx = tx;
		header.ty = ty;
		header.tlayer = i;
		dtVcopy(header.bmin, layer->bmin);
		dtVcopy(header.bmax, layer->bmax);

		// Tile info.
		header.width = (unsigned char)layer->width;
		header.height = (unsigned char)layer->height;
		header.minx = (unsigned char)layer->minx;
		header.maxx = (unsigned char)layer->maxx;
		header.miny = (unsigned char)layer->miny;
		header.maxy = (unsigned char)layer->maxy;
		header.hmin = (unsigned short)layer->hmin;
		header.hmax = (unsigned short)layer->hmax;

		dtStatus status = dtBuildTileCacheLayer(&comp, &header, layer->heights, layer->areas, layer->cons,
												&tile->data, &tile->dataSize);
		if (dtStatusFailed(status))
		{
			return 0;
		}
	}

	// Transfer ownsership of tile data from build context to the caller.
	int n = 0;
	for (int i = 0; i < rcMin(rc.ntiles, maxTiles); ++i)
	{
		tiles[n++] = rc.tiles[i];
		rc.tiles[i].data = 0;
		rc.tiles[i].dataSize = 0;
	}

	return n;
}

bool NavigationSystem::BuildTest()
{
	SAFE_DELETE(m_InputGeomety);
	m_InputGeomety = new InputGeom;
	if (!m_InputGeomety->loadMesh(m_ctx))
		return false;


	if (m_InputGeomety)
	{
		const float* bmin = m_InputGeomety->getNavMeshBoundsMin();
		const float* bmax = m_InputGeomety->getNavMeshBoundsMax();

		int gw = 0, gh = 0;
		rcCalcGridSize(bmin, bmax, CellSize, &gw, &gh);
		const int ts = (int)m_tileSize;
		const int tw = (gw + ts - 1) / ts;
		const int th = (gh + ts - 1) / ts;
		

		// Max tiles and max polys affect how the tile IDs are caculated.
		// There are 22 bits available for identifying a tile and a polygon.
		int tileBits = rcMin((int)dtIlog2(dtNextPow2(tw * th * EXPECTED_LAYERS_PER_TILE)), 14);
		if (tileBits > 14) tileBits = 14;
		int polyBits = 22 - tileBits;
		m_maxTiles = 1 << tileBits;
		m_maxPolysPerTile = 1 << polyBits;

	} 
	
	

	dtStatus status;

	m_tmproc->init(m_InputGeomety);

	// Init cache
	const float* bmin = m_InputGeomety->getNavMeshBoundsMin();
	const float* bmax = m_InputGeomety->getNavMeshBoundsMax();
	int gw = 0, gh = 0;
	rcCalcGridSize(bmin, bmax, CellSize, &gw, &gh);
	const int ts = (int)m_tileSize;
	const int tw = (gw + ts - 1) / ts;
	const int th = (gh + ts - 1) / ts;

	// Generation params.partitionType 
	rcConfig cfg;
	memset(&cfg, 0, sizeof(cfg));
	cfg.cs = CellSize;
	cfg.ch = CellHeight;
	cfg.walkableSlopeAngle = (AgentMaxSlope);
	cfg.walkableHeight = (int)ceilf(AgentHeight / cfg.ch);
	cfg.walkableClimb = (int)floorf((AgentMaxClimb) / cfg.ch);
	cfg.walkableRadius = (int)ceilf(AgentRadius/ cfg.cs);
	cfg.maxEdgeLen = (int)(m_edgeMaxLen / CellSize);
	cfg.maxSimplificationError = m_edgeMaxError;
	cfg.minRegionArea = (int)rcSqr(m_regionMinSize);		// Note: area = size*size
	cfg.mergeRegionArea = (int)rcSqr(m_regionMergeSize);	// Note: area = size*size
	cfg.maxVertsPerPoly = (int)m_vertsPerPoly;
	cfg.tileSize = (int)m_tileSize;
	cfg.borderSize = cfg.walkableRadius + 3; // Reserve enough padding.
	cfg.width = cfg.tileSize + cfg.borderSize * 2;
	cfg.height = cfg.tileSize + cfg.borderSize * 2;
	cfg.detailSampleDist = m_detailSampleDist < 0.9f ? 0 : CellSize * m_detailSampleDist;
	cfg.detailSampleMaxError = CellHeight * m_detailSampleMaxError;
	
	rcVcopy(cfg.bmin, bmin);
	rcVcopy(cfg.bmax, bmax);

	// Tile cache params.
	dtTileCacheParams tcparams;
	memset(&tcparams, 0, sizeof(tcparams));
	rcVcopy(tcparams.orig, bmin);
	tcparams.cs = CellSize;
	tcparams.ch = CellHeight;
	tcparams.width = (int)m_tileSize;
	tcparams.height = (int)m_tileSize;
	tcparams.walkableHeight = AgentHeight;
	tcparams.walkableRadius = AgentRadius;
	tcparams.walkableClimb = (AgentMaxClimb);
	tcparams.maxSimplificationError = m_edgeMaxError;
	tcparams.maxTiles = tw * th * EXPECTED_LAYERS_PER_TILE;
	tcparams.maxObstacles = 128;

	dtFreeTileCache(m_tileCache);

	m_tileCache = dtAllocTileCache();
	if (!m_tileCache)
	{
		//m_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate tile cache.");
		return false;
	}
	status = m_tileCache->init(&tcparams, m_talloc, m_tcomp, m_tmproc);
	if (dtStatusFailed(status))
	{
		//m_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init tile cache.");
		return false;
	}

	dtFreeNavMesh(m_navMesh);

	m_navMesh = dtAllocNavMesh();
	if (!m_navMesh)
	{
		//m_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate navmesh.");
		return false;
	}

	dtNavMeshParams params;
	memset(&params, 0, sizeof(params));
	rcVcopy(params.orig, bmin);
	params.tileWidth = m_tileSize * CellSize;
	params.tileHeight = m_tileSize * CellSize;
	params.maxTiles = m_maxTiles;
	params.maxPolys = m_maxPolysPerTile;

	status = m_navMesh->init(&params);
	if (dtStatusFailed(status))
	{
	//	m_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init navmesh.");
		return false;
	}

	status = m_navQuery->init(m_navMesh, 2048);
	if (dtStatusFailed(status))
	{
		//m_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init Detour navmesh query");
		return false;
	}


	// Preprocess tiles.

	m_ctx->resetTimers();

	
	m_cacheLayerCount = 0;
	m_cacheCompressedSize = 0;
	m_cacheRawSize = 0;



	for (int y = 0; y < th; ++y)
	{
		for (int x = 0; x < tw; ++x)
		{
			TileCacheData tiles[MAX_LAYERS];
			memset(tiles, 0, sizeof(tiles));
			int ntiles = rasterizeTileLayers(x, y, cfg, tiles, MAX_LAYERS);

			for (int i = 0; i < ntiles; ++i)
			{
				TileCacheData* tile = &tiles[i];
				status = m_tileCache->addTile(tile->data, tile->dataSize, DT_COMPRESSEDTILE_FREE_DATA, 0);
				if (dtStatusFailed(status))
				{
					dtFree(tile->data);
					tile->data = 0;
					continue;
				}

				m_cacheLayerCount++;
				m_cacheCompressedSize += tile->dataSize;
				m_cacheRawSize += calcLayerBufferSize(tcparams.width, tcparams.height);
			}
		}
	}

	// Build initial meshes
	m_ctx->startTimer(RC_TIMER_TOTAL);
	for (int y = 0; y < th; ++y)
		for (int x = 0; x < tw; ++x)
			m_tileCache->buildNavMeshTilesAt(x, y, m_navMesh);
	m_ctx->stopTimer(RC_TIMER_TOTAL);

	m_cacheBuildTimeMs = m_ctx->getAccumulatedTime(RC_TIMER_TOTAL) / 1000.0f;
	m_cacheBuildMemUsage = static_cast<unsigned int>(m_talloc->high);


	const dtNavMesh* nav = m_navMesh;
	int navmeshMemUsage = 0;
	for (int i = 0; i < nav->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = nav->getTile(i);
		if (tile->header)
			navmeshMemUsage += tile->dataSize;
	}
	//printf("navmeshMemUsage = %.1f kB", navmeshMemUsage / 1024.0f);



	return true;

}

void NavigationSystem::drawTiles(duDebugDraw* dd, dtTileCache* tc)
{
	unsigned int fcol[6];
	float bmin[3], bmax[3];

	for (int i = 0; i < tc->getTileCount(); ++i)
	{
		const dtCompressedTile* tile = tc->getTile(i);
		if (!tile->header) continue;

		tc->calcTightTileBounds(tile->header, bmin, bmax);

		const unsigned int col = duIntToCol(i, 64);
		duCalcBoxColors(fcol, col, col);
		duDebugDrawBox(dd, bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[2], fcol);
	}

	for (int i = 0; i < tc->getTileCount(); ++i)
	{
		const dtCompressedTile* tile = tc->getTile(i);
		if (!tile->header) continue;

		tc->calcTightTileBounds(tile->header, bmin, bmax);

		const unsigned int col = duIntToCol(i, 255);
		const float pad = tc->getParams()->cs * 0.1f;
		duDebugDrawBoxWire(dd, bmin[0] - pad, bmin[1] - pad, bmin[2] - pad,
						   bmax[0] + pad, bmax[1] + pad, bmax[2] + pad, col, 2.0f);
	}
}

void NavigationSystem::BeginPlay()
{
	m_Begin = true;

	m_NavInit = false;
}

void NavigationSystem::EndPlay()
{
	m_Begin = false;
}

void NavigationSystem::InitCrowdAgent()
{
	if (!m_InputGeomety)
		return;
	m_crowd->init(MaxAgent, AgentRadius, m_navMesh);	

	dtObstacleAvoidanceParams params;
	// Use mostly default settings, copy from dtCrowd.
	memcpy(&params, m_crowd->getObstacleAvoidanceParams(0), sizeof(dtObstacleAvoidanceParams));

	// Low (11)
	params.velBias = 0.5f;
	params.adaptiveDivs = 5;
	params.adaptiveRings = 2;
	params.adaptiveDepth = 1;
	m_crowd->setObstacleAvoidanceParams(0, &params);

	//// Medium (22)
	//params.velBias = 0.5f;
	//params.adaptiveDivs = 5;
	//params.adaptiveRings = 2;
	//params.adaptiveDepth = 2;
	//m_crowd->setObstacleAvoidanceParams(1, &params);

	//// Good (45)
	//params.velBias = 0.5f;
	//params.adaptiveDivs = 7;
	//params.adaptiveRings = 2;
	//params.adaptiveDepth = 3;
	//m_crowd->setObstacleAvoidanceParams(2, &params);

	//// High (66)
	//params.velBias = 0.5f;
	//params.adaptiveDivs = 7;
	//params.adaptiveRings = 3;
	//params.adaptiveDepth = 3;

	//m_crowd->setObstacleAvoidanceParams(3, &params);

}

int32 NavigationSystem::AddAgent(const SVector3& InPos)
{
	int32 AgentID = INDEX_NONE;

	if (m_crowd)
	{
		dtCrowdAgentParams ap;
		memset(&ap, 0, sizeof(ap));
		ap.radius = AgentRadius;
		ap.height = AgentHeight;
		ap.maxAcceleration = 10.0f;
		ap.maxSpeed = 10.0f;
		ap.collisionQueryRange = ap.radius * 12.0f;
		ap.pathOptimizationRange = ap.radius * 30.0f;
		ap.updateFlags = DT_CROWD_ANTICIPATE_TURNS | DT_CROWD_SEPARATION | DT_CROWD_OBSTACLE_AVOIDANCE ;
		ap.obstacleAvoidanceType = 3;		
		ap.separationWeight = 2.0f;		
		
		AgentID = m_crowd->addAgent(InPos.Data(), &ap);
	
	}

	return AgentID;
}

void NavigationSystem::SetAgentLocation(const int32 Id, const SVector3& InAgentLocation)
{
	dtCrowdAgent* ag = m_crowd->getEditableAgent(Id);

	if(ag)
	{		
		ag->npos[0] = InAgentLocation.x;
		ag->npos[1] = InAgentLocation.y;
		ag->npos[2] = InAgentLocation.z;	
	}
	
}

void NavigationSystem::SetAgentVelocity(const int32 Id, float maxSpeed)
{
	dtCrowdAgentParams params = m_crowd->getAgent(Id)->params;
	params.maxSpeed = maxSpeed;
	params.maxAcceleration = 999999.0f;

	//params.height = 
	
	m_crowd->updateAgentParameters(Id, &params);
}


void NavigationSystem::SetAgentRadius(const int32 Id, float Radius)
{
	dtCrowdAgentParams params = m_crowd->getAgent(Id)->params;
	params.radius = Radius;
	m_crowd->updateAgentParameters(Id, &params);
}

void NavigationSystem::SetAgentHeight(const int32 Id, float Height)
{
	dtCrowdAgentParams params = m_crowd->getAgent(Id)->params;
	params.height = Height;
	m_crowd->updateAgentParameters(Id, &params);
}

void NavigationSystem::GetAgentLocation(const int32 Id, SVector3& OutLoc)
{	
	dtCrowdAgent* ag = m_crowd->getEditableAgent(Id);
	if(ag)
	{
		OutLoc.x = ag->npos[0];
		OutLoc.y = ag->npos[1];
		OutLoc.z = ag->npos[2];		
	}
}

void NavigationSystem::ResetMoveTarget(const int32 Id)
{
	m_crowd->resetMoveTarget(Id);
	
}

void NavigationSystem::SetAgentActivate(const int32 Id, bool bActivate)
{
	dtCrowdAgent* ag = m_crowd->getEditableAgent(Id);
	if(ag)
	{
		ag->active = bActivate;		
	}
	

}

void NavigationSystem::GetAgentVelocity(const int32 Id, SVector3& OutVel)
{
	dtCrowdAgent* ag = m_crowd->getEditableAgent(Id);

	if (ag)
	{
		OutVel.x = ag->vel[0];
		OutVel.y = ag->vel[1];
		OutVel.z = ag->vel[2];
	}
}

void NavigationSystem::SetMoveTarget(const int32 Id , const SVector3& InTargetPos)
{
	
	const dtQueryFilter* filter = m_crowd->getFilter(0);
	const float* halfExtents = m_crowd->getQueryExtents();

	//bool adjust = true;
	//if (adjust)
	//{
	//	float vel[3];
	//	// Request velocity
	//	if (Id != INDEX_NONE)
	//	{
	//		const dtCrowdAgent* ag = m_crowd->getAgent(Id);
	//		if (ag && ag->active)
	//		{
	//			calcVel(vel, ag->npos, InTargetPos.Data(), ag->params.maxSpeed);
	//			m_crowd->requestMoveVelocity(Id, vel);
	//		}
	//	}/* else
	//	{
	//		for (int i = 0; i < m_crowd->getAgentCount(); ++i)
	//		{
	//			const dtCrowdAgent* ag = m_crowd->getAgent(i);
	//			if (!ag->active) continue;
	//			calcVel(vel, ag->npos, p, ag->params.maxSpeed);
	//			m_crowd->requestMoveVelocity(i, vel);
	//		}
	// 
	//	}*/
	//} 
	//else
	{
		dtPolyRef targetRef = 0;
		SVector3 targetPos = SVector3::ZeroVector;
		

		m_navQuery->findNearestPoly(InTargetPos.Data(), halfExtents, filter, &targetRef, targetPos.Data());
		
		if (Id != INDEX_NONE)
		{
			const dtCrowdAgent* ag = m_crowd->getAgent(Id);
			if (ag && ag->active)
			{
				m_crowd->resetMoveTarget(Id);
				
				m_crowd->requestMoveTarget(Id, targetRef, InTargetPos.Data());

			}
		}
	}
	
}

void NavigationSystem::RemoveAgent(int32& Id)
{
	m_crowd->removeAgent(Id);
	Id = INDEX_NONE;
}
