#pragma once
#include "PanelSlot.h"

struct SMargin
{
	float Left;

	float Top;
		
	float Right;
	
	float Bottom;

	SMargin()
		: Left(0.0f)
		, Top(0.0f)
		, Right(0.0f)
		, Bottom(0.0f)
	{}

	SMargin(float UniformMargin)
		: Left(UniformMargin)
		, Top(UniformMargin)
		, Right(UniformMargin)
		, Bottom(UniformMargin)
	{}

	SMargin(float Horizontal, float Vertical)
		: Left(Horizontal)
		, Top(Vertical)
		, Right(Horizontal)
		, Bottom(Vertical)
	{}

	SMargin(const SVector2& InVector)
		: Left(InVector.x)
		, Top(InVector.y)
		, Right(InVector.x)
		, Bottom(InVector.y)
	{}

	SMargin(float InLeft, float InTop, float InRight, float InBottom)
		: Left(InLeft)
		, Top(InTop)
		, Right(InRight)
		, Bottom(InBottom)
	{}

	SMargin(const SVector4& InVector)
		: Left(InVector.x)
		, Top(InVector.y)
		, Right(InVector.z)
		, Bottom(InVector.w)
	{
	}

	SVector2 GetTopLeft() const
	{
		return SVector2(Left, Top);
	}

	SVector2 GetDesiredSize() const
	{
		return SVector2(Left + Right, Top + Bottom);
	}


	SMargin operator*(float Scale) const
	{
		return SMargin(Left * Scale, Top * Scale, Right * Scale, Bottom * Scale);
	}


	SMargin operator*(const SMargin& InScale) const
	{
		return SMargin(Left * InScale.Left, Top * InScale.Top, Right * InScale.Right, Bottom * InScale.Bottom);
	}


	SMargin operator+(const SMargin& InDelta) const
	{
		return SMargin(Left + InDelta.Left, Top + InDelta.Top, Right + InDelta.Right, Bottom + InDelta.Bottom);
	}


	SMargin operator-(const SMargin& Other) const
	{
		return SMargin(Left - Other.Left, Top - Other.Top, Right - Other.Right, Bottom - Other.Bottom);
	}

	bool operator==(const SMargin& Other) const
	{
		return (Left == Other.Left) && (Right == Other.Right) && (Top == Other.Top) && (Bottom == Other.Bottom);
	}


	bool operator!=(const SMargin& Other) const
	{
		return Left != Other.Left || Right != Other.Right || Top != Other.Top || Bottom != Other.Bottom;
	}
};


struct SAnchors
{
	SVector2 Minimum;

	SVector2 Maximum;

	SAnchors()
		: Minimum(0.0f, 0.0f)
		, Maximum(0.0f, 0.0f)
	{}

	
	SAnchors(float UnifromAnchors)
		: Minimum(UnifromAnchors, UnifromAnchors)
		, Maximum(UnifromAnchors, UnifromAnchors)
	{}

	/** Construct a Anchors where Horizontal describes Left and Right spacing while Vertical describes Top and Bottom spacing */
	SAnchors(float Horizontal, float Vertical)
		: Minimum(Horizontal, Vertical)
		, Maximum(Horizontal, Vertical)
	{}

	/** Construct Anchors where the spacing on each side is individually specified. */
	SAnchors(float MinX, float MinY, float MaxX, float MaxY)
		: Minimum(MinX, MinY)
		, Maximum(MaxX, MaxY)
	{}


	bool operator==(const SAnchors& Other) const
	{
		return Minimum == Other.Minimum && Maximum == Other.Maximum;
	}

	bool operator!=(const SAnchors& Other) const
	{
		return !(*this == Other);
	}

	/** Returns true if the anchors represent a stretch along the vertical axis */
	bool IsStretchedVertical() const { return Minimum.y != Maximum.y; }

	/** Returns true if the anchors represent a stretch along the horizontal axis */
	bool IsStretchedHorizontal() const { return Minimum.x != Maximum.x; }
};

struct SAnchorData
{	
	SMargin Offsets;
	
	SAnchors Anchors;

	SVector2 Alignment;


	bool operator==(const SAnchorData& Other) const
	{
		return Offsets == Other.Offsets &&
			Anchors == Other.Anchors &&
			Alignment == Other.Alignment;
	}

	
	bool operator!=(const SAnchorData& Other) const
	{
		return !(*this == Other);
	}
};

class CanvasPanelSlot :public PanelSlot
{
	CLASS(CanvasPanelSlot, PanelSlot)

protected:
	CanvasPanelSlot();
	CanvasPanelSlot(const CanvasPanelSlot& Other);
	virtual ~CanvasPanelSlot();

public:


	virtual void SetupGeometry(class Renderer* pRenderer) override;





public:


public:
	//Transient
	PROP(SAnchorData,LayoutData,DetailsEdit ,"CanvasLayout")
	SAnchorData LayoutData;
	//X크기 및 y 크기 및 위치
	
	PROP(bool, SizeToContent, DetailsEdit, "CanvasLayout")
		bool SizeToContent;

	PROP(int32, ZOrder, DetailsEdit,"CanvasLayout")
	int32 ZOrder;

};

