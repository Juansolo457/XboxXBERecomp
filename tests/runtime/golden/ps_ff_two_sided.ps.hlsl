
struct VOut {
    float4 pos : SV_Position;
    float4 d0 : COLOR0;
    float4 d1 : COLOR1;
    float4 b0 : COLOR2;
    float4 b1 : COLOR3;
    float4 tc0 : TEXCOORD0;
    float4 tc1 : TEXCOORD1;
    float4 tc2 : TEXCOORD2;
    float4 tc3 : TEXCOORD3;
    float fogamt : TEXCOORD4;
};
cbuffer VC : register(b0) { float4 c[192]; };
cbuffer DC : register(b1) {
    float4 surf;
    float4 fogp;
    float4 fogcolor;
    float4 misc;
    float4 pc[18];
    float4 tfactor;
    float4 texsize[4];
    float4 bumpmat[4];
    float4 bumplum[4];
    float4x4 wvp;
    float4 ext;
    float4 spread[4];
    float4x4 wv;
    float4x4 wvn;
    float4x4 tm[4];
    float4 mat[5];
    float4 amb;
    float4 lt[56];
    float4 rtshift;
    float4 zparams;
};

// unorm to byte b, less lo under 128 or hi from 128, over d
float3 sbyte(float3 u, float lo, float hi, float d) { float3 b = u * 255; return (b - (b >= 128 ? hi : lo)) / d; }
// the sampler compares, a linear filter blends the four results like the console does
float4 shadow(Texture2D<float> t, SamplerComparisonState s, float2 uv, float z, int fmt, float2 px, float2 size) {
    float mx = fmt == 2 ? 65535.0 : 16777215.0;
    return t.SampleCmpLevelZero(s, uv, clamp(z, 0, mx) / mx).xxxx;
}
float4 spreadtap(Texture2D t, SamplerState s, float2 uv, float4 w, float m) {
    float4 c = 0;
    int n = (int)w.z;
    for (int y = 0; y < n; y++)
        for (int x = 0; x < n; x++) {
            float4 v = t.SampleLevel(s, uv + w.xy * (float2(x, y) - (n - 1) * 0.5), 0);
            c += float4(saturate(v.rgb * m), v.a);
        }
    return c / (n * n);
}
void alphatest(float a) {
    int f = (int)misc.y;
    int v = (int)round(a * 255), r = (int)misc.x;
    bool ok = f == 0 ? false : f == 1 ? v < r : f == 2 ? v == r : f == 3 ? v <= r : f == 4 ? v > r : f == 5 ? v != r : f == 6 ? v >= r : true;
    if (!ok) discard;
}
Texture2D tex0 : register(t0);
SamplerState smp0 : register(s0);
Texture2D tex1 : register(t1);
SamplerState smp1 : register(s1);
Texture2D tex2 : register(t2);
SamplerState smp2 : register(s2);
Texture2D tex3 : register(t3);
SamplerState smp3 : register(s3);
float4 main(VOut i, bool front : SV_IsFrontFace) : SV_Target {
    float4 tc0 = i.tc0, tc1 = i.tc1, tc2 = i.tc2, tc3 = i.tc3;
    float4 v0 = front ? i.d0 : i.b0, v1 = front ? i.d1 : i.b1;
    if (misc.w == 0) v1 = float4(0, 0, 0, 1);
    float4 fogin = float4(fogcolor.rgb, saturate(i.fogamt));
float4 cur = v0, tmp = 0;
float4 t0 = tex0.Sample(smp0, (tc0.xy) + texsize[0].zw);
{ float3 c3 = saturate((t0).rgb * (v0).rgb); float c1 = saturate((t0).a * (v0).a); cur = float4(c3, c1); }
cur.rgb = saturate(cur.rgb + v1.rgb);
float4 outc = float4(lerp(fogin.rgb, cur.rgb, fogin.a), cur.a);
outc.rgb = saturate(outc.rgb);
return outc;
}
