struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput vertex(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = position;
    result.color = color;

    return result;
}

float4 fragment(PSInput input) : SV_TARGET
{
    return input.color;
}