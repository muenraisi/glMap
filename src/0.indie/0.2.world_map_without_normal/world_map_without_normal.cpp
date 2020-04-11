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

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadTextureRec(const char *path, int& width, int& height);
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
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader shader("3.1.blending.vs", "3.1.blending.fs");
	Shader terrainShader("terrain.vs", "terrain.fs", "terrain.gs", "terrain.cs", "terrain.es");

	std::string mapPath = "resources/world/";

	int worldWidth, worldHeight;
	unsigned int worldMap = loadTextureRec(FileSystem::getPath(mapPath + "height.png").c_str(), worldWidth, worldHeight);
	unsigned int terrainMap = loadTextureRec(FileSystem::getPath(mapPath + "terrain.png").c_str(), worldWidth, worldHeight);
	unsigned int riversMap = loadTextureRec(FileSystem::getPath(mapPath + "rivers.png").c_str(), worldWidth, worldHeight);
	int normalWidth, normalHeight;
	unsigned int normalMap = loadTextureRec(FileSystem::getPath(mapPath + "normal.png").c_str(), normalWidth, normalHeight);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float cubeVertices[] = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	float scale = (float)(worldWidth + worldHeight) / 2.f / 10.f;
	std::cout << "scale size is " << scale << std::endl;
	float scaleWidth = (float)(worldWidth / scale);
	float scaleHeight = (float)(worldHeight / scale);

	float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};
	//glPatchParameteri(GL_PATCH_VERTICES, 4);

	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// transparent VAO
	unsigned int transparentVAO, transparentVBO;
	glGenVertexArrays(1, &transparentVAO);
	glGenBuffers(1, &transparentVBO);
	glBindVertexArray(transparentVAO);
	glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));


	glBindVertexArray(0);
	// load textures
	// -------------

	unsigned int rockTexture = loadTexture(FileSystem::getPath("resources/terrain/rock.tga").c_str());
	unsigned int grassTexture = loadTexture(FileSystem::getPath("resources/terrain/grass.tga").c_str());
	unsigned int snowTexture = loadTexture(FileSystem::getPath("resources/terrain/snow.tga").c_str());
	unsigned int waterTexture = loadTexture(FileSystem::getPath("resources/terrain/water.tga").c_str());

	// transparent vegetation locations
	// --------------------------------
	vector<glm::vec3> vegetation
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};

	// shader configuration
	// --------------------
	shader.use();
	shader.setInt("texture1", 0);

	terrainShader.use();
	terrainShader.setInt("grassTexture", 0);
	terrainShader.setInt("rockTexture", 1);
	terrainShader.setInt("snowTexture", 2);
	terrainShader.setInt("waterTexture", 3);
	terrainShader.setInt("worldMap", 4);
	terrainShader.setInt("normalMap", 5);
	terrainShader.setInt("terrainMap", 6);
	terrainShader.setInt("riversMap", 7);



	// lighting info
	// -------------
	glm::vec3 lightPos(0.0f, 3.0f, 0.0f);

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
		shader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		//// cubes
		//glBindVertexArray(cubeVAO);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, cubeTexture);
		//model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		//shader.setMat4("model", model);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		//shader.setMat4("model", model);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//// vegetation
		//glBindVertexArray(transparentVAO);
		//glBindTexture(GL_TEXTURE_2D, transparentTexture);
		//for (GLuint i = 0; i < vegetation.size(); i++)
		//{
		//    model = glm::mat4(1.0f);
		//    model = glm::translate(model, vegetation[i]);
		//    shader.setMat4("model", model);
		//    glDrawArrays(GL_TRIANGLES, 0, 6);
		//}

		// floor
		terrainShader.use();
		terrainShader.setMat4("projection", projection);
		terrainShader.setMat4("view", view);
		terrainShader.setFloat("scale", scale);

		GLfloat waterLevel = (float)(94. / 255.);
		GLfloat snowLevel = 0.9f;
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
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_RECTANGLE, terrainMap);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_RECTANGLE, riversMap);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-scaleWidth / 2., 0.0f, -scaleHeight / 2));
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



		renderWorld(scaleWidth, scaleHeight, terrainShader);
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
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
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
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	std::cout << path << " loaded sucessfuly with (" << width << "," << height << ")-" << nrComponents << std::endl;
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTextureRec(char const * path, int& width, int& height)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	std::cout << path << " loaded sucessfuly with (" << width << "," << height << ")-" << nrComponents << std::endl;
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_RECTANGLE, textureID);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_RECTANGLE);


		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// renders a 1x1 quad to represents the world in NDC with manually calculated tangent vectors
// ------------------------------------------------------------------
unsigned int worldVAO = 0;
unsigned int worldVBO, worldEBO;
void renderWorld(float width, float height, Shader& shader)
{
	//width = 2; height = 1;
	const int grid_width = 11 * 16;
	const int grid_height = 4 * 16;
	if (worldVAO == 0)
	{
		// positions
		glm::vec3 pos1(width, 0.0f, height);
		glm::vec3 pos2(0.0f, 0.0f, height);
		glm::vec3 pos3(0.0f, 0.0f, 0);
		glm::vec3 pos4(width, 0.0f, 0);
		// texture coordinates
		glm::vec2 uv1(0.0f, 1.0f);
		glm::vec2 uv2(0.0f, 0.0f);
		glm::vec2 uv3(1.0f, 0.0f);
		glm::vec2 uv4(1.0f, 1.0f);
		// normal vector
		glm::vec3 nm(0.0f, 1.0f, 0.0f);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent, bitangent;
		// triangle 1
		// ----------
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent = glm::normalize(tangent);

		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent = glm::normalize(bitangent);

		shader.use();
		shader.setVec3("aTangent", tangent);
		shader.setVec3("aNormal", nm);
		shader.setVec3("aBitangent", bitangent);

		float worldVertices[3 * (grid_width + 1) * (grid_height + 1)];
		GLushort elems[4 * grid_width * grid_height];

		int indx, start;
		//verts array
		for (int i = 0; i <= grid_width; i++)   //100 vertices on a 10x10 grid
		{
			for (int j = 0; j <= grid_height; j++)
			{
				indx = (grid_height + 1) * i + j;
				worldVertices[3 * indx] = static_cast <float> (i * width / grid_width);        //x  varies from -45 to +45
				worldVertices[3 * indx + 1] = 0.;            //y  is set to 0 (ground plane)
				worldVertices[3 * indx + 2] = static_cast <float> (j * height / grid_height);        //z  varies from 0 to -90
			}
		}

		//std::cout << "world vertices: ";
		//for (int i =0; i< 3 * (grid_width + 1) * (grid_height + 1); i++)
		//  std::cout << worldVertices[i] << " ";
		//std::cout << std::endl;

		//elems array
		for (int i = 0; i < grid_width; i++)
		{
			for (int j = 0; j < grid_height; j++)
			{
				indx = grid_height * i + j;
				start = (grid_height + 1)  * i + j;
				elems[4 * indx] = start;
				elems[4 * indx + 1] = start + grid_height + 1;
				elems[4 * indx + 2] = start + grid_height + 2;
				elems[4 * indx + 3] = start + 1;
			}
		}
		//std::cout << "elems: ";
		//for (int i = 0; i< 4 * grid_width * grid_height; i++)
		//  std::cout << elems[i] << " ";
		//std::cout << std::endl;

		// configure plane VAO
		glGenVertexArrays(1, &worldVAO);
		glGenBuffers(1, &worldVBO);
		glGenBuffers(1, &worldEBO);
		glBindVertexArray(worldVAO);
		glBindBuffer(GL_ARRAY_BUFFER, worldVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(worldVertices), &worldVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, worldEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}
	glBindVertexArray(worldVAO);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawElements(GL_PATCHES, 4 * grid_width * grid_height, GL_UNSIGNED_SHORT, (const GLvoid *)0);
}