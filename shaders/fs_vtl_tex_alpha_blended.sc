$input v_color0, v_color1, v_texcoord0, v_texcoord1

#include "common.sh"

SAMPLER2D(s_texColor,  0);

void main()
{
    vec4 texColor_var = texture2D(s_texColor, v_texcoord0).rgba;

    // Vertex colour is swizzled.
    vec4 vCol0 = vec4(v_color0.b, v_color0.g, v_color0.r, v_color0.a);

    vec4 outCol = texColor_var * vCol0;

    gl_FragColor = outCol;
}