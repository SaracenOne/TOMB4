$input a_position, a_color0, a_color1, a_texcoord0, a_texcoord1
$output v_color0, v_color1, v_texcoord0, v_texcoord1

#include "common.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position.xyz, a_position.w) );
	v_color0 = a_color0;
	v_color1 = a_color1;

	v_texcoord0 = vec2(a_texcoord0.x, a_texcoord0.y);
}