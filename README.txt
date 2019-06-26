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
Left Arrow	- Yaw(+)
Right Arrow	- Yaw(-)
Up Arrow		- Pitch(+)
Down Arrow	- Pitch(-)

+----------------------------------------------------------+
Helpful Features:
+----------------------------------------------------------+
Misc:
	F11 	- Toggle Fullscreen
	F1	- Toggle Infinite Perspective	{WT}
	
Camera Manipulation:
	+/=	- Camera Speed Increase		{WT}
	-/_	- Camera Speed Decrease		{WT}
	Alt+X	- Camera Zoom Increase		{WT}
	Alt+Z	- Camera Zoom Decrease		{WT}
	Shift+X	- Camera Nearplane Increase	{WT}
	Shift+Z	- Camera Nearplane Decrease	{WT}
	Ctrl+X	- Camera Farplane Increase	{WT}
	Ctrl+Z	- Camera Farplane Decrease	{WT}

Planet Fun:
	F2	- Toggle Planet Selection 	{WT}	{1}
	Ctrl+F	- Look-At Planet
	T	- Teleport to Planet			{1}
	
Light Fun:
	Numpad +	- Increase Point Light Attenuation
	Numpad -	- Decrease Point Light Attenuation
	
	Ctrl+(Numpad+)	- Increase Inner Cone's Attenuation
	Ctrl+(Numpad-)	- Decrease Inner Cone's Attenuation
	
	Shift+(Numpad+)	- Increase Outer Cone's Attenuation
	Shift+(Numpad-)	- Decrease Outer Cone's Attenuation
	
Key:
	{WT} = Window Title 
	{1} = Once upon RELEASE
	
+----------------------------------------------------------+
Milestone III Help
+----------------------------------------------------------+
Milestone 3 Help:
Lights demonstrate popular specular
	- Press "F2" Twice. This should switch planet view to "Earth"
	- Press "T" to teleport to earth.
	- This uses a specular map, The continents should not shine but the water does
	
Space Theme:
	- Skybox is Space
	- Looking at the Window Title, You can see "Mercury". Pressing F2 will toggle between Mercury to Pluto (And all other objects present!). 
	  Pressing (Ctrl+F) will allow you to look at the object selected, or (T) for Teleporting.
	- Two Spaceships inside the sun, and one going around the orbit
	- Earth Contains a moon.

+----------------------------------------------------------+
Milestone II Help
+----------------------------------------------------------+
Milestone 2 Help:
Applying functional per pixel spot light to complex mesh
	- The Color for the spot light is BLUE.
	- Hold Shift + Press (Numpad+/-) would adjust the outer cone’s attenuation
	- Hold Ctrl + Press (Numpad+/-) Would adjust the inner cone’s attenuation
	- Not Holding Shift/Ctrl will adjust the attenuation for Point Light (See Milestone I Help)
Demonstate dynamic change in direction of directional lighting
	- The Ships are adjusted based on the direction of the spotlight, rotating.
	- Nice reminder that the directional light is GREEN.
Demonstrate dynamic change in position and direction of spot lighting
	- The floor should show both the position and direction of spot light. The Ships adjust to it.
Normal Mapping (Full 3D Object)
	- By Pressing T, you should be able to teleport to mercury to see the normal mapping.
	- Mercury, Venus, Earth, Mars[Super Slightly] and Pluto have normal maps applied.
Full FPS Style flythrough camera
	- Z-Rolling should have been eliminated from previous comment.

	
+----------------------------------------------------------+
Milestone I Help
+----------------------------------------------------------+
Milestone 1 Help:

Applying functional directional light to complex mesh (4)
Applying functional point light to complex mesh (4)
Combining 2 functional lights on the same drawn geometry (2)
	- The Green light is the directional light.
	- The Red Light is the spot light
	- The Floor’s Normals is (0,1,0) and the lights should apply to the ships too.
	
Demonstrates dynamic change in position of point light. (1)
	- The Spotlight should be rotating around (0,0,0)
	
Applying applicable color map texturing to drawn geometry (4)
	- The pyramid and the ships should be textured on start.
	
Infinite Skybox (3)
	- The skybox is the blue-ish background with the white stars.
	
Unique Shader that modifies outgoing data based on position, time and a wave (3)
	- The Japanese Flag should demonstrate it. In “./shaders/GLSL” there should be a file 	called “vertFlag.vert” with the position, time sent, and sin as sine wave.

Use Obj2Header to generate a complex mesh (3)
	- The Ship in the middle should have the “HFILE” Mesh Drawn.

Drawing proceduraly created 3D line mesh (2)
	- The Grid should be seen on start

Drawing indexed model loaded from file (4)
	- The Ship on the right side is loaded from FBX loader. Also pressing “T” will 		   	teleport you to Mercury, which is also loaded from FBX Loader

MSAA applied to rasterized geometry (2)
	- It should be applied to all Geometry, as I force it to use the supported MSAA from 	device

Camera Position and aspect ratio are preserved when window is resized or toggled to fullscreen (2)
	- Press F11 to turn on Fullscreen

Manually Adjust Camera Zoom Level / Near- & Far- Planes (1 + 1)
	- Shift + Z/X Toggles Nearplane
	- Ctrl + Z/X Toggles Farplane*
	- Alt + Z/X Toggles Camera Zoom
	* Toggle Infinite Perspective Off with F1 (Far Plane != Inf)

Camera Tracks moving object using Look-At/Turn-To Algorithm (1)
	- Hold Left Ctrl+F and it should look at the planet selected on window title
