
//constant buffer
cbuffer CBuf
{
    matrix model;       //model transform
    matrix modelViewProj;       //concatenated model view projection
};

struct VSOut
{
    float3 worldPos : Position;     
    float3 normal : Normal;     
    float4 pos : SV_Position;       
};

VSOut main(float3 pos : POSITION, float3 n : NORMAL)
{
    
    VSOut vso;
    
    //calculate the world position of the pixel
    vso.worldPos = (float3) mul(float4(pos, 1.0f), model);
    
    //
    vso.normal = mul(n, (float3x3) model);
    
    //
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    
    return vso;
}