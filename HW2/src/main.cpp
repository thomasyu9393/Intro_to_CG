#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <bits/stdc++.h>

#include "./header/Object.h"
#include "./header/stb_image.h"

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int createShader(const string &filename, const string &type);
unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader);
unsigned int modelVAO(Object &model);
unsigned int loadTexture(const char *tFileName);
void drawModel(const string& name, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, int r, int g, int b);
void init();

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

// Shader
unsigned int vertexShader, fragmentShader, shaderProgram;

// Texture
unsigned int airplaneTexture, earthTexture;

// VAO, VBO
unsigned int airplaneVAO, earthVAO, cubeVAO;

// Objects to display
Object *airplaneObject, *earthObject, *cubeObject;

// Constants you may need
const int airplaneHeight = 27;
const int rotateEarthSpeed = 30;
const int rotateAirplaneSpeed = 90;
const int squeezeSpeed = 90;
const int rainbowSpeed = 72;

// You can use these parameters
float rotateEarthDegree = 0.0f;
float rotateAirplaneDegree = 0.0f;
float rotateAxisDegree = 180.0f;
bool useSqueeze = false;
float squeezeFactor = 0.0f;
bool useRainbowColor = false;
float rainbowDegree = 0.0f;
glm::vec3 rainbowColor = glm::vec3(1.0f, 1.0f, 1.0f);

// Bonus: Helicopter
bool useHelicopter = false;
int h_counter = 0;

int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    /* TODO#0: Change window title to "HW2 - [your student id]"
     *        Ex. HW2 - 312550000
     */

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW2 - 111550172", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // TODO#1: Finish function createShader
    // TODO#2: Finish function createProgram
    // TODO#3: Finish function modelVAO
    // TODO#4: Finish function loadTexture
    // You can find the above functions right below the main function

    // Initialize Object, Shader, Texture, VAO, VBO
    init();

    // Enable depth test, face culling
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // Set viewport
    glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // render loop variables
    double dt;
    double lastTime = glfwGetTime();
    double currentTime;

    /* TODO#5: Data connection - Retrieve uniform variable locations
     *    1. Retrieve locations for model, view, and projection matrices.
     *    2. Retrieve locations for squeezeFactor, rainbowColor, and other parameters.
     * Hint:
     *    glGetUniformLocation
     */

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // render
        glClearColor(0 / 255.0, 0 / 255.0, 0 / 255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 50.0f, 90.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

        glm::mat4 base(1.0f), airplaneModel(1.0f), earthModel(1.0f);

        /* TODO#6-1: Render Airplane
         *    1. Set up airplane model matrix.
         *    2. Send model, view, and projection matrices to the program.
         *    3. Send squeezeFactor, rainbowColor, or other parameters to the program.
         *    4. Apply the texture, and render the airplane.
         * Hint:
         *	  rotate, translate, scale
         *    glUniformMatrix4fv, glUniform1f, glUniform3fv
         *    glActiveTexture, glBindTexture, glBindVertexArray, glDrawArrays
         */
        airplaneModel = glm::rotate(airplaneModel, glm::radians(rotateAxisDegree), glm::vec3(0.0f, 1.0f, 0.0f));
        float dy = sin(glm::radians(rotateAirplaneDegree)) * airplaneHeight;
        float dz = cos(glm::radians(rotateAirplaneDegree)) * airplaneHeight;
        airplaneModel = glm::translate(airplaneModel, glm::vec3(0.0f, dy, dz));
        airplaneModel = glm::rotate(airplaneModel, glm::radians(rotateAirplaneDegree - 90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

        /* TODO#6-2: Render Earth
         *    1. Set up earth model matrix.
         *    2. Send model, view, and projection matrices to the program.
         *    3. Send squeezeFactor, rainbowColor, or other parameters to the program.
         *    4. Apply the texture, and render the earth.
         * Hint:
         *	  rotate, translate, scale
         *    glUniformMatrix4fv, glUniform1f, glUniform3fv
         *    glActiveTexture, glBindTexture, glBindVertexArray, glDrawArrays
         */
        earthModel = glm::scale(earthModel, glm::vec3(10.0f)); // Scale the Earth by 10x
        earthModel = glm::rotate(earthModel, glm::radians(rotateEarthDegree), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotation around Y-axis
        drawModel("earth", earthModel, view, projection, 1.0f, 1.0f, 1.0f);

        /* Bonus: Helicopter
         *    1. Body
              2. Connector
              3. Blades
         */
        glm::mat4 helicopterModel(1.0f);
        helicopterModel = glm::rotate(helicopterModel, glm::radians(rotateAxisDegree), glm::vec3(0.0f, 1.0f, 0.0f));
        helicopterModel = glm::translate(helicopterModel, glm::vec3(0.0f, dy, dz));
        helicopterModel = glm::rotate(helicopterModel, glm::radians(rotateAirplaneDegree - 90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        helicopterModel = glm::rotate(helicopterModel, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        
        glm::vec3 mainBodyPosition(0.0f, 0.0f, 0.0f);
        glm::mat4 mainBodyModel(1.0f);
        mainBodyModel = glm::translate(helicopterModel, mainBodyPosition);
        mainBodyModel = glm::scale(mainBodyModel, glm::vec3(10.0f, 10.0f, 10.0f));
        glm::mat4 secondBodyModel(1.0f);
        secondBodyModel = glm::translate(helicopterModel, mainBodyPosition + glm::vec3(8.0f, 0.0f, 0.0f));
        secondBodyModel = glm::scale(secondBodyModel, glm::vec3(8.0f, 8.0f, 8.0f));
        glm::mat4 thirdBodyModel(1.0f);
        thirdBodyModel = glm::translate(helicopterModel, mainBodyPosition + glm::vec3(14.0f, 0.0f, 0.0f));
        thirdBodyModel = glm::scale(thirdBodyModel, glm::vec3(6.0f, 6.0f, 6.0f));

        glm::mat4 connectorModel(1.0f);
        connectorModel = glm::translate(helicopterModel, mainBodyPosition + glm::vec3(0.0f, 6.0f, 0.0f));
        connectorModel = glm::scale(connectorModel, glm::vec3(2.0f, 4.0f, 2.0f));
        connectorModel = glm::rotate(connectorModel, (float)h_counter * glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        
        vector<glm::vec3> RGB = {
            glm::vec3(0, 0, 0),
            glm::vec3(205, 133, 63),
            glm::vec3(139, 69, 19),
            glm::vec3(255, 222, 173)
        };
        vector<glm::mat4> bladeModels;
        for (int i = 0; i < 4; ++i) {
            glm::mat4 bladeModel(1.0f);
            bladeModel = glm::rotate(connectorModel, glm::radians(90.0f * i), glm::vec3(0.0f, 1.0f, 0.0f));
            bladeModel = glm::translate(bladeModel, glm::vec3(2.5f, 0.4f, 0.0f));
            bladeModel = glm::scale(bladeModel, glm::vec3(4.0f, 0.2f, 1.0f));
            bladeModels.emplace_back(bladeModel);
        }

        if (!useHelicopter) {
            drawModel("airplane", airplaneModel, view, projection, 1.0f, 1.0f, 1.0f);
        } else {
            drawModel("cube", mainBodyModel, view, projection, 102, 178, 255);
            drawModel("cube", secondBodyModel, view, projection, 102, 102, 255);
            drawModel("cube", thirdBodyModel, view, projection, 178, 102, 255);
            drawModel("cube", connectorModel, view, projection, 255, 255, 255);
            for (int i = 0; i < 4; ++i) {
                drawModel("cube", bladeModels[i], view, projection, RGB[i][0], RGB[i][1], RGB[i][2]);
            }
        }

        // Status update
        currentTime = glfwGetTime();
        dt = currentTime - lastTime;
        lastTime = currentTime;

        /* TODO#7: Update "rotateEarthDegree", "rotateAirplaneDegree", "rotateAxisDegree",
         *          "squeezeFactor", "rainbowColor"
         */
        rotateEarthDegree += rotateEarthSpeed * dt;
        if (rotateEarthDegree >= 360.0f) rotateEarthDegree -= 360.0f;
        rotateAirplaneDegree += rotateAirplaneSpeed * dt;
        if (rotateAirplaneDegree >= 360.0f) rotateAirplaneDegree -= 360.0f;
        h_counter = (h_counter + 1) % 72;

        if (useSqueeze) {
            squeezeFactor += squeezeSpeed * dt;
            if (squeezeFactor >= 360.0f) squeezeFactor -= 360.0f;
        }

        if (useRainbowColor) {
            rainbowDegree += rainbowSpeed * dt;
            if (rainbowDegree >= 360.0f) rainbowDegree -= 360.0f;
            // Convert HSV to RGB
            float hue = rainbowDegree;
            float saturation = 1.0f;
            float value = 1.0f;
            float c = value * saturation;
            float x = c * (1 - fabs(fmod(hue / 60.0f, 2.0f) - 1));
            float m = value - c;
            if (hue < 60) {
                rainbowColor = glm::vec3(c, x, 0);
            } else if (hue < 120) {
                rainbowColor = glm::vec3(x, c, 0);
            } else if (hue < 180) {
                rainbowColor = glm::vec3(0, c, x);
            } else if (hue < 240) {
                rainbowColor = glm::vec3(0, x, c);
            } else if (hue < 300) {
                rainbowColor = glm::vec3(x, 0, c);
            } else {
                rainbowColor = glm::vec3(c, 0, x);
            }
            rainbowColor += glm::vec3(m);
        } else {
            rainbowColor = glm::vec3(1.0f);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

/* TODO#1: createShader
 * input:
 *      filename: shader file name
 *      type: shader type, "vert" means vertex shader, "frag" means fragment shader
 * output: shader object
 * Hint:
 *      glCreateShader, glShaderSource, glCompileShader
 */
unsigned int createShader(const string &filename, const string &type) {
    GLuint shader;  // Create shader object
    if (type == "vert") {
        shader = glCreateShader(GL_VERTEX_SHADER);
        // GLuint glCreateShader(GLenum shaderType);
        // Specifies the type of shader to be created and creates an empty shader object.
    } else if (type == "frag") {
        shader = glCreateShader(GL_FRAGMENT_SHADER);
    }

    std::ifstream shaderFile(filename);
    std::stringstream ss;
    ss << shaderFile.rdbuf();
    string shaderCode = ss.str();
    shaderFile.close();
    const char* shaderSource = shaderCode.c_str();

    glShaderSource(shader, 1, &shaderSource, NULL);
    // void glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
    // Sets the source code in shader to the source code in the array of strings specified by "string".

    glCompileShader(shader);
    // void glCompileShader(GLuint shader);
    // Compile the shader.

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> infoLog(logLength);
        glGetShaderInfoLog(shader, logLength, NULL, infoLog.data());
        std::cerr << "[Error compiling shader " << filename << "] " << infoLog.data() << std::endl;
        return 0;
    }

    return shader;
}

/* TODO#2: createProgram
 * input:
 *      vertexShader: vertex shader object
 *      fragmentShader: fragment shader object
 * output: shader program
 * Hint:
 *      glCreateProgram, glAttachShader, glLinkProgram, glDetachShader
 */
unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    GLuint program = glCreateProgram();
    // GLuint glCreateProgram(void);
    // Creates a program object.

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    // void glAttachShader(GLuint program, GLuint shader);
    // Attach the shader object to the program object.

    glLinkProgram(program);
    // void glLinkProgram(GLuint program);
    // Link this program.

    // Check for linking errors
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> infoLog(logLength);
        glGetProgramInfoLog(program, logLength, NULL, infoLog.data());
        std::cerr << "[Error linking program] " << infoLog.data() << std::endl;
        return 0;
    }

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    // void glDetachShader(GLuint program, GLuint shader);
    // Detaches the shader object from the program object.

    return program;
}

/* TODO#3: modelVAO
 * input:
 *      model: Object you want to render
 * output: VAO
 * Hint:
 *      glGenVertexArrays, glBindVertexArray, glGenBuffers, glBindBuffer, glBufferData,
 *      glVertexAttribPointer, glEnableVertexAttribArray,
 */
unsigned int modelVAO(Object &model) {
    unsigned int VAO, VBO[3];

    // Vertex Array Object (VAO) (Ref. p17-18)
    glGenVertexArrays(1, &VAO);
    // void glGenVertexArrays(GLsizei n, GLuint* arrays);
    // Generate vertex array objects (VAOs).
    // n: Specifies the number of vertex array object names to be generated.
    // arrays: Specifies an array in which the generated vertex array object names are stored.

    glBindVertexArray(VAO);
    // void glBindVertexArray(GLuint array);
    // Bind **a** vertex array object (VAO).
    // array: Specifies the name of the vertex array to bind.

    // (Ref. p8-10)
    glGenBuffers(3, VBO);
    // void glGenBuffers(GLsizei n, GLuint* buffers);
    // Generate vertex buffer objects (VBOs).
    // n: Specifies the number of buffer object names to be generated.
    // buffers: Specifies an array in which the generated buffer object names are stored.

    // VBO[0]: Positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    // void glBindBuffer(GLenum target, GLuint buffer);
    // Bind the target buffer, which is GL_ARRAY_BUFFER here.
    // target: GL_ARRAY_BUFFER, GL_TEXTURE_BUFFER, ...
    // buffer: Specifies the name of a buffer object.
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * model.positions.size(), &model.positions[0], GL_STATIC_DRAW);
    // void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
    // Copy the data into the target.
    // target: GL_ARRAY_BUFFER, GL_TEXTURE_BUFFER, ...
    // size: Specifies the size in bytes of the buffer object's new data store.
    // data: Specifies a pointer to data that will be copied into the data store for initialization, or NULL if no data is to be copied.
    // usage: Specifies the expected usage pattern of the data store, e.g. GL_STATIC_DRAW means the data store contents will be modified once and used at most a few times.
    // (Ref. p12-13)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); // layout(location = 0) in vec3 in_position;
    // void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
    // Link the vertex buffer with the vertex shader input.
    // index: Specifies the index of the generic vertex attribute to be modified.
    // size: Specifies the number of components per generic vertex attribute.
    // type: Specifies the data type of each component in the array, e.g. GL_FLOAT
    // normalized: Specifies whether fixed-point data values should be normalized or not.
    // stride: Specifies the byte offset between consecutive generic vertex attributes.
    // pointer: Specifies a offset of the first component of the first generic vertex attribute in the array in the data store of the buffer currently bound to the GL_ARRAY_BUFFER target. The initial value is 0.
    glEnableVertexAttribArray(0);

    // VBO[1]: Normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * model.normals.size(), &model.normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); // location 1, vec3
    glEnableVertexAttribArray(1);

    // VBO[2]: Texture Coordinates
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * model.texcoords.size(), &model.texcoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0); // location 2, vec2
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
    glBindVertexArray(0); // Unbind VAO
    return VAO;
}

/* TODO#4: loadTexture
 * input:
 *      filename: texture file name
 * output: texture object
 * Hint:
 *      glEnable, glGenTextures, glBindTexture, glTexParameteri, glTexImage2D
 */
unsigned int loadTexture(const string &filename) {
    glEnable(GL_TEXTURE_2D);

    GLuint textureID;
    glGenTextures(1, &textureID);
    // void glGenTextures(GLsizei n, GLuint* textures);
    // Takes as input how many textures we want to generate and stores them in a unsigned int array.

    // Load image data using stb_image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        // void glBindTexture(GLenum target, GLuint texture);
        // Bind a named texture to a texturing target (e.g. GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_1D_ARRAY)

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // void glTexParameteri(GLenum target, GLenum pname, GLint param);
        // Texture wrapping: Texture coordinates usually range from (0,0) to (1,1) but if we specify coordinates outside this range, the default behavior of OpenGL is to repeat the texture images.

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
        // Texture filtering: Texture coordinates do not depend on resolution but can be any floating point value, thus OpenGL has to figure out which texture pixel to map the texture coordinate to.

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data);
        // Generate a two-dimensional texture image

        glGenerateMipmap(GL_TEXTURE_2D);  // Generate mipmaps

        stbi_image_free(data);
    } else {
        std::cerr << "[Failed to load texture " << filename << "]" << std::endl;
        stbi_image_free(data);
        return 0;
    }

    return textureID;
}

/* TODO#8: Key callback
 *    1. Press 'd' to increase the "rotateAxisDegree" by 1.
 *    2. Press 'a' to decrease the "rotateAxisDegree" by 1.
 *    3. Press 's' to squeeze the earth.
 *    4. Press 'r' to make the color of the airplane rainbow.
 * Hint:
 *      GLFW_KEY_D, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_R
 *      GLFW_PRESS, GLFW_REPEAT
 */
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT) {
        return;
    }
    if (key == GLFW_KEY_D) {
        rotateAxisDegree += 1.0f;
        if (rotateAxisDegree >= 360.0f) rotateAxisDegree -= 360.0f;
    } else if (key == GLFW_KEY_A) {
        rotateAxisDegree -= 1.0f;
        if (rotateAxisDegree < 0.0f) rotateAxisDegree += 360.0f;
    } else if (key == GLFW_KEY_S) {
        useSqueeze = !useSqueeze;
    } else if (key == GLFW_KEY_R) {
        useRainbowColor = !useRainbowColor;
        if (useRainbowColor == false) {
            rainbowColor = glm::vec3(1.0f);
            rainbowDegree = 0.0f;
        }
    } else if (key == GLFW_KEY_H) {
        useHelicopter = !useHelicopter;
    } else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void drawModel(const string& name, 
               const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, 
               int r, int g, int b) {
    // Data connection: Retrieve uniform variable locations (Ref. p20)
    // Vertex Shader
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint squeezeFactorLoc = glGetUniformLocation(shaderProgram, "squeezeFactor");

    // Fragment Shader
    GLint ourTextureLoc = glGetUniformLocation(shaderProgram, "ourTexture");
    GLint rainbowColorLoc = glGetUniformLocation(shaderProgram, "rainbowColor");
    GLuint useRainbowColorLoc = glGetUniformLocation(shaderProgram, "useRainbowColor");

    GLint cubeColorLoc = glGetUniformLocation(shaderProgram, "cubeColor");
    GLuint useHelicopterLoc = glGetUniformLocation(shaderProgram, "useHelicopter");

    // Send matrices to vertex shader programs.
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    if (name == "earth") {
        glUniform1f(squeezeFactorLoc, glm::radians(squeezeFactor));  // Squeezing
    } else {
        glUniform1f(squeezeFactorLoc, 0.0f); // No squeeze effect on airplane
    }

    // Send uniforms to fragment shader programs.
    if (name == "earth") {
        glUniform1i(ourTextureLoc, 0);
        glUniform3fv(rainbowColorLoc, 1, glm::value_ptr(glm::vec3(1.0f)));  // No rainbow effect for earth
        glUniform1i(useRainbowColorLoc, false);  // Disable rainbow effect
        glUniform3fv(cubeColorLoc, 1, glm::value_ptr(glm::vec3(0.0f)));
        glUniform1i(useHelicopterLoc, false);
    } else if (name == "airplane"){
        glUniform1i(ourTextureLoc, 0);
        glUniform3fv(rainbowColorLoc, 1, glm::value_ptr(rainbowColor));
        glUniform1i(useRainbowColorLoc, useRainbowColor);
        glUniform3fv(cubeColorLoc, 1, glm::value_ptr(glm::vec3(0.0f)));
        glUniform1i(useHelicopterLoc, useHelicopter);
    } else if (name == "cube") {
        glUniform1i(ourTextureLoc, 0);
        glUniform3fv(rainbowColorLoc, 1, glm::value_ptr(rainbowColor));
        glUniform1i(useRainbowColorLoc, useRainbowColor);
        glUniform3fv(cubeColorLoc, 1, glm::value_ptr(glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f)));
        glUniform1i(useHelicopterLoc, useHelicopter);
    }

    // Apply texture and "render". (Ref. "When Rendering" p19)
    if (name == "earth") {
        glActiveTexture(GL_TEXTURE0);  // (Ref. p27,29)
        // void glActiveTexture(GLenum textureUnit);
        // Selects which texture unit subsequent texture state calls will affect. You can using the textureUnit from GL_TEXTURE0 to GL_TEXTUREn, 0 <= n < GL_MAX_TEXTURE_UNITS, and texture units are subsequent, you can use GL_TEXTUREn or GL_TEXTURE0 + n. (e.g. GL_TEXTURE2 = GL_TEXTURE0 + 2)
        glBindTexture(GL_TEXTURE_2D, earthTexture);
        glBindVertexArray(earthVAO);
        glDrawArrays(GL_TRIANGLES, 0, earthObject->positions.size() / 3);
        // void glDrawArrays(GLenum mode, GLint first, GLsizei count); 
        // Render primitives from vertex array data.
        // mode: Specifies what kind of primitives to render, e.g. GL_POINTS, GL_LINES, GL_TRIANGLE_STRIP
        // first: Specifies the starting index in the enabled arrays.
        // count: Specifies the number of indices to be rendered.
    } else if (name == "airplane") {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, airplaneTexture);
        glBindVertexArray(airplaneVAO);
        glDrawArrays(GL_TRIANGLES, 0, airplaneObject->positions.size() / 3);
    } else if (name == "cube") {
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, cubeObject->positions.size() / 3);
    }
    glBindVertexArray(0); // Unbind VAO
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void init() {
#if defined(__linux__) || defined(__APPLE__)
    string dirShader = "../../src/shaders/";
    string dirAsset = "../../src/asset/obj/";
    string dirTexture = "../../src/asset/texture/";
#else
    string dirShader = "..\\..\\src\\shaders\\";
    string dirAsset = "..\\..\\src\\asset\\obj\\";
    string dirTexture = "..\\..\\src\\asset\\texture\\";
#endif

    // Object
    airplaneObject = new Object(dirAsset + "airplane.obj");
    earthObject = new Object(dirAsset + "earth.obj");
    cubeObject = new Object(dirAsset + "cube.obj");

    // Shader
    vertexShader = createShader(dirShader + "vertexShader.vert", "vert");
    fragmentShader = createShader(dirShader + "fragmentShader.frag", "frag");
    shaderProgram = createProgram(vertexShader, fragmentShader);
    glUseProgram(shaderProgram); // (Ref. p7,20)

    // Texture
    airplaneTexture = loadTexture(dirTexture + "airplane.jpg");
    earthTexture = loadTexture(dirTexture + "earth.jpg");

    // VAO, VBO
    airplaneVAO = modelVAO(*airplaneObject);
    earthVAO = modelVAO(*earthObject);
    cubeVAO = modelVAO(*cubeObject);
}
