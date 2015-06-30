cbuffer cbChangesPerFrame : register( b0 )
{
    matrix mvp;
};

cbuffer viewPosition : register( b1 )
{
    float4 viewPos;
};

TextureCube cubeMap : register( t0 );
SamplerState colorSampler : register( s0 );

struct VS_Input
{
    float4 pos  : POSITION;
    float4 color : COLOR0;
};

struct PS_Input
{
    float4 pos  : SV_POSITION;
    float3 dir : TEXCOORD0;
};


PS_Input VS_Main( VS_Input vertex )
{
    PS_Input vsOut = ( PS_Input )0;
    vsOut.dir = vertex.pos.xyz;
    vertex.pos.xyz += viewPos.xyz;
    vsOut.pos = mul( vertex.pos, mvp );
    return vsOut;
}


float4 PS_Main( PS_Input frag ) : SV_TARGET
{
    return cubeMap.Sample( colorSampler, frag.dir);
}

technique11 Color
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS_Main()));
        SetPixelShader(CompileShader(ps_4_0, PS_Main()));
    }
}