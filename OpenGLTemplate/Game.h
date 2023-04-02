#pragma once

#include "Common.h"
#include "GameWindow.h"
#include "CatmullRom.h"

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CAudio;
class CCatmullRom;
class MyObject;
class CCube;

class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render();

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CPlane *m_pPlanarTerrain;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh *m_pBarrelMesh;
	COpenAssetImportMesh *m_pHorseMesh;
	COpenAssetImportMesh* m_pFigherMesh;
	COpenAssetImportMesh* m_pKartMesh;
	CSphere *m_pSphere;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CAudio *m_pAudio;
	CCatmullRom* m_pCatmullRom;
	MyObject* m_object;
	CCube* m_pCube;

	// Some other member variables
	double m_dt;
	double m_time = 0;
	int m_framesPerSecond;
	bool m_appActive;

	//spline for camera

	//CCatmullRom m_spline = CCatmullRom();
	glm::vec3 p0 = glm::vec3(-500, 10, -200);
	glm::vec3 p1 = glm::vec3(0, 10, -200);
	glm::vec3 p2 = glm::vec3(0, 10, 200);
	glm::vec3 p3 = glm::vec3(-500, 10, 200);


public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void GameLoop();
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;

	float m_t;
	glm::vec3 m_spaceShipPosition;
	glm::mat4 m_spaceShipOrientation;
	float m_cameraRotation;

	float m_currentDistance;
	float m_rotationSpeed = .001f;



	bool rotateLeft = false;
	bool rotateRight = false;


	//kart stuff
	glm::vec3 m_kartPos = glm::vec3(0);
	glm::mat4 m_kartRoation = glm::mat4(1);

	//camera stuff

};
