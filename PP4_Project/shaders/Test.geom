#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

void build_plane(vec4 position)
{  
	
    gl_Position = position + vec4(-100.0, 0.0, -100.0, 0.0);    // 1:bottom-left
    EmitVertex();   
    gl_Position = position + vec4( 100.0, 0.0, -100.0, 0.0);    // 2:bottom-right
    EmitVertex();
    gl_Position = position + vec4(-100.0, 0.0,  100.0, 0.0);    // 3:top-left
    EmitVertex();
    gl_Position = position + vec4( 100.0, 0.0, 100.0, 0.0);    // 4:top-right
    EmitVertex();
    EndPrimitive();
}

void main() {    
    build_plane(gl_in[0].gl_Position);
}  