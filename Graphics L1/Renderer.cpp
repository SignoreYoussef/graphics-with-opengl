#include "Renderer.h"

Renderer::Renderer()
{

}

Renderer::~Renderer()
{
	Cleanup();
}

int Renderer::GetScore(){
	return Score;
}

void Renderer::rand_food() {
	srand(time(NULL));
	FoodMat = translate(-food_x, -food_y, 0.0f) * FoodMat;
	while (true)
	{
		float temp_foodX = food_x;
		temp_foodX = rand() % 10 - 6;
		if (temp_foodX > -4 && temp_foodX < 3 && temp_foodX != food_x)
		{
			food_x = temp_foodX;
			break;
		}
	}
	while (true)
	{
		float temp_foodY = food_y;
		temp_foodY = rand() % 10 - 6;
		if (temp_foodY > -4 && temp_foodY < 3 && temp_foodY != food_y)
		{
			food_y = temp_foodY;
			break;
		}
	}
	FoodMat = translate(1 * food_x, 0 + food_y, 0.0f) * FoodMat;
	eaten = false;
}

bool Renderer::eat(float head_x, float food_x, float head_y, float food_y)
{
	float deltaX, deltaY;
	deltaX = abs(head_x - food_x);
	deltaY = abs(head_y - food_y);
	if (deltaX < 0.4 && deltaY < 0.4)
		return true;
	return false;
}

bool Renderer::collide_with_wall(float head_x, float head_y)
{
	float wall1X = 3.75, wall1Y = -1;
	float wall2X = -5.75, wall2Y = -1;
	float wall3X = -1, wall3Y = 3.75;
	float wall4X = -1, wall4Y = -5.75;
	float deltaX1 = abs(head_x - wall1X), deltaY1 = abs(head_y - wall1Y);
	float deltaX2 = abs(head_x - wall2X), deltaY2 = abs(head_y - wall2Y);
	float deltaX3 = abs(head_x - wall3X), deltaY3 = abs(head_y - wall3Y);
	float deltaX4 = abs(head_x - wall4X), deltaY4 = abs(head_y - wall4Y);
	if ((deltaX1 <= 0.4 && deltaY1 <= 4.65)
		|| (deltaX2 <= 0.4 && deltaY2 <= 4.65)
		|| (deltaY3 <= 0.4 && deltaX3 <= 4.65)
		|| (deltaY4 <= 0.4 && deltaX4 <= 4.65))
		return true;
	return false;
}

bool Renderer::collide_with_tail()
{
	float deltaX, deltaY;
	for (int i = 4; i < Snake.size(); i++)
	{
		deltaX = abs(Snake[0].X_POS - Snake[i].X_POS);
		deltaY = abs(Snake[0].Y_POS - Snake[i].Y_POS);
		if (deltaX < 0.4	&& deltaY < 0.4)
			return true;
	}
	return false;
}

void Renderer::moveSnake()
{
	if (x_trans == 0 && y_trans == 0) return;
	camX = Snake[2].X_POS + x_trans;
	camY = Snake[2].Y_POS + y_trans;
	MyCamera->SetLookAt(
		vec3(+1.0f, +4.0f, +3.0f),// Camera Position
		vec3(camX, camY, +1.0f),// Look at Point
		vec3(+0.0f, +0.0f, +1.0f) // Up Vector
	);
	for (int i = Snake.size() - 1; i > 0; i--)
	{
		Snake[i] = Snake[i - 1];
	}
	Snake[0].ModelMatrix = translate(x_trans, y_trans, 0.0f)*Snake[0].ModelMatrix;
	Snake[0].X_POS += x_trans;
	Snake[0].Y_POS += y_trans;
	//Eat Food
	if (eat(Snake[0].X_POS, food_x, Snake[0].Y_POS, food_y) && !eaten)
	{
		PlaySound("Untitled 1.wav", NULL, SND_ASYNC | SND_FILENAME);
		Score += eaten = true;
		CELL New_Tail;
		New_Tail.X_POS = Snake.back().X_POS + 0.4, New_Tail.Y_POS = Snake.back().Y_POS + 0;
		New_Tail.ModelMatrix = translate(0.4f, 0.0f, 0.0f)*Snake.back().ModelMatrix;
		Snake.push_back(New_Tail);
		rand_food();
	}
	//Hit Tail
	if (collide_with_tail())
	{
		PlaySound("Untitled 2.wav", NULL, SND_ASYNC | SND_FILENAME);
		system("CLS");
		system("Color 4F");
		cout << "\n\t\t\t\t\tYour Snake Is Dead and Your Game is Over\t\t" << endl;
		Colided = true;
		//exit(0);
	}
	//Hit Walls
	if (collide_with_wall(Snake[0].X_POS, Snake[0].Y_POS))
	{
		PlaySound("Untitled 2.wav", NULL, SND_ASYNC | SND_FILENAME);
		system("CLS");
		system("Color 4F");
		cout << "\n\t\t\t\t\tYou Hit The Wall and Your Game is Over\t\t" << endl;
		Colided = true;
		//exit(0);
	}
}

char Renderer::Move_aux(char* _aux, char curr) {
	for (int i = 0; i < 4; i++) {
		if (curr == _aux[i]) {
			return _aux[(i + 1) % 4];
		}
	}
}

void Renderer::initPointLight()
{
	//////////////////////////////////////////////////////////////////////////
	// Configure the light.
	//setup the light position.
	LightPositionID = glGetUniformLocation(ProgramID, "LightPosition_worldspace");
	lightPosition = vec3(+1.0f, +4.0f, +6.0f);
	glUniform3fv(LightPositionID, 1, &lightPosition[0]);
	//setup the ambient light component.
	AmbientLightID = glGetUniformLocation(ProgramID, "ambientLight");
	ambientLight = vec3(0.1, 0.1, 0.1);
	glUniform3fv(AmbientLightID, 1, &ambientLight[0]);
	//setup the eye position.
	EyePositionID = glGetUniformLocation(ProgramID, "EyePosition_worldspace");
	//////////////////////////////////////////////////////////////////////////
}

void Renderer::Initialize(int WindowWidth, int WindowHeight)
{
	R_aux = new char[4];
	L_aux = new char[4];
	R_aux[0] = 'U', R_aux[1] = 'R', R_aux[2] = 'D', R_aux[3] = 'L';
	L_aux[0] = 'U', L_aux[1] = 'L', L_aux[2] = 'D', L_aux[3] = 'R';

	PreTime = glfwGetTime();

	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


	//////////////////////////////////////////////////////////////////////////
	// Camera
	MyCamera = new FirstPersonCamera();
	// Projection matrix : 
	MyCamera->SetPerspectiveProjection(60.0f, (float)WindowWidth / (float)WindowHeight, 0.1f, 1000.0f);
	// View matrix : 
	MyCamera->SetLookAt(
		vec3(+1.0f, +4.0f, +8.0f),// Camera Position
		vec3(+1.0f, +0.0f, +0.0f),// Look at Point
		vec3(+0.0f, +0.0f, +1.0f) // Up Vector
		);
	glUniform3fv(EyePositionID, 1, &MyCamera->GetEyePosition()[0]);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Create and compile our GLSL program from the shaders

	ProgramID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
	mRenderingModeID = glGetUniformLocation(ProgramID, "RenderingMode");

	// Use our shader
	glUseProgram(ProgramID);
	// Get the ID of VP from shaders
	ViewProjectionMatrixID = glGetUniformLocation(ProgramID, "ViewProjectionMatrix");
	// Get the ID of ModelMatrix from shaders
	ModelMatrixID = glGetUniformLocation(ProgramID, "ModelMatrix");
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Initialization using the Ordinary Way
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLfloat VertexData[] = {
		//Head
		-1.0f, -1.0f, -1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 1, 0, 0,
		-1.0f, -1.0f, 1.0f, 0, 1, 0, 0.0f, 0.0f, 1, 0, 0,
		-1.0f, 1.0f, 1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 1, 0, 0,
		//
		1.0f, 1.0f, -1.0f, 0, 1, 0, 0.0f, 0.0f, 0, 0, -1,
		-1.0f, -1.0f, -1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 0, 0, -1,
		-1.0f, 1.0f, -1.0f, 0, 1, 0, 0.0f, 0.0f, 0, 0, -1,
		//
		1.0f, -1.0f, 1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, -1.0f, 0, 1, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, -1.0f, 0, 1, 0, 0.0f, 0.0f, 0, 0, 1,
		//
		1.0f, 1.0f, -1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 0, -1, 0,
		1.0f, -1.0f, -1.0f, 0, 1, 0, 0.0f, 0.0f, 0, -1, 0,
		-1.0f, -1.0f, -1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 0, -1, 0,
		//
		-1.0f, -1.0f, -1.0f, 0, 1, 0, 0.0f, 0.0f, 1, 0, 0,
		-1.0f, 1.0f, 1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 1, 0, 0,
		-1.0f, 1.0f, -1.0f, 0, 1, 0, 0.0f, 0.0f, 1, 0, 0,
		//
		1.0f, -1.0f, 1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, 1.0f, 0, 1, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, -1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		//
		-1.0f, 1.0f, 1.0f, 0, 1, 0, 0.0f, 0.0f, 0, -1, 0,
		-1.0f, -1.0f, 1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 0, -1, 0,
		1.0f, -1.0f, 1.0f, 0, 1, 0, 0.0f, 0.0f, 0, -1, 0,
		//
		1.0f, 1.0f, 1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, -1.0f, 0, 1, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, 1.0f, -1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,

		1.0f, -1.0f, -1.0f, 0, 1, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, 1.0f, 1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, 1.0f, 0, 1, 0, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, 1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		1.0f, 1.0f, -1.0f, 0, 1, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, -1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, 1.0f, 0, 1, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, -1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, 1.0f, 0, 1, 0, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, 1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, 1.0f, 0, 1, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, 1.0f, 0.5, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		//Tail
		-1.0f, -1.0f, -1.0f, 0.25, 0, 0.25, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, 1.0f, 0.25, 0, 0.25, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, 1.0f, 0.5, 0, 1, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, -1.0f, 0.25, 0, 0.25, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, -1.0f, 0.5, 0, 1, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, -1.0f, 0.5, 0, 1, 0.0f, 0.0f, 0, 0, 1,

		1.0f, -1.0f, 1.0f, 0.25, 0, 0.25, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, -1.0f, 1, 0.5, 0.75, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, -1.0f, 1, 0, 1, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, -1.0f, 0.5, 0, 1, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, -1.0f, 1, 0, 1, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, -1.0f, 1, 0.5, 0.75, 0.0f, 0.0f, 0, 0, 1,

		-1.0f, -1.0f, -1.0f, 1, 0, 1, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, 1.0f, 1, 0, 1, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, -1.0f, 1, 0.5, 0.75, 0.0f, 0.0f, 0, 0, 1,

		1.0f, -1.0f, 1.0f, 1, 0, 1, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, 1.0f, 0.5, 0, 0.25, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, -1.0f, 1, 0, 1, 0.0f, 0.0f, 0, 0, 1,

		-1.0f, 1.0f, 1.0f, 0.5, 0, 0.25, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, 1.0f, 1, 0, 1, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, 1.0f, 0.5, 0, 0.25, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, 1.0f, 1, 0.5, 0.75, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, -1.0f, 1, 0.5, 0.75, 0.0f, 0.0f, 0, 0, 1,
		1.0f, 1.0f, -1.0f, 0.5, 0, 0.25, 0.0f, 0.0f, 0, 0, 1,

		1.0f, -1.0f, -1.0f, 1, 0, 1, 0.0f, 0.0f, 0, 0, 1,
		1.0f, 1.0f, 1.0f, 1, 0.5, 0.75, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, 1.0f, 0.25, 0, 0.25, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, 1.0f, 0.5, 0, 0.25, 0.0f, 0.0f, 0, 0, 1,
		1.0f, 1.0f, -1.0f, 0.25, 0, 0.25, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, -1.0f, 1, 0.5, 0.75, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, 1.0f, 1, 0, 1, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, -1.0f, 1, 0.5, 0.75, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, 1.0f, 1, 0, 1, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, 1.0f, 1, 0, 1, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, 1.0f, 0.25, 0, 0.25, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, 1.0f, 0.25, 0, 0.25, 0.0f, 0.0f, 0, 0, 1,
		//Food
		-1.0f, -1.0f, -1.0f, 1, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, 1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, 1.0f, 1, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, -1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, -1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, -1.0f, 1, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,

		1.0f, -1.0f, 1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, -1.0f, 1, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, -1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, -1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, -1.0f, 1, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, -1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,

		-1.0f, -1.0f, -1.0f, 1, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, 1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, -1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,

		1.0f, -1.0f, 1.0f, 1, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, 1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, -1.0f, 1, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,

		-1.0f, 1.0f, 1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, 1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, 1.0f, 1, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, 1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, -1.0f, 1, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		1.0f, 1.0f, -1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,

		1.0f, -1.0f, -1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, 1.0f, 1.0f, 1, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, 1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, 1.0f, 1, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		1.0f, 1.0f, -1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, -1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, 1.0f, 1, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, -1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, 1.0f, 1, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, 1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, 1.0f, 0.8, 0.8, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, 1.0f, 1, 1, 0.5, 0.0f, 0.0f, 0, 0, 1,
		//Wall 
		-1.0f, -1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,

		1.0f, -1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,

		-1.0f, -1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,

		1.0f, -1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,

		-1.0f, 1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, -1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, 1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,

		1.0f, -1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, 1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, 1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, -1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,

		1.0f, 1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		-1.0f, 1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
		1.0f, -1.0f, 1.0f, 0.5, 0.25, 0, 0.0f, 0.0f, 0, 0, 1,
	};
	S = scale(0.2f, 0.2f, 0.2f);
	R = rotate(90.0f, vec3(1, 0, 0));
	CELL Tmp_aux;
	Tmp_aux.X_POS = Tmp_aux.Y_POS = 0;
	Tmp_aux.ModelMatrix = R * S;
	for (int i = 0; i < 3; i++)
	{
		Snake.push_back(Tmp_aux);
		Tmp_aux.ModelMatrix = translate(0.4f, 0.0f, 0.0f) * Tmp_aux.ModelMatrix;
		Tmp_aux.X_POS += 0.4;
	}
	T = translate(0.0f, -1.0f, 0.0f);
	S = scale(0.2f, 0.2f, 0.2f);
	R = rotate(90.0f, vec3(1, 0, 0));
	FoodMat = T*R*S;
	WallMat1 = translate(3.75f, -1.0f, 0.0f) * rotate(0.0f, vec3(1, 0, 0)) * scale(0.2f, 4.95f, 0.2f);
	WallMat2 = translate(-5.75f, -1.0f, 0.0f) * rotate(0.0f, vec3(1, 0, 0)) * scale(0.2f, 4.95f, 0.2f);
	WallMat3 = translate(-1.0f, 3.75f, 0.0f) * rotate(90.0f, vec3(0, 0, 1)) * scale(0.2f, 4.95f, 0.2f);
	WallMat4 = translate(-1.0f, -5.75f, 0.0f) * rotate(90.0f, vec3(0, 0, 1)) * scale(0.2f, 4.95f, 0.2f);

	glGenBuffers(1, &VertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData), VertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(03 * sizeof(GLfloat)));

	glGenBuffers(1, &mUVDataBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, mUVDataBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData), VertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2); //specify the right number 
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11, (void*)6);

	glGenBuffers(1, &mNormalsBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, mNormalsBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData), VertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11, (void*)8);

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Initialization using Model Class
	// Square consists of 4 vertices
	MySquare = new Model();
	// adding 4 positions
	MySquare->VertexData.push_back(vec3(-1, +1, 0));
	MySquare->VertexData.push_back(vec3(+1, +1, 0));
	MySquare->VertexData.push_back(vec3(+1, -1, 0));
	MySquare->VertexData.push_back(vec3(-1, -1, 0));
	// adding 4 colors
	MySquare->ColorData.push_back(vec3(+0.4, +0.9, +1));
	MySquare->ColorData.push_back(vec3(+0.5, +0.5, +1));
	MySquare->ColorData.push_back(vec3(+0.5, 0.5, +1));
	MySquare->ColorData.push_back(vec3(+0.4, +0.9, +1));

	MySquare->IndicesData.push_back(0);
	MySquare->IndicesData.push_back(1);
	MySquare->IndicesData.push_back(2);
	MySquare->IndicesData.push_back(0);
	MySquare->IndicesData.push_back(2);
	MySquare->IndicesData.push_back(3);
	// adding 4 normals
	MySquare->NormalsData.push_back(vec3(+0, +0, +1));
	MySquare->NormalsData.push_back(vec3(+0, +0, +1));
	MySquare->NormalsData.push_back(vec3(+0, +0, +1));
	MySquare->NormalsData.push_back(vec3(+0, +0, +1));
	// adding 6 indices


	MySquare->UVData.push_back(glm::vec2(0.0f, 0.0f));
	MySquare->UVData.push_back(glm::vec2(1.0f, 0.0f));
	MySquare->UVData.push_back(glm::vec2(0.0f, 1.0f));
	MySquare->UVData.push_back(glm::vec2(1.0f, 1.0f));
	// send data to GPU
	MySquare->Initialize();
	// Transformations
	S = scale(5.0f, 5.0f, 5.0f);
	T = translate(-1.0f, -1.0f, -1.0f);
	R = rotate(0.0f, vec3(0, 1, 0));
	MySquare->ModelMatrix = T * R * S;
	//Face2
	MySquare1 = new Model();
	MySquare1->VertexData = MySquare->VertexData;
	MySquare1->ColorData = MySquare->ColorData;
	MySquare1->IndicesData = MySquare->IndicesData;
	MySquare1->NormalsData.push_back(vec3(-1, +0, +0));
	MySquare1->NormalsData.push_back(vec3(-1, +0, +0));
	MySquare1->NormalsData.push_back(vec3(-1, +0, +0));
	MySquare1->NormalsData.push_back(vec3(-1, +0, +0));
	MySquare1->UVData = MySquare->UVData;
	S = scale(5.0f, 5.0f, 5.0f);
	T = translate(4.0f, -1.0f, 4.0f);
	R = rotate(90.0f, vec3(0, 1, 0));
	MySquare1->ModelMatrix = T* R * S;
	//Face3
	MySquare2 = new Model();
	MySquare->VertexData = MySquare1->VertexData;
	MySquare2->ColorData = MySquare1->ColorData;
	MySquare2->IndicesData = MySquare1->IndicesData;
	MySquare2->NormalsData.push_back(vec3(+0, +0, -1));
	MySquare2->NormalsData.push_back(vec3(+0, +0, -1));
	MySquare2->NormalsData.push_back(vec3(+0, +0, -1));
	MySquare2->NormalsData.push_back(vec3(+0, +0, -1));
	MySquare2->UVData = MySquare1->UVData;
	S = scale(5.0f, 5.0f, 5.0f);
	T = translate(-1.0f, -1.0f, 9.0f);
	R = rotate(0.0f, vec3(0, 1, 0));
	MySquare2->ModelMatrix = T* R * S;
	//Face4
	MySquare3 = new Model();
	MySquare3->VertexData = MySquare2->VertexData;
	MySquare3->ColorData = MySquare2->ColorData;
	MySquare3->IndicesData = MySquare2->IndicesData;
	MySquare3->NormalsData.push_back(vec3(+1, +0, +0));
	MySquare3->NormalsData.push_back(vec3(+1, +0, +0));
	MySquare3->NormalsData.push_back(vec3(+1, +0, +0));
	MySquare3->NormalsData.push_back(vec3(+1, +0, +0));
	MySquare3->UVData = MySquare2->UVData;
	S = scale(5.0f, 5.0f, 5.0f);
	T = translate(-6.0f, -1.0f, 4.0f);
	R = rotate(90.0f, vec3(0, 1, 0));
	MySquare3->ModelMatrix = T* R * S;
	//Face5
	MySquare4 = new Model();
	MySquare4->VertexData = MySquare3->VertexData;
	MySquare4->ColorData = MySquare3->ColorData;
	MySquare4->IndicesData = MySquare3->IndicesData;
	MySquare4->NormalsData.push_back(vec3(+0, +1, +0));
	MySquare4->NormalsData.push_back(vec3(+0, +1, +0));
	MySquare4->NormalsData.push_back(vec3(+0, +1, +0));
	MySquare4->NormalsData.push_back(vec3(+0, +1, +0));
	MySquare4->UVData = MySquare3->UVData;
	S = scale(5.0f, 5.0f, 5.0f);
	T = translate(-1.0f, -6.0f, 4.0f);
	R = rotate(90.0f, vec3(1, 0, 0));
	MySquare4->ModelMatrix = T * R * S;
	//Face6
	MySquare5 = new Model();
	MySquare5->VertexData = MySquare4->VertexData;
	MySquare5->ColorData = MySquare4->ColorData;
	MySquare5->IndicesData = MySquare4->IndicesData;
	MySquare5->NormalsData.push_back(vec3(+0, -1, +0));
	MySquare5->NormalsData.push_back(vec3(+0, -1, +0));
	MySquare5->NormalsData.push_back(vec3(+0, -1, +0));
	MySquare5->NormalsData.push_back(vec3(+0, -1, +0));
	MySquare5->UVData = MySquare4->UVData;
	S = scale(5.0f, 5.0f, 5.0f);
	T = translate(-1.0f, 4.0f, 4.0f);
	R = rotate(90.0f, vec3(1, 0, 0));
	MySquare5->ModelMatrix = T* R * S;

	//Floor
	Floor = new Model();
	// adding 4 positions
	Floor->VertexData.push_back(vec3(-1, +1, 0));
	Floor->VertexData.push_back(vec3(+1, +1, 0));
	Floor->VertexData.push_back(vec3(+1, -1, 0));
	Floor->VertexData.push_back(vec3(-1, -1, 0));
	// adding 4 colors
	Floor->ColorData.push_back(vec3(+1, +0.5, +0.4));
	Floor->ColorData.push_back(vec3(+1, +0.5, +0.4));
	Floor->ColorData.push_back(vec3(+1, +0.5, +0.4));
	Floor->ColorData.push_back(vec3(+1, +0.5, +0.4));
	// adding 4 normals
	Floor->NormalsData.push_back(vec3(+0, +0, +1));
	Floor->NormalsData.push_back(vec3(+0, +0, +1));
	Floor->NormalsData.push_back(vec3(+0, +0, +1));
	Floor->NormalsData.push_back(vec3(+0, +0, +1));
	// adding 6 indices
	Floor->IndicesData.push_back(0);
	Floor->IndicesData.push_back(1);
	Floor->IndicesData.push_back(2);
	Floor->IndicesData.push_back(0);
	Floor->IndicesData.push_back(2);
	Floor->IndicesData.push_back(3);
	Floor->UVData.push_back(vec2(0.0f, 0.0f));
	Floor->UVData.push_back(vec2(1.0f, 0.0f));
	Floor->UVData.push_back(vec2(0.0f, 1.0f));
	Floor->UVData.push_back(vec2(1.0f, 1.0f));
	// send data to GPU
	Floor->Initialize();
	// Transformations
	S = scale(5.0f, 5.0f, 5.0f);
	T = translate(-1.0f, -1.0f, -0.2f);
	R = rotate(0.0f, vec3(0, 1, 0));
	Floor->ModelMatrix = T * R * S;
	//////////////////////////////////////////////////////////////////////////

	mTexture1 = std::unique_ptr<Texture>(new Texture("4989a24b34b947bd7eff4d844b074103.jpg", 0));
}

void Renderer::HandleKeyboardInput(int Key, int Action)
{
	if (Action == GLFW_PRESS || Action == GLFW_REPEAT)
	{
		switch (Key)
		{
		case GLFW_KEY_UP:
			moveSnake();
			break;
		case GLFW_KEY_DOWN:
			//rand_food();
			break;
		case GLFW_KEY_RIGHT:
			if (Head_pos == 'U') x_trans = 0, y_trans = -0.4;
			else if (Head_pos == 'R')	x_trans = -0.4, y_trans = 0;
			else if (Head_pos == 'D')	x_trans = 0, y_trans = 0.4;
			else if (Head_pos == 'L') x_trans = 0.4, y_trans = 0;
			Head_pos = Move_aux(R_aux, Head_pos);
			break;
		case GLFW_KEY_LEFT:
			if (Head_pos == 'U') x_trans = 0, y_trans = 0.4;
			else if (Head_pos == 'L') x_trans = -0.4, y_trans = 0;
			else if (Head_pos == 'D')	x_trans = 0, y_trans = -0.4;
			else if (Head_pos == 'R')	x_trans = 0.4, y_trans = 0;
			Head_pos = Move_aux(L_aux, Head_pos);
			break;
		case GLFW_KEY_W:
			MyCamera->Walk(0.5);
			break;
		case GLFW_KEY_S:
			MyCamera->Walk(-0.5);
			break;
		case GLFW_KEY_D:
			MyCamera->Strafe(0.1);
			break;
		case GLFW_KEY_A:
			MyCamera->Strafe(-0.1);
			break;
		case GLFW_KEY_R:
			MyCamera->Fly(0.1);
			break;
		case GLFW_KEY_F:
			MyCamera->Fly(-0.1);
			break;
		case GLFW_KEY_I:
			MyCamera->Pitch(1);
			break;
		case GLFW_KEY_K:
			MyCamera->Pitch(-1);
			break;
		case GLFW_KEY_L:
			MyCamera->Yaw(-1);
			break;
		case GLFW_KEY_J:
			MyCamera->Yaw(1);
			break;
		case GLFW_KEY_O:
			MyCamera->Roll(-1);
			break;
		case GLFW_KEY_U:
			MyCamera->Roll(1);
			break;
		case GLFW_KEY_P:
			x_trans = y_trans = 0;
		default:
			break;
		}
	}
}

void Renderer::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(ProgramID);

	mRenderingMode = RenderingMode::NO_TEXTURE;
	glUniform1i(mRenderingModeID, mRenderingMode);
	glm::mat4 ViewProjectionMatrix = MyCamera->GetProjectionMatrix() * MyCamera->GetViewMatrix();

	glUniformMatrix4fv(ViewProjectionMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

	//////////////////////////////////////////////////////////////////////////
	// Drawing using the Ordinary Way
	glBindVertexArray(VertexArrayID);
	for (int i = 0; i < Snake.size(); i++)
	{
		ModelMatrix = Snake[i].ModelMatrix;
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		if (i == 0)
			glDrawArrays(GL_TRIANGLES, 0, 36);
		else
			glDrawArrays(GL_TRIANGLES, 36, 72);
	}
	mTexture1->Bind();
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &FoodMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 72, 108);
	mRenderingMode = RenderingMode::NO_TEXTURE;
	glUniform1i(mRenderingModeID, mRenderingMode);
	mTexture1->Bind();
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &WallMat1[0][0]);
	glDrawArrays(GL_TRIANGLES, 108, 143);
	mTexture1->Bind();
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &WallMat2[0][0]);
	glDrawArrays(GL_TRIANGLES, 108, 143);
	mTexture1->Bind();
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &WallMat3[0][0]);
	glDrawArrays(GL_TRIANGLES, 108, 143);
	mTexture1->Bind();
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &WallMat4[0][0]);
	glDrawArrays(GL_TRIANGLES, 108, 143);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Drawing using Model Class
	mTexture1->Bind();
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &MySquare->ModelMatrix[0][0]);
	MySquare->Draw();
	mTexture1->Bind();
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &MySquare1->ModelMatrix[0][0]);
	MySquare1->Draw();
	mTexture1->Bind();
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &MySquare2->ModelMatrix[0][0]);
	MySquare2->Draw();
	mTexture1->Bind();
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &MySquare3->ModelMatrix[0][0]);
	MySquare3->Draw();
	mTexture1->Bind();
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &MySquare4->ModelMatrix[0][0]);
	MySquare4->Draw();
	mTexture1->Bind();
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &MySquare5->ModelMatrix[0][0]);
	MySquare5->Draw();

	mRenderingMode = RenderingMode::TEXTURE_ONLY;
	glUniform1i(mRenderingModeID, mRenderingMode);
	mTexture1->Bind();
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Floor->ModelMatrix[0][0]);
	Floor->Draw();
	//////////////////////////////////////////////////////////////////////////
}

void Renderer::Update()
{
	if(x_trans != 0 || y_trans != 0) PlaySound("snakehit.wav", NULL, SND_ASYNC | SND_FILENAME);
	double CurrentTime = glfwGetTime();
	double DeltaTime = CurrentTime - PreTime;
	PreTime = CurrentTime;
	moveSnake();
}

void Renderer::HandleWindowResize(int WindowWidth, int WindowHeight)
{
	MyCamera->SetPerspectiveProjection(45.0f, (float)WindowWidth / (float)WindowHeight, 0.1f, 1000.0f);
}

void Renderer::Cleanup()
{
	glDeleteProgram(ProgramID);
	delete MyCamera;
}