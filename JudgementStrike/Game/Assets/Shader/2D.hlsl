

struct VS_INPUT
{
	float3 Position : POSITION;
	float4 Color : COLOR;
	float2 UV : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color : TEXCOORD0;
	float2 UV : TEXCOORD1;
};

void VS(
	in VS_INPUT In,
	out VS_OUTPUT Out
)
{
	Out.Position.xyz = In.Position.xyz;

	Out.Position.w = 1.0f;
	Out.Color.rgba = In.Color.bgra;
	Out.UV = In.UV;
}

typedef VS_OUTPUT PS_INPUT;


Texture2D diffuse : register(t0);
SamplerState samplerDiffuse : register(s0);

void PS(
	in PS_INPUT In,
	out float4 OutColor : SV_Target0
)
{
	OutColor = In.Color;
}
void PSTex(
	in PS_INPUT In,
	out float4 OutColor : SV_Target0
)
{
	float4 tex = diffuse.Sample(samplerDiffuse, In.UV);
	OutColor = In.Color * tex;
}

void PSFont(
	in PS_INPUT In,
	out float4 OutColor : SV_Target0
)
{
	float4 tex = diffuse.Sample(samplerDiffuse, In.UV);

	OutColor.rgb = In.Color.rgb;
	OutColor.a = In.Color.a * tex.r;
}

cbuffer CustomParam : register(b0)
{
	float4 param;
};

void PSCustom(
	in PS_INPUT In,
	out float4 OutColor : SV_Target0
)
{
	float4 tex = diffuse.Sample(samplerDiffuse, In.UV);
	OutColor = In.Color * tex;
	OutColor.rgb = dot(OutColor.rgb, param.rgb);
	OutColor.a *= param.a;
}

//cbuffer ColorPhaseParam : register(b1)
//{
//	float4 param;	// 色相
//}


float Epsilon = 1e-10;

float3 RGBtoHCV(float3 RGB)
{
	// Based on work by Sam Hocevar and Emil Persson
	float4 P = (RGB.g < RGB.b) ? float4(RGB.bg, -1.0, 2.0 / 3.0) : float4(RGB.gb, 0.0, -1.0 / 3.0);
	float4 Q = (RGB.r < P.x) ? float4(P.xyw, RGB.r) : float4(RGB.r, P.yzx);
	float C = Q.x - min(Q.w, Q.y);
	float H = abs((Q.w - Q.y) / (6 * C + Epsilon) + Q.z);
	return float3(H, C, Q.x);
}

float3 RGBtoHSV(float3 RGB)
{
	float3 HCV = RGBtoHCV(RGB);
	float S = HCV.y / (HCV.z + Epsilon);
	return float3(HCV.x, S, HCV.z);
}


float3 HUEtoRGB(float H)
{
	float R = abs(H * 6 - 3) - 1;
	float G = 2 - abs(H * 6 - 2);
	float B = 2 - abs(H * 6 - 4);
	return saturate(float3(R, G, B));
}


float3 HSVtoRGB(in float3 HSV)
{
	float3 RGB = HUEtoRGB(HSV.x);
	return ((RGB - 1) * HSV.y + 1) * HSV.z;
}

void PSChangeHSV(in PS_INPUT In, out float4 OutColor : SV_Target0)
{
	float4 tex = diffuse.Sample(samplerDiffuse, In.UV);
	In.Color *= tex;

	float3 hsv = RGBtoHSV(In.Color.rgb);

	// 色相変化
	hsv.x += param.x;
	if (hsv.x >= 1.0) {
		hsv.x -= 1.0;
	}
	// 彩度変化
	hsv.y *= param.y;
	// 輝度変化
	hsv.z *= param.z;

	OutColor.rgb = HSVtoRGB(hsv);
	OutColor.a = In.Color.a;

	//float4 tex = diffuse.Sample(samplerDiffuse, In.UV);
	//OutColor = In.Color * tex;
	//OutColor.rgb = float3(param.x, 0, 0);

	//// RGB->HSV
	//float3 hsv;

	//float maxVal = max(In.Color.r, max(In.Color.g, In.Color.b));
	//float minVal = min(In.Color.r, min(In.Color.g, In.Color.b));
	//float delta = maxVal - minVal;
	//
	//// V（明度）
	//hsv.z = maxVal;

	//// S（彩度）
	//if (maxVal != 0.0) {
	//	hsv.y = delta / maxVal;
	//}
	//else {
	//	hsv.y = 0.0;
	//}
	//
	//// H（色相）
	//if (hsv.y > 0.0) {
	//	if (In.Color.r == maxVal) {
	//		hsv.x = (In.Color.r - In.Color.b) / delta;
	//	}
	//	else if (In.Color.b == maxVal) {
	//		hsv.x = 2 + (In.Color.b - In.Color.r) / delta;
	//	}
	//	else {
	//		hsv.x = 4 + (In.Color.r - In.Color.g) / delta;
	//	}
	//	hsv.x /= 6.0;
	//	if (hsv.x < 0) {
	//		hsv.x += 1.0;
	//	}
	//}

	//// 色相変化
	//hsv.x = param.x;

	//float4 tex = diffuse.Sample(samplerDiffuse, In.UV);
	//OutColor = In.Color * tex;

	//// HSV->RGB
	//if (hsv.y == 0) {
	//	OutColor.rgb = hsv.z;
	//}
	//else {
	//	hsv.x *= 6.0;
	//	float i = floor(hsv.x);
	//	float f = hsv.x - i;
	//	float aa = hsv.z * (1 - hsv.y);
	//	float bb = hsv.z * (1 - (hsv.y * f));
	//	float cc = hsv.z * (1 - (hsv.y * (1 - f)));

	//	if (i < 1) {
	//		OutColor.rgb = float3(hsv.z, cc, aa);
	//	}
	//	else if (i < 2) {
	//		OutColor.rgb = float3(bb, hsv.z, cc);
	//	}
	//	else if (i < 3) {
	//		OutColor.rgb = float3(aa, hsv.z, cc);
	//	}
	//	else if (i < 4) {
	//		OutColor.rgb = float3(aa, bb, hsv.z);
	//	}
	//	else if (i < 5) {
	//		OutColor.rgb = float3(cc, aa, hsv.z);
	//	}
	//	else {
	//		OutColor.rgb = float3(hsv.z, aa, bb);
	//	}
	//}
}
