#pragma once

#pragma warning(push, 0)   
#include <LinearMath/btIDebugDraw.h>
#pragma warning(pop)
#include <DebugDraw.h>
//DebugUtils\Include\

class PhysicsDebugDraw :public btIDebugDraw, public duDebugDraw
{
	friend class PhysicsSystem;
	friend class RecastInputMesh;
	friend class NavigationSystem;

private:
	PhysicsDebugDraw(class RenderManager* renderManager);
	virtual ~PhysicsDebugDraw(){}

public:
	// Begin btIDebugDraw implementation
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override;
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override { drawLine(from, to, color, color); }
	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
	virtual void reportErrorWarning(const char* warningString) override;
	virtual void draw3dText(const btVector3& location, const char* textString) override {}
	virtual void setDebugMode(const int debugMode) override { m_debugMode = debugMode; }
	virtual int getDebugMode() const override { return m_debugMode; }	

	void drawArrow(const SVector3& From,const SVector3& To ,const float& as0,const float& as1,const SLinearColor& color, const float& Width);
	// End btIDebugDraw implementation


	void drawSolidTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color);

	// Begin duDebugDraw implementation
	virtual void depthMask(bool state)
	{
	}
	virtual void texture(bool state)
	{
	}
	virtual void begin(duDebugDrawPrimitives prim, float size = 1.0f);
	virtual void vertex(const float* pos, uint32 color);
	virtual void vertex(const float x, const float y, const float z, uint32 color);
	virtual void vertex(const float* pos, uint32 color, const float* uv);
	virtual void vertex(const float x, const float y, const float z, uint32 color, const float u, const float v);
	virtual void end();
	// End duDebugDraw implementation


public:
	//* Only Editor
	class RenderManager* SetImitateRenderManager(class RenderManager* ImitateRenderManager)
	{
		class RenderManager* ret = m_cahcedRenderManager;
		m_cahcedRenderManager = ImitateRenderManager;

		return ret;
	}	

	

private:
	class RenderManager* m_cahcedRenderManager;	

	int32 m_debugMode;

	//for NavMesh
private:
	duDebugDrawPrimitives m_PrimType;
	int32 curPrimVertIndex = 0;
	btVector3 primInput[4];

	btVector3 navMeshColor{ 0.0f, 1.0f, 1.0f };
};

