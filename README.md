# RuntimeStaticMeshImporter
 Plugin for loading static mesh at runtime from fbx files.

 Select the UCG Directory (whatever folder within your project directory that you want to store player made fbx) and assign it in the "Get UGC Directories and FBX" node.

 Connect the output paths struct to the "Load FBX File" node. It will search the directory for any FBX objects and create data from the mesh within, then build mesh data.

 Plug FBX Data output (the array returned from "Load FBX File") into "Create PMC From Data" node. This will create a temporary procedural mesh. You can use this but I suggest converting to static mesh to save resources.

 Plug the output of 'Out Mesh' into "Populate Static Mesh From PMC", what your static mesh component as the target. It will build a new static mesh for that component.

 You can take the 'Textures' array from the STMI Object Data (from "Load FBX File") to assign the mesh material textures.

See the example image for blueprint example.
