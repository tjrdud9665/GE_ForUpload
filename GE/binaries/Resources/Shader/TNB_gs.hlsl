

struct GSIn
{
	float3 T : TANGENT;
	float3 N : NORMAL;
	float3 B : BINORMAL;
	float3 WorldPosition : POSITION;
};

struct PSIn
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

cbuffer renderConsts
{
	matrix viewProj;
	int mode;
};

// Note about 'maxvertexcount(N)'
// [NVIDIA08]:	peak perf when GS outputs 1-20 scalars
//				50% perf if 27-40 scalars are output
//				where scalar count = N * PSIn(scalarCount)
[maxvertexcount(6 * 3)]
void GSMain(triangle GSIn gin[3], inout LineStream<PSIn> lineStream)
{
	PSIn Out[18];

	for (int i = 0; i < 3; ++i)
	{
		const float scale = 1.0f;
		const float4 VertPos = mul(viewProj, float4(gin[i].WorldPosition, 1));
		const float4 TPos    = mul(viewProj, float4(gin[i].WorldPosition + gin[i].T*scale, 1));
		const float4 NPos    = mul(viewProj, float4(gin[i].WorldPosition + gin[i].N*scale, 1));
		const float4 BPos    = mul(viewProj, float4(gin[i].WorldPosition + gin[i].B*scale, 1));

		const float3 red	= float3(1, 0, 0) * 1.8;	// bloom fun
		const float3 blue	= float3(0, 0, 1) * 1.2;
		const float3 green	= float3(0, 1, 0) * 2;

		switch (3)
		{
		case 0:
		{
			// T
			Out[i * 6 + 0].position = VertPos;
			Out[i * 6 + 1].position = TPos;
			Out[i * 6 + 0].color = red;
			Out[i * 6 + 1].color = red;
			lineStream.RestartStrip();
			lineStream.Append(Out[i * 6 + 0]);
			lineStream.Append(Out[i * 6 + 1]);

		}
			break;

		case 1:
		{
			// B
			Out[i * 6 + 4].position = VertPos;
			Out[i * 6 + 5].position = BPos;
			Out[i * 6 + 4].color = green;
			Out[i * 6 + 5].color = green;
			lineStream.RestartStrip();
			lineStream.Append(Out[i * 6 + 4]);
			lineStream.Append(Out[i * 6 + 5]);
		}
			break;

		case 2:
		{
			// N
			Out[i * 6 + 2].position = VertPos;
			Out[i * 6 + 3].position = NPos;
			Out[i * 6 + 2].color = blue;
			Out[i * 6 + 3].color = blue;
			lineStream.RestartStrip();
			lineStream.Append(Out[i * 6 + 2]);
			lineStream.Append(Out[i * 6 + 3]);
		}
			break;

		case 3:
		{
			// T
			Out[i * 6 + 0].position = VertPos;
			Out[i * 6 + 1].position = TPos;
			Out[i * 6 + 0].color = red;
			Out[i * 6 + 1].color = red;
			lineStream.RestartStrip();
			lineStream.Append(Out[i * 6 + 0]);
			lineStream.Append(Out[i * 6 + 1]);

			// N
			Out[i * 6 + 2].position = VertPos;
			Out[i * 6 + 3].position = NPos;
			Out[i * 6 + 2].color = blue;
			Out[i * 6 + 3].color = blue;
			lineStream.RestartStrip();
			lineStream.Append(Out[i * 6 + 2]);
			lineStream.Append(Out[i * 6 + 3]);

			// B
			Out[i * 6 + 4].position = VertPos;
			Out[i * 6 + 5].position = BPos;
			Out[i * 6 + 4].color = green;
			Out[i * 6 + 5].color = green;
			lineStream.RestartStrip();
			lineStream.Append(Out[i * 6 + 4]);
			lineStream.Append(Out[i * 6 + 5]);
		}
		break;
		}


	}
}