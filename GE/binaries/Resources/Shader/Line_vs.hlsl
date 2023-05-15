



//
struct VSIn
{
    uint instanceID : SV_InstanceID;
};

struct GSIn
{
    uint instanceID : SV_InstanceID;
};

GSIn VSMain(VSIn In)
{    
	GSIn Out;
    Out.instanceID = In.instanceID;
	return Out;
}