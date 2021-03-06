/*
CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/

#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;
shared_ptr<Shape> skybox;

double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}
class EffectsManager
{
public:
    float inc, dec, total, space, cycle;
    
    EffectsManager() {
        inc = dec = space = 0;
        total = 1;
        cycle = -1;
    }
    
    float manageWaves(double ftime) {
        if(inc == 1) {
            total += .6*ftime;
        }
        else if(dec == 1) {
            if(total > 1) {
                total += -.6*ftime;
            }
        }
        return total;
    }
    
    float manageCycle() {
        if(space == 1) {
            cycle+=.01;
            
        }
        return (std::sin(cycle)+1)/2;
    }
};
EffectsManager ev;

class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d, up, down;
	camera()
	{
		w = a = s = d = up = down = 0;
		pos = rot = glm::vec3(0, -4, -50);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0;
		if (w == 1)
			speed = 10*ftime;
		else if (s == 1)
			speed = -10*ftime;
        
		float yangle=0;
		if (a == 1)
			yangle = -3*ftime;
		else if(d==1)
			yangle = 3*ftime;
        
        float xangle = 0;
        if(up == 1)
            xangle = -1*ftime;
        else if(down == 1)
            xangle = 1*ftime;
        
		rot.y += yangle;
        rot.x += xangle;
        glm::mat4 Rx = glm::rotate(glm::mat4(1), rot.x, glm::vec3(1, 0, 0));
		glm::mat4 Ry = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
        glm::mat4 R = Rx*Ry;
		glm::vec4 dir = glm::vec4(0, 0, speed,1);
		dir = dir*R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
        glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R*T;
	}
};

camera mycam;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> skyboxshader, heightshader;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint MeshPosID, MeshTexID, IndexBufferIDBox;

	//texture data
	GLuint Texture,HeightTex;
	GLuint Texture2,HeightTex2;
    GLuint Texture3,HeightTex3;
    GLuint Texture4,HeightTex4;
    GLuint TextureSky, TextureNight;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
        if(key == GLFW_KEY_UP && action == GLFW_PRESS)
        {
            mycam.up = 1;
        }
        if(key == GLFW_KEY_UP && action == GLFW_RELEASE)
        {
            mycam.up = 0;
        }
        if(key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        {
            mycam.down = 1;
        }
        if(key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
        {
            mycam.down = 0;
        }
        if(key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        {
            ev.dec = 1;
        }
        if(key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
        {
            ev.dec = 0;
        }
        if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        {
            ev.inc = 1;
        }
        if(key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
        {
            ev.inc = 0;
        }
        if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        {
            ev.space = 1;
        }
        if(key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
        {
            ev.space = 0;
        }
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;

			//change this to be the points converted to WORLD
			//THIS IS BROKEN< YOU GET TO FIX IT - yay!
			newPt[0] = 0;
			newPt[1] = 0;

			std::cout << "converted:" << newPt[0] << " " << newPt[1] << std::endl;
			glBindBuffer(GL_ARRAY_BUFFER, MeshPosID);
			//update the vertex array with the updated points
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*6, sizeof(float)*2, newPt);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}
    
#define MESHSIZE 300
	void init_mesh()
	{
		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &MeshPosID);
		glBindBuffer(GL_ARRAY_BUFFER, MeshPosID);
		vec3 vertices[MESHSIZE * MESHSIZE * 4];
		for(int x=0;x<MESHSIZE;x++)
			for (int z = 0; z < MESHSIZE; z++)
				{
				vertices[x * 4 + z*MESHSIZE * 4 + 0] = vec3(0.0, 0.0, 0.0) + vec3(x, 0, z);
				vertices[x * 4 + z*MESHSIZE * 4 + 1] = vec3(1.0, 0.0, 0.0) + vec3(x, 0, z);
				vertices[x * 4 + z*MESHSIZE * 4 + 2] = vec3(1.0, 0.0, 1.0) + vec3(x, 0, z);
				vertices[x * 4 + z*MESHSIZE * 4 + 3] = vec3(0.0, 0.0, 1.0) + vec3(x, 0, z);
				}
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * MESHSIZE * MESHSIZE * 4, vertices, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//tex coords
		float t = 1. / (float)MESHSIZE;
		vec2 tex[MESHSIZE * MESHSIZE * 4];
		for (int x = 0; x<MESHSIZE; x++)
			for (int y = 0; y < MESHSIZE; y++)
			{
				tex[x * 4 + y*MESHSIZE * 4 + 0] = vec2(0.0, 0.0)+ vec2(x, y)*t;
				tex[x * 4 + y*MESHSIZE * 4 + 1] = vec2(t, 0.0)+ vec2(x, y)*t;
				tex[x * 4 + y*MESHSIZE * 4 + 2] = vec2(t, t)+ vec2(x, y)*t;
				tex[x * 4 + y*MESHSIZE * 4 + 3] = vec2(0.0, t)+ vec2(x, y)*t;
			}
		glGenBuffers(1, &MeshTexID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, MeshTexID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * MESHSIZE * MESHSIZE * 4, tex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &IndexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		GLushort elements[MESHSIZE * MESHSIZE * 6];
		int ind = 0;
		for (int i = 0; i<MESHSIZE * MESHSIZE * 6; i+=6, ind+=4)
			{
			elements[i + 0] = ind + 0;
			elements[i + 1] = ind + 1;
			elements[i + 2] = ind + 2;
			elements[i + 3] = ind + 0;
			elements[i + 4] = ind + 2;
			elements[i + 5] = ind + 3;
			}			
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*MESHSIZE * MESHSIZE * 6, elements, GL_STATIC_DRAW);
		glBindVertexArray(0);
	}
	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
		//initialize the net mesh
		init_mesh();

		string resourceDirectory = "../../resources";
		int width, height, channels;
		char filepath[1000];
        
        // *************************************************
        // Skybox
        // *************************************************
        skybox = make_shared<Shape>();
        skybox->loadMesh(resourceDirectory + "/sphere.obj");
        skybox->resize();
        skybox->init();
        //texture skybox
        string str = resourceDirectory + "/sky.jpg";
        strcpy(filepath, str.c_str());
        unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &TextureSky);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureSky);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        GLuint SkyTexLocation = glGetUniformLocation(skyboxshader->pid, "skytex");
        
        str = resourceDirectory + "/sky_night.jpg";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &TextureNight);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, TextureNight);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        GLuint NightTexLocation = glGetUniformLocation(skyboxshader->pid, "nighttex");
        // Then bind the uniform samplers to texture units:
        glUseProgram(skyboxshader->pid);
        glUniform1i(SkyTexLocation, 0);
        glUniform1i(NightTexLocation, 1);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // ******************************
        //Heightshader
        // ******************************
        glUseProgram(heightshader->pid);
		//texture 1 normal map
		str = resourceDirectory + "/nm1.png";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//texture 1 height map
		str = resourceDirectory + "/wh1.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &HeightTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, HeightTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
        
        //texture 2 normal map
        str = resourceDirectory + "/nm5.png";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &Texture2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, Texture2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //texture 2 height map
        str = resourceDirectory + "/wh5.jpg";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &HeightTex2);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, HeightTex2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //texture 3 normal
        str = resourceDirectory + "/nm3.png";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &Texture3);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, Texture3);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //texture 3 height map
        str = resourceDirectory + "/wh3.png";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &HeightTex3);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, HeightTex3);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //texture 4 normal
        str = resourceDirectory + "/nm4.png";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &Texture4);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, Texture4);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //texture 4 height map
        str = resourceDirectory + "/wh4.png";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &HeightTex4);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, HeightTex4);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        GLuint NightTexLocation2 = glGetUniformLocation(heightshader->pid, "nighttex");
        GLuint SkyTexLocation2 = glGetUniformLocation(heightshader->pid, "skytex");
		GLuint Tex1Location = glGetUniformLocation(heightshader->pid, "n1");
		GLuint Tex2Location = glGetUniformLocation(heightshader->pid, "h1");
        GLuint Tex3Location = glGetUniformLocation(heightshader->pid, "n2");
        GLuint Tex4Location = glGetUniformLocation(heightshader->pid, "h2");
        GLuint Tex5Location = glGetUniformLocation(heightshader->pid, "n3");
        GLuint Tex6Location = glGetUniformLocation(heightshader->pid, "h3");
        GLuint Tex7Location = glGetUniformLocation(heightshader->pid, "n4");
        GLuint Tex8Location = glGetUniformLocation(heightshader->pid, "h4");
		// Then bind the uniform samplers to texture units:
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);
        glUniform1i(Tex3Location, 2);
        glUniform1i(Tex4Location, 3);
        glUniform1i(Tex5Location, 4);
        glUniform1i(Tex6Location, 5);
        glUniform1i(Tex7Location, 6);
        glUniform1i(Tex8Location, 7);
        glUniform1i(SkyTexLocation2, 8);
        glUniform1i(NightTexLocation2, 9);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the SkyBox program.
		skyboxshader = std::make_shared<Program>();
		skyboxshader->setVerbose(true);
		skyboxshader->setShaderNames(resourceDirectory + "/sky_vert.glsl", resourceDirectory + "/sky_frag.glsl");
		if (!skyboxshader->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		skyboxshader->addUniform("P");
		skyboxshader->addUniform("V");
		skyboxshader->addUniform("M");
		skyboxshader->addUniform("campos");
        skyboxshader->addUniform("cycle");
		skyboxshader->addAttribute("vertPos");
		skyboxshader->addAttribute("vertNor");
		skyboxshader->addAttribute("vertTex");

		// Initialize the Wave program.
		heightshader = std::make_shared<Program>();
		heightshader->setVerbose(true);
		heightshader->setShaderNames(resourceDirectory + "/height_vertex.glsl", resourceDirectory + "/height_frag.glsl");
		if (!heightshader->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		heightshader->addUniform("P");
		heightshader->addUniform("V");
		heightshader->addUniform("M");
		heightshader->addUniform("camoff");
		heightshader->addUniform("campos");
        heightshader->addUniform("cycle");
        heightshader->addUniform("waveFactor");
        heightshader->addUniform("texoff");
        heightshader->addUniform("texoff2");
		heightshader->addAttribute("vertPos");
		heightshader->addAttribute("vertTex");
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		double frametime = get_last_elapsed_time();

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClearColor(0.8f, 0.8f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = glm::mat4(1);
		M = glm::mat4(1);
		// Apply orthographic projection....
        V = mycam.process(frametime);
		P = glm::ortho(-1 * aspect, 1 * aspect, -1.0f, 1.0f, -2.0f, 100.0f);		
		if (width < height)
			{
			P = glm::ortho(-1.0f, 1.0f, -1.0f / aspect,  1.0f / aspect, -2.0f, 100.0f);
			}
		// ...but we overwrite it (optional) with a perspective projection.
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones

		//animation with the model matrix:
        float sangle = 3.1415926 / 2.;
        glm::mat4 RotateXSky = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::vec3 camp = -mycam.pos;
        glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
        glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));
        
        M = TransSky * RotateXSky * SSky;

		// Draw the box using GLSL.
		skyboxshader->bind();
        float cycle_factor = ev.manageCycle();
        glUniformMatrix4fv(skyboxshader->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(skyboxshader->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        glUniformMatrix4fv(skyboxshader->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glUniform3fv(skyboxshader->getUniform("campos"), 1, &mycam.pos[0]);
        glUniform1fv(skyboxshader->getUniform("cycle"), 1, &cycle_factor);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureSky);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, TextureNight);
        glDisable(GL_DEPTH_TEST);
        skybox->draw(skyboxshader, false);
        skyboxshader->unbind();

		heightshader->bind();
        float wave_factor = ev.manageWaves(frametime);
        // Moving simulated
        static vec2 texoff = vec2(0.,0.);
        texoff.x += .00005;
        texoff.y -= .00003;
        static vec2 texoff2 = vec2(0.,0.);
        texoff2.x += .00035;
        glUniform1fv(heightshader->getUniform("cycle"), 1, &cycle_factor);
        glUniform1fv(heightshader->getUniform("waveFactor"), 1, &wave_factor);
        glUniform2fv(heightshader->getUniform("texoff"), 1, &texoff[0]);
        glUniform2fv(heightshader->getUniform("texoff2"), 1, &texoff2[0]);
        glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.,1.,1.));
		glm::mat4 TransY = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, -3.0f, -50));
		M = TransY*S;
		glUniformMatrix4fv(heightshader->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniformMatrix4fv(heightshader->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(heightshader->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		
		
		vec3 offset = mycam.pos;
		offset.y = 0;
		offset.x = (int)offset.x;
		offset.z = (int)offset.z;
		glUniform3fv(heightshader->getUniform("camoff"), 1, &offset[0]);
		glUniform3fv(heightshader->getUniform("campos"), 1, &mycam.pos[0]);
		glBindVertexArray(VertexArrayID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, HeightTex);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, Texture2);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, HeightTex2);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, Texture3);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, HeightTex3);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, Texture4);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, HeightTex4);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, TextureSky);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, TextureNight);
        glm::mat4 TransZAll = glm::translate(glm::mat4(1.0), glm::vec3(-100,0, 20));
        M = TransZAll * M;
        glUniformMatrix4fv(heightshader->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, MESHSIZE*MESHSIZE*6, GL_UNSIGNED_SHORT, (void*)0);
        
        glm::mat4 TransZB = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 50));
        glm::mat4 Mb = TransZB * M;
        glUniformMatrix4fv(heightshader->getUniform("M"), 1, GL_FALSE, &Mb[0][0]);
        glDrawElements(GL_TRIANGLES, MESHSIZE*MESHSIZE*6, GL_UNSIGNED_SHORT, (void*)0);
        
        glm::mat4 TransZF = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, -50));
        glm::mat4 Mf = TransZF * M;
        glUniformMatrix4fv(heightshader->getUniform("M"), 1, GL_FALSE, &Mf[0][0]);
        glDrawElements(GL_TRIANGLES, MESHSIZE*MESHSIZE*6, GL_UNSIGNED_SHORT, (void*)0);
        
        glm::mat4 TransL = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 100));
        glm::mat4 Ml = TransL * M;
        glUniformMatrix4fv(heightshader->getUniform("M"), 1, GL_FALSE, &Ml[0][0]);
        glDrawElements(GL_TRIANGLES, MESHSIZE*MESHSIZE*6, GL_UNSIGNED_SHORT, (void*)0);
        
        glm::mat4 TransR = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, -100));
        glm::mat4 Mr = TransR * M;
        glUniformMatrix4fv(heightshader->getUniform("M"), 1, GL_FALSE, &Mr[0][0]);
        glDrawElements(GL_TRIANGLES, MESHSIZE*MESHSIZE*6, GL_UNSIGNED_SHORT, (void*)0);
		heightshader->unbind();

	}

};
//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
