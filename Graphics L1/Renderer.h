#ifndef Renderer_h__
#define Renderer_h__

#include <gl\glew.h>
#include <gl\glfw3.h>
#include "glm/glm.hpp"
#include <memory>
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shader.hpp"
#include "Model.h"
#include "texture.h"
#include "FirstPersonCamera.h"
#include<time.h>
#include<algorithm>
#include<iostream>>
#include<windows.h>
#include<mmsystem.h>

using namespace glm;
using namespace std;

class Renderer
{
	struct CELL{
		mat4 ModelMatrix;
		float X_POS, Y_POS;
	};
	vector<CELL>Snake;

	enum RenderingMode
	{
		NO_TEXTURE,
		TEXTURE_ONLY,
		BLEND
	};

	GLuint AmbientLightID;
	GLuint LightPositionID;
	GLuint EyePositionID;
	vec3 ambientLight;
	vec3 lightPosition;

	GLuint mRenderingModeID;
	GLuint mNormalsBufferID;
	RenderingMode mRenderingMode;
	GLuint ProgramID;
	GLuint VertexBufferID;
	GLuint VertexArrayID;
	GLuint ViewProjectionMatrixID;
	GLuint ModelMatrixID;
	GLuint mUVDataBufferID;
	FirstPersonCamera* MyCamera;
	unique_ptr<Texture> mTexture1;

	Model* MySquare;
	Model* MySquare1;
	Model* MySquare2;
	Model* MySquare3;
	Model* MySquare4;
	Model* MySquare5;

	Model* Floor;

	mat4 ModelMatrix, FoodMat, WallMat1, WallMat2, WallMat3, WallMat4, S, R, T;

	bool eaten = false;
	float x_trans = -0.4;
	float y_trans = 0;
	float camX = +1;
	float camY = +1;
	float camZ = +0;
	float food_x = 0, food_y = -1;
	char Head_pos = 'D';
	char *R_aux;
	char *L_aux;
	double PreTime;
	int Score = 0;

public:
	Renderer();
	~Renderer();

	bool Colided = false;
	int GetScore();
	void Initialize(int WindowWidth, int WindowHeight);
	void Draw();
	void rand_food();
	char Move_aux(char*_aux, char curr);
	void moveSnake();
	bool eat(float snake_x, float food_x, float snake_y, float food_y);
	bool collide_with_wall(float head_x, float head_y);
	bool collide_with_tail();
	void HandleKeyboardInput(int Key, int Action);
	void HandleWindowResize(int WindowWidth, int WindowHeight);
	void Update();
	void Cleanup();

	void initPointLight();

};

#endif