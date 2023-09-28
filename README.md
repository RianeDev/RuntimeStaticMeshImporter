# RuntimeStaticMeshImporter
 Plugin for loading static mesh at runtime from fbx files.

 Select the UCG Directory (whatever folder within your project directory that you want to store player made fbx) and assign it in the "Get UGC Directories and FBX" node.

 Connect the output paths struct to the "Load FBX File" node. It will search the directory for any FBX objects and create data from the mesh within, then build mesh data.

 Plug FBX Data output (the array returned from "Load FBX File") into "Create PMC From Data" node. This will create a temporary procedural mesh. You can use this but I suggest converting to static mesh to save resources.

 Plug the output of 'Out Mesh' into "Populate Static Mesh From PMC", what your static mesh component as the target. It will build a new static mesh for that component.

 You can take the 'Textures' array from the STMI Object Data (from "Load FBX File") to assign the mesh material textures.

See the example image for blueprint example.


If you are having issues with packaging your build/the system complaining about the assimp dll being missing, look in your packaged project for a 'Plugins/RuntimeStaticMeshImporter' folder and follow it until you find the assimp-vc140-mt.dll. 
Copy this and paste it in Project/Binaries/Win64. 

The system works best with FBX files that have a single, solid mesh. If something looks weird when importing, it may be that there are too many meshes within the FBX, or your normals might be off. Make sure to check them.
Replication works fine for lower-poly objects, but with high-poly objects there is a hardcap on array limits. This could result in a client being kicked from the server if the array gets too big. Am working on this.


![BlueprintExample](https://github.com/RianeDev/RuntimeStaticMeshImporter/assets/42573189/e13f4074-e011-4267-8503-27ac17b1333a)


Example using a mini STL converted to FBX in Blender:

![1](https://github.com/RianeDev/RuntimeStaticMeshImporter/assets/42573189/b8c54f76-87b0-4e41-b4ec-51b18566d4fd)
