#include <tlocCore/tloc_core.h>
#include <tlocGraphics/tloc_graphics.h>
#include <tlocMath/tloc_math.h>
#include <tlocPrefab/tloc_prefab.h>
#include <tlocApplication/tloc_application.h>

#include <gameAssetsPath.h>

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

//typedefs
	typedef ecs_ptr																			Scene;
	typedef core::smart_ptr::VirtualPtr<graphics::component_system::MeshRenderSystem>		MeshRenderSystem;
	typedef core::smart_ptr::VirtualPtr<core::component_system::Entity>						Entity;
	typedef gfx_cs::material_sptr 															Material;


//struct for a 3D object
	struct Object
	{
	private:
		Scene				scene;			//reference to the scene
		core_str::String	objectPath;		//the path to the obj file
		Entity				mesh;			//the actual object
		Material			material;		//the material of the object


		//movement variables
		float			mAngleX, mAngleY;	//variables for rotation
		float			mZoomFactor;		//variable  for zoom
		math_t::Vec3f32 mTranslation;		//variable  for translation

	public:
		//intialize and create the object
		Object(Scene sceneReference, core_str::String filePath, Material materialReference) :
			mAngleX(0.0f), mAngleY(0.0f), mZoomFactor(1.0f), mTranslation(0.0f, 0.0f, 0.0f) //set the transform
		{
			objectPath	= filePath;
			scene		= sceneReference;

			mesh	 = createMesh(objectPath);
			material = materialReference;

			scene->GetEntityManager()->InsertComponent(core_cs::EntityManager::Params(mesh, material));
		}

		//getters for the mesh and material
		Entity	 GetMesh()		{ return mesh;	   }
		Material GetMaterial()  { return material; }

		//setters for the object's transform
		void SetRotation(float angleX, float angleY) 
		{ 
			mAngleX -= angleX;
			mAngleY -= angleY;

			mesh->GetComponent<math_cs::Transform>()->SetOrientation(

			math::types::Matrix_T<f32, 3>( cos(mAngleY),  cos(mAngleX) + sin(mAngleX) * sin(mAngleY), sin(mAngleX) - cos(mAngleX) * sin(mAngleY),
									      -cos(mAngleY),  cos(mAngleX) - sin(mAngleX) * sin(mAngleY), sin(mAngleX) + cos(mAngleX) * sin(mAngleY),
										   sin(mAngleY),                -sin(mAngleX) * cos(mAngleY),                cos(mAngleX) * cos(mAngleY))
			);
		}
		void SetZoom(float scale)
		{
			mZoomFactor -= scale;
			if (mZoomFactor <= 0.0f) mZoomFactor = 0.01f; //check to make sure we're not negatively scaled.

			mesh->GetComponent<math_cs::Transform>()->SetScale(math_t::Vec3f32(mZoomFactor, mZoomFactor, mZoomFactor));
		}
		void SetTranslation(float xDirection, float yDirection)
		{
			mTranslation[0] += xDirection;
			mTranslation[1] -= yDirection;

			//bounds checking
			if (mTranslation[0] < -5.0f) mTranslation[0] = -5.0f;
			if (mTranslation[0] >  5.0f) mTranslation[0] = 5.0f;

			if (mTranslation[1] < -5.0f) mTranslation[1] = -5.0f;
			if (mTranslation[1] >  5.0f) mTranslation[1] = 5.0f;


			mesh->GetComponent<math_cs::Transform>()->SetPosition(math_t::Vec3f32(mTranslation[0], mTranslation[1], 0.0f));
		}

		//get the path of the given string
		core_io::Path getPath(core_str::String objectPath)
		{
			//get the path to the object file
			return core_io::Path(core_str::String(GetAssetsPath()) + objectPath);
			//any place you want to pass a string or const char, use a core_io::String (which is a BufferArg), converts to and from both.
		}
		//load the passed object
		gfx_med::ObjLoader::vert_cont_type loadObject(core_str::String object)
		{
			//open up the .obj file, and report error if necessary
			core_io::FileIO_ReadA objFile(getPath(object));
			if (objFile.Open() != ErrorSuccess)
			{
				TLOC_LOG_GFX_ERR() << "Could not open " << getPath(object);
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


			//vertices of the object
			gfx_med::ObjLoader::vert_cont_type vertices;

			//unpack the vertices of the object
			{ objLoader.GetUnpacked(vertices, 0); }


			return vertices;
		}

		//create mesh from given object path
		Entity createMesh(core_str::String object)
		{
			return scene->CreatePrefab<pref_gfx::Mesh>().Create(loadObject(objectPath));
		}
	};


//variables
	Scene				scene;		//the scene from the application
	MeshRenderSystem	meshSystem; //the render system

	Material defaultMaterial; //the default material with per-fragment lighting
	
	Object* sphere; //the sphere

	gfx_gl::uniform_vso lightPosition; //position of the light


	//input manager, keyboard, mouse
	input::input_mgr_b_ptr		mInputManager;
	input_hid::keyboard_b_vptr  mKeyboard;
	input_hid::mouse_b_vptr     mMouse;

	float mouseSensitivity = 100.0f; //value to hold how sensitive the mouse movement will relate to acting on the object.


//after calling the constructor
	error_type Post_Initialize() override
	{

	//load the scene
		loadScene();

	//create a default material and set the light position
		defaultMaterial = createMaterial(shaderPathVS, shaderPathFS);

	//initialize the sphere
		sphere = new Object(scene, "/models/sphere-smooth.obj", defaultMaterial);


		return Application::Post_Initialize();
	}

//load the scene
	void loadScene()
	{
		scene = GetScene();
					 scene->AddSystem<gfx_cs::MaterialSystem>();	//add material system
					 scene->AddSystem<gfx_cs::CameraSystem>();		//add camera
		meshSystem = scene->AddSystem<gfx_cs::MeshRenderSystem>();	//add mesh render system	


	//set renderer
		meshSystem->SetRenderer(GetRenderer());

	//create and set the camera
		meshSystem->SetCamera(createCamera(true, 0.1f, 100.0f, 90.0f, math_t::Vec3f32(0, 0, 15)));

	//set the light position
		setLightPosition(math_t::Vec3f32(1.0f, 1.0f, 3.0f));

	//set up the mouse and keyboard
		createInputDevices();
	}

//create a camera
	entity_ptr createCamera(bool isPerspectiveView, float nearPlane, float farPlane, float verticalFOV_degrees, math_t::Vec3f32 position)
	{
		entity_ptr cameraEntity = scene->CreatePrefab<pref_gfx::Camera>()
			.Perspective(isPerspectiveView)
			.Near(nearPlane)
			.Far(farPlane)
			.VerticalFOV(math_t::Degree(verticalFOV_degrees))
			.Create(GetWindow()->GetDimensions());

		//change camera's position
		cameraEntity->GetComponent<math_cs::Transform>()->SetPosition(position);

		return cameraEntity;
	}
//create material
	Material createMaterial(core_str::String vertexShader, core_str::String fragmentShader)
	{
		auto materialEntity = scene->CreatePrefab<pref_gfx::Material>()
			.AssetsPath(GetAssetsPath())
			.AddUniform(lightPosition.get())
			.Create(core_io::Path(vertexShader), core_io::Path(fragmentShader));

		return materialEntity->GetComponent<gfx_cs::Material>();
	}
//create the mouse and keyboard
	void createInputDevices()
	{
		//create the input manager, and initialize the keyboard and mouse.
		ParamList<core_t::Any> params;
		params.m_param1 = GetWindow()->GetWindowHandle();

		mInputManager	= core_sptr::MakeShared<input::InputManagerB>(params);
		mKeyboard		= mInputManager->CreateHID<input_hid::KeyboardB>();
		mMouse			= mInputManager->CreateHID<input_hid::MouseB>();

		//check if there is a mouse and keyboard attached.
		TLOC_LOG_CORE_WARN_IF(mKeyboard == nullptr) << "No keyboard detected";
		TLOC_LOG_CORE_WARN_IF(mMouse == nullptr)	<< "No mouse detected";
	}


// called each update
	void DoUpdate(sec_type) override
	{
		//check input from HIDs
		CheckInput();

		//if escape key is pressed, exit program
		if (mKeyboard && mKeyboard->IsKeyDown(input_hid::KeyboardEvent::escape)) { exit(0); }
	}
// receive update events, and change values accordingly
	void CheckInput()
	{
		//update input manager
		mInputManager->Update();

		//get current mouse state
		input_hid::MouseEvent currentMouseState = mMouse->GetState();

		//check to see if either of the control buttons are pressed
		if (mKeyboard && mKeyboard->IsKeyDown(input_hid::KeyboardEvent::left_control) ||
			mKeyboard && mKeyboard->IsKeyDown(input_hid::KeyboardEvent::right_control))
		{
			//left mouse button --- rotate
			if (mMouse && mMouse->IsButtonDown(input_hid::MouseEvent::left))
			{
				sphere->SetRotation(core_utils::CastNumber<tl_float>(currentMouseState.m_Y.m_rel) / mouseSensitivity, 
									core_utils::CastNumber<tl_float>(currentMouseState.m_X.m_rel) / mouseSensitivity);
			}

			//right mouse button --- zoom
			if (mMouse && mMouse->IsButtonDown(input_hid::MouseEvent::right))
			{
				sphere->SetZoom(core_utils::CastNumber<tl_float>(currentMouseState.m_Y.m_rel / mouseSensitivity));
			}

			//middle mouse button --- translate
			if (mMouse && mMouse->IsButtonDown(input_hid::MouseEvent::middle))
			{
				sphere->SetTranslation(core_utils::CastNumber<tl_float>(currentMouseState.m_X.m_rel) / mouseSensitivity,
									   core_utils::CastNumber<tl_float>(currentMouseState.m_Y.m_rel) / mouseSensitivity);
			}
		}
	}


//set the shader's light positions
	void setLightPosition(math_t::Vec3f32 position)
	{
		lightPosition->SetName("u_lightPosition").SetValueAs(position);
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