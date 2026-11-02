
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

[maxvertexcount(4)]
void main(point VOut v[1], inout TriangleStream<VOut> strip) {
    float2 half_pixel = v[0].pos.w / surf.xy;  // in clip units, the target spans 2 across surf.xy pixels
    // a target is read half a host pixel back (surf.w host pixels per console pixel): left and top reach that far
    float2 back = (0.5 - 1e-3) * 2 * half_pixel / surf.w;
    for (int i = 0; i < 4; i++) {
        VOut o = v[0];
        o.pos.x += (i & 1) ? half_pixel.x : -half_pixel.x - back.x;
        o.pos.y += (i & 2) ? -half_pixel.y : half_pixel.y + back.y;
        strip.Append(o);
    }
}
