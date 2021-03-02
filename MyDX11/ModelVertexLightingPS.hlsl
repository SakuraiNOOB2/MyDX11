
//constant  buffer for chaning the light position every frame
cbuffer LightCBuf
{
    
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

//constant buffer for pixelmaterial constant
cbuffer ObjectCBuf
{
   
    float specularIntensity;
    float specularPower;
    float padding[2];
};

Texture2D tex;
SamplerState splr;

//taking position and normal of the pixel
float4 main(float3 worldPos : Position, float3 n : Normal, float2 tc : TEXCOORD) : SV_Target
{
    
    float4 normal = normalize(n);
    float light = 0.5f - dot(n.xyz, lightPos) * 0.5f;
    
    float4 outDiffuse;
    outDiffuse = tex.Sample(splr, tc);
    outDiffuse.rgb *= diffuseColor * light;
    outDiffuse.a *= diffuseIntensity;
    
    return outDiffuse;
}