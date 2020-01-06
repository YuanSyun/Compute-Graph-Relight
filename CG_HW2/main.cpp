/*

	CG Homework2 - Shadow Mapping & Dissolve Effects

	Objective - learning Shadow Implmentation and Dissolve Effects

	Overview:

		1. Render the model with Shadow using shadow mapping

		2. Implement dissolve effect

	!!!IMPORTANT!!! 

	1. Make sure to change the window name to your student ID!
	2. You are allow to use glmDraw this time.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /*for function: offsetof */
#include <math.h>
#include <vector>
#include <string.h>
#include "../GL/glew.h"
#include "../GL/glut.h"
#include "../shader_lib/shader.h"
#include "glm/glm.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
extern "C"
{
	#include "glm_helper.h"
}

/*you may need to do something here
you may use the following struct type to perform your single VBO method,
or you can define/declare multiple VBOs for VAO method.
Please feel free to modify it*/
struct Vertex
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};
typedef struct Vertex Vertex;

//no need to modify the following function declarations and gloabal variables
void init(void);
void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void keyboardup(unsigned char key, int x, int y);
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void idle(void);
void draw_light_bulb(void);
void camera_light_ball_move();
GLuint loadTexture(char* name, GLfloat width, GLfloat height);
void print_mat4(char* s, float* m);
void render_scene(GLuint framebuffer_name, GLuint shadow_image, GLuint hightlight_image, glm::mat4 projection, glm::mat4 viewMatrix, glm::mat4 lightSpaceMatrix1, GLuint depth_texture1, glm::mat4 lightSpaceMatrix2, GLuint depth_texture2);
void draw_plane(GLuint shader_name);
void draw_cube(GLuint shader_name);
void blending(glm::mat4 projection, glm::mat4 matViewMatrix);
void render_depthmap(GLuint fbo, glm::mat4 lightSpaceMatrix);

namespace
{
	char *obj_file_dir = "../Resources/TestCube3.obj";
	char *main_tex_dir = "../Resources/Stone.ppm";
	char *plane_file_dir = "../Resources/plane.obj";

	char* tex00_dir = "../Resources/t00.ppm";
	char* tex01_dir = "../Resources/t01.ppm";
	char* tex10_dir = "../Resources/t10.ppm";
	char* tex11_dir = "../Resources/t11.ppm";
	
	GLfloat light_rad = 0.05; //radius of the light bulb
	float eyet = -29.470; //theta in degree
	float eyep = 107.700; //phi in degree
	bool mleft = false;
	bool mright = false;
	bool mmiddle = false;
	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	bool lforward = false;
	bool lbackward = false;
	bool lleft = false;
	bool lright = false;
	bool lup = false;
	bool ldown = false;
	bool bforward = false;
	bool bbackward = false;
	bool bleft = false;
	bool bright = false;
	bool bup = false;
	bool bdown = false;
	bool bx = false;
	bool by = false;
	bool bz = false;
	bool brx = false;
	bool bry = false;
	bool brz = false;

	int mousex = 0;
	int mousey = 0;
}

// You can modify the moving/rotating speed if it's too fast/slow for you
const float speed = 0.03; // camera / light / ball moving speed
const float rotation_speed = 0.05; // ball rotating speed

//you may need to use some of the following variables in your program 

// No need for model texture, 'cause glmModel() has already loaded it for you.
// To use the texture, check glmModel documentation.
GLuint mainTextureID; // TA has already loaded this texture for you

GLMmodel *model; // TA has already loaded the model for you(!but you still need to convert it to VBO(s)!)
GLMmodel *planeModel;
GLMmodel *subModel;

float eyex = 3.024;
float eyey = 2.304;
float eyez = 11.125;

GLfloat light_pos1[] = { 1.1, 3.5, 1.3 };
GLfloat light_pos2[] = { 1.1, 3.5, 1.3 };
GLfloat ball_pos[] = { 0.0, -3, 0.0 };
GLfloat ball_rot[] = { 0.0, 0.0, 0.0 };
GLfloat plane_pos[] = { 0.0, -5.0, 0.0 };
GLfloat plane_rot[] = { 0.0, 0.0, 0.0 };
GLfloat subModel_pos[] = { -2.295, -5.0, -2.0 };
GLfloat subModel_rot[] = { 0.0, 0.0, 0.0 };

// ball model data
GLuint ballVertexArrayID;
unsigned int ballVertexBufferId, ballUvBufferId, ballNormalBufferId;
std::vector<glm::vec3> ballVertices;
std::vector<glm::vec2> ballUvs;
std::vector<glm::vec3> ballNormals;

// plane model data
GLuint planeVertexArrayID;
unsigned int planeVertexBufferId, planeUvBufferId, planeNormalBufferId;
std::vector<glm::vec3> planeVertices;
std::vector<glm::vec2> planeUvs;
std::vector<glm::vec3> planeNormals;

// shader 
GLuint program;
GLuint program_depth;
GLuint blending_program;
const GLuint  SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

// window size
int screenWidth = 512;
int screenHeight = 512;

// depth texture
GLuint fbo_depth1, fbo_depth2;
GLuint depthTexture1, depthTexture2;
GLfloat now_model_matrix[16];
glm::mat4 ModelMatrix;

// dissolving effect
GLfloat dissolvingThreshold;
GLboolean enableDissolving;
GLint dissolvingEffects = 1;

// projective texture
int texture_width = 1340;
int texture_height = 1125;
GLuint texture00ID, texture01ID, texture10ID, texture11ID;
GLuint fbo_render1, fbo_redner2;
GLuint resultTex1, resultTex2;
float blending_time = 0.5;

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	
	// remember to replace "YourStudentID" with your own student ID
	glutCreateWindow("CG_FinalProject_Relight");
	glutReshapeWindow(512, 512);

	glewInit();

	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutMainLoop();

	glmDelete(model);
	return 0;
}

void init(void)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glEnable(GL_CULL_FACE);

	texture00ID = loadTexture(tex00_dir, texture_width, texture_height);
	texture01ID = loadTexture(tex01_dir, texture_width, texture_height);
	texture10ID = loadTexture(tex10_dir, texture_width, texture_height);
	texture11ID = loadTexture(tex11_dir, texture_width, texture_height);

	// create ball vao
	model = glmReadOBJ(obj_file_dir);
	glmUnitize(model);
	glmFacetNormals(model);
	glmVertexNormals(model, 90.0, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	print_model_info(model);

	for (int i = 0; i < (int)model->numtriangles; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			GLuint vindice = model->triangles[i].vindices[j];
			glm::vec3 position;
			position.x = model->vertices[vindice * 3 + 0];
			position.y = model->vertices[vindice * 3 + 1];
			position.z = model->vertices[vindice * 3 + 2];
			ballVertices.push_back(position);
		}
		for (int k = 0; k < 3; k++)
		{
			GLuint tindice = model->triangles[i].tindices[k];
			glm::vec2 uv;
			uv.x = model->texcoords[tindice * 2 + 0];
			uv.y = model->texcoords[tindice * 2 + 1];
			ballUvs.push_back(uv);
		}
		for (int l = 0; l < 3; l++)
		{
			GLuint nindices = model->triangles[i].nindices[l];
			glm::vec3 normal;
			normal.x = model->normals[nindices * 3 + 0];
			normal.y = model->normals[nindices * 3 + 1];
			normal.z = model->normals[nindices * 3 + 2];
			ballNormals.push_back(normal);
		}
	}
	printf("ball vertices: %d, uvs: %d, noramls: %d\n", ballVertices.size(), ballUvs.size(), ballNormals.size());
	glGenVertexArrays(1, &ballVertexArrayID);
	glBindVertexArray(ballVertexArrayID);
	glGenBuffers(1, &ballVertexBufferId);																	
	glBindBuffer(GL_ARRAY_BUFFER, ballVertexBufferId);												
	glBufferData(GL_ARRAY_BUFFER, ballVertices.size() * sizeof(glm::vec3), &ballVertices[0], GL_STATIC_DRAW);	
	glGenBuffers(1, &ballUvBufferId);										
	glBindBuffer(GL_ARRAY_BUFFER, ballUvBufferId);											
	glBufferData(GL_ARRAY_BUFFER, ballUvs.size() * sizeof(glm::vec2), &ballUvs[0], GL_STATIC_DRAW);				
	glGenBuffers(1, &ballNormalBufferId);																	
	glBindBuffer(GL_ARRAY_BUFFER, ballNormalBufferId);														
	glBufferData(GL_ARRAY_BUFFER, ballNormals.size() * sizeof(glm::vec3), &ballNormals[0], GL_STATIC_DRAW);

	// create plane vao
	planeModel = glmReadOBJ(plane_file_dir);
	glmFacetNormals(planeModel);
	glmVertexNormals(planeModel, 90.0, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	print_model_info(planeModel);	

	for (int i = 0; i < (int)planeModel->numtriangles; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			GLuint vindice = planeModel->triangles[i].vindices[j];
			glm::vec3 position;
			position.x = planeModel->vertices[vindice * 3 + 0];
			position.y = planeModel->vertices[vindice * 3 + 1];
			position.z = planeModel->vertices[vindice * 3 + 2];
			planeVertices.push_back(position);
		}
		for (int k = 0; k < 3; k++)
		{
			GLuint tindice = planeModel->triangles[i].tindices[k];
			glm::vec2 uv;
			uv.x = planeModel->texcoords[tindice * 2 + 0];
			uv.y = planeModel->texcoords[tindice * 2 + 1];
			planeUvs.push_back(uv);
		}
		for (int l = 0; l < 3; l++)
		{
			GLuint nindices = planeModel->triangles[i].nindices[l];
			glm::vec3 normal;
			normal.x = planeModel->normals[nindices * 3 + 0];
			normal.y = planeModel->normals[nindices * 3 + 1];
			normal.z = planeModel->normals[nindices * 3 + 2];
			planeNormals.push_back(normal);
		}
	}
	printf("plane vertices: %d, uvs: %d, noramls: %d\n", planeVertices.size(), planeUvs.size(), planeNormals.size());
	glGenVertexArrays(1, &planeVertexArrayID);
	glBindVertexArray(planeVertexArrayID);
	glGenBuffers(1, &planeVertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, planeVertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, planeVertices.size() * sizeof(glm::vec3), &planeVertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &planeUvBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, planeUvBufferId);
	glBufferData(GL_ARRAY_BUFFER, planeUvs.size() * sizeof(glm::vec2), &planeUvs[0], GL_STATIC_DRAW);
	glGenBuffers(1, &planeNormalBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, planeNormalBufferId);
	glBufferData(GL_ARRAY_BUFFER, planeNormals.size() * sizeof(glm::vec3), &planeNormals[0], GL_STATIC_DRAW);

	GLfloat shaowborderColor[] = { 1.0, 0.0, 0.0, 0.0 };

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	glGenTextures(1, &depthTexture1);
	glBindTexture(GL_TEXTURE_2D, depthTexture1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, shaowborderColor);

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	glGenTextures(1, &depthTexture2);
	glBindTexture(GL_TEXTURE_2D, depthTexture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, shaowborderColor);

	// create result 1's texture
	glGenTextures(1, &resultTex1);
	glBindTexture(GL_TEXTURE_2D, resultTex1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// create result 2's texture
	glGenTextures(1, &resultTex2);
	glBindTexture(GL_TEXTURE_2D, resultTex2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &fbo_depth1);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_depth1);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture1, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &fbo_depth2);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_depth2);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture2, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Set "renderedTexture" as our colour attachement #0
	glGenFramebuffers(1, &fbo_render1);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_render1);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultTex1, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Set "renderedTexture" as our colour attachement #0
	glGenFramebuffers(1, &fbo_redner2);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_redner2);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, resultTex2, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// APIs for creating shaders and creating shader programs have been done by TAs
	// following is an example for creating a shader program using given vertex shader and fragment shader
	GLuint vert_depth = createShader("Shaders/depth.vert", "vertex");
	GLuint frag_depth = createShader("Shaders/depth.frag", "fragment");
	program_depth = createProgram(vert_depth, frag_depth);
	printf("loaded depth map shader\n");
	
	GLuint vert = createShader("Shaders/shadow_mapping.vert", "vertex");
	GLuint frag = createShader("Shaders/shadow_mapping.frag", "fragment");
	program = createProgram(vert, frag);
	printf("loaded shadow map shader\n");

	GLuint vert_blending = createShader("Shaders/blending.vert", "vertex");
	GLuint frag_blending = createShader("Shaders/blending.frag", "fragment");
	blending_program = createProgram(vert_blending, frag_blending);
	printf("loaded blending shader\n");
}

void display(void)
{
	printf("eye pos: %f, %f, %f, %f, %f\n", eyex, eyey, eyez, eyet, eyep);
	
	/*------------------------------------------------------------------------------------------------*/
	glm::mat4 lightProjection, lightView1, lightView2;
	glm::mat4 lightSpaceMatrix1, lightSpaceMatrix2;
	GLfloat near_plane = 0.01f, far_plane = 50.0f;

	//取得燈光的projection matrix
	glMatrixMode(GL_PROJECTION);
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

	//取得燈光的view matrix
	glMatrixMode(GL_MODELVIEW);
	lightView1 = glm::lookAt(glm::vec3(light_pos1[0], light_pos1[1], light_pos1[2]), glm::vec3(ball_pos[0], ball_pos[1], ball_pos[2]), glm::vec3(0.0, 1.0, 0.0));
	lightView2 = glm::lookAt(glm::vec3(light_pos2[0], light_pos2[1], light_pos2[2]), glm::vec3(ball_pos[0], ball_pos[1], ball_pos[2]), glm::vec3(0.0, 1.0, 0.0));

	//傳入PV matrix
	lightSpaceMatrix1 = lightProjection * lightView1;
	lightSpaceMatrix2 = lightProjection * lightView2;

	render_depthmap(fbo_depth1, lightSpaceMatrix1);
	render_depthmap(fbo_depth2, lightSpaceMatrix2);

	/*------------------------------------------------------------------------------------------------*/

	//取得projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);

	//調整視野
	glPushMatrix();
		glLoadIdentity();
		gluLookAt(
			eyex, eyey,	eyez,
			eyex + cos(eyet * M_PI / 180) * cos(eyep * M_PI / 180),
			eyey + sin(eyet * M_PI / 180),
			eyez - cos(eyet * M_PI / 180) * sin(eyep * M_PI / 180),
			0.0, 1.0, 0.0
		);

		//取得view matrix
		GLfloat viewMatrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);
		glm::mat4 matViewMatrix = glm::mat4{
			viewMatrix[0], viewMatrix[1], viewMatrix[2], viewMatrix[3],
			viewMatrix[4], viewMatrix[5], viewMatrix[6], viewMatrix[7],
			viewMatrix[8], viewMatrix[9], viewMatrix[10], viewMatrix[11],
			viewMatrix[12], viewMatrix[13], viewMatrix[14], viewMatrix[15]
		};

	//調整視野結束
	glPopMatrix();

	/*------------------------------------------------------------------------------------------------*/
	
	render_scene(fbo_render1, texture00ID, texture10ID, projection, matViewMatrix, lightSpaceMatrix1, depthTexture1, lightSpaceMatrix2, depthTexture2);
	render_scene(fbo_redner2, texture01ID, texture11ID, projection, matViewMatrix, lightSpaceMatrix1, depthTexture1, lightSpaceMatrix2, depthTexture2);

	/*------------------------------------------------------------------------------------------------*/

	blending(projection, matViewMatrix);
	
	/*------------------------------------------------------------------------------------------------*/

	glutSwapBuffers();
	camera_light_ball_move();
}

void draw_plane(GLuint shader_name)
{
	//繪製平面
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(plane_pos[0], plane_pos[1], plane_pos[2]);
	glRotatef(plane_rot[0], 1, 0, 0);
	glRotatef(plane_rot[1], 0, 1, 0);
	glRotatef(plane_rot[2], 0, 0, 1);
	glColor3f(1, 1, 1);

	//取得平面model matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, now_model_matrix);
	ModelMatrix = glm::mat4(now_model_matrix[0], now_model_matrix[1], now_model_matrix[2], now_model_matrix[3],
		now_model_matrix[4], now_model_matrix[5], now_model_matrix[6], now_model_matrix[7],
		now_model_matrix[8], now_model_matrix[9], now_model_matrix[10], now_model_matrix[11],
		now_model_matrix[12], now_model_matrix[13], now_model_matrix[14], now_model_matrix[15]);
	glUniformMatrix4fv(glGetUniformLocation(shader_name, "model"), 1, GL_FALSE, &ModelMatrix[0][0]);

	//畫出圖形
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, planeVertexBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, planeNormalBufferId);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, planeUvBufferId);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_TRIANGLES, 0, planeVertices.size());
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	//繪製平面結束
	glPopMatrix();
}

void draw_cube(GLuint shader_name)
{
	//繪製球體
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]);
	glRotatef(ball_rot[0], 1, 0, 0);
	glRotatef(ball_rot[1], 0, 1, 0);
	glRotatef(ball_rot[2], 0, 0, 1);
	glScalef(2.0f, 2.0f, 2.0f);
	glColor3f(1, 1, 1);

	//設定球體model matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, now_model_matrix);// pass the model matrix
	ModelMatrix = glm::mat4(now_model_matrix[0], now_model_matrix[1], now_model_matrix[2], now_model_matrix[3],
		now_model_matrix[4], now_model_matrix[5], now_model_matrix[6], now_model_matrix[7],
		now_model_matrix[8], now_model_matrix[9], now_model_matrix[10], now_model_matrix[11],
		now_model_matrix[12], now_model_matrix[13], now_model_matrix[14], now_model_matrix[15]
	);
	glUniformMatrix4fv(glGetUniformLocation(shader_name, "model"), 1, GL_FALSE, &ModelMatrix[0][0]);

	//渲染球體
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, ballVertexBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, ballNormalBufferId);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, ballUvBufferId);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_TRIANGLES, 0, ballVertices.size());
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	//繪製球體結束
	glPopMatrix();
}

// please implement mode increase/decrease dissolve threshold in case '-' and case '=' (lowercase)
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
	{	// ESC
		break;
	}
	case '-': // increase dissolve threshold
	{
		// you may need to do somting here
		blending_time += 0.1f;
		if (blending_time > 1.0f) blending_time = 1.0f;
		break;
	}
	case '=': // decrease dissolve threshold
	{
		// you may need to do somting here
		blending_time -= 0.1f;
		if (blending_time < 0.0f) blending_time = 0.0f;
		break;
	}
	case 'd':
	{
		right = true;
		break;
	}
	case 'a':
	{
		left = true;
		break;
	}
	case 'w':
	{
		forward = true;
		break;
	}
	case 's':
	{
		backward = true;
		break;
	}
	case 'q':
	{
		up = true;
		break;
	}
	case 'e':
	{
		down = true;
		break;
	}
	case 't':
	{
		lforward = true;
		break;
	}
	case 'g':
	{
		lbackward = true;
		break;
	}
	case 'h':
	{
		blending_time -= 0.01f;
		printf("%f\n", blending_time);
		if (blending_time < 0.1f)
		{
			blending_time = 0.1f;
			break;
		}
		else
		{
			lright = true;
			printf("lright");
			break;
		}
	}
	case 'f':
	{
		printf("%f\n", blending_time);
		blending_time += 0.01f;
		if (blending_time > 0.9f)
		{
			blending_time = 0.9f;
			break;
		}
		else
		{
			lleft = true;
			break;
		}
	}
	case 'r':
	{
		lup = true;
		break;
	}
	case 'y':
	{
		ldown = true;
		break;
	}
	case 'i':
	{
		bforward = true;
		break;
	}
	case 'k':
	{
		bbackward = true;
		break;
	}
	case 'l':
	{
		bright = true;
		break;
	}
	case 'j':
	{
		bleft = true;
		break;
	}
	case 'u':
	{
		bup = true;
		break;
	}
	case 'o':
	{
		bdown = true;
		break;
	}
	case '7':
	{
		bx = true;
		break;
	}
	case '8':
	{
		by = true;
		break;
	}
	case '9':
	{
		bz = true;
		break;
	}
	case '4':
	{
		brx = true;
		break;
	}
	case '5':
	{
		bry = true;
		break;
	}
	case '6':
	{
		brz = true;
		break;
	}

	//special function key
	case 'z'://move light source to front of camera
	{
		light_pos1[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180);
		light_pos1[1] = eyey + sin(eyet*M_PI / 180);
		light_pos1[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180);
		break;
	}
	case 'x'://move ball to front of camera
	{
		ball_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) * 3;
		ball_pos[1] = eyey + sin(eyet*M_PI / 180) * 5;
		ball_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180) * 3;
		break;
	}
	case 'c'://reset all pose
	{
		light_pos1[0] = 1.1;
		light_pos1[1] = 3.5;
		light_pos1[2] = 1.3;
		ball_pos[0] = 0;
		ball_pos[1] = -3;
		ball_pos[2] = 0;
		ball_rot[0] = 0;
		ball_rot[1] = 0;
		ball_rot[2] = 0;
		eyex = 1.953;
		eyey = 0.499;
		eyez = 11.221;
		eyet = -5.59; //theta in degree
		eyep = 83.2; //phi in degree
		break;
	}
	case 'v':
	{
		dissolvingEffects = 1;
		break;
	}
	case 'b':
	{
		dissolvingEffects = 2;
		break;
	}
	default:
	{
		break;
	}
	}
}

//no need to modify the following functions
void reshape(int width, int height)
{
	screenWidth = width;
	screenHeight = height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.001f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void motion(int x, int y)
{
	if (mleft)
	{
		eyep -= (x-mousex)*0.1;
		eyet -= (y - mousey)*0.12;
		if (eyet > 89.9)
			eyet = 89.9;
		else if (eyet < -89.9)
			eyet = -89.9;
		if (eyep > 360)
			eyep -= 360;
		else if (eyep < 0)
			eyep += 360;
	}
	mousex = x;
	mousey = y;
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN && !mright && !mmiddle)
		{
			mleft = true;
			mousex = x;
			mousey = y;
		}
		else
			mleft = false;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mmiddle)
		{
			mright = true;
			mousex = x;
			mousey = y;
		}
		else
			mright = false;
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mright)
		{
			mmiddle = true;
			mousex = x;
			mousey = y;
		}
		else
			mmiddle = false;
	}
}

void camera_light_ball_move()
{
	GLfloat dx = 0, dy = 0, dz=0;
	if(left|| right || forward || backward || up || down)
	{ 
		if (left)
			dx = -speed;
		else if (right)
			dx = speed;
		if (forward)
			dy = speed;
		else if (backward)
			dy = -speed;
		eyex += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		eyey += dy*sin(eyet*M_PI / 180);
		eyez += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (up)
			eyey += speed;
		else if (down)
			eyey -= speed;
	}
	if(lleft || lright || lforward || lbackward || lup || ldown)
	{
		dx = 0;
		dy = 0;
		if (lleft)
			dx = -speed;
		else if (lright)
			dx = speed;
		if (lforward)
			dy = speed;
		else if (lbackward)
			dy = -speed;
		light_pos1[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		light_pos1[1] += dy*sin(eyet*M_PI / 180);
		light_pos1[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (lup)
			light_pos1[1] += speed;
		else if(ldown)
			light_pos1[1] -= speed;
	}
	if (bleft || bright || bforward || bbackward || bup || bdown)
	{
		dx = 0;
		dy = 0;
		if (bleft)
			dx = -speed;
		else if (bright)
			dx = speed;
		if (bforward)
			dy = speed;
		else if (bbackward)
			dy = -speed;
		ball_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		ball_pos[1] += dy*sin(eyet*M_PI / 180);
		ball_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (bup)
			ball_pos[1] += speed;
		else if (bdown)
			ball_pos[1] -= speed;
	}
	if(bx||by||bz || brx || bry || brz)
	{
		dx = 0;
		dy = 0;
		dz = 0;
		if (bx)
			dx = -rotation_speed;
		else if (brx)
			dx = rotation_speed;
		if (by)
			dy = rotation_speed;
		else if (bry)
			dy = -rotation_speed;
		if (bz)
			dz = rotation_speed;
		else if (brz)
			dz = -rotation_speed;
		ball_rot[0] += dx;
		ball_rot[1] += dy;
		ball_rot[2] += dz;
	}
}

void draw_light_bulb()
{
	GLUquadric *quad;
	quad = gluNewQuadric();
	glPushMatrix();
		glColor3f(0.4, 0.5, 0);
		glTranslatef(light_pos1[0], light_pos1[1], light_pos1[2]);
		gluSphere(quad, light_rad, 40, 20);
	glPopMatrix();
}

void keyboardup(unsigned char key, int x, int y)
{
	switch (key) {
	case 'd':
	{
		right =false;
		break;
	}
	case 'a':
	{
		left = false;
		break;
	}
	case 'w':
	{
		forward = false;
		break;
	}
	case 's':
	{
		backward = false;
		break;
	}
	case 'q':
	{
		up = false;
		break;
	}
	case 'e':
	{
		down = false;
		break;
	}
	case 't':
	{
		lforward = false;
		break;
	}
	case 'g':
	{
		lbackward = false;
		break;
	}
	case 'h':
	{
		lright = false;
		break;
	}
	case 'f':
	{
		lleft = false;
		break;
	}
	case 'r':
	{
		lup = false;
		break;
	}
	case 'y':
	{
		ldown = false;
		break;
	}
	case 'i':
	{
		bforward = false;
		break;
	}
	case 'k':
	{
		bbackward = false;
		break;
	}
	case 'l':
	{
		bright = false;
		break;
	}
	case 'j':
	{
		bleft = false;
		break;
	}
	case 'u':
	{
		bup = false;
		break;
	}
	case 'o':
	{
		bdown = false;
		break;
	}
	case '7':
	{
		bx = false;
		break;
	}
	case '8':
	{
		by = false;
		break;
	}
	case '9':
	{
		bz = false;
		break;
	}
	case '4':
	{
		brx = false;
		break;
	}
	case '5':
	{
		bry = false;
		break;
	}
	case '6':
	{
		brz = false;
		break;
	}

	default:
	{
		break;
	}
	}
}

void idle(void)
{
	subModel_rot[1] += 1;
	glutPostRedisplay();
}

GLuint loadTexture(char* name, GLfloat width, GLfloat height)
{
	return glmLoadTexture(name, false, true, true, true, &width, &height);
}

void print_mat4(char* s, float *m)
{
	printf("%s:\n[%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n %f %f %f %f]\n", s, m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);
}

void render_scene(GLuint framebuffer_name, GLuint shadow_image, GLuint hightlight_image, glm::mat4 projection, glm::mat4 viewMatrix, glm::mat4 lightSpaceMatrix1, GLuint depth_texture1, glm::mat4 lightSpaceMatrix2, GLuint depth_texture2)
{
	//回復視野範圍
	glViewport(0, 0, texture_width, texture_height);

	//啟動frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_name);

	//清除資料
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//使用shadow mapping shader
	glUseProgram(program);

	//繪製燈光位置
	glPushMatrix();
	//glMatrixMode(GL_MODELVIEW);
	glColor3f(1, 1, 1);
	draw_light_bulb();
	glPopMatrix();

	/*
		Setting projective texture shader
		As the projective space is [-1, 1], we translated it to [0, 1].
	*/
	glm::mat4 scaleTrans = glm::mat4(
		0.5f, 0, 0, 0,
		0, -0.5f, 0, 0,
		0, 0, 0.5f, 0,
		0.5f, 0.5f, 0.5f, 1
	);
	glm::mat4 projectiveTextureMatrix = scaleTrans * projection * viewMatrix;

	//傳入shader屬性
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix1"), 1, GL_FALSE, &lightSpaceMatrix1[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix2"), 1, GL_FALSE, &lightSpaceMatrix2[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "ProjectorMatrix"), 1, GL_FALSE, &projectiveTextureMatrix[0][0]);
	glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, &light_pos1[0]);
	GLfloat cameraPos[3] = { eyex, eyey, eyez };
	glUniform3fv(glGetUniformLocation(program, "viewPos"), 1, &cameraPos[0]);
	glUniform1f(glGetUniformLocation(program, "dissolvingThreshold"), dissolvingThreshold);

	//Loading textures for shader
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depth_texture1);
	glUniform1i(glGetUniformLocation(program, "shadowMap1"), 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depth_texture2);
	glUniform1i(glGetUniformLocation(program, "shadowMap2"), 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, shadow_image);
	glUniform1i(glGetUniformLocation(program, "texture0"), 3);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, hightlight_image);
	glUniform1i(glGetUniformLocation(program, "texture1"), 4);

	draw_plane(program);
	draw_cube(program);

	//製作frame buffer 結束
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//關閉shadow mapping shader
	glUseProgram(0);

	//清除資料
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//回復視野範圍
	glViewport(0, 0, screenWidth, screenHeight);
}

void blending(glm::mat4 projection, glm::mat4 matViewMatrix)
{
	//start blending the result
	glUseProgram(blending_program);

	//assigning the matrixs
	glm::mat4 blendingSampleScale = glm::mat4(
		0.5f, 0, 0, 0,
		0, 0.5f, 0, 0,
		0, 0, 0.5f, 0,
		0.5f, 0.5f, 0.5f, 1
	);
	glm::mat4 projectorMatrix = blendingSampleScale * projection * matViewMatrix;
	glUniformMatrix4fv(glGetUniformLocation(blending_program, "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(blending_program, "view"), 1, GL_FALSE, &matViewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(blending_program, "ProjectorMatrix"), 1, GL_FALSE, &projectorMatrix[0][0]);

	//assigning result 1's texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, resultTex1);
	glUniform1i(glGetUniformLocation(blending_program, "reusltTex1"), 0);

	//assigning result 2's texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, resultTex2);
	glUniform1i(glGetUniformLocation(blending_program, "reusltTex2"), 1);

	//assigining the time
	glUniform1f(glGetUniformLocation(blending_program, "time"), blending_time);

	draw_plane(blending_program);
	draw_plane(blending_program);

	//finished the blending
	glUseProgram(0);
}

void render_depthmap(GLuint fbo, glm::mat4 lightSpaceMatrix)
{
	//設定渲染depth map視野
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	//啟動frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//清除資料
	glClear(GL_DEPTH_BUFFER_BIT);

	//使用深度shader
	glUseProgram(program_depth);

	glUniformMatrix4fv(glGetUniformLocation(program_depth, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);

	draw_cube(program_depth);

	//取消shader
	glUseProgram(0);

	//製作frame buffer 結束
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//回復視野範圍
	glViewport(0, 0, screenWidth, screenHeight);

	//清除資料
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}