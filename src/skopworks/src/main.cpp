#include <tlocCore/tloc_core.h>
#include <tlocGraphics/tloc_graphics.h>
#include <tlocMath/tloc_math.h>
#include <tlocPrefab/tloc_prefab.h>
#include <tlocApplication/tloc_application.h>

#include <gameAssetsPath.h>


//namespace
using namespace tloc;


//shader paths
namespace 
{
    core_str::String shaderPathVS("/shaders/tlocOneTextureVS.glsl");
    core_str::String shaderPathFS("/shaders/tlocOneTextureFS.glsl");

	const core_str::String g_assetsPath(GetAssetsPath());
};

/////////////////////////////////////////////////////////////////////////
// lighting sample
class Program : public Application
{
public:
	Program() : Application("2LoC Engine") { }

private:
	//after calling the constructor
	error_type Post_Initialize() override
	{

	//get the path to the object file
		auto path = core_io::Path(core_str::String(GetAssetsPath()) + "/models/sphere.obj");
			//any place you want to pass a string or const char, use a BufferArg, converts to and from both.

	//open up the .obj file, and report error if necessary
		core_io::FileIO_ReadA objFile(path);
		if (objFile.Open() != ErrorSuccess)
		{
			TLOC_LOG_GFX_ERR() << "Could not open " << path;
			return ErrorFailure;
		}

	//get contents of the .obj file
		core_str::String objFileContents;
		objFile.GetContents(objFileContents);

	//try loading the object, and check for parsing errors
		gfx_med::ObjLoader objLoader;
		if (objLoader.Init(objFileContents) != ErrorSuccess)
		{
			TLOC_LOG_GFX_ERR() << "Failed to parse .obj file.";
			return ErrorFailure;
		}

	//unpack the vertices of the object
		gfx_med::ObjLoader::vert_cont_type vertices;
		objLoader.GetUnpacked(vertices, 0);

		return Application::Post_Initialize();
	}
};



//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// main method
int TLOC_MAIN(int , char *[])
{
	//declare and initialize the program
	Program program;
	program.Initialize(core_ds::MakeTuple(800, 600));

	//run the program
	program.Run();

	//on exit
	TLOC_LOG_CORE_INFO() << "Exiting normally";

	return 0;
}
