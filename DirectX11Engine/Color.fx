cbuffer cbChangesPerFrame : register( b0 )
{
    matrix mvp_;
};

struct VS_Input
{
    float4 pos  : POSITION;
    float4 color : COLOR;
};

struct PS_Input
{
    float4 pos  : SV_POSITION;
    float4 color : COLOR;
};


PS_Input VS_Main( VS_Input vertex )
{
    PS_Input vsOut = ( PS_Input )0;
    vsOut.pos = mul( vertex.pos, mvp_ );
    vsOut.color = vertex.color;

    return vsOut;
}


float4 PS_Main( PS_Input frag ) : SV_TARGET
{
    return frag.color;
}

technique11 Color
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetPixelShader(CompileShader(ps_5_0, PS_Main()));
    }
}