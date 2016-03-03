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
    core_str::String shaderPathVS("/shaders/tlocSimpleLightingVS_perFragment.glsl");
    core_str::String shaderPathFS("/shaders/tlocSimpleLightingFS_perFragment.glsl");

	const core_str::String g_assetsPath(GetAssetsPath());
};





/////////////////////////////////////////////////////////////////////////
// lighting sample

class Program : public Application
{
public:
	Program() : Application("lighting example") { }



private:

//vertices of the sphere
	gfx_med::ObjLoader::vert_cont_type vertices;

//position of the light
	gfx_gl::uniform_vso u_lightPosition;




	//after calling the constructor
	error_type Post_Initialize() override
	{

	//load the object.
		loadSphere();

	//get the scene from the application
		auto			  scene = GetScene();
						  scene->AddSystem<gfx_cs::MaterialSystem>();	//add material system
						  scene->AddSystem<gfx_cs::CameraSystem>();		//add camera
		auto meshSystem = scene->AddSystem<gfx_cs::MeshRenderSystem>();	//add mesh render system

		scene->

	//set renderer
		meshSystem->SetRenderer(GetRenderer());

		setLightPosition();

	//create mesh with the vertices, and create material with the shaders.
		auto meshEntity = scene->CreatePrefab<pref_gfx::Mesh>().Create(vertices);
						  scene->CreatePrefab<pref_gfx::Material>().AssetsPath(GetAssetsPath())
																   .AddUniform(u_lightPosition.get())
																   .Add(meshEntity, core_io::Path(shaderPathVS), core_io::Path(shaderPathFS));

	

	//set the camera
		meshSystem->SetCamera(cameraEntity);



		return Application::Post_Initialize();
	}




	core_io::Path getPathOfSphere()
	{
	//get the path to the object file
		return core_io::Path(core_str::String(GetAssetsPath()) + "/models/sphere-smooth.obj");
			//any place you want to pass a string or const char, use a BufferArg, converts to and from both.
	}




	void loadSphere()
	{
	//open up the .obj file, and report error if necessary
		core_io::FileIO_ReadA objFile(getPathOfSphere());
		if (objFile.Open() != ErrorSuccess)
		{
			TLOC_LOG_GFX_ERR() << "Could not open " << getPathOfSphere();
		}


	//get contents of the .obj file
		core_str::String objFileContents;
		objFile.GetContents(objFileContents);


	//try loading the object, and check for parsing errors
		gfx_med::ObjLoader objLoader;
		if (objLoader.Init(objFileContents) != ErrorSuccess)
		{
			TLOC_LOG_GFX_ERR() << "Failed to parse .obj file.";
		}


	//unpack the vertices of the object
		{ objLoader.GetUnpacked(vertices, 0); }
	}

	void setLightPosition()
	{
	//set the light's position
		u_lightPosition->SetName("u_lightPosition").SetValueAs(math_t::Vec3f32(1.0f, 1.0f, 3.0f));
	}

	void createCamera()
	{
		auto scene = GetScene();


		//add camera
		auto cameraEntity = scene->CreatePrefab<pref_gfx::Camera>()
			.Perspective(true)
			.Near(0.1f)
			.Far(100.0f)
			.VerticalFOV(math_t::Degree(60.0f))
			.Create(GetWindow()->GetDimensions());

		//change camera's position
		cameraEntity->GetComponent<math_cs::Transform>()->SetPosition(math_t::Vec3f32(0, 0, 5));
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
