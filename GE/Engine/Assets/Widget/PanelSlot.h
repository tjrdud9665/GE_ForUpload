#pragma once
#include "Assets/Mesh/Mesh.h"




class PanelSlot : public XObject
{
	CLASS(PanelSlot, XObject)
	friend class PanelWidget;
	friend class Widget;
	friend class TextBlock;
	friend class UserWidget;
	friend class WidgetTree;
	friend class UIManager;

protected:
	PanelSlot();
	PanelSlot(const PanelSlot& Other);
	virtual ~PanelSlot();	

	virtual void Construct(const std::string& InName /* = "" */) override;



public:
	class PanelWidget* GetParent() const
	{
		return Parent;
	}



	virtual void Render(class Renderer* pRenderer);	

	virtual void SetupGeometry(class Renderer* pRenderer);	

	class Widget* GetContent() const
	{
		return Content;
	}
	
	
	const std::vector<SVertex_PUV>& GetGeometryVertices() const;
	

	void SetClipSpace(const SScissorsRect& InClipSpace);


protected:
	class PanelWidget* Parent;

	TSharedPtr<Widget>Content;
	//class Widget* Content;

protected:
	//std::vector<SVector3>		m_VerticesPos;
	//std::vector<SVector2>		m_VerticesUVs;

	std::vector<SVertex_PUV>	m_Vertices;
	
	//Mesh						m_Geometry;

	bool m_GeometryDirty;

	class UIManager* m_cachedManager;

	
	SScissorsRect		m_ClipSpace;

	
};

