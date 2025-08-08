cbuffer ObjectData : register(b0) {
    float4x4 mvp;
}

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput vertex(float3 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = mul(float4(position, 1.0f), mvp);
    result.color = color;

    return result;
}

float4 fragment(PSInput input) : SV_TARGET
{
    return input.color;
}
