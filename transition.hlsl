struct VS_OUTPUT {
  float4 position : SV_POSITION;
  float2 tex : TEXCOORD0;
  float4 color : COLOR0;
};

cbuffer psConstants1 : register(b1) {
  float4 g_range;
};

SamplerState sampler0 : register(s0);
Texture2D rule_tex : register(t0);
Texture2D texture1 : register(t1);

float4 PS(VS_OUTPUT input) : SV_Target{
  float alpha = rule_tex.Sample(sampler0, input.tex).a;
  alpha = (g_range.z + smoothstep(g_range.x, g_range.y, alpha)) * g_range.w;
  float4 srcColor = texture1.Sample(sampler0, input.tex);
  srcColor.a *= alpha;
  return srcColor;
}
