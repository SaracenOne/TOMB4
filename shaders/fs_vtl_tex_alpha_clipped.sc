$input v_color0, v_color1, v_texcoord0, v_texcoord1

#include "common.sh"

SAMPLER2D(s_texColor,  0);

void main()
{
    vec4 texColor_var = texture2D(s_texColor, v_texcoord0).rgba;

    // Vertex colour is swizzled.
    vec4 vCol0 = vec4(v_color0.b, v_color0.g, v_color0.r, 1.0);
    vec4 vCol1 = vec4(v_color1.b, v_color1.g, v_color1.r, 1.0);

    vec4 outCol = texColor_var * vCol0;

    // TODO: BGFX idiomatically uses u_alphaRef but we don't know how this is set. 
    float alphaRef = 0.5;
    outCol.a = (outCol.a - alphaRef) / max(fwidth(outCol.a), 0.0001) + alphaRef;
    if (outCol.a < 0.00) clip(-1);

    // Blend fog
    //outCol.rgb = lerp(outCol.rgb, vCol1.rgb, v_color1.a);
    gl_FragColor = outCol;
}