cbuffer cbChangesPerFrame : register( b0 )
{
    matrix mvp_;
};

Texture2D colorMap_ : register( t0 );
SamplerState colorSampler_ : register( s0 );

struct VS_Input
{
    float4 pos  : POSITION;
    float3 n : NORMAL0;
    float2 tex : TEXCOORD0;
};

struct PS_Input
{
    float4 pos  : SV_POSITION;
    float2 tex : TEXCOORD0;
};


PS_Input VS_Main( VS_Input vertex )
{
    PS_Input vsOut = ( PS_Input )0;
    vertex.pos.w = 1.0f;
    vsOut.pos = mul( vertex.pos, mvp_ );
    vsOut.tex = vertex.tex;
    return vsOut;
}


float4 PS_Main( PS_Input frag ) : SV_TARGET
{
    return colorMap_.Sample(colorSampler_ , frag.tex);
}

technique11 Color
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS_Main()));
        SetPixelShader(CompileShader(ps_4_0, PS_Main()));
    }
}