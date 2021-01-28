
//constant  buffert for chaning the light position every frame
cbuffer LightCBuf
{
    
    float3 lightPos;
    float3 materialColor;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};


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