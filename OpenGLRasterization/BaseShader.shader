#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 texCoord;

out vec2 v_Texcoord;
void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
   v_Texcoord = texCoord;
};

#shader fragment
#version 330 core

in vec2 v_Texcoord;
layout(location = 0)out vec4 color;
uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main()
{
	vec4 texColor = texture(u_Texture, v_Texcoord);
	color = texColor * u_Color;
};