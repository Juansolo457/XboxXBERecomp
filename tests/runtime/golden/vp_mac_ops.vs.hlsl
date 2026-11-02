
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

float4 mac_mul(float4 a, float4 b) { return (a == 0 || b == 0) ? (float4)0 : a * b; }
int mac_arl(float v) { return (int)floor(v + 1e-3); }
// magnitude clamped between 1 / 2^64 and 2^64, sign kept
float magclamp(float v) { float m = clamp(abs(v), 5.42101e-20, 1.84467e+19); return v < 0 ? -m : m; }
float4 ilu_rcp(float v) { return (1.0 / v).xxxx; }
float4 ilu_rcc(float v) { return magclamp(1.0 / v).xxxx; }
float4 ilu_rsq(float v) { return v == 0 ? (1.#INF).xxxx : (rsqrt(abs(v))).xxxx; }
// 2^floor(v), fraction, 2^v
float4 ilu_exp(float v) {
    float whole = floor(v);
    return float4(exp2(whole), v - whole, exp2(v), 1);
}
// exponent, mantissa, log2 of |v|; zero logs to negative infinity
float4 ilu_log(float v) {
    float mag = abs(v), l2 = log2(mag), ex = floor(l2);
    return mag == 0 ? float4(-1.#INF, 1, -1.#INF, 1) : float4(ex, mag / exp2(ex), l2, 1);
}
// power clamped to 128 less 1/256
float4 ilu_lit(float4 s) {
    const float cap = 127.99609375;
    float diffuse = max(s.x, 0);
    float spec = exp2(clamp(s.w, -cap, cap) * log2(max(s.y, 0)));
    return float4(1, diffuse, diffuse > 0 ? spec : 0, 1);
}
float fogfactor(float d) {
    if (fogp.w == 3) return (fogp.y - d) / (fogp.y - fogp.x);
    if (fogp.w == 1) return exp(-d * fogp.z);
    if (fogp.w == 2) return exp(-(d * fogp.z) * (d * fogp.z));
    return 1;
}
struct VIn {
    float4 v0 : ATTR0;
    float4 v3 : ATTR3;
    float4 v9 : ATTR9;
};
VOut main(VIn i) {

    int a0 = 0;
    // precise: EQUAL depth passes need bit exact positions
    precise float4 R0 = 0, R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0, R11 = 0;
    precise float4 oPos = 0;
    float4 oD0 = float4(0, 0, 0, 1), oD1 = 0, oB0 = float4(0, 0, 0, 1), oB1 = 0, oFog = 0, oPts = 1;
    float4 oT0 = float4(0, 0, 0, 1), oT1 = float4(0, 0, 0, 1), oT2 = float4(0, 0, 0, 1), oT3 = float4(0, 0, 0, 1);
    {
        precise float4 mr = ((i.v0).xyzw);
        R1.xyzw = mr.xyzw;
    }
    {
        precise float4 mr = mac_mul(((i.v0).xyzw), ((R0).xyzw));
        R2.xyzw = mr.xyzw;
    }
    {
        precise float4 mr = (((i.v0).xyzw) + ((c[100]).xyzw));
        R3.xyzw = mr.xyzw;
    }
    {
        precise float4 mr = (mac_mul(((i.v0).xyzw), ((R0).xyzw)) + ((c[100]).xyzw));
        R4.xyzw = mr.xyzw;
    }
    {
        precise float4 mr = dot(((i.v0).xyzw).xyz, ((R0).xyzw).xyz).xxxx;
        R5.xyzw = mr.xyzw;
    }
    {
        precise float4 mr = dot(float4(((i.v0).xyzw).xyz, 1), ((R0).xyzw)).xxxx;
        R6.xyzw = mr.xyzw;
    }
    {
        precise float4 mr = dot(((i.v0).xyzw), ((R0).xyzw)).xxxx;
        R7.xyzw = mr.xyzw;
    }
    {
        precise float4 mr = float4(1, ((i.v0).xyzw).y * ((R0).xyzw).y, ((i.v0).xyzw).z, ((R0).xyzw).w);
        R8.xyzw = mr.xyzw;
    }
    {
        precise float4 mr = min(((i.v0).xyzw), ((R0).xyzw));
        R9.xyzw = mr.xyzw;
    }
    {
        precise float4 mr = max(((i.v0).xyzw), ((R0).xyzw));
        R10.xyzw = mr.xyzw;
    }
    {
        precise float4 mr = (float4)(((i.v0).xyzw) < ((R0).xyzw));
        R0.xyzw = mr.xyzw;
    }
    {
        precise float4 mr = (float4)(((i.v0).xyzw) >= ((R0).xyzw));
        R1.xyzw = mr.xyzw;
    }
    {
        precise float4 mr = ((R1).xyzw);
        oPos.xyzw = mr.xyzw;
    }

    VOut o;
    precise float4 p = oPos;
    p.w = magclamp(p.w);
    precise float4 q;
    // full screen passes drawn half a pixel up-left for the console's integer pixel centers stop half a pixel short
    // of the far edges on D3D12's, which upscaled leaves the last host columns and rows undrawn
    // surf.w set: a point, which stays put since pulled out it would leave the target
    if (surf.w == 0 && abs(p.x - (surf.x - 0.5)) < 1e-3) p.x = surf.x;
    if (surf.w == 0 && abs(p.y - (surf.y - 0.5)) < 1e-3) p.y = surf.y;
    q.x = p.x * 2 / surf.x - 1;
    q.y = 1 - p.y * 2 / surf.y;
    q.z = p.z * surf.z;
    q.xyz *= p.w;
    q.w = p.w;
    o.pos = q;
    o.d0 = saturate(oD0);
    o.d1 = saturate(oD1);
    o.b0 = saturate(oB0);
    o.b1 = saturate(oB1);
    o.tc0 = oT0;
    o.tc1 = oT1;
    o.tc2 = oT2;
    o.tc3 = oT3;
    o.fogamt = fogfactor(oFog.x);
    return o;
}
