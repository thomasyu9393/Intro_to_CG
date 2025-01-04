#include <bits/stdc++.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "header/cube.h"
#include "header/object.h"
#include "header/shader.h"
#include "header/stb_image.h"

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int loadCubemap(std::vector<string> &mFileName);

struct material_t{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float gloss;
};

struct light_t{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct model_t{
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
    Object* object;
};

struct camera_t{
    glm::vec3 position;
    glm::vec3 up;
    float rotationY;
};

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

std::string cubemapDir = "..\\..\\src\\asset\\texture\\skybox\\";

// cube map 
unsigned int cubemapTexture;
unsigned int cubemapVAO, cubemapVBO;

// shader programs 
int shaderProgramIndex = 0;
std::vector<shader_program_t*> shaderPrograms;
shader_program_t* cubemapShader;

// additional dependencies
light_t light;
material_t material;
camera_t camera;
model_t ditto;
model_t foongus;
model_t pokeball;
model_t lic;

// model matrix
int moveDir = -1;
glm::mat4 dittoModel;
glm::mat4 foongusModel;
glm::mat4 pokeballModel;
glm::mat4 cameraModel;
glm::mat4 licModel;

//////////////////////////////////////////////////////////////////////////
// Parameter setup, 
// You can change any of the settings if you want

void camera_setup(){
    camera.position = glm::vec3(60.0, 60.0, 120.0);
    camera.up = glm::vec3(0.0, 1.0, 0.0);
    camera.rotationY = 0;
}

void light_setup(){
    light.position = glm::vec3(0.0, 1000.0, 0.0);
    light.ambient = glm::vec3(1.0);
    light.diffuse = glm::vec3(1.0);
    light.specular = glm::vec3(1.0);
}

void material_setup(){
    material.ambient = glm::vec3(1.0);
    material.diffuse = glm::vec3(1.0);
    material.specular = glm::vec3(0.7);
    material.gloss = 10.5;
}
//////////////////////////////////////////////////////////////////////////

void model_setup(){

// Load the object and texture for each model here 

#if defined(__linux__) || defined(__APPLE__)
    std::string objDir = "../../src/asset/obj/";
    std::string textureDir = "../../src/asset/texture/";
#else
    std::string objDir = "..\\..\\src\\asset\\obj\\";
    std::string textureDir = "..\\..\\src\\asset\\texture\\";
#endif
    foongusModel = glm::mat4(1.0f);
    dittoModel = glm::mat4(1.0f);
    pokeballModel = glm::mat4(1.0f);
    licModel = glm::mat4(1.0f);

    ditto.position = glm::vec3(0.0f, 0.0f, 0.0f);
    ditto.scale = glm::vec3(0.5f, 0.5f, 0.5f);
    ditto.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    ditto.object = new Object(objDir + "newditto.obj");
    ditto.object->load_to_buffer();
    ditto.object->load_texture(textureDir + "newditto.jpg");

    foongus.position = glm::vec3(1000.0f, -50.0f, 0.0f);
    foongus.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    foongus.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    foongus.object = new Object(objDir + "foongus.obj");
    foongus.object->load_to_buffer();
    foongus.object->load_texture(textureDir + "foongus.jpg");

    pokeball.position = glm::vec3(1000.0f, 1000.0f, 0.0f);
    pokeball.scale = glm::vec3(1.5f);
    pokeball.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    pokeball.object = new Object(objDir + "pokeball.obj");
    pokeball.object->load_to_buffer();
    pokeball.object->load_texture(textureDir + "pokeball.jpg");
    
    lic.position = glm::vec3(1000.0f, 1000.0f, 0.0f);
    lic.scale = glm::vec3(1.0f);
    lic.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    lic.object = new Object(objDir + "lic.obj");
    lic.object->load_to_buffer();
    lic.object->load_texture(textureDir + "lic.jpg");
}

void shader_setup(){

// Setup the shader program for each shading method

#if defined(__linux__) || defined(__APPLE__)
    std::string shaderDir = "../../src/shaders/";
#else
    std::string shaderDir = "..\\..\\src\\shaders\\";
#endif

    std::vector<std::string> shadingMethod = {
        "default",                              // default shading
        "bling-phong", "gouraud", "metallic",   // addional shading effects (basic)
        "glass_schlick", "glass_empricial",     // addional shading effects (advanced)
    };

    for(int i=0; i<shadingMethod.size(); i++){
        std::string vpath = shaderDir + shadingMethod[i] + ".vert";
        std::string fpath = shaderDir + shadingMethod[i] + ".frag";

        shader_program_t* shaderProgram = new shader_program_t();
        shaderProgram->create();
        shaderProgram->add_shader(vpath, GL_VERTEX_SHADER);
        if (shadingMethod[i] == "default") {
            std::string gpath = shaderDir + "my.geom"; // New geometry shader
            shaderProgram->add_shader(gpath, GL_GEOMETRY_SHADER);
        }
        shaderProgram->add_shader(fpath, GL_FRAGMENT_SHADER);
        
        shaderProgram->link_shader();
        shaderPrograms.push_back(shaderProgram);
    } 
}


void cubemap_setup(){

// Setup all the necessary things for cubemap rendering
// Including: cubemap texture, shader program, VAO, VBO

#if defined(__linux__) || defined(__APPLE__)
    std::string cubemapDir = "../../src/asset/texture/skybox/";
    std::string shaderDir = "../../src/shaders/";
#else
    // std::string cubemapDir = "..\\..\\src\\asset\\texture\\skybox4\\";
    std::string shaderDir = "..\\..\\src\\shaders\\";
#endif

    // setup texture for cubemap
    std::vector<std::string> faces
    {
        cubemapDir + "right.jpg",
        cubemapDir + "left.jpg",
        cubemapDir + "top.jpg",
        cubemapDir + "bottom.jpg",
        cubemapDir + "front.jpg",
        cubemapDir + "back.jpg"
    };
    cubemapTexture = loadCubemap(faces);   

    // setup shader for cubemap
    std::string vpath = shaderDir + "cubemap.vert";
    std::string fpath = shaderDir + "cubemap.frag";
    
    cubemapShader = new shader_program_t();
    cubemapShader->create();
    cubemapShader->add_shader(vpath, GL_VERTEX_SHADER);
    cubemapShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    cubemapShader->link_shader();

    glGenVertexArrays(1, &cubemapVAO);
    glGenBuffers(1, &cubemapVBO);
    glBindVertexArray(cubemapVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), &cubemapVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
}

void setup(){

    // Initialize shader model camera light material
    light_setup();
    model_setup();
    shader_setup();
    camera_setup();
    cubemap_setup();
    material_setup();

    // Enable depth test, face culling ...
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // Debug: enable for debugging
    // glEnable(GL_DEBUG_OUTPUT);
    // glDebugMessageCallback([](  GLenum source, GLenum type, GLuint id, GLenum severity, 
    //                             GLsizei length, const GLchar* message, const void* userParam) {

    // std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") 
    //           << "type = " << type 
    //           << ", severity = " << severity 
    //           << ", message = " << message << std::endl;
    // }, nullptr);
}

int accerlation = 0;
bool useRainbowColor = false;
float rainbowDegree = 0;
int rainbowSpeed = 72;
glm::vec3 rainbowColor = glm::vec3(1.0f, 1.0f, 1.0f);
void rainbow(float dt) {
    if (useRainbowColor) {      // Update rainbow color
        rainbowSpeed += accerlation;
        rainbowDegree += rainbowSpeed * dt; // H increases by 72 degrees per second
        if (rainbowDegree >= 360.0f) rainbowDegree -= 360.0f;

        float H = rainbowDegree;    // Hue in degrees
        float S = 1.0f;             // Saturation
        float V = 1.0f;             // Value

        // HSV to RGB conversion
        float C = V * S; // Chroma
        float X = C * (1.0f - fabs(fmod(H / 60.0f, 2.0f) - 1.0f));
        float m = V - C;

        if (H >= 0.0f && H < 60.0f) {
            rainbowColor.r = C + m; rainbowColor.g = X + m; rainbowColor.b = m;
        } else if (H >= 60.0f && H < 120.0f) {
            rainbowColor.r = X + m; rainbowColor.g = C + m; rainbowColor.b = m;
        } else if (H >= 120.0f && H < 180.0f) {
            rainbowColor.r = m; rainbowColor.g = C + m; rainbowColor.b = X + m;
        } else if (H >= 180.0f && H < 240.0f) {
            rainbowColor.r = m; rainbowColor.g = X + m; rainbowColor.b = C + m;
        } else if (H >= 240.0f && H < 300.0f) {
            rainbowColor.r = X + m; rainbowColor.g = m; rainbowColor.b = C + m;
        } else {
            rainbowColor.r = C + m; rainbowColor.g = m; rainbowColor.b = X + m;
        }
    } else {
        rainbowColor = glm::vec3(1.0f, 1.0f, 1.0f);
    }
}


// render loop variables
double dt;
double initTime = 0;
double lastTime = glfwGetTime();
double currentTime;
double elapseTime;
double tmpTime, fixTime;
double accumTime = 0;

const int rotateCameraSpeed = 120;
const int cameraSpeed = 55;
const int pokeballSpeed = 20;
int rotatePokeballSpeed = 1000;
int rotateDittoSpeed = 50;

int changed = 0;

void update() {
    /*  Time  */
    if (initTime == 0) {
        lastTime = glfwGetTime();
        initTime = lastTime;
    }
    currentTime = glfwGetTime();
    dt = currentTime - lastTime;
    lastTime = currentTime;
    elapseTime = currentTime - initTime;
    /**********/

    if (elapseTime < 1.0) {
        0;
    } else if (elapseTime < 4.0) {
        camera.rotationY += rotateCameraSpeed * dt;
        camera.rotationY = (camera.rotationY > 360.0) ? 0.0 : camera.rotationY;
        camera.position.z += cameraSpeed * dt;
        pokeball.position = glm::vec3(78.0f, 65.0f, 0.0f);
    } else if (elapseTime < 6.4) {
        //camera.position.z -= (cameraSpeed) * dt;
        //if (camera.position.z < 120.0) camera.position.z = 120.0;
        pokeball.position.x -= 1.6f * pokeballSpeed * dt;
        pokeball.position.y -= 1.2f * pokeballSpeed * dt;
        pokeball.rotation.z += rotatePokeballSpeed * dt;
        pokeball.rotation.z = (pokeball.rotation.z > 360.0) ? 0.0 : pokeball.rotation.z;
        tmpTime = currentTime;
    } else if (elapseTime < 10.0) {
        //camera.position.z += (cameraSpeed + 50) * dt;
        // pokeball.rotation.z = 0.0f;
        rotatePokeballSpeed += 0.1 * dt;
        pokeball.rotation.z += rotatePokeballSpeed * dt;
        pokeball.rotation.z = (pokeball.rotation.z > 360.0) ? 0.0 : pokeball.rotation.z;
        fixTime = currentTime;
    } else if (elapseTime < 13.0) {
        camera.position.z -= (cameraSpeed + 100) * dt;
        if (camera.position.z < 120.0) camera.position.z = 120.0;
        rotatePokeballSpeed += 0.1 * dt;
        pokeball.rotation.z += rotatePokeballSpeed * dt;
        pokeball.rotation.z = (pokeball.rotation.z > 360.0) ? 0.0 : pokeball.rotation.z;
        pokeball.scale.x -= 0.6f * dt;
        pokeball.scale.y -= 0.6f * dt;
        pokeball.scale.z -= 0.6f * dt;
        if (pokeball.scale.x < 0) pokeball.scale = glm::vec3(0.0f);
    } else if (elapseTime < 16.0) {
        camera.position.z -= (cameraSpeed + 150) * dt;
        if (camera.position.z < 120.0) camera.position.z = 120.0;
        useRainbowColor = true;
        rainbow(dt);
        accerlation = 3;
        ditto.rotation.y += rotateDittoSpeed * dt;
        ditto.rotation.y = (ditto.rotation.y > 360.0) ? 0.0 : ditto.rotation.y;
        rotateDittoSpeed += accerlation;
        pokeball.position = glm::vec3(1000.0f, 50.0f, 0.0f);
    } else if (elapseTime < 20.0) {
        rainbow(dt);
        ditto.rotation.y += rotateDittoSpeed * dt;
        ditto.rotation.y = (ditto.rotation.y > 360.0) ? 0.0 : ditto.rotation.y;
        rotateDittoSpeed += accerlation;
        ditto.scale -= glm::vec3(0.1f * dt);
        foongus.scale = glm::vec3(0.1f, 0.1f, 0.1f);
    //} else if (elapseTime < 21.5) {
    //    foongus.position = glm::vec3(0.0f, 0.0f, 0.0f);
    //    foongus.scale += glm::vec3(0.2f * dt);
    //    useRainbowColor = false;
    //    rainbow(dt);
    //    ditto.rotation.y += rotateDittoSpeed * dt;
    //    ditto.rotation.y = (ditto.rotation.y > 360.0) ? 0.0 : ditto.rotation.y;
    //    rotateDittoSpeed += accerlation;
    } else if (elapseTime < 23.0) {
        useRainbowColor = false;
        accerlation = 0;
        rainbow(dt);
        //ditto.position = glm::vec3(1000.0f, 1000.0f, 0.0f);
        foongus.position = glm::vec3(0.0f, 0.0f, 0.0f);
        foongus.scale += glm::vec3(1.0f * dt);
        camera.position.y += (cameraSpeed - 20) * dt;
        camera.position.z += (cameraSpeed) * dt;
        pokeball.scale = glm::vec3(2.0f);
        pokeball.position = glm::vec3(100.0f, 60.0f, 0.0f);
    } else if (elapseTime < 25.0) {
        camera.position.z += (cameraSpeed + 50) * dt;
        camera.rotationY += 180 * dt;
        camera.rotationY = (camera.rotationY > 360.0) ? 0.0 : camera.rotationY;
    } else if (elapseTime < 26.0) {
        pokeball.position.x -= 5.0f * pokeballSpeed * dt;
        pokeball.position.y -= 2.5f * pokeballSpeed * dt;
    } else if (elapseTime < 28.0) {
        if (!changed){
            cubemapDir = "..\\..\\src\\asset\\texture\\skybox4\\";
            cubemap_setup();
            changed = 1;
        }
        //ditto.position = glm::vec3(1000.0f, 1000.0f, 0.0f);
        foongus.position = glm::vec3(0.0f, 0.0f, 0.0f);
        //pokeball.position = glm::vec3(1000.0f, 1000.0f, 0.0f);
        camera.rotationY += 180 * dt;
        camera.rotationY = (camera.rotationY > 360.0) ? 0.0 : camera.rotationY;
        accumTime += dt;
        if (accumTime > 0.1) {
            shaderProgramIndex = (shaderProgramIndex == 0 ? 5 : 0);
            accumTime = 0;
        }
    } else if (elapseTime < 28.5) {
        shaderProgramIndex = 5;
        fixTime = currentTime;
    } else if (elapseTime < 30.0) {
        shaderProgramIndex = 0;
    } else {
        lic.position = glm::vec3(0.0f);
        lic.rotation.y += 100 * dt;
        lic.rotation.y = (lic.rotation.y > 360.0) ? 0.0 : lic.rotation.y;
        lic.scale += glm::vec3(5.0f * dt);
        // foongus.position = glm::vec3(1000.0f, 1000.0f, 0.0f);
    }

    cameraModel = glm::mat4(1.0f);
    cameraModel = glm::rotate(cameraModel, glm::radians(camera.rotationY), camera.up);
    cameraModel = glm::translate(cameraModel, camera.position);

    foongusModel = glm::mat4(1.0f);
    foongusModel = glm::translate(foongusModel, foongus.position);
    foongusModel = glm::scale(foongusModel, foongus.scale);
    
    dittoModel = glm::mat4(1.0f);
    dittoModel = glm::scale(dittoModel, ditto.scale);
    dittoModel = glm::rotate(dittoModel, glm::radians(ditto.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    dittoModel = glm::translate(dittoModel, ditto.position);

    pokeballModel = glm::mat4(1.0f);
    pokeballModel = glm::scale(pokeballModel, pokeball.scale);
    pokeballModel = glm::translate(pokeballModel, pokeball.position);
    pokeballModel = glm::rotate(pokeballModel, glm::radians(pokeball.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    licModel = glm::mat4(1.0f);
    licModel = glm::scale(licModel, lic.scale);
    licModel = glm::translate(licModel, lic.position);
    licModel = glm::rotate(licModel, glm::radians(lic.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
}

void render(){

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate view, projection matrix
    glm::mat4 view = glm::lookAt(glm::vec3(cameraModel[3]), glm::vec3(0.0), camera.up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

    // Set matrix for view, projection, model transformation
    shaderPrograms[shaderProgramIndex]->use();
    shaderPrograms[shaderProgramIndex]->set_uniform_value("model", dittoModel);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("view", view);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("projection", projection);
    
    // TODO 1
    // Set uniform value for each shader program

    // Set light/material properties
    shaderPrograms[shaderProgramIndex]->set_uniform_value("light.position", light.position);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("light.ambient", light.ambient);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("light.diffuse", light.diffuse);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("light.specular", light.specular);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("material.ambient", material.ambient);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("material.diffuse", material.diffuse);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("material.specular", material.specular);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("material.gloss", material.gloss);

    // Set camera position
    shaderPrograms[shaderProgramIndex]->set_uniform_value("viewPos", glm::vec3(cameraModel[3]));

    // For metallic shader
    shaderPrograms[shaderProgramIndex]->set_uniform_value("bias", 0.2f);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("alpha", 0.4f);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("lightIntensity", 1.0f);

    // For glass shader
    shaderPrograms[shaderProgramIndex]->set_uniform_value("AIR_coeff", 1.0f);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("GLASS_coeff", 1.52f);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("scale", 0.7f);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("power", 2.0f);

    // Bind the cubemap texture for shaders that need it
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("skybox", 1);



    if (elapseTime < 20.0) { // Ditto 21.5
        // Rainbow
        shaderPrograms[shaderProgramIndex]->set_uniform_value("rainbowColor", rainbowColor);
        shaderPrograms[shaderProgramIndex]->set_uniform_value("exp", 0);
        shaderPrograms[shaderProgramIndex]->set_uniform_value("time", (float)(currentTime));
        shaderPrograms[shaderProgramIndex]->set_uniform_value("dup", 0);
        ditto.object->render();
    }

    if (elapseTime < 26.0) { // Ball
        shaderPrograms[shaderProgramIndex]->set_uniform_value("rainbowColor", rainbowColor);
        // Geometry shader
        if (elapseTime >= 6.4 && elapseTime < 16.0) {
            shaderPrograms[shaderProgramIndex]->set_uniform_value("exp", 0);
            shaderPrograms[shaderProgramIndex]->set_uniform_value("time", (float)(fixTime - tmpTime) / 2.5f);
            shaderPrograms[shaderProgramIndex]->set_uniform_value("dup", 1);
        } else {
            shaderPrograms[shaderProgramIndex]->set_uniform_value("exp", 0);
            shaderPrograms[shaderProgramIndex]->set_uniform_value("time", (float)(currentTime));
            shaderPrograms[shaderProgramIndex]->set_uniform_value("dup", 0);
        }
        shaderPrograms[shaderProgramIndex]->set_uniform_value("model", pokeballModel);
        pokeball.object->render();
    }

    { // Foongus
        shaderPrograms[shaderProgramIndex]->set_uniform_value("rainbowColor", rainbowColor);
        if (elapseTime >= 28.5 && (true || elapseTime < 30.0)) {
            shaderPrograms[shaderProgramIndex]->set_uniform_value("exp", 1);
            shaderPrograms[shaderProgramIndex]->set_uniform_value("time", (float)(currentTime - fixTime));
            shaderPrograms[shaderProgramIndex]->set_uniform_value("dup", 0);
        } else {
            shaderPrograms[shaderProgramIndex]->set_uniform_value("exp", 0);
            shaderPrograms[shaderProgramIndex]->set_uniform_value("time", (float)(currentTime));
            shaderPrograms[shaderProgramIndex]->set_uniform_value("dup", 0);
        }
        shaderPrograms[shaderProgramIndex]->set_uniform_value("model", foongusModel);
        foongus.object->render();
    }
    
    { // LIC
        shaderPrograms[shaderProgramIndex]->set_uniform_value("rainbowColor", rainbowColor);
        shaderPrograms[shaderProgramIndex]->set_uniform_value("exp", 0);
        shaderPrograms[shaderProgramIndex]->set_uniform_value("time", (float)(currentTime));
        shaderPrograms[shaderProgramIndex]->set_uniform_value("dup", 0);
        shaderPrograms[shaderProgramIndex]->set_uniform_value("model", licModel);
        lic.object->render();
    }

    shaderPrograms[shaderProgramIndex]->release();

    // TODO 4-2 
    // Rendering cubemap environment
    // Hint:
    // 1. All the needed things are already set up in cubemap_setup() function.
    // 2. You can use the vertices in cubemapVertices provided in the header/cube.h
    // 3. You need to set the view, projection matrix.
    // 4. Use the cubemapShader to render the cubemap 
    //    (refer to the above code to get an idea of how to use the shader program)
    cubemapShader->use();

    // Remove translation from the view matrix
    cubemapShader->set_uniform_value("view", glm::mat4(glm::mat3(view)));
    cubemapShader->set_uniform_value("projection", projection);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    cubemapShader->set_uniform_value("skybox", 1);

    // Render the cubemap
    glBindVertexArray(cubemapVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    cubemapShader->release();
}


int main() {
    
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW3-111550172", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
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

    // set viewport
    glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Setup texture, model, shader ...e.t.c
    setup();
    
    // Render loop, main logic can be found in update, render function
    while (!glfwWindowShouldClose(window)) {
        update(); 
        render(); 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// Add key callback
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {

    // The action is one of GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE.
    // Events with GLFW_PRESS and GLFW_RELEASE actions are emitted for every key press.
    // Most keys will also emit events with GLFW_REPEAT actions while a key is held down.
    // https://www.glfw.org/docs/3.3/input_guide.html

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // shader program selection
    if (key == GLFW_KEY_0 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shaderProgramIndex = 0;
    if (key == GLFW_KEY_1 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shaderProgramIndex = 1;
    if (key == GLFW_KEY_2 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shaderProgramIndex = 2;
    if (key == GLFW_KEY_3 && (action == GLFW_REPEAT || action == GLFW_PRESS))
        shaderProgramIndex = 3;
    if (key == GLFW_KEY_4 && (action == GLFW_REPEAT || action == GLFW_PRESS))
        shaderProgramIndex = 4;
    if (key == GLFW_KEY_5 && (action == GLFW_REPEAT || action == GLFW_PRESS))
        shaderProgramIndex = 5;

    // camera movement
    float cameraSpeed = 0.5f;
    if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
        camera.position.z -= 10.0;
    if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
        camera.position.z += 10.0;
    if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
        camera.rotationY -= 10.0;
    if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
        camera.rotationY += 10.0;
    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

// Loading cubemap texture
unsigned int loadCubemap(vector<std::string>& faces){

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(false);
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}  
