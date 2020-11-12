//Pixel shader just determines the colors of each pixels
//The rasterizer determines the pixel positions instead

//SV_TARGET stands for render target
float4 main(float3 color:Color) : SV_TARGET
{
	return float4(color,1.0f);
}