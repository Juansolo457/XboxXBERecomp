
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
    float4 v2 : ATTR2;
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
    float4 ip = float4(i.v0.xyz, 1);
    float4 cp = mul(ip, wvp);
    float3 vp = mul(ip, wv).xyz;
    float3 vn = mul(float4(i.v2.xyz, 0), wvn).xyz;
    vn = dot(vn, vn) > 0 ? normalize(vn) : vn;
    float4 md = mat[0], ms = mat[2], ma = mat[1], me = mat[3];
    float3 la = 0, ld = 0, ls = 0;
    float3 eye = float3(0, 0, 1);
    {
        float3 l;
        float att = 1;
        l = -normalize(lt[4].xyz);
        la += att * lt[2].rgb;
        float nl = dot(vn, l);
        ld += att * lt[0].rgb * max(nl, 0);
        if (nl > 0) ls += att * lt[1].rgb * pow(max(dot(vn, normalize(l + eye)), 0), mat[4].x);
    }
    {
        float3 l;
        float att = 1;
        float3 d = lt[10].xyz - vp;
        float dist = length(d);
        l = d / max(dist, 1e-6);
        att = dist > lt[10].w ? 0 : 1 / max(lt[12].x + lt[12].y * dist + lt[12].z * dist * dist, 1e-6);
        la += att * lt[9].rgb;
        float nl = dot(vn, l);
        ld += att * lt[7].rgb * max(nl, 0);
        if (nl > 0) ls += att * lt[8].rgb * pow(max(dot(vn, normalize(l + eye)), 0), mat[4].x);
    }
    {
        float3 l;
        float att = 1;
        float3 d = lt[17].xyz - vp;
        float dist = length(d);
        l = d / max(dist, 1e-6);
        att = dist > lt[17].w ? 0 : 1 / max(lt[19].x + lt[19].y * dist + lt[19].z * dist * dist, 1e-6);
        float rho = dot(-l, normalize(lt[18].xyz));
        att *= rho > lt[20].x ? 1 : rho <= lt[20].y ? 0 : pow(saturate((rho - lt[20].y) / (lt[20].x - lt[20].y)), lt[18].w);
        la += att * lt[16].rgb;
        float nl = dot(vn, l);
        ld += att * lt[14].rgb * max(nl, 0);
        if (nl > 0) ls += att * lt[15].rgb * pow(max(dot(vn, normalize(l + eye)), 0), mat[4].x);
    }
    oD0 = float4(me.rgb + ma.rgb * (amb.rgb + la) + md.rgb * ld, md.a);
    oD1 = float4(ms.rgb * ls, ms.a);
    oB0 = oD0;
    oB1 = oD1;
    {
        float4 tc = mul(float4(i.v9.xy, 1, 0), tm[0]);
        oT0 = float4(tc.xy, 0, 1);
    }
    oT1 = i.v9;
    oT2 = i.v9;
    oT3 = i.v9;
    oFog = float4(0,0,0,0);

    VOut o;
    // like the pretransformed ones, target sized passes end half a console pixel short of the far edges
    float2 sp = (float2(cp.x, -cp.y) / cp.w + 1) * 0.5 * surf.xy;
    if (surf.w == 0 && abs(sp.x - (surf.x - 0.5)) < 1e-3) cp.x = cp.w;
    if (surf.w == 0 && abs(sp.y - (surf.y - 0.5)) < 1e-3) cp.y = -cp.w;
    o.pos = cp;
    o.d0 = saturate(oD0);
    o.d1 = saturate(oD1);
    o.b0 = o.d0;
    o.b1 = o.d1;
    o.tc0 = oT0;
    o.tc1 = oT1;
    o.tc2 = oT2;
    o.tc3 = oT3;
    o.fogamt = fogfactor(abs(cp.z));
    return o;
}
