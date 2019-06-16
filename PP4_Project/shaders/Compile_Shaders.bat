@echo off
SET shdr=Shader
@echo on

C:\Users\Tiny\source\repos\PP4_Project\PP4_Project\vendor\1.1.101.0\Bin\glslangValidator.exe -V vert%shdr%.vert
C:\Users\Tiny\source\repos\PP4_Project\PP4_Project\vendor\1.1.101.0\Bin\glslangValidator.exe -V frag%shdr%.frag

@echo off
move ./vert.spv ./vert%shdr%.spv
move ./frag.spv ./frag%shdr%.spv
PAUSE