#include "EnginePCH.h"
#include "PhysicsDebugDraw.h"
#include "RenderManager.h"
#include "BulletPhysicsHelper.h"


PhysicsDebugDraw::PhysicsDebugDraw(class RenderManager* renderManager)
{	
	m_cahcedRenderManager = renderManager;
	m_debugMode = DBG_DrawWireframe | DBG_DrawContactPoints | DBG_DrawConstraints | DBG_DrawConstraintLimits;

	m_PrimType = static_cast<duDebugDrawPrimitives>(-1);

}

void PhysicsDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
	m_cahcedRenderManager->DrawLine(ToVector3(from), ToVector3(to), ToVector4(fromColor));
}


void PhysicsDebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	const btVector3& from = PointOnB;
	const btVector3 to = PointOnB + normalOnB * distance;
	drawLine(from, to, color);
}

void PhysicsDebugDraw::reportErrorWarning(const char* warningString)
{

}

void PhysicsDebugDraw::drawArrow(const SVector3& From, const SVector3& To, const float& as0, const float& as1, const SLinearColor& color, const float& Width)
{		
	duDebugDrawArrow(this,From.x, From.y, From.z,To.x,To.y,To.z,as0,as1,color.ToUINT(),Width);	
}

void PhysicsDebugDraw::drawSolidTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color)
{
	m_cahcedRenderManager->DrawSolid(ToVector3(a), ToVector3(b), ToVector3(c), ToVector3(color));
}

void PhysicsDebugDraw::begin(duDebugDrawPrimitives prim, float size /*= 1.0f*/)
{
	curPrimVertIndex = 0;
	m_PrimType = prim;
}

void PhysicsDebugDraw::vertex(const float* pos, uint32 color)
{
	vertex(pos[0], pos[1], pos[2], color);
}

void PhysicsDebugDraw::vertex(const float x, const float y, const float z, uint32 color)
{
	primInput[curPrimVertIndex] = { x, y, z };

	switch (m_PrimType)
	{
	case DU_DRAW_POINTS:
		// is this okay?
		drawLine(primInput[0], primInput[0], navMeshColor * 0.75f);
		break;
	case DU_DRAW_LINES:
		if (curPrimVertIndex == 1)
		{
			drawLine(primInput[0], primInput[1], navMeshColor * 0.6f);
		}
		break;
	case DU_DRAW_TRIS:
		if (curPrimVertIndex == 2)
		{
			drawSolidTriangle(primInput[0], primInput[1], primInput[2], navMeshColor);
		}
		break;
	case DU_DRAW_QUADS:
		if (curPrimVertIndex == 3)
		{
			drawSolidTriangle(primInput[0], primInput[1], primInput[2], navMeshColor);
			drawSolidTriangle(primInput[2], primInput[3], primInput[0], navMeshColor);
		}
		break;
	default:
		assert(false && "Current primitive type is not set - did you forget to call begin?");
		break;
	}

	curPrimVertIndex = (curPrimVertIndex + 1) % static_cast<int>(m_PrimType + 1);
}

void PhysicsDebugDraw::vertex(const float* pos, uint32 color, const float* uv)
{
	vertex(pos[0], pos[1], pos[2], color, uv[0], uv[1]);

}

void PhysicsDebugDraw::vertex(const float x, const float y, const float z, uint32 color, const float u, const float v)
{
	vertex(x, y, z, color);
}

void PhysicsDebugDraw::end()
{
	m_PrimType = static_cast<duDebugDrawPrimitives>(-1);
}
