

struct VSIn
{  
	uint instanceID : SV_InstanceID;
};


struct GSIn
{    
    float3 viewPosition : POSITION0;
    float2 uv : TEXCOORD0;

	uint instanceID			: SV_InstanceID;

};

GSIn VSMain(VSIn input)
{
    GSIn output = (GSIn) 0;

    output.instanceID = input.instanceID;    
    
    return output;
}