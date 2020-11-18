//Pixel shader just determines the colors of each pixels
//The rasterizer determines the pixel positions instead

cbuffer CBuf {

	float4 face_colors[6];
};

//SV_TARGET stands for render target
float4 main(uint tid:SV_PrimitiveID) : SV_Target
{
	return face_colors[tid/2];
}