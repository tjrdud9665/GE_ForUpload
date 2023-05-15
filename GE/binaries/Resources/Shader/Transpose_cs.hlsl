
#if R32F
RWTexture2D<float> texTranspozeOut;
RWTexture2D<float> texImageIn;
#else

//#if RGBA32F
RWTexture2D<float4> texTranspozeOut;
RWTexture2D<float4> texImageIn;
//#endif
#endif

[numthreads(16, 16, 1)]
void CSMain(
	uint3 groupID     : SV_GroupID,
	uint3 groupTID    : SV_GroupThreadID,
	uint3 dispatchTID : SV_DispatchThreadID,
	uint  groupIndex  : SV_GroupIndex
)
{
	//if(dispatchTID.x < 1920 && dispatchTID.y < 1080)
		texTranspozeOut[dispatchTID.yx] = texImageIn[dispatchTID.xy];
}
