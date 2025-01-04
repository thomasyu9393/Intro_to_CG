#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <bits/stdc++.h>

#include "./header/Shader.h"
#include "./header/Object.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void drawModel(const string& name, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, int r, int g, int b);
void init();

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

// Shader
Shader *shader;

// Objects to display
Object *cube, *cylinder, *I, *C, *G, *tree_up, *tree_btn;

/* Ground Collision Detection Variables */
glm::vec3 mainBodyPosition(0.0f, 5.5f, 0.0f);
float y_threshold = 5.5f;

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ICG_2024_HW1", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);
    
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    // TODO: Enable depth test, face culling
    glEnable(GL_DEPTH_TEST);  // Enable depth testing
    glDepthFunc(GL_LEQUAL);   // Pass if the fragment depth <= the stored depth in the buffer
    glEnable(GL_CULL_FACE);   // Enable face culling
    glFrontFace(GL_CCW);      // Counter-clockwise winding order considered front
    glCullFace(GL_BACK);      // Cull the back faces

    // Display loop
	glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Initialize Object and Shader
    init();

    /* Rotation Controller */
    int w_counter = 0;
    int h_counter = 0;

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // render
        glClearColor(153/255.0, 204/255.0, 255/255.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use shader
        shader->use();
        
        
        /*=================== Example of creating model matrix ======================= 
        1. translate
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 1.0f, 0.0f));
        drawModel("Cube",model,view,projection,255, 0, 0);
        
        2. scale
        glm::mat4 model(1.0f);
        model = glm::scale(model, glm::vec3(0.5f, 1.0f, 2.0f)); 
        drawModel("Cube",model,view,projection,255, 0, 0);
        
        3. rotate
        glm::mat4 model(1.0f);
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        drawModel("Cube",model,view,projection,255, 0, 0);
        ==============================================================================*/

        // TODO: Create model, view, and perspective matrix
        glm::mat4 model(1.0f);
        glm::mat4 view = glm::lookAt(
                            glm::vec3(0, 50, 90), // Camera position
                            glm::vec3(0, 0, 0),   // Target position
                            glm::vec3(0, 1, 0)    // Up vector
                        );
        glm::mat4 projection = glm::perspective(
                                    glm::radians(45.0f), // fov: Specify field of view in radians
                                    (float)SCR_WIDTH / (float)SCR_HEIGHT, // Aspect ratio
                                    0.1f,   // Specify near plane
                                    1000.0f // Specify far plane
                                );

        // TODO: Helicopter (Body + Connector + Rotor Blades)
        // Body (At least 3 different color cubes)
        glm::mat4 mainBodyModel(1.0f);
        mainBodyModel = glm::translate(mainBodyModel, mainBodyPosition);
        mainBodyModel = glm::scale(mainBodyModel, glm::vec3(10.0f, 10.0f, 10.0f));
        drawModel("Cube", mainBodyModel, view, projection, 102, 178, 255);

        glm::mat4 secondBodyModel(1.0f);
        secondBodyModel = glm::translate(secondBodyModel, mainBodyPosition + glm::vec3(8.0f, 0.0f, 0.0f));
        secondBodyModel = glm::scale(secondBodyModel, glm::vec3(8.0f, 8.0f, 8.0f));
        drawModel("Cube", secondBodyModel, view, projection, 102, 102, 255);

        glm::mat4 thirdBodyModel(1.0f);
        thirdBodyModel = glm::translate(thirdBodyModel, mainBodyPosition + glm::vec3(14.0f, 0.0f, 0.0f));
        thirdBodyModel = glm::scale(thirdBodyModel, glm::vec3(6.0f, 6.0f, 6.0f));
        drawModel("Cube", thirdBodyModel, view, projection, 178, 102, 255);

        // Connector (One cube)
        //     - On the body
        //     - Rotate: 5 degrees/frame about +y axis
        glm::mat4 connectorModel(1.0f);
        connectorModel = glm::translate(connectorModel, mainBodyPosition + glm::vec3(0.0f, 6.0f, 0.0f));
        connectorModel = glm::scale(connectorModel, glm::vec3(2.0f, 4.0f, 2.0f));
        connectorModel = glm::rotate(connectorModel, (float)h_counter * glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        drawModel("Cube", connectorModel, view, projection, 255, 255, 255);

        // Rotor Blades (4 different color cubes)
        //     - On the connector
        //     - Each cubes is offset by 90 degrees
        // You can be creative in design !!
        int R[4] = {0, 205, 139, 255};
        int G[4] = {0, 133, 69, 222};
        int B[4] = {0, 63, 19, 173};
        for (int i = 0; i < 4; ++i) {
            glm::mat4 bladeModel(1.0f);
            bladeModel = glm::rotate(connectorModel, (float)((h_counter + 18 * i) % 72) * glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            bladeModel = glm::translate(bladeModel, glm::vec3(2.5f, 0.4f, 0.0f));
            bladeModel = glm::scale(bladeModel, glm::vec3(4.0f, 0.2f, 1.0f));
            drawModel("Cube", bladeModel, view, projection, R[i], G[i], B[i]);
        }

        // TODO: Draw Ground (Scale: (100, 1, 120))
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

        glm::mat4 groundModel(1.0f);
        groundModel = glm::scale(model, glm::vec3(100.0f, 1.0f, 120.0f));
        drawModel("Cube", groundModel, view, projection, 0, 255, 0);

        // TODO: Draw Tree1 (Position: (-30, 0, -20) relative to the Ground, Scale: (4, 4, 4))
        glm::mat4 tree1Model(1.0f);
        tree1Model = glm::translate(model, glm::vec3(-30.0f, 0.0f, -20.0f));
        tree1Model = glm::scale(tree1Model, glm::vec3(4.0f, 4.0f, 4.0f));
        drawModel("Tree_up", tree1Model, view, projection, 0, 128, 0);
        drawModel("Tree_down", tree1Model, view, projection, 165, 42, 42);

        // TODO: Draw Tree2 (Position: (30, 0, 20) relative to the Ground, Scale: (3.5, 3.5, 3.5))
        glm::mat4 tree2Model(1.0f);
        tree2Model = glm::translate(model, glm::vec3(30.0f, 0.0f, 20.0f));
        tree2Model = glm::scale(tree2Model, glm::vec3(3.5f, 3.5f, 3.5f));
        drawModel("Tree_up", tree2Model, view, projection, 0, 128, 0);
        drawModel("Tree_down", tree2Model, view, projection, 165, 42, 42);

        // TODO: Draw C
        glm::mat4 CModel(1.0f);
        CModel = glm::translate(CModel, glm::vec3(-25.0f, 2.0f, 25.0f));
        drawModel("C", CModel, view, projection, 255, 153, 204);

        // TODO: Draw I (Rotate: 0.8 degrees/ frame about +y axis around the C)
        glm::mat4 IModel(1.0f);
        IModel = glm::rotate(CModel, (float)w_counter * glm::radians(0.8f), glm::vec3(0.0f, 1.0f, 0.0f));
        IModel = glm::translate(IModel, glm::vec3(-12.0f, 0.0f, 0.0f));
        drawModel("I", IModel, view, projection, 255, 153, 204);

        // TODO: Draw G (Rotate: 0.8 degrees/ frame about +y axis around the C)
        glm::mat4 GModel(1.0f);
        GModel = glm::rotate(CModel, (float)w_counter * glm::radians(0.8f), glm::vec3(0.0f, 1.0f, 0.0f));
        GModel = glm::translate(GModel, glm::vec3(12.0f, 0.0f, 0.0f));
        drawModel("G", GModel, view, projection, 255, 153, 204);

        // TODO: Control rotation
        w_counter = (w_counter + 1) % 450;
        h_counter = (h_counter + 1) % 72;

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// TODO:
// Add key callback
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
        
    // The action is one of GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE. 
    // Events with GLFW_PRESS and GLFW_RELEASE actions are emitted for every key press.
    // Most keys will also emit events with GLFW_REPEAT actions while a key is held down.
    // https://www.glfw.org/docs/3.3/input_guide.html

    // 1. Press W to move the helicopter in the Y direction by 1
    // 2. Press S to move the helicopter in the Y direction by -1 
    // 3. Press D to move the helicopter in the X direction by 1
    // 4. Press A to move the helicopter in the X direction by -1
    // 5. The helicopter cannot fly below the ground.
    // ** Both GLFW_PRESS and GLFW_REPEAT are OK! You can try it out and compare the differences. **
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_W) {
            mainBodyPosition.y += 1.0f;
        } else if (key == GLFW_KEY_S) {
            mainBodyPosition.y -= 1.0f;
        } else if (key == GLFW_KEY_D) {
            mainBodyPosition.x += 1.0f;
        } else if (key == GLFW_KEY_A) {
            mainBodyPosition.x -= 1.0f;
        }
        if (mainBodyPosition.y < y_threshold) {
            mainBodyPosition.y = y_threshold;
        }
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void drawModel(const string& name, 
               const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, 
               int r, int g, int b){

    shader->set_uniform("projection",projection);
    shader->set_uniform("view",view);
    shader->set_uniform("model",model);
    shader->set_uniform("objectColor", glm::vec3(r/255.0,g/255.0,b/255.0));
    
    if(name == "Cube"){
        cube->draw();
    }else if(name == "Cylinder"){
        cylinder->draw();
    }else if(name == "I"){
        I->draw();
    }else if(name == "C"){
        C->draw();
    }else if(name == "G"){
        G->draw();
    }else if(name == "Tree_down"){
        tree_btn->draw();
    }else if(name == "Tree_up"){
        tree_up->draw();
    }
}

void init(){
    #if defined(__linux__) || defined(__APPLE__)
        string dirShader = "../../src/shaders/";
        string dirAsset = "../../src/asset/";
    #else
        string dirShader = "..\\..\\src\\shaders\\";
        string dirAsset = "..\\..\\src\\asset\\";
    #endif

    string v = dirShader + "easy.vert";
    string f = dirShader + "easy.frag";
    shader = new Shader(v.c_str(),f.c_str());
    cube = new Object(dirAsset + "cube.obj");
    cylinder = new Object(dirAsset + "cylinder.obj");
    I = new Object(dirAsset + "I.obj");
    C = new Object(dirAsset + "C.obj");
    G = new Object(dirAsset + "G.obj");
    tree_up = new Object(dirAsset + "tree_up.obj");
    tree_btn = new Object(dirAsset + "tree_btn.obj");
}