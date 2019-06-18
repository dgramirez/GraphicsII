@echo off
SET shdr=NormalMapped
@echo on

C:\Users\Tiny\source\repos\PP4_Project\PP4_Project\vendor\1.1.101.0\Bin\glslangValidator.exe -V %shdr%.vert
C:\Users\Tiny\source\repos\PP4_Project\PP4_Project\vendor\1.1.101.0\Bin\glslangValidator.exe -V %shdr%.frag

@echo off
move ./vert.spv ./%shdr%.vert.spv
move ./frag.spv ./%shdr%.frag.spv
PAUSE