cbuffer cbShadowMap : register(b0)
{
    row_major float4x4 matLightWVP;
};

float4 main( float4 pos : POSITION ) : SV_POSITION
{
    return mul(pos, matLightWVP);
}

void PS( float4 pos : SV_POSITION)
{
}