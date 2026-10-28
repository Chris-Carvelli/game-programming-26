#version 430

layout(set = 1, binding = 0) uniform UBO_Frame {
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

layout(location = 0) in vec3  inPosition;
layout(location = 1) in vec3  inNormal;
layout(location = 2) in vec2  inTexCoord;
layout(location = 3) in mat4  inGlobalTransform;

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;

void main()
{
	vec4 world_pos     = inGlobalTransform * vec4(inPosition, 1.0);
	vec4 world_normal  = inGlobalTransform * vec4(inNormal,   0.0);

	gl_Position = data_frame.camera_projection * data_frame.camera_view * world_pos;

	fragPosition = world_pos.xyz;
	fragNormal   = world_normal.xyz;
	fragTexCoord = inTexCoord;
}