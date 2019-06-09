+----------------------------------------------------------+
Camera Movement:
+----------------------------------------------------------+
W	- Forward
A	- Left
S	- Backwards
D	- Right

Q	- Up
W	- Down

Camera Rotation:
Left Arrow		- Yaw(+)
Right Arrow		- Yaw(-)
Up Arrow		- Pitch(+)
Down Arrow	- Pitch(-)

Right Ctrl		- Roll(-)
Left Ctrl			- Roll(+)

+----------------------------------------------------------+
Helpful Features:
+----------------------------------------------------------+
Misc:
	F11 	- Toggle Fullscreen
	F1		- Toggle Infinite Perspective		{WT}
	
Camera Manipulation:
	+/=		- Camera Speed Increase		{WT}
	-/_		- Camera Speed Decrease		{WT}
	Alt+X		- Camera Zoom Increase		{WT}
	Alt+Z		- Camera Zoom Decrease		{WT}
	Shift+X	- Camera Nearplane Increase	{WT}
	Shift+Z	- Camera Nearplane Decrease	{WT}
	Ctrl+X	- Camera Farplane Increase	{WT}
	Ctrl+Z	- Camera Farplane Decrease	{WT}

Planet Fun:
	F2			- Toggle Planet Selection 		{WT} {1}
	Ctrl+F	- Look-At Planet
	T			- Teleport to Planet				{1}
	
Key:
	{WT} = Window Title 
	{1} = Once upon RELEASE
	
+----------------------------------------------------------+
Milestone I Help
+----------------------------------------------------------+
When started, you should see:
	- Applying Applicable Color Map
	- Infinite Skybox
	- Use Obj2Header to generate a mesh header from a .obj
	- Drawing Procedurally created 3D line mesh
	- Drawing Indexed Model from file
	
Applying functional directional light to complex mesh
	- Press T to teleport to a planet (Mercury is Default)
	
MSAA applied to rasterized geometry
	- It should be applied to all Geometry, as I force it to use the supported MSAA from device
	
Camera Position and aspect ratio are preserved when window is resized or toggled to fullscreen
	- Press F11 to turn on Fullscreen
	
Manually Adjust Camera Zoom Level / Near- & Far- Planes
	- Shift + Z/X Toggles Nearplane
	- Ctrl + Z/X Toggles Farplane*
	- Alt + Z/X Toggles Camera Zoom
	* Toggle Infinite Perspective Off with F1 (Far Plane != Inf)
	
Camera Tracks moving object using Look-At/Turn-To Algorithm
	- Hold Left Ctrl+F and it should look at the planet selected on window title
