FBX_DataExporter

This program was created by Derrick Ramirez to extract the following data into binary data:

1.) Indices		- Required
2.) Vertices	- Required
3.) Tangents	- Optional
4.) Binormals	- Optional
5.) UVs			- Optional

And it also creates color data based on the following:
R - Vertex#/Total Indices
G - 
B - 

[Structure of the binary data]
1.) unsigned int index_amount;
2.) unsigned int* indices;
3.) unsigned int vertex_amount;
4.) FDM_Vertex* vertices;

struct FMD_Vertex
{
	glm::vec4 position;
	glm::vec4 color;
	glm::vec4 normal;
	glm::vec4 tangent;
	glm::vec4 binormal;
	glm::vec2 uv;
};

And put it into a *fmd file (FBX Mesh Data)

+----------How to use---------+
It is used like a command prompt.
FBX_DataExporter.exe [Filename] [Scale]

FBX_DataExporter.exe is the name of the program
[Filename] is the FBX filename [eg. Pyramid.fbx]
[Scale] is the scale of the vertices you want. It is a multiplicitive scale, so 2 is making it 2 times bigger. (0.5) is cutting it in half.

If put correctly, and FBX contains at least indices and vertices, it will create a FMD (FBX Mesh Data) file for you to inport

+-------------FAQ-------------+
Q: Why did you make this program?
A: I was getting tired of compactifying every object on my project. So i pre-did it here.

Q: Once i have the FMD, how do i use it?
A: Look at the structure on the top of this README file. You'll need to do the following:
	- Read in the index amount
	- Create a new array