
//constant  buffert for chaning the light position every frame
cbuffer LightCBuf
{
    
    float3 lightPos;
};


//other lighting parameters
static const float3 materialColor = { 0.7f, 0.7f, 0.9f };
static const float3 ambient = { 0.05f, 0.05f, 0.05f };
static const float3 diffuseColor = { 1.0f, 1.0f, 1.0f };
static const float diffuseIntensity = 1.0f;
static const float attConst = 1.0f;
static const float attLin = 0.045f;
static const float attQuad = 0.045f;

//taking position and normal of the pixel
float4 main(float3 worldPos : Position, float3 n : Normal) : SV_Target
{
    //fragment to light vector data
    const float3 vToL = lightPos - worldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    
    //diffuse attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
    
    //diffuse intensity base on dot product
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
    
    //final color calculation 
    return float4(saturate((diffuse + ambient) * materialColor), 1.0f);
    
}