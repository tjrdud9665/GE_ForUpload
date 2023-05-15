

struct PSIn
{
	float4 position : SV_POSITION;
};


float4 PSMain(PSIn In) : SV_TARGET
{
    return In.position;
	
	//return(0.0f, 0.0f, 0.0f, 0.0f);
	// if discard is called, depth buffer won't have any data
}