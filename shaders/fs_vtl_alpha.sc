$input v_color0, v_color1, v_texcoord0, v_texcoord1

#include "common.sh"

SAMPLER2D(s_texColor,  0);

void main()
{
	gl_FragColor = texture2D(s_texColor, v_texcoord0).rgba * vec4(v_color0.b, v_color0.g, v_color0.r, 1.0);
}
