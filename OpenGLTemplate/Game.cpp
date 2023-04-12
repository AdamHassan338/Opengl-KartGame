/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 

 version 6.0a 29/01/2019
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

 version 6.1a 13/02/2022 - Sorted out Release mode and a few small compiler warnings
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

*/
#define _CRT_SECURE_NO_WARNINGS

#include "game.h"
#include "stdio.h"
#include <iostream>

// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CatmullRom.h"
#include "MyObject.h"
#include "Cube.h"

#include "include/glm/gtx/string_cast.hpp"
#define GLM_ENABLE_EXPERIMENTAL

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_pFtFont = NULL;
	m_pKartMesh = NULL;
	m_stoneMesh = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_pCatmullRom = NULL;
	m_object = NULL;
	m_pCube = NULL;
	m_quad = NULL;
	m_pFBO = NULL;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;


}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pFtFont;
	delete m_pKartMesh;
	delete m_stoneMesh;
	delete m_planet;
	delete m_pAudio;
	delete m_pCatmullRom;
	delete m_object;
	delete m_pCube;
	delete m_quad;
	delete m_pFBO;
	//delete m_obstacles;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{

	m_t = 0;

	m_cameraRotation = 0;
	m_cameraMode = Game::Freecam;

	m_currentDistance = 0.0f;

	m_rockPositions.push_back(glm::vec3(200, 28, 180));
	m_rockPositions.push_back(glm::vec3(500, 30, 310));
	m_rockPositions.push_back(glm::vec3(280, 28, 100));
	m_rockPositions.push_back(glm::vec3(710, 37, 413));
	m_rockPositions.push_back(glm::vec3(670, 19, 600));
	m_rockPositions.push_back(glm::vec3(400, 50, 150));
	m_rockPositions.push_back(glm::vec3(500, 11, 800));




	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pPlanarTerrain = new CPlane;
	m_pFtFont = new CFreeTypeFont;
	m_pKartMesh = new COpenAssetImportMesh;
	m_stoneMesh = new COpenAssetImportMesh;
	m_planet = new COpenAssetImportMesh;
	m_pAudio = new CAudio;
	m_object = new MyObject;
	m_pCatmullRom = new CCatmullRom;
	m_pCube = new CCube;
	m_quad = new Quad;
	m_pFBO = new CFrameBufferObject;
	//m_obstacle = new Obstacle;
	//m_pCatmullRom->CreatePath(p0,p1,p2,p3);
	m_pCatmullRom->CreateCentreline();
	m_pCatmullRom->CreateOffsetCurves(40.0f);
	m_pCatmullRom->CreateTrack("resources\\textures\\rainbow.png",50); // Downloaded from https://opengameart.org/content/seamless-rainbow-colors on 01 April 2023 Licence: CC0
	
	int offsets[] = {0 ,15,-4,8,-12,-1,8,-10,0,6,-8,-15,2,-4};


	for (int i = 0; i < m_pCatmullRom->getLength() - 300; i += 300) {
		m_obstacles.push_back(new Obstacle);
		//m_obstacles.at(m_obstacles.size() - 1)->Create("resources\\textures\\", "warning.jpg");
		glm::vec3 p;
		glm::vec3 pNext;
		float distance = i + 300;
		m_pCatmullRom->Sample(distance, p);
		m_pCatmullRom->Sample(distance + 1.0f, pNext);


		int delta = offsets[ i % ( sizeof(offsets) / sizeof(offsets[0]) ) ];

		m_obstacles.at(m_obstacles.size() - 1)->set(i+ 300, delta, m_pCatmullRom);
	}


	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	sShaderFileNames.push_back("myShader.frag");
	sShaderFileNames.push_back("myShader.vert");
	sShaderFileNames.push_back("hudShader.vert");
	sShaderFileNames.push_back("hudShader.frag");
	sShaderFileNames.push_back("stencilShader.vert");
	sShaderFileNames.push_back("stencilShader.frag");


	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram* pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);


	// Create a shader program for my object
	CShaderProgram* pMyProgram = new CShaderProgram;
	pMyProgram->CreateProgram();
	pMyProgram->AddShaderToProgram(&shShaders[4]);
	pMyProgram->AddShaderToProgram(&shShaders[5]);
	pMyProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMyProgram);



	// Create a shader program for my hud
	CShaderProgram* pHudProgam = new CShaderProgram;
	pHudProgam->CreateProgram();
	pHudProgam->AddShaderToProgram(&shShaders[6]);
	pHudProgam->AddShaderToProgram(&shShaders[7]);
	pHudProgam->LinkProgram();
	m_pShaderPrograms->push_back(pHudProgam);


	// Create a shader program for my outline
	CShaderProgram* pStencilProgam = new CShaderProgram;
	pStencilProgam->CreateProgram();
	pStencilProgam->AddShaderToProgram(&shShaders[8]);
	pStencilProgam->AddShaderToProgram(&shShaders[9]);
	pStencilProgam->LinkProgram();
	m_pShaderPrograms->push_back(pStencilProgam);

	// You can follow this pattern to load additional shaders

	// Create the skybox
	// Skybox downloaded from http://www.akimbo.in/forum/viewtopic.php?f=10&t=9
	m_pSkybox->Create(2500.0f);
	
	// Create the planar terrain
	m_pPlanarTerrain->Create("resources\\textures\\", "grassfloor01.jpg", 2000.0f, 2000.0f, 50.0f); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013

	m_pFtFont->LoadSystemFont("arial.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pKartMesh->Load("resources\\models\\kart\\kart3.obj"); // Downloaded from https://opengameart.org/content/racing-kart on 01 April 2023 Licence: CC0
	m_stoneMesh->Load("resources\\models\\Stone\\stone.obj"); // Downloaded from https://opengameart.org/content/stones on 06 April 2023 Licence: CC-BY 3.0
	m_planet->Load("resources\\models\\planet\\Planet.obj"); // Downloaded from https://opengameart.org/content/low-poly-planet-0 on 07 April 2023 Licence: CC0


	
	// Create a my object
	m_object->Create("resources\\textures\\", "warning.jpg"); // Downloaded from https://www.publicdomainpictures.net/en/view-image.php?image=483861 on 07 April 2023 Licence: CC0 

	m_pCube->Create("resources\\textures\\800px-Smiley.svg.png");

	//create hud quad
	m_quad->Create("resources\\textures\\heart.png"); // Downloaded from https://opengameart.org/content/heart-1 on 06 April 2023 Licence: CC0
	
	glEnable(GL_CULL_FACE);

	// Initialise audio and play background music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("resources\\Audio\\Boing.wav");					// Royalty free sound from freesound.org
	m_pAudio->LoadMusicStream("resources\\Audio\\DST-Garote.mp3");	// Royalty free music from http://www.nosoapradio.us/
	//m_pAudio->PlayMusicStream();


	m_pFBO->Create(width, height);
}



// Render method runs repeatedly in a loop
void Game::Render() 
{
	
	m_pFBO->Bind();
	RenderScene(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	RenderScene(1);
		
	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();

	//draw hud
	DrawHud();



	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());		

}

void Game::RenderMap()
{
}

void Game::RenderScene(int pass)
{


	// Clear the buffers and enable depth testing (z-buffering)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

	//


	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram* pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10;
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);

	
	// Set the projection matrix


	//switch between top down camera and game camera
	static std::vector<glm::vec3> cameraProps;

	if (pass == 0){
		cameraProps = m_pCamera->GetProps();
		m_pCamera->Set(glm::vec3(260, 880, 400), glm::vec3(259, 0, 399), glm::vec3(0, 0, -1));
		pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		
	}
	else {
		m_pCamera->Set(cameraProps.at(0), cameraProps.at(1), cameraProps.at(2));
		pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
		}
	
	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);


	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4(-100, 100, -100, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("worldLight.position", viewMatrix * lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("worldLight.La", glm::vec3(0.3));		// Ambient colour of light
	pMainProgram->SetUniform("worldLight.Ld", glm::vec3(.8f));		// Diffuse colour of light
	pMainProgram->SetUniform("worldLight.Ls", glm::vec3(1.0f));		// Specular colour of light
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(1.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property


	// Set light and materials in main shader program
	m_carLightpos; // Position of light source *in world coordinates*
	pMainProgram->SetUniform("carLight.position", viewMatrix * m_carLightpos); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("carLight.La", glm::vec3(0));		// Ambient colour of light
	pMainProgram->SetUniform("carLight.Ld", glm::vec3(10.0f));		// Diffuse colour of light
	pMainProgram->SetUniform("carLight.Ls", glm::vec3(5.0f));		// Specular colour of light
	pMainProgram->SetUniform("carLight.direction", glm::normalize(viewNormalMatrix * m_carLightDirection));		// Specular colour of light
	pMainProgram->SetUniform("carLight.exponent", 20.f);		// exponent of light
	pMainProgram->SetUniform("carLight.constant", 1.0f);
	pMainProgram->SetUniform("carLight.linear", 0.01f);
	pMainProgram->SetUniform("carLight.cutoff", 30.0f);		// Specular colour of light
	pMainProgram->SetUniform("carLight.attenuationExp;", 0.00001f);		// Specular colour of light




	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("renderSkybox", true);
	// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
	glm::vec3 vEye = m_pCamera->GetPosition();
	modelViewMatrixStack.Translate(vEye);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pSkybox->Render(cubeMapTextureUnit);
	pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	// Render the planar terrain
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	//m_pPlanarTerrain->Render();
	modelViewMatrixStack.Pop();


	// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance	





	// Render the cube

	glm::vec3 cubePos;
	m_pCatmullRom->Sample(m_pCatmullRom->getLength() - 10.0f, cubePos);
	cubePos.y += 7;
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(cubePos);
	modelViewMatrixStack.Scale(10.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
	//pMainProgram->SetUniform("bUseTexture", false);
	m_pCube->Render();
	modelViewMatrixStack.Pop();


	// Render the Kart
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(m_kartPos);
	modelViewMatrixStack *= m_kartRoation;
	modelViewMatrixStack.Scale(1.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
	pMainProgram->SetUniform("bUseTexture", true);
	m_pKartMesh->Render();
	modelViewMatrixStack.Pop();

	// RENDER THE SPLINE
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture", true); // turn off texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix",
		m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// Render your object here
		//m_pCatmullRom->RenderPath();
	//m_pCatmullRom->RenderCentreline();
	//m_pCatmullRom->RenderOffsetCurves();
	m_pCatmullRom->RenderTrack();
	modelViewMatrixStack.Pop();



	// Render the asteroids
	for (int i = 0; i < m_rockPositions.size(); i++) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(m_rockPositions[i]);
		modelViewMatrixStack.Scale(5.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
		pMainProgram->SetUniform("bUseTexture", true);
		m_stoneMesh->Render();
		modelViewMatrixStack.Pop();
	}

	//render the planet

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(320, 30, 500));
		modelViewMatrixStack.Scale(30);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
		pMainProgram->SetUniform("bUseTexture", true);
		m_planet->Render();
	modelViewMatrixStack.Pop();

	// Use the my shader program 
	pMainProgram = (*m_pShaderPrograms)[2];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("sampler0", 0);

	// Set light and materials in main shader program
	lightPosition1 = glm::vec4(-100, 100, -100, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("worldLight.position", viewMatrix * lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("worldLight.La", glm::vec3(0.3));		// Ambient colour of light
	pMainProgram->SetUniform("worldLight.Ld", glm::vec3(.8f));		// Diffuse colour of light
	pMainProgram->SetUniform("worldLight.Ls", glm::vec3(1.0f));		// Specular colour of light
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(1.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property


	// Set light and materials in main shader program
	m_carLightpos; // Position of light source *in world coordinates*
	pMainProgram->SetUniform("carLight.position", viewMatrix * m_carLightpos); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("carLight.La", glm::vec3(0));		// Ambient colour of light
	pMainProgram->SetUniform("carLight.Ld", glm::vec3(10.0f));		// Diffuse colour of light
	pMainProgram->SetUniform("carLight.Ls", glm::vec3(5.0f));		// Specular colour of light
	pMainProgram->SetUniform("carLight.direction", glm::normalize(viewNormalMatrix * m_carLightDirection));		// Specular colour of light
	pMainProgram->SetUniform("carLight.exponent", 20.f);		// exponent of light
	pMainProgram->SetUniform("carLight.constant", 1.0f);
	pMainProgram->SetUniform("carLight.linear", 0.01f);
	pMainProgram->SetUniform("carLight.cutoff", 30.0f);		// Specular colour of light
	pMainProgram->SetUniform("carLight.attenuationExp;", 0.00001f);		// Specular colour of light
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.

	//pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);


	// Set the projection matrix
	pMainProgram->SetUniform("projection", m_pCamera->GetPerspectiveProjectionMatrix());

	// Render the my Object
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(25.0f, 0.0f, 150.0f));
		modelViewMatrixStack.Scale(5.0f);
		pMainProgram->SetUniform("modelView", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
		//pMainProgram->SetUniform("bUseTexture", false);
		//m_object->Render();
	modelViewMatrixStack.Pop();


	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	// Render the my Obstacle
	for (Obstacle* o : m_obstacles) {
		modelViewMatrixStack.Push();

			modelViewMatrixStack.Translate(o->m_pos);
			modelViewMatrixStack *= o->m_rotation;
			modelViewMatrixStack.Scale(5.0f);

			pMainProgram->SetUniform("inverseViewMatrix", glm::inverse(m_pCamera->GetViewMatrix()));
			pMainProgram->SetUniform("modelView", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));

			m_pSkybox->m_cubemapTexture.Bind(1);
			m_object->Render();

		modelViewMatrixStack.Pop();
	}

	glStencilFunc(GL_NOTEQUAL, 1, 255);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);

	pMainProgram = (*m_pShaderPrograms)[4];
	pMainProgram->UseProgram();
	// Set the projection matrix
	pMainProgram->SetUniform("projection", m_pCamera->GetPerspectiveProjectionMatrix());


	// Render the my Obstacle with outline
	for (Obstacle* o : m_obstacles) {
		modelViewMatrixStack.Push();

			modelViewMatrixStack.Translate(o->m_pos.x, o->m_pos.y - 0.5, o->m_pos.z);
			modelViewMatrixStack *= o->m_rotation;
			modelViewMatrixStack.Scale(6.0f);

			pMainProgram->SetUniform("modelView", modelViewMatrixStack.Top());

			m_object->Render();

		modelViewMatrixStack.Pop();
	}

	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilMask(0xFF);
	glEnable(GL_DEPTH_TEST);


}

void Game::collide() {
	// radius collison
	for (std::vector<Obstacle*>::iterator it = m_obstacles.begin(); it != m_obstacles.end();) {
		
		if (glm::distance(m_kartPos, (*it)->m_pos) < 10.0f) {
			(*it)->~Obstacle();
			m_lives--;	
			it = m_obstacles.erase(it);
			printf("hit\n");
			if (m_lives <= 0)
				m_end = true;
			
		}
		else {
			it++;
		}
	}
	glm::vec3 p;
	m_pCatmullRom->Sample(m_pCatmullRom->getLength() - 10.0f, p);
	
	if (glm::distance(p, m_kartPos) < 10.0f) {
		m_end = true;
		m_won = true;
	}


}

// Update method runs repeatedly with the Render method
void Game::Update() 
{

	if (m_end == true) {
		return;
	}
	std::cout << glm::to_string(m_pCamera->GetPosition()) << "\n";

	collide();
							// TNB Frame
	m_currentDistance += m_dt * m_speed;
	glm::vec3 p;
	glm::vec3 pNext;
	m_pCatmullRom->Sample(m_currentDistance, p);
	m_pCatmullRom->Sample(m_currentDistance + 1.0f, pNext);

	glm::vec3 T = glm::normalize(pNext - p);

	glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0, 1, 0)));
	glm::vec3 B = glm::normalize(glm::cross(N, T));

	float w = 40.0f;//track width


	glm::vec3 l = p - (w / 2) * N;
	glm::vec3 r = p + (w / 2) * N;

						// Cart update
	

	m_kartPos = glm::vec3(p.x, p.y - 1, p.z);
	m_kartRoation = glm::mat4(glm::mat3(T, B, N));
	//glm::vec3 oldPos = m_kartPos;

	//kart controles
	if(m_cameraMode != Game::Freecam){
		if (m_moveRight) {
			m_kartOffset += m_dt * m_turnSpeed;
			if (m_kartOffset > 15.0f) {
				m_kartOffset = 15.0f;
			}
		}

		if (m_moveLeft) {
			m_kartOffset -= m_dt * m_turnSpeed;
			if (m_kartOffset < -15.0f) {
				m_kartOffset = -15.0f;
			}
		}
	}

	//move kart lateraly on track
	m_kartPos += N * m_kartOffset;

	//update kart light 
	
	m_carLightpos = glm::vec4(m_kartPos + (4.0f * T) + (8.0f * B),1);
	m_carLightDirection = T;

									//set camera

	if (m_cameraMode == Game::Freecam) {
		//Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
		m_pCamera->setUpVector(glm::vec3(0, 1, 0));
		m_pCamera->Update(m_dt);
	}
	if (m_cameraMode == Game::FirstPerson) {
		
		glm::vec3 newPos = m_kartPos + (10.0f * B) - (5.0f * T);
		m_pCamera->Set(newPos, m_kartPos  + (20.0f * T) + (4.0f * B), B);

		//glm::vec3 newPos = m_kartPos + (7.0f * B) - (4.0f * T);
		//m_pCamera->Set(newPos, m_kartPos + (20.0f * T) + (10.0f * B), B);
	}

	if (m_cameraMode == Game::ThirdPerson) {
		glm::vec3 newPos = m_kartPos + (20.0f * B) - (30.0f * T);
		m_pCamera->Set(newPos, m_kartPos + (20.0f * T), B);
	}

	if (m_cameraMode == Game::TopDown) {
		glm::vec3 newPos = m_kartPos + (200.0f * B);
		m_pCamera->Set(newPos, m_kartPos, T);
	}
	


	

	p.y += 5.0f;

	if (rotateRight) {
		m_cameraRotation += m_rotationSpeed * m_dt;
	}

	if (rotateLeft) {
		m_cameraRotation -= m_rotationSpeed * m_dt;
	}
	

	m_pAudio->Update();
	
}



void Game::DisplayFrameRate()
{


	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);
	}
}

void Game::DrawHud()
{

	CShaderProgram* pHudProgam = (*m_pShaderPrograms)[3];
	CShaderProgram* fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	int width = dimensions.right - dimensions.left;
	m_gameWindow.SCREEN_HEIGHT;

	glm::mat4 model = glm::mat4(1);
	glm::mat4 view = glm::mat4(1);
	
	






		// Use the font shader program
		pHudProgam->UseProgram();
		pHudProgam->SetUniform("sampler0", 0);

		glDisable(GL_DEPTH_TEST);


		glutil::MatrixStack modelMatrix;
		
		for (int i = 0; i < m_lives; i++) {
			modelMatrix.SetIdentity();
			modelMatrix.Scale(0.1f);
			modelMatrix.Translate(glm::vec3(8-i*2, -8, 0));
			pHudProgam->SetUniform("useTex", true);
			pHudProgam->SetUniform("model", modelMatrix.Top());
			m_quad->Render();
		}


		// draw minimap bg
		modelMatrix.SetIdentity();
		modelMatrix.Translate(glm::vec3(-0.7, -0.7, 0));
		modelMatrix.Scale(0.31);
		pHudProgam->SetUniform("model", modelMatrix.Top());
		pHudProgam->SetUniform("useTex", false);
		pHudProgam->SetUniform("colour", glm::vec3(1.0));
		m_pFBO->BindTexture(0);
		m_quad->RenderNoTexture();

		//draw the minimap
		modelMatrix.SetIdentity();
		modelMatrix.Translate(glm::vec3(-0.7, -0.7, 0));
		modelMatrix.Scale(0.3);
		pHudProgam->SetUniform("model", modelMatrix.Top());
		pHudProgam->SetUniform("useTex", true);
		m_pFBO->BindTexture(0);
		m_quad->RenderNoTexture();




		fontProgram->UseProgram();
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pFtFont->Render(700, height - 550, 20, "Lives");

		if (m_lives <= 0) {
			m_pFtFont->Render(width - width / 2 - 200, height - height / 2, 100, "You lost");
		}
		if (m_won) {
			m_pFtFont->Render(width - width / 2 - 200, height - height / 2, 100, "You won");
		}

		


}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	/*
	// Fixed timer
	dDt = pHighResolutionTimer->Elapsed();
	if (dDt > 1000.0 / (double) Game::FPS) {
		pHighResolutionTimer->Start();
		Update();
		Render();
	}
	*/

	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
	

}


WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '8':
			m_pAudio->PlayEventSound();
			break;
		case VK_F1:
			m_pAudio->PlayEventSound();
			break;
		case VK_LEFT:
			rotateLeft = true;
			break;
		case VK_RIGHT:
			rotateRight = true;
			break;

		case 'A':
			m_moveLeft = true;
			break;
		case 'D':
			m_moveRight = true;
			break;

		case '1':
			m_cameraMode = Game::Freecam;
			break;
		case '2':
			m_cameraMode = Game::FirstPerson;
			break;
		case '3':
			m_cameraMode = Game::ThirdPerson;
			break;
		case '4':
			m_cameraMode = Game::TopDown;
			break;
		}
		
		break;

	case WM_KEYUP:
		switch (w_param) {
		case VK_LEFT:
			rotateLeft = false;
			break;
		case VK_RIGHT:
			rotateRight = false;
			break;

		case 'A':
			m_moveLeft = false;
			break;
		case 'D':
			m_moveRight = false;
			break;
		
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	printf("Debugging Window:\n");
	return int(game.Execute());
}

