

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_i.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <learnopengl/terrain.h>



#include <iostream>
#include <learnopengl\terrain.h>
#include "..\..\..\includes\learnopengl\QuadTree.h"


#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


#if defined(DEBUG) | defined(_DEBUG)
#if defined(DEBUG) | defined(_DEBUG)
void APIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar* msg, const void* scale)
{
	char* _source;
	char* _type;
	char* _severity;

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		_source = "API";
		break;

	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		_source = "WINDOW SYSTEM";
		break;

	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		_source = "SHADER COMPILER";
		break;

	case GL_DEBUG_SOURCE_THIRD_PARTY:
		_source = "THIRD PARTY";
		break;

	case GL_DEBUG_SOURCE_APPLICATION:
		_source = "APPLICATION";
		break;

	case GL_DEBUG_SOURCE_OTHER:
		_source = "OTHER";
		break;

	default:
		_source = "UNKNOWN";
		break;
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		_type = "ERROR";
		break;

	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		_type = "DEPRECATED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		_type = "UDEFINED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_PORTABILITY:
		_type = "PORTABILITY";
		break;

	case GL_DEBUG_TYPE_PERFORMANCE:
		_type = "PERFORMANCE";
		break;

	case GL_DEBUG_TYPE_OTHER:
		_type = "OTHER";
		break;

	case GL_DEBUG_TYPE_MARKER:
		_type = "MARKER";
		break;

	default:
		_type = "UNKNOWN";
		break;
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		_severity = "HIGH";
		break;

	case GL_DEBUG_SEVERITY_MEDIUM:
		_severity = "MEDIUM";
		break;

	case GL_DEBUG_SEVERITY_LOW:
		_severity = "LOW";
		break;

	case GL_DEBUG_SEVERITY_NOTIFICATION:
		_severity = "NOTIFICATION";
		break;

	default:
		_severity = "UNKNOWN";
		break;
	}

	fprintf(stderr, "GL CALLBACK: %s type = %s, severity = %s, message = %s, source = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		_type, _severity, msg,_source);

	if (type == GL_DEBUG_TYPE_ERROR)
		abort();
}
#endif
#endif

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadTextureRec(const char* path, int& width, int& height);
void renderWorld(float width, float height, Shader& shader);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 2.0f, 5.0f));
float lastX = (float)(SCR_WIDTH / 2.0);
float lastY = (float)(SCR_HEIGHT / 2.0);
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// glfw: initialize and configure
	// ------------------------------
	std::cout << "Starting GLFW context, OpenGL 4.0" << std::endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
		//Let OpenGL Log errors
#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openglCallbackFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
#endif
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader terrainShader("terrain.vs", "terrain.fs", "terrain.gs", "terrain.cs", "terrain.es");

	std::string mapPath = "resources/world/";

	int worldWidth, worldHeight;
	unsigned int worldMap = loadTextureRec(FileSystem::getPath(mapPath + "height.png").c_str(), worldWidth, worldHeight);
	//unsigned int terrainMap = loadTextureRec(FileSystem::getPath(mapPath + "terrain.png").c_str(), worldWidth, worldHeight);
	//unsigned int riversMap = loadTextureRec(FileSystem::getPath(mapPath + "rivers.png").c_str(), worldWidth, worldHeight);
	int normalWidth, normalHeight;
	unsigned int normalMap = loadTextureRec(FileSystem::getPath(mapPath + "normal.png").c_str(), normalWidth, normalHeight);


	float scale = (float)(worldWidth + worldHeight) / 2.f / 10.f;
	std::cout << "scale size is " << scale << std::endl;
	float scaleWidth = (float)(worldWidth / scale);
	float scaleHeight = (float)(worldHeight / scale);


	glBindVertexArray(0);
	// load textures
	// -------------

	unsigned int rockTexture = loadTexture(FileSystem::getPath("resources/terrain/rock.tga").c_str());
	unsigned int grassTexture = loadTexture(FileSystem::getPath("resources/terrain/grass.tga").c_str());
	unsigned int snowTexture = loadTexture(FileSystem::getPath("resources/terrain/snow.tga").c_str());
	unsigned int waterTexture = loadTexture(FileSystem::getPath("resources/terrain/water.tga").c_str());


	// shader configuration
	// --------------------

	terrainShader.use();
	terrainShader.setInt("grassTexture", 0);
	terrainShader.setInt("rockTexture", 1);
	terrainShader.setInt("snowTexture", 2);
	terrainShader.setInt("waterTexture", 3);
	terrainShader.setInt("worldMap", 4);
	terrainShader.setInt("normalMap", 5);
	//terrainShader.setInt("terrainMap", 6);
	//terrainShader.setInt("riversMap", 7);



	// lighting info
	// -------------
	glm::vec3 lightPos(0.0f, 3.0f, 0.0f);


	Terrain worldTerrain(scaleWidth, scaleHeight);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw objects
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);


		// floor
		terrainShader.use();
		terrainShader.setMat4("projection", projection);
		terrainShader.setMat4("view", view);
		terrainShader.setFloat("scale", scale);

		GLfloat waterLevel = (float)(0. / 255.);
		GLfloat snowLevel = 0.5f;
		terrainShader.setFloat("waterLevel", waterLevel);
		terrainShader.setFloat("snowLevel", snowLevel);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, grassTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, rockTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, snowTexture);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, waterTexture);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_RECTANGLE, worldMap);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_RECTANGLE, normalMap);
		//glActiveTexture(GL_TEXTURE6);
		//glBindTexture(GL_TEXTURE_RECTANGLE, terrainMap);
		//glActiveTexture(GL_TEXTURE7);
		//glBindTexture(GL_TEXTURE_RECTANGLE, riversMap);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.f, 0.0f, 0.f));
		terrainShader.setMat4("model", model);

		glm::mat4 mvMatrix = view * model;
		glm::mat4 mvpMatrix = projection * view * model;

		terrainShader.setMat4("mvMatrix", mvMatrix);
		terrainShader.setMat4("mvpMatrix", mvpMatrix);

		terrainShader.setVec3("viewPos", camera.Position);
		terrainShader.setVec3("lightPos", lightPos);

		//verify the coordinate transfomation
		//glm::vec4 testPoint(0.0f, 0.0f, 0.0f, 1.0f);
		//glm::vec4 testPointM = model * testPoint;
		//glm::vec4 testPointMV = view * testPointM;
		//glm::vec4 testPointMVP = projection * testPointMV;

		//std::cout << "M:" << testPointM.x << " " <<  testPointM.y << " " << testPointM.z << " " << testPointM.w << std::endl;
		//std::cout << "MV:" << testPointMV.x << " " << testPointMV.y << " " << testPointMV.z << " " << testPointMV.w << std::endl;
		//std::cout << "MVP:" << testPointMVP.x << " " << testPointMVP.y << " " << testPointMVP.z << " " << testPointMVP.w << std::endl;



		worldTerrain.render(terrainShader, camera);
		/*glPatchParameteri(GL_PATCH_VERTICES, 4);
		glDrawElements(GL_PATCHES, 4, GL_UNSIGNED_SHORT, (const GLvoid *)0);*/


		glBindVertexArray(0);
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------


	glfwTerminate();

	
	//_CrtDumpMemoryLeaks();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* scale = stbi_load(path, &width, &height, &nrComponents, 0);
	std::cout << path << " loaded sucessfuly with (" << width << "," << height << ")-" << nrComponents << std::endl;
	if (scale)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, scale);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(scale);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(scale);
	}

	return textureID;
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTextureRec(char const* path, int& width, int& height)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int nrComponents;
	unsigned char* scale = stbi_load(path, &width, &height, &nrComponents, 0);
	std::cout << path << " loaded sucessfuly with (" << width << "," << height << ")-" << nrComponents << std::endl;
	if (scale)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_RECTANGLE, textureID);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, scale);

		//glGenerateMipmap(GL_TEXTURE_RECTANGLE);


		stbi_image_free(scale);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(scale);
	}

	return textureID;
}


