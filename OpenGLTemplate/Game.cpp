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
	m_pBarrelMesh = NULL;
	m_pHorseMesh = NULL;
	m_pFigherMesh = NULL;
	m_pKartMesh = NULL;
	m_stoneMesh = NULL;
	m_pSphere = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_pCatmullRom = NULL;
	m_object = NULL;
	m_pCube = NULL;
	m_quad = NULL;
	//m_obstacle = NULL;
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
	delete m_pBarrelMesh;
	delete m_pHorseMesh;
	delete m_pKartMesh;
	delete m_stoneMesh;
	delete m_pSphere;
	delete m_pAudio;
	delete m_pCatmullRom;
	delete m_object;
	delete m_pFigherMesh;
	delete m_pCube;
	delete m_quad;
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
	m_spaceShipPosition = glm::vec3(0);
	m_spaceShipOrientation = glm::mat4(1);
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
	m_pBarrelMesh = new COpenAssetImportMesh;
	m_pHorseMesh = new COpenAssetImportMesh;
	m_pFigherMesh = new COpenAssetImportMesh;
	m_pKartMesh = new COpenAssetImportMesh;
	m_stoneMesh = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_pAudio = new CAudio;
	m_object = new MyObject;
	m_pCatmullRom = new CCatmullRom;
	m_pCube = new CCube;
	m_quad = new Quad;
	//m_obstacle = new Obstacle;
	//m_pCatmullRom->CreatePath(p0,p1,p2,p3);
	m_pCatmullRom->CreateCentreline();
	m_pCatmullRom->CreateOffsetCurves(40.0f);
	m_pCatmullRom->CreateTrack("resources\\textures\\rainbow.png",50); // Downloaded from https://opengameart.org/content/seamless-rainbow-colorsz on 01 April 2023
	
	for (int i = 0; i < m_pCatmullRom->getLength() - 300; i += 300) {
		m_obstacles.push_back(new Obstacle);
		m_obstacles.at(m_obstacles.size() - 1)->Create("resources\\textures\\", "stonebrick.jpg");
		m_obstacles.at(m_obstacles.size() - 1)->set(i+ 300, m_pCatmullRom);
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



	// Create a shader program for my object
	CShaderProgram* pHudProgam = new CShaderProgram;
	pHudProgam->CreateProgram();
	pHudProgam->AddShaderToProgram(&shShaders[6]);
	pHudProgam->AddShaderToProgram(&shShaders[7]);
	pHudProgam->LinkProgram();
	m_pShaderPrograms->push_back(pHudProgam);

	// You can follow this pattern to load additional shaders

	// Create the skybox
	// Skybox downloaded from http://www.akimbo.in/forum/viewtopic.php?f=10&t=9
	m_pSkybox->Create(2500.0f);
	
	// Create the planar terrain
	m_pPlanarTerrain->Create("resources\\textures\\", "grassfloor01.jpg", 2000.0f, 2000.0f, 50.0f); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013

	m_pFtFont->LoadSystemFont("arial.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pBarrelMesh->Load("resources\\models\\Barrel\\Barrel02.obj");  // Downloaded from http://www.psionicgames.com/?page_id=24 on 24 Jan 2013
	m_pHorseMesh->Load("resources\\models\\Horse\\Horse2.obj");  // Downloaded from http://opengameart.org/content/horse-lowpoly on 24 Jan 2013
	m_pFigherMesh->Load("resources\\models\\Fighter\\fighter1.obj");
	m_pKartMesh->Load("resources\\models\\kart\\kart3.obj"); // Downloaded from https://opengameart.org/content/racing-kart on 01 April 2023
	m_stoneMesh->Load("resources\\models\\Stone\\stone.obj"); // Downloaded from https://opengameart.org/content/stones on 06 April 2023

	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "dirtpile01.jpg", 25, 25);  // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013
	
	// Create a my object
	m_object->Create("resources\\textures\\", "stonebrick.jpg");

	m_pCube->Create("resources\\textures\\800px-Smiley.svg.png");

	//create hud quad
	m_quad->Create("resources\\textures\\heart.png");
	
	glEnable(GL_CULL_FACE);

	// Initialise audio and play background music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("resources\\Audio\\Boing.wav");					// Royalty free sound from freesound.org
	m_pAudio->LoadMusicStream("resources\\Audio\\DST-Garote.mp3");	// Royalty free music from http://www.nosoapradio.us/
	//m_pAudio->PlayMusicStream();

}

// Render method runs repeatedly in a loop
void Game::Render() 
{
	
	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10; 
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	

	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);

	
	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4(-100, 100, -100, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light1.position", viewMatrix * lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light1.La", glm::vec3(0.5));		// Ambient colour of light
	pMainProgram->SetUniform("light1.Ld", glm::vec3(.8f));		// Diffuse colour of light
	pMainProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property


	// Set light and materials in main shader program
	m_spotLightpos; // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light2.position", viewMatrix * m_spotLightpos); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light2.La", glm::vec3(0,0,0));		// Ambient colour of light
	pMainProgram->SetUniform("light2.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pMainProgram->SetUniform("light2.Ls", glm::vec3(1.0f));		// Specular colour of light

		

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


	// Render the horse 
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
		//modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(2.5f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHorseMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(150.0f, 0.0f, 2.0f));
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 25.0f);
		modelViewMatrixStack.Scale(2.5f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHorseMesh->Render();
	modelViewMatrixStack.Pop();


	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(100.0f, 0.0f, 5.0f));
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 220.0f);
		modelViewMatrixStack.Scale(2.5f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHorseMesh->Render();
	modelViewMatrixStack.Pop();

	//Render the Figher
	modelViewMatrixStack.Push();
		//modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(90.f));
		modelViewMatrixStack.Translate(m_spaceShipPosition);
		modelViewMatrixStack *= m_spaceShipOrientation;
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pFigherMesh->Render();
	modelViewMatrixStack.Pop();




	
	// Render the barrel 
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(100.0f, 0.0f, 0.0f));
		modelViewMatrixStack.Scale(5.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarrelMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(110.0f, 0.0f, 0.0f));
		modelViewMatrixStack.Scale(2.5f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarrelMesh->Render();
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(90.0f, 0.0f, 0.0f));
		modelViewMatrixStack.Scale(10.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarrelMesh->Render();
	modelViewMatrixStack.Pop();

	// Render the cube
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0, 2, 0));
	modelViewMatrixStack.Scale(2.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
	//pMainProgram->SetUniform("bUseTexture", false);
	m_pCube->Render();
	modelViewMatrixStack.Pop();



	

	// Render the sphere
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(0.0f, 6.0f, 150.0f));
		modelViewMatrixStack.Scale(6.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
		//pMainProgram->SetUniform("bUseTexture", false);
		//m_pSphere->Render();
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
	for(int i= 0; i< m_rockPositions.size(); i++){
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

	// Use the my shader program 
	pMainProgram = (*m_pShaderPrograms)[2];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
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
	m_object->Render();
	modelViewMatrixStack.Pop();

	// Render the my Obstacle
	for (Obstacle* o : m_obstacles) {
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(o->m_pos);
		modelViewMatrixStack *= o->m_rotation;
		modelViewMatrixStack.Scale(5.0f);
		pMainProgram->SetUniform("modelView", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
		//pMainProgram->SetUniform("bUseTexture", false);
		o->Render();
		modelViewMatrixStack.Pop();
	}
	






		
	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();

	//draw hud
	DrawHud();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());		

}

void Game::collide() {
	for (std::vector<Obstacle*>::iterator it = m_obstacles.begin(); it != m_obstacles.end();) {
		
		if (glm::distance(m_kartPos, (*it)->m_pos) < 10.0f) {
			(*it)->~Obstacle();
			m_lives--;	
			it = m_obstacles.erase(it);
			printf("hit\n");
			
		}
		else {
			it++;
		}
	}
}

// Update method runs repeatedly with the Render method
void Game::Update() 
{
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

	float w = 40.0f;


	glm::vec3 l = p - (w / 2) * N;
	glm::vec3 r = p + (w / 2) * N;

	// Cart update
	//glm::vec3 up = glm::rotate(glm::vec3(0, 1, 0), m_cameraRotation, T);

	m_kartPos = glm::vec3(p.x, p.y - 1, p.z);
	m_kartRoation = glm::mat4(glm::mat3(T, B, N));
	//glm::vec3 oldPos = m_kartPos;

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

	m_kartPos += N * m_kartOffset;
	//update light pos
	
	m_spotLightpos = glm::vec4(m_kartPos + (4.0f * T) + (6.0f * B),1);

	//set camera

	if (m_cameraMode == Game::Freecam) {
		m_pCamera->Update(m_dt);
	}
	if (m_cameraMode == Game::FirstPerson) {
		
		//glm::vec3 newPos =  p + (10.0f * B) - (5.0f * T);
		//m_pCamera->Set(newPos,  p + (20.0f * T) + (4.0f * B), B);

		glm::vec3 newPos = m_kartPos + (7.0f * B) - (4.0f * T);
		m_pCamera->Set(newPos, m_kartPos + (20.0f * T) + (10.0f * B), B);
	}

	if (m_cameraMode == Game::ThirdPerson) {
		glm::vec3 newPos = m_kartPos + (20.0f * B) - (30.0f * T);
		m_pCamera->Set(newPos, m_kartPos + (20.0f * T), B);
	}
	//Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
	

	

	p.y += 5.0f;

	if (rotateRight) {
		m_cameraRotation += m_rotationSpeed * m_dt;
	}

	if (rotateLeft) {
		m_cameraRotation -= m_rotationSpeed * m_dt;
	}
	

	
	//m_pCamera->Set(p, 10.0f* t + p , up);
	/*
	m_t += 0.001f * (float)m_dt;
	
	float radius = 75.0f;
	glm::vec3 x = glm::vec3(1, 0, 0);
	glm::vec3 y = glm::vec3(0, 1, 0);
	glm::vec3 z = glm::vec3(0, 0, 1);
	m_spaceShipPosition = radius * cos(m_t) * x + 50.0f * y + radius * sin(m_t) * z;

	
	//glm::vec3 T = glm::normalize(-radius * sin(m_t) * x + radius * cos(m_t) * z);
	//glm::vec3 N = glm::normalize(glm::cross(T, y));
	//glm::vec3 B = glm::normalize(glm::cross(N, T));

	m_spaceShipOrientation = glm::mat4(glm::mat3(T, B, N));
	*/
	/*
	int distance = 30;
	static double theta = 0;

	float x = 25;
	float z = 150;
	float y = 15;
	glm::vec3 position = { 25,15,150 };

	double angularS = 1.0;

	position.x += distance * sin(theta);
	position.z += distance * cos(theta);

	m_pCamera->Set(position, glm::vec3(25.0f, 0.0f, 150.0f),glm::vec3(0,1,0));
	theta += angularS * m_dt/(double)1000;
	*/

	m_pAudio->Update();
	

/*
static float t = 0.0f;
t += 0.0005f * (float)m_dt;
if (t > 1.0f)
	t = 0.0f;
glm::vec3 x = m_spline.Interpolate(p0, p1, p2, p3, t);
m_pCamera->Set(x, { 0,0,0 }, {0,1,0});
*/
//m_pCamera->Set(glm::vec3(0, 0, 0), { 100,0,0 }, { 0,1,0 });
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
			pHudProgam->SetUniform("model", modelMatrix.Top());
			m_quad->Render();
		}

		fontProgram->UseProgram();
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pFtFont->Render(700, height - 550, 20, "Lives");




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

