cbuffer cbChangesPerFrame : register( b0 )
{
    matrix mwvp;
};

Texture2D colorMap_ : register( t0 );
SamplerState colorSampler_ : register( s0 );

struct VS_Input
{
    float4 pos  : POSITION;
    float2 tex : TEXCOORD0;
    float4 texScale : TEXCOORD1;
    float3 instancePos : TEXCOORD2;
    float3 posScale : TEXCOORD3;
};

struct PS_Input
{
    float4 pos  : SV_POSITION;
    float2 tex : TEXCOORD0;
};

PS_Input VS_Main( VS_Input vertex )
{
    PS_Input vsOut = ( PS_Input )0;
    vertex.pos.xyz += vertex.instancePos.xyz;
    vertex.pos.xyz *= vertex.posScale.xyz;
    vsOut.pos = mul( vertex.pos, mwvp );
    vsOut.tex = vertex.tex;
    vsOut.tex.xy *= vertex.texScale.zw;
    vsOut.tex.xy += vertex.texScale.xy;
    return vsOut;
}

float4 PS_Main( PS_Input frag ) : SV_TARGET
{
    return colorMap_.Sample( colorSampler_, frag.tex );
}