$input v_color0, v_color1, v_texcoord0, v_texcoord1

#include "common.sh"

SAMPLER2D(s_texColor,  0);
uniform vec4 u_fogColor;

void main()
{
	vec4 texColor_var = texture2D(s_texColor, v_texcoord0).rgba;

	// Vertex colour is swizzled.
	vec4 vCol0 = vec4(v_color0.b, v_color0.g, v_color0.r, 1.0);
	vec4 vCol1 = vec4(v_color1.b, v_color1.g, v_color1.r, v_color1.a);

	vec4 outCol = texColor_var * vCol0;

	// Blend fog
	outCol.rgb = mix(outCol.rgb, u_fogColor.rgb, 1.0 - vCol1.a);
	gl_FragColor = outCol;
}
