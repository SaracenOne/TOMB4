$input v_color0, v_texcoord0

#include "common.sh"

SAMPLER2D(s_texColor,  0);

void main()
{
	gl_FragColor = vec4(texture2D(s_texColor, v_texcoord0).rgb * vec4(v_color0.b, v_color0.g, v_color0.r, v_color0.a).rgb, 1.0);
}
