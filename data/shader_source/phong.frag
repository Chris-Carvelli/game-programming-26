#version 430

layout(set = 3, binding = 0) uniform UBO_Frame {
	// camera
	mat4 camera_view;
	mat4 camera_projection;
	vec3 camera_position;

	// light
	vec4 light_direction; // w = 1 is positional, w = 0 is directional;
	vec4 light_color;
	vec4 light_ambient_color;

	// misc
	float time;

	// TMP material data
	
	float k_ambient;
	float k_diffuse;
	float k_specular;
	float exp_specular;
} data_frame;

layout(set = 2, binding = 0) uniform sampler2D   tex_albedo;

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {

	vec3 albedo = texture(tex_albedo, fragTexCoord).rgb;

	vec3 V = normalize(data_frame.camera_position - fragPosition);
	vec3 N = normalize(fragNormal);
	vec3 L = normalize(data_frame.light_direction.xyz);
	vec3 H = normalize(L + V);

	vec3 final_color =
		data_frame.k_ambient  * albedo * data_frame.light_ambient_color.rgb +
		data_frame.k_diffuse  * albedo * max(dot(N, L), 0.0) +
		data_frame.k_specular * pow(max(dot(N, H), 0.0), data_frame.exp_specular);

	final_color = clamp(final_color, 0.0, 1.0);

	outColor = vec4(final_color, 1.0);
	outColor = vec4(final_color, 1.0);
//	outColor = vec4(L, 1.0);
}
