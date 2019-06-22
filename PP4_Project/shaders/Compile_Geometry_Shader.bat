@echo off
SET shdr=Test
@echo on

C:\Users\Tiny\source\repos\PP4_Project\PP4_Project\vendor\1.1.101.0\Bin\glslangValidator.exe -V %shdr%.geom

@echo off
move ./geom.spv ./%shdr%.geom.spv
PAUSE