cbuffer ConstantBuffer
{
	float xCoef;
	float xAff;
	float yCoef;
	float yAff;
}

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VOut VShader(float2 position : POSITION)
{
	VOut output;

	output.position = float4(xCoef * position.x + xAff, yCoef * position.y + yAff, 0.0f, 1.0f);
	output.color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	return output;
}


float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	return color;
}