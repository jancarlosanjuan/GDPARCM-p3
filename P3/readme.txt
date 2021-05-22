Robert Nathan Roleda 11830379
The textures for the objects I got from the Gamedev_reusables package given to us from GDFUNDA, the skybox texture I got from learnOpenGL in their cubemaps tutorial. The models for the objects I made myself using Blender.
_____
Controls 
The input is simple WASD FPS movement with mouse movement for panning the camera and scrollwheel for zoom in or out to a limit. Escape closes the window as the cursor is locked and hidden. The speed uses delta time to make sure that it does not vary based on frames per second.
O and P swap the camera to orthographic and perspective respectively with perspective being the default state.
_____
Program flow
When the program begins, it starts with initializing gloabal variables and constants.

Then it creates the shader programs that will be used later on, sets the random number generator seed, and enables z-buffering.

After that the object initialization begins with 3 objects that will only have 1 instance and then the others that will have a set amount of instances scattered around an area and both are parameters in their creation functions. This is so that I can move the area or scale up as needed without having to do anything aside from tweaking the arguments passed in.

Textures are next to be loaded and their index in the rendering programs is set.

Next is setting up the depthMap to be used in creating shadows.

The skybox is last among the objects to be initialized with a cubemap for a texture.

Lastly the index of the map will be placed in the necessary rendering programs.


Next stage is the display which is on loop while the window is open which starts with clearing the previous frame.

After that is setting up the vision of the light source which will be through orthographic projection as it is a directional light so that it can create the shadowMap that will be used when rendering the scene. Followed by setting a viewport with a sufficient size to generate decently accurate shadows and then creating the shadowMap from there.

When that step is done the viewport is changed back to the size we need to render the scene with the skybox and clears the buffer bits from the previous step.

The uniform variables are set first before rendering the scene which will end up with shadows on render. The camera is a perspective type.

The lighting model used is Blinn-Phong given the usage of the halfway angle for the specular reflection.

With the current shader used in the rendering program, I am unable to assign a color value to them without causing the objects to be incapable of casting shadows for some reason even if the variable is not used in the fragment shader. But I need to be able to do that for the lake object as the texture I am using from the Gamedev_Reusables package given to us in GDFUNDA does not have a color which would result in a gray lake. To deal with this I created a separate shader program for the lake that uses colors and it does not matter if it cannot cast shadows as there is nothing for it to do that on.

The VAOs, VBOs, EBOs are in vectors so that they can be dynamically adjusted according to how many trees, grass patches I want without having to edit more than just the parameters I pass to their creation functions. The first index where each object type is found is set in their creation functions so it can be kept track of for generating the objects with the right textures applied with the dynamic lists.
