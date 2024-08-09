//
// Created by YouTooLife_PC on 18.09.2020.
//

#include "Render.h"

#include <iostream>
#include <vector>
#include <cstdio>
#include <cmath>
#include <atomic>
#include <unordered_map>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#define NK_IMPLEMENTATION

#include <nuklear.h>
#include "nuklear_glfw_gl3.h"



// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <list>
#include <memory>

#include "../EEmbSimulator/EEmbDevice.h"

extern std::string SimPath;

namespace EEmbSimulator
{

    std::list<std::shared_ptr<EEmbPeriph>> periphs;

    void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    void mouse_callback(GLFWwindow *window, double xpos, double ypos);
    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
    void processInput(GLFWwindow *window);
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void DragAndDropCallback(GLFWwindow* window, int count,const char* paths[]);
    unsigned int loadTexture(char const *path);

    // timing
    float deltaTime = 0.0f; // time between current frame and last frame
    float lastFrame = 0.0f;

    void cursor_position_callback(GLFWwindow *window, double x, double y);
    void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

    int cp_x;
    int cp_y;
    int offset_cpx;
    int offset_cpy;
    int w_posx;
    int w_posy;
    int buttonEvent;

    bool isTarget;

    struct Targets
    {
        glm::mat4 view;
        glm::vec4 color;
        glm::vec2 pos;
        glm::vec2 size;
        uint8_t btnPos;
        // EEmbPeriph* periph;
    };

    

    std::shared_ptr<EEmbPeriph> hoverElement = nullptr;
    std::shared_ptr<EEmbPeriph> selectedElement = nullptr;
    std::shared_ptr<EEmbPeriph> movedElement = nullptr;

    glm::vec4 selectedMove = glm::vec4(0.0f);
    glm::vec4 howerColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

    glm::mat4 plcView;
    glm::mat4 screenView;

    struct nk_context *ctx;

    std::vector<std::string> typeList({"NONE",
                                       "IMG",
                                       "UI",
                                       "RS-485",
                                       "AO",
                                       "DO",
                                       "LED",
                                       "Display"
    });


    

    std::unordered_map<std::string, uint32_t> texturesMap;


    void RecreateTexture(std::shared_ptr<EEmbDisplay> pDisp)
    {
        int width = pDisp->screenDataWidth, height = pDisp->screenDataHeight;

        if (pDisp->screenData != nullptr)
        {
            GLenum format = GL_RGB;

            
            // glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msScreenTexture);
            // glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, format, width, height, GL_TRUE);   
        
            // glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_REPEAT);
            // glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            // //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            // glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            // glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

            glBindTexture(GL_TEXTURE_2D, pDisp->screenTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pDisp->screenData.get());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0);
        }

    }

    int Render::run()
    {

        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
        // glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);

        int count, windowWidth, windowHeight, monitorX, monitorY;

        // I am assuming that main monitor is in the 0 position
        GLFWmonitor **monitors = glfwGetMonitors(&count);
        const GLFWvidmode *videoMode = glfwGetVideoMode(monitors[0]);
        // width: 75% of the screen
        windowWidth = WINDOW_WIDTH; // static_cast<int>(videoMode->width * 0.5f);
        // aspect ratio 16 to 9
        windowHeight = WINDOW_HEIGHT; // static_cast<int>(videoMode->height * 0.5f);

        glfwGetMonitorPos(monitors[0], &monitorX, &monitorY);
        // glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        // glfw window creation
        // --------------------
        GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "EEmbSimulator", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetDropCallback(window, DragAndDropCallback);

        // Options
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetWindowAttrib(window, GLFW_VISIBLE, GLFW_FALSE);
        // glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);

        glfwSetWindowPos(window, monitorX + (videoMode->width - windowWidth) / 2,
                         monitorY + (videoMode->height - windowHeight) / 2);

        // glfwShowWindow(window);

        // tell GLFW to capture our mouse
        // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
        glewExperimental = GL_TRUE;
        // Initialize GLEW to setup the OpenGL Function pointers
        glewInit();

        struct nk_colorf bg;

        ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
        /* Load Fonts: if none of these are loaded a default font will be used  */
        /* Load Cursor: if you uncomment cursor loading please hide the cursor */
        {
            struct nk_font_atlas *atlas;
            nk_glfw3_font_stash_begin(&atlas);
            /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
            /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 14, 0);*/
            /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
            /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
            /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
            /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
            nk_glfw3_font_stash_end();
        }

        bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

        // configure global opengl state
        // -----------------------------
        // glEnable(GL_DEPTH_TEST);

        // build and compile our shader zprogram
        // ------------------------------------
        // ShaderOpenGL ourShader("../../../DMAEngine/Render/Shaders/SimpleShader.vsh", "../../../DMAEngine/Render/Shaders/SimpleShader.frag");

        // ShaderOpenGL lightingShader("ml.vs", "ml.fs");
        // ShaderOpenGL lightCubeShader("cl.vs", "cl.fs");
        ShaderOpenGL quadShader((SimPath + "quad.vert").c_str(), (SimPath + "quad.frag").c_str());
        ShaderOpenGL quadShader2((SimPath + "quad2.vert").c_str(), (SimPath + "quad2.frag").c_str());
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float vertices[] = {
            // positions          // texture coords
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // top right
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f   // top left
        };
        unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };
        unsigned int VBO, VAO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // load textures (we now use a utility function to keep the code more organized)

        addTexture((SimPath + "IS20C01D.png").c_str());
        // -----------------------------------------------------------------------------
        // unsigned int texture0 = loadTexture((SimPath + "IS20C01D.png").c_str());
        // glGenTextures(1, &screenTexture);

        // unsigned int multisampledFBO;
        // glGenFramebuffers(1, &multisampledFBO);

        // unsigned int intermediateFBO;
        // glGenFramebuffers(1, &intermediateFBO);

        // glGenTextures(1, &screenTexture);
        // glBindTexture(GL_TEXTURE_2D, screenTexture);

        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // glBindTexture(GL_TEXTURE_2D, 0);

        // glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // memset(screenData, 0xFF, 256 * 256 * 3);
        // RecreateTexture(screenData);

        // shader configuration
        // --------------------

        // load models
        // -----------
        // Model ourModel("Models/Survival_BackPack_2.fbx");

        // ShaderOpenGL ourShader("model_loading.vs", "model_loading.fs");

        // create transformations
        quadShader.Use();
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 view0 = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        model = glm::translate(model, glm::vec3(0.5f, 0.5f, 0.0f));
        // model = glm::scale(model, glm::vec3(200.0f, 300.0f, 0.0f));

        view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));
        view = glm::scale(view, glm::vec3(WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f));

        plcView = view;

        screenView = glm::mat4(1.f);
        screenView = glm::translate(screenView, glm::vec3(175.0f, WINDOW_HEIGHT - 366.0f, 0.0f));
        screenView = glm::scale(screenView, glm::vec3(201.f, 162, 0.0f));

        // projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        projection = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT); //, -1.0f, 1.0f);// , 0.1f, 100.f);
        // quadShader.setMat4("view", view);
        quadShader.setMat4("model", model);
        quadShader.setMat4("projection", projection);
        quadShader.setVec4("color", {1.0f, 1.0f, 1.0f, 1.0f});
        // quadShader.setVec2("texUV", { 1.0f, 1.0f });
        // quadShader.setVec2("texUV", { 0.625f, 0.5f });
        // quadShader.setInt("texture0", 0);

        quadShader2.Use();
        quadShader2.setMat4("model", model);
        quadShader2.setMat4("projection", projection);
        quadShader2.setVec4("color", {1.0f, 1.0f, 1.0f, 0.0f});
        quadShader2.setVec2("texUV", {1.0f, 1.0f});

        // uint32_t it = 0;
        // for (auto& target : targets)
        // {
        //     target.view = glm::translate(target.view,
        //         glm::vec3(target.pos.x, WINDOW_HEIGHT - target.pos.y - target.size.y, 0.0f));
        //     target.view = glm::scale(target.view, glm::vec3(target.size.x, target.size.y, 0.0f));
        //     target.btnPos = it;

        //     if (it >= EEMB_PERIPH_ID_UI1 && it <= EEMB_PERIPH_ID_UI11)
        //     {
        //         //target.periph = &UIs[it-EEMB_PERIPH_ID_UI1];
        //         //EEmbUI UIs[11];
        //         //MB_modbus_t uarts[2];
        //         //MB_aoutput_t AOs[3];
        //         //MB_doutput_t DOs[6];
        //     }
        //     else if (it >= EEMB_PERIPH_ID_RS485_1A && it <= EEMB_PERIPH_ID_RS485_2A)
        //     {
        //         //target.periph = &uarts[(it- EEMB_PERIPH_ID_RS485_1A)];
        //     }
        //     else if (it >= EEMB_PERIPH_ID_AO1 && it <= EEMB_PERIPH_ID_AO3)
        //     {
        //         //target.periph = &AOs[it - EEMB_PERIPH_ID_AO1];
        //     }
        //     else if (it >= EEMB_PERIPH_ID_RELAY1 && it <= EEMB_PERIPH_ID_RELAY6)
        //     {
        //         //target.periph = &DOs[it - EEMB_PERIPH_ID_RELAY1];
        //     }

        //     it++;
        // }

        // screenData[127 * 256 * 3] = 0xFF;
        // screenData[127 * 256 * 3 + 3] = 0xFF;
        // screenData[127 * 256 * 3 + 6] = 0xFF;
        // screenData[127 * 256 * 3 + 9] = 0xFF;

        // screenData[124 * 256 * 3 + 159 * 3] = 0xFF;
        // screenData[125 * 256 * 3 + 159 * 3] = 0xFF;
        // screenData[126 * 256 * 3 + 159 * 3] = 0xFF;
        // screenData[127 * 256 * 3 + 159 * 3] = 0xFF;
        // RecreateTexture(screenData);

        struct nk_color table[NK_COLOR_COUNT];
        table[NK_COLOR_TEXT] = nk_rgba(210, 210, 210, 255);
        table[NK_COLOR_WINDOW] = nk_rgba(57, 67, 71, 230);
        table[NK_COLOR_HEADER] = nk_rgba(51, 51, 56, 230);
        table[NK_COLOR_BORDER] = nk_rgba(46, 46, 46, 255);
        table[NK_COLOR_BUTTON] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_BUTTON_HOVER] = nk_rgba(58, 93, 121, 255);
        table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(63, 98, 126, 255);
        table[NK_COLOR_TOGGLE] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 53, 56, 255);
        table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SELECT] = nk_rgba(57, 67, 61, 255);
        table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SLIDER] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(48, 83, 111, 245);
        table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
        table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
        table[NK_COLOR_PROPERTY] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_EDIT] = nk_rgba(50, 58, 61, 225);
        table[NK_COLOR_EDIT_CURSOR] = nk_rgba(210, 210, 210, 255);
        table[NK_COLOR_COMBO] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART_COLOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
        table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
        table[NK_COLOR_TAB_HEADER] = nk_rgba(48, 83, 111, 255);
        nk_style_from_table(ctx, table);

        while (!glfwWindowShouldClose(window))
        {

            // SimulatorLoop();
            //  per-frame time logic
            //  --------------------
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // input
            // -----
            processInput(window);

            if (buttonEvent == 1)
            {
                glfwGetWindowPos(window, &w_posx, &w_posy);
                glfwSetWindowPos(window, w_posx + offset_cpx, w_posy + offset_cpy);
                offset_cpx = 0;
                offset_cpy = 0;
                cp_x += offset_cpx;
                cp_y += offset_cpy;
            }

            nk_glfw3_new_frame();

            /* GUI */
            // ctx->style.window.background = nk_rgba(255, 0, 0, 255);
            // ctx->style.window.contextual_border_color = nk_rgba(255, 0, 0, 255);
            // ctx->style.window.border_color = nk_rgba(255, 0, 0, 255);
            // ctx->style.window.combo_border_color = nk_rgba(255, 0, 0, 255);
            // ctx->style.window.group_border_color = nk_rgba(255, 0, 0, 255);
            // ctx->style.window.tooltip_border_color = nk_rgba(255, 0, 0, 255);
            // ctx->style.window.menu_border_color = nk_rgba(255, 0, 0, 255);
            // ctx->style.

            // if (currentTarget != nullptr)
            // {
            //     // if (nk_begin(ctx, labels[currentTarget->btnPos].c_str(),
            //     //     nk_rect(currentTarget->pos.x, currentTarget->pos.y-40, 100, 30), NK_WINDOW_NO_SCROLLBAR))
            //     // {
            //     //     nk_layout_row_dynamic(ctx, 25, 1);
            //     //     nk_label(ctx, labels[currentTarget->btnPos].c_str(), NK_TEXT_CENTERED);
            //     // }
            //     // nk_end(ctx);
            // }

            // for (auto& target : targets)
            // {
            //     // if (target.periph != nullptr)
            //     // {
            //     //     target.periph->drawGUI(target.btnPos);
            //     // }
            // }

            if (selectedElement != nullptr && movedElement != nullptr && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
                selectedElement->targetRect.x += selectedMove.z;
                selectedElement->targetRect.y += selectedMove.w;
                selectedMove.z = 0;
                selectedMove.w = 0;
            }

            if (nk_begin(ctx, "Menu", nk_rect(0, 0, 280, 200), 
                         NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR))
            {

                static uint32_t newElementType = 1;
                nk_layout_row_dynamic(ctx, 25, 3);
                if (nk_button_label(ctx, "+"))
                {
                    std::shared_ptr<EEmbPeriph> newElement = nullptr;

                    if (newElementType == PERIPH_TYPE_NONE)
                    {
                        newElement = std::make_shared<EEmbPeriph>();
                    }
                    else if (newElementType == PERIPH_TYPE_IMG)
                    {
                        auto newElement0 = std::make_shared<EEmbImg>();
                        newElement0->imgPath = texturesMap.begin()->first;
                        newElement = newElement0;
                        newElement->targetRect.w = 100;
                        newElement->targetRect.h = 100;
                        
                    }
                    else if (newElementType == PERIPH_TYPE_UI)
                    {
                        newElement = std::make_shared<EEmbUI>();
                    }
                    else if (newElementType == PERIPH_TYPE_RS_485)
                    {
                        auto newElement0 = std::make_shared<MB_modbus_t>();
                        newElement0->modbusInstance = std::make_unique<MBL_modbus_t>();
                        newElement = newElement0;
                    }
                    else if (newElementType == PERIPH_TYPE_AO)
                    {
                        newElement = std::make_shared<EEmbAO>();
                    }
                    else if (newElementType == PERIPH_TYPE_DO)
                    {
                        newElement = std::make_shared<EEmbDO>();
                    }
                    else if (newElementType == PERIPH_TYPE_DISPLAY)
                    {
                        auto newElement0 = std::make_shared<EEmbDisplay>();
                        newElement = newElement0;
                        newElement0->targetRect.w = 160;
                        newElement0->targetRect.h = 128;

                        glGenTextures(1, &newElement0->screenTexture);
                    }
                    else if (newElementType == PERIPH_TYPE_LED)
                    {
                        newElement = std::make_shared<EEmbLED>();
                    }

                    periphs.push_back(newElement);
                    newElement->targetRect.x = WINDOW_WIDTH / 2;
                    newElement->targetRect.y = WINDOW_HEIGHT / 2;

                    selectedElement = periphs.back();
                }
                if (nk_button_label(ctx, "-"))
                {
                    if (selectedElement != nullptr)
                    {
                        periphs.remove(selectedElement);
                    }
                }

                static const char *typeInput = typeList[1].c_str();
                // nk_vec2(nk_widget_width(ctx)

                if (nk_combo_begin_label(ctx, typeInput, nk_vec2(nk_widget_width(ctx), 80)))
                {
                    nk_layout_row_dynamic(ctx, 25, 1);
                    for (auto i = 1; i < typeList.size(); ++i)
                    {
                        auto str = typeList[i].c_str();
                        if (nk_combo_item_label(ctx, str, NK_TEXT_ALIGN_CENTERED))
                        {
                            typeInput = str;
                            newElementType = i;
                        }
                    }
                    nk_combo_end(ctx);
                }

                auto total_space = nk_window_get_content_region(ctx);
                nk_layout_row_dynamic(ctx, total_space.h-70, 1);
                if ( nk_group_begin(ctx, "elements", NK_WINDOW_BORDER) )
                {

                    nk_layout_row_dynamic(ctx, 25, 1);

                    for (auto &periph : periphs)
                    {
                        //nk_layout_row_begin(ctx, NK_DYNAMIC, 25, 2);
                        //nk_layout_row_push(ctx, 0.2f);

                        nk_bool isSelected = periph == selectedElement;
                        isSelected = nk_checkbox_label(ctx, periph->label.c_str(), &isSelected);
                        if (isSelected)
                        {
                            selectedElement = periph;
                        }

                        // nk_layout_row_push(ctx, 0.3f);
                        // nk_label(ctx, periph->label.c_str(), NK_TEXT_ALIGN_CENTERED);
                        //nk_layout_row_push(ctx, 0.7f);
                        //nk_label(ctx, periph->label.c_str(), NK_TEXT_ALIGN_CENTERED);
                    }
                    
                    nk_group_end(ctx);
                }

                nk_layout_row_dynamic(ctx, 25, 2);
                if (nk_button_label(ctx, "Build"))
                {
                    
                }
                if (nk_button_label(ctx, "Save"))
                {
                    
                }
            }
            nk_end(ctx);

            if (selectedElement != nullptr)
            {
                static uint32_t wX =  WINDOW_WIDTH-300;
                static uint32_t wY =  0;
                if (nk_begin(ctx, "Selected Element",
                             nk_rect(wX, wY, 250, 310), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_TITLE))
                {

                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_label(ctx, typeList[selectedElement->typeId].c_str(), NK_TEXT_ALIGN_CENTERED);

                    nk_layout_row_dynamic(ctx, 25, 2);
                    nk_property_float(ctx, "x: ", 0, &selectedElement->targetRect.x, WINDOW_WIDTH, 1, .1f);
                    nk_property_float(ctx, "y: ", 0, &selectedElement->targetRect.y, WINDOW_HEIGHT, 1, .1f);
                    nk_property_float(ctx, "w: ", 0, &selectedElement->targetRect.w, WINDOW_WIDTH, 1, .1f);
                    nk_property_float(ctx, "h: ", 0, &selectedElement->targetRect.h, WINDOW_HEIGHT, 1, .1f);

                    

                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_spacing(ctx, 1);


                    selectedElement->isEditLabel = nk_check_label(ctx, "is edit label", selectedElement->isEditLabel);

                    if (selectedElement->isEditLabel)
                    {

                        if (selectedElement->label.empty())
                        {
                            selectedElement->label = "label";
                        }

                        nk_label(ctx, "label: ", NK_TEXT_ALIGN_LEFT);
                        char inputBuffer[256];
                        int32_t inputBufferSize = selectedElement->label.size();
                        auto prevSize = inputBufferSize;
                        memcpy(inputBuffer, selectedElement->label.c_str(), inputBufferSize+1);
                        nk_edit_string(ctx, NK_EDIT_FIELD, inputBuffer, &inputBufferSize, 256, nk_filter_default);
                        selectedElement->label = std::string(inputBuffer, inputBufferSize);

                        float w = selectedElement->labelRect.w != 0 ?  selectedElement->labelRect.w : 100;
                        float h = selectedElement->labelRect.h != 0 ?  selectedElement->labelRect.h : 25;

                        auto x = selectedElement->labelRect.x == 0 ?  selectedElement->targetRect.x + 40 : selectedElement->labelRect.x;
                        auto y = selectedElement->labelRect.y == 0 ?  selectedElement->targetRect.y + 40 : selectedElement->labelRect.y;

                        nk_layout_row_dynamic(ctx, 25, 2);
                        nk_property_float(ctx, "label x: ", 0, &x, WINDOW_WIDTH, 1.0f, 1.0f);
                        nk_property_float(ctx, "label y: ", 0, &y, WINDOW_HEIGHT, 1.0f, 1.0f);
                        nk_property_float(ctx, "label w: ", 0, &w, WINDOW_WIDTH, 1.0f, 1.0f);
                        nk_property_float(ctx, "label h: ", 0, &h, WINDOW_HEIGHT, 1.0f, 1.0f);

                        selectedElement->labelRect.x = x;
                        selectedElement->labelRect.y = y;
                        selectedElement->labelRect.w = w;
                        selectedElement->labelRect.h = h;

                    }
                    
                }
                auto bounds = nk_window_get_bounds(ctx);
                wX = bounds.x;
                wY = bounds.y;
                nk_end(ctx);
            }




            // render
            // ------
            // glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_BLEND);
            glEnable(GL_MULTISAMPLE);
            // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

        

            quadShader.setVec4("color", {1.0f, 1.0f, 1.0f, 1.0f});

            auto isAnyShowLabel = false;
            for (auto &periph : periphs)
            {

                // auto modelView = glm::translate(glm::mat4(1.f), glm::vec3(periph->targetRect.x, WINDOW_HEIGHT - periph->targetRect.x - periph->targetRect.h, 0.0f));
                // modelView = glm::scale(modelView, glm::vec3(periph->targetRect.w, periph->targetRect.h, 0.0f));

                auto modelView = glm::translate(glm::mat4(1.f), glm::vec3(periph->targetRect.x, WINDOW_HEIGHT - periph->targetRect.y - periph->targetRect.h, 0.0f));
                modelView = glm::scale(modelView, glm::vec3(periph->targetRect.w, periph->targetRect.h, 0.0f));

                if (periph->typeId < PERIPH_TYPE_UI)
                {

                    auto pImg = std::static_pointer_cast<EEmbImg>(periph);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, texturesMap[pImg->imgPath]);
                    quadShader.Use();
                    quadShader.setMat4("view", modelView);
                    quadShader.setInt("texture0", 0);
                    quadShader.setVec2("texUV", { 1.0f, 1.0f });
                    //quadShader.setInt("figureType", (int)periph->targetHoverType);
                    //quadShader.setVec4("color", periph == hoverElement ? howerColor : (periph == selectedElement ? glm::vec4(0.9f, 0.9f, 0.01f, 0.35f) : glm::vec4(0.12f, 0.0f, 0.0f, 0.1f)));
                    //quadShader.setVec4("borderColor", periph == selectedElement ? glm::vec4(0.0f, 0.0f, 0.0f, 0.98f) : (periph == hoverElement ? glm::vec4(0.0f, 0.0f, 0.0f, 0.75f) : glm::vec4(0.0f, 0.0f, 0.0f, 0.45f)));
                    glBindVertexArray(VAO);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    
                }
                else if (periph->typeId < PERIPH_TYPE_DISPLAY)
                {

                    quadShader2.Use();
                    quadShader2.setMat4("view", modelView);
                    quadShader2.setInt("figureType", (int)periph->targetHoverType);
                    if (periph->typeId != PERIPH_TYPE_LED)
                    {
                        quadShader2.setVec4("color", periph == hoverElement ? howerColor : (periph == selectedElement ? glm::vec4(0.9f, 0.9f, 0.01f, 0.35f) : glm::vec4(0.12f, 0.0f, 0.0f, 0.1f)));

                    }
                    else
                    {
                        auto pLed = std::static_pointer_cast<EEmbLED>(periph);
                        

                        if (periph == selectedElement)
                        {
                            auto modelView2 = glm::translate(glm::mat4(1.f), glm::vec3(periph->targetRect.x-1, WINDOW_HEIGHT - periph->targetRect.y - periph->targetRect.h - 1, 0.0f));
                            modelView2 = glm::scale(modelView2, glm::vec3(periph->targetRect.w*1.1f, periph->targetRect.h*1.1f, 0.0f));
                            quadShader2.setMat4("view", modelView2);
                            quadShader2.setVec4("color",  pLed->value ? glm::vec4(1.0f-pLed->colorOn.x, 1.0f-pLed->colorOn.y, 1.0f-pLed->colorOn.w, 0.2f) :
                             glm::vec4(1.0f-pLed->colorOff.x, 1.0f-pLed->colorOff.y, 1.0f-pLed->colorOff.w, 0.2f));
                            quadShader2.setVec4("borderColor", periph == selectedElement ? glm::vec4(0.0f, 0.0f, 0.0f, 0.98f) : (periph == hoverElement ? glm::vec4(0.0f, 0.0f, 0.0f, 0.75f) : glm::vec4(0.0f, 0.0f, 0.0f, 0.45f)));
                            glBindVertexArray(VAO);
                            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                            quadShader2.setMat4("view", modelView);
                        }

                        quadShader2.setVec4("color", pLed->value ? glm::vec4(pLed->colorOn.x, pLed->colorOn.y, pLed->colorOn.w, pLed->colorOn.h) 
                            : glm::vec4(pLed->colorOff.x, pLed->colorOff.y, pLed->colorOff.w, pLed->colorOff.h));

                    }
                    
                    quadShader2.setVec4("borderColor", periph == selectedElement ? glm::vec4(0.0f, 0.0f, 0.0f, 0.98f) : (periph == hoverElement ? glm::vec4(0.0f, 0.0f, 0.0f, 0.75f) : glm::vec4(0.0f, 0.0f, 0.0f, 0.45f)));
                    glBindVertexArray(VAO);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                   
                }
                else if (periph->typeId >= PERIPH_TYPE_DISPLAY)
                {

                    auto pDisp = std::static_pointer_cast<EEmbDisplay>(periph);

                    if (pDisp->isRedraw)
                    {
                        RecreateTexture(pDisp);
                    }

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, pDisp->screenTexture);
                    quadShader.Use();
                    quadShader.setMat4("view", modelView);
                    quadShader.setInt("texture0", 0);
                    quadShader.setVec2("texUV", { ((float)pDisp->width)/((float)pDisp->screenDataWidth), ((float)pDisp->height)/((float)pDisp->screenDataHeight) });
                    auto currentDisplayBrights = pDisp->currentDisplayBrights.load();
                    quadShader.setVec4("color", { 1.f, 1.f, 1.f, currentDisplayBrights});
                    glBindVertexArray(VAO);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }

                
                periph->drawGUI();


                if (!isAnyShowLabel && (periph == hoverElement && !periph->label.empty() || periph == selectedElement && periph->isEditLabel)) 
                {
                    isAnyShowLabel = true;

                    float w = periph->labelRect.w != 0 ?  periph->labelRect.w : 100;
                    float h = periph->labelRect.h != 0 ?  periph->labelRect.h : 25;

                    auto x = periph->labelRect.x == 0 ?  periph->targetRect.x + 40 : periph->labelRect.x;
                    auto y = periph->labelRect.y == 0 ?  periph->targetRect.y + 40 : periph->labelRect.y;

                    if (nk_begin(ctx, "Selected Element Label",
                             nk_rect(x, y, w, h), NK_WINDOW_NO_SCROLLBAR))
                    {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_label(ctx, periph->label.c_str(), NK_TEXT_ALIGN_CENTERED);
                    }
                    nk_end(ctx);
                }
            }

            nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        // optional: de-allocate all resources once they've outlived their purpose:
        // ------------------------------------------------------------------------
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        // glDeleteFramebuffers(1, &multisampledFBO);
        // glDeleteFramebuffers(1, &intermediateFBO);

        nk_glfw3_shutdown();
        // glfw: terminate, clearing all previously allocated GLFW resources.
        // ------------------------------------------------------------------
        glfwTerminate();

        std::cout << "Simulator was stopped.\n";

        return 0;
    }


    void DragAndDropCallback(GLFWwindow* window, int count,const char* paths[])
    {
        for (auto i = 0; i < count; ++i)
        {
            addTexture(paths[i]);
        }
    }

    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void processInput(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // if (selectedElement != nullptr)
        // {
        //     if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
        //     {
        //         selectedElement->targetRect.y += 1;
        //     }

        //     if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
        //     {
        //         selectedElement->targetRect.y -= 1;
        //     }

        //     if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
        //     {
        //         selectedElement->targetRect.x -= 1;
        //     }

        //     if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
        //     {
        //         selectedElement->targetRect.x += 1;
        //     }
        // }
    }

    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (selectedElement != nullptr && hoverElement != nullptr && action == GLFW_RELEASE)
        {
            if (key == GLFW_KEY_W)
            {
                selectedElement->targetRect.y -= 1;
            }

            if (key == GLFW_KEY_S)
            {
                selectedElement->targetRect.y += 1;
            }

            if (key == GLFW_KEY_D)
            {
                selectedElement->targetRect.x += 1;
            }

            if (key == GLFW_KEY_A)
            {
                selectedElement->targetRect.x -= 1;
            }
        }
    }

    void cursor_position_callback(GLFWwindow *window, double x, double y)
    {

        // y = WINDOW_HEIGHT - y;

        if (buttonEvent == 1)
        {
            offset_cpx = x - cp_x;
            offset_cpy = y - cp_y;
        }

        if (selectedElement != nullptr && movedElement != nullptr && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            selectedMove.z = x - selectedMove.x;
            selectedMove.w = y - selectedMove.y;

            selectedMove.x = x;
            selectedMove.y = y;
        }

        auto prevHoverElement = hoverElement;

        hoverElement = nullptr;

        if (nk_window_is_any_hovered(ctx))
        {
            howerColor = glm::vec4(0.1f, 0.0f, 0.0f, 0.3f);
            return;
        }

        std::cerr << x << " : " << y << "\n";

        //for (auto &periph : periphs)
        for (auto it = periphs.rbegin(); it != periphs.rend(); ++it)
        {
            auto &periph = *it;
            if (x > periph->targetRect.x && x < (periph->targetRect.x + periph->targetRect.w) && y > periph->targetRect.y && y < (periph->targetRect.y + periph->targetRect.h))
            {
                hoverElement = periph;
                if (hoverElement != prevHoverElement)
                {
                    howerColor = glm::vec4(0.35f, 0.75f, 0.1f, 0.75f);
                }
                return;
            }
        }

        if (hoverElement == nullptr)
        {
            howerColor = glm::vec4(0.1f, 0.0f, 0.0f, 0.3f);
        }
    }

    void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
    {

        uint32_t fx = 0;
        uint32_t fy = 0;

        if (action == GLFW_PRESS)
        {
            double x, y;
            glfwGetCursorPos(window, &x, &y);

            // y = WINDOW_HEIGHT - y;

            fx = floor(x);
            fy = floor(y);
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            buttonEvent = 1;
            cp_x = fx;
            cp_y = fy;

            if (selectedElement != nullptr && selectedElement->isEditLabel)
            {
                selectedElement->labelRect.x = fx;
                selectedElement->labelRect.y = fy;
            }
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        {
            buttonEvent = 0;

            cp_x = 0;
            cp_y = 0;

        }

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {

            if (hoverElement != nullptr)
            {
                howerColor = glm::vec4(0.2f, 0.2f, 0.5f, 0.75f);
                selectedElement = hoverElement;
                selectedMove.x = fx;
                selectedMove.y = fy;
                //if (selectedElement->typeId >= PERIPH_TYPE_UI)
                {
                    movedElement = selectedElement;

                }
            }
            else
            {
                if (!nk_window_is_any_hovered(ctx))
                {
                    selectedElement = nullptr;
                }
            }
        }

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {

            if (hoverElement != nullptr)
            {
                howerColor = glm::vec4(0.35f, 0.75f, 0.1f, 0.75f);
                selectedElement = hoverElement;
                selectedElement->isShowMenu = true;
            }

            // if (currentTarget != nullptr)
            // {
            //     currentTarget->color = glm::vec4(0.35f, 0.75f, 0.1f, 0.75f);
            //     //btnsReleased |= (1 << currentTarget->btnPos);
            //     // if (currentTarget->periph != nullptr)
            //     // {
            //     //     currentTarget->periph->isShowMenu = true;
            //     // }
            // }

            movedElement = nullptr;
            selectedMove.x = 0;
            selectedMove.y = 0;
        }

        nk_glfw3_mouse_button_callback(window, button, action, mods);
    }

    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // ---------------------------------------------------------------------------------------------
    void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }

    // glfw: whenever the mouse moves, this callback is called
    // -------------------------------------------------------
    void mouse_callback(GLFWwindow *window, double xpos, double ypos)
    {
    }

    // glfw: whenever the mouse scroll wheel scrolls, this callback is called
    // ----------------------------------------------------------------------
    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
    {
        // std::cerr << "off: " << xoffset << " : " << yoffset << "\n";
        if (selectedElement != nullptr && selectedElement == hoverElement)
        {
            // std::cerr << "off: " << xoffset << " : " << yoffset << "\n";
            selectedElement->targetRect.w += yoffset;
            selectedElement->targetRect.h += yoffset;
        }

        nk_gflw3_scroll_callback(window, xoffset, yoffset);
        // camera.ProcessMouseScroll(yoffset);
    }



    void clearTextures()
    {
        for (auto& tex : texturesMap)
		{
			glDeleteTextures(1, &tex.second);
		}
	    texturesMap.clear();
        addTexture((SimPath + "IS20C01D.png").c_str());
    }


    void addTexture(const char* path)
    {
        auto texId = loadTexture(path);
        texturesMap[path] = texId;
    }

    unsigned int loadTexture(char const *path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = SOIL_load_image(path, &width, &height, &nrComponents, SOIL_LOAD_RGBA);
        if (data)
        {
            GLenum format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            SOIL_free_image_data(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            SOIL_free_image_data(data);
        }

        return textureID;
    }

    ShaderOpenGL::ShaderOpenGL(const GLchar *vertexPath, const GLchar *fragmentPath)
    {

        // 1. Retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensures ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::badbit);
        try
        {
            // Open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // Read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // Convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }

        const GLchar *vShaderCode = vertexCode.c_str();
        const GLchar *fShaderCode = fragmentCode.c_str();

        // 2. Compile shaders
        GLuint vertex, fragment;
        GLint success;
        GLchar infoLog[512];

        // Vertex ShaderOpenGL
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);
        // Print compile errors if any
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }

        // Fragment ShaderOpenGL
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);
        // Print compile errors if any
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }
        // ShaderOpenGL Program
        this->Program = glCreateProgram();
        glAttachShader(this->Program, vertex);
        glAttachShader(this->Program, fragment);
        glLinkProgram(this->Program);
        // Print linking errors if any
        glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(this->Program, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                      << infoLog << std::endl;
        }
        // Delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // Uses the current shader
    void ShaderOpenGL::Use()
    {
        glUseProgram(this->Program);
    }

}
