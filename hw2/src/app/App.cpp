#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "app/App.h"
#include "shape/Circle.h"
#include "shape/Triangle.h"
#include "util/Shader.h"
#include <fstream>


App & App::getInstance()
{
    static App instance;
    return instance;
}


glm::vec2 App::screenToNDC(double x, double y) {
    float ndcX = 2.0f * (float)x / kWindowWidth - 1.0f;
    float ndcY = 1.0f - 2.0f * (float)y / kWindowHeight;
    return glm::vec2(ndcX, ndcY);
}

bool App::canPlaceBall(const glm::vec2 &center, float radius) {

    if (center.x - radius < -1.0f) return false;
    if (center.x + radius >  1.0f) return false;
    if (center.y - radius < -1.0f) return false;
    if (center.y + radius >  1.0f) return false;

    for (const Ball &b : balls) {
        float dist = glm::distance(center, b.center);
        if (dist < (radius + b.r)) {
            return false;
        }
    }
    return true;
}

void App::spawnBall(const glm::vec2& center, float r, glm::vec2 &velocity){
    float px = (center.x*0.5f+0.5f)* App::kWindowWidth;
    float py = (center.y * -0.5f + 0.5f)*App::kWindowHeight;

    auto drawable=std::make_unique<Circle>(
        pCircleShader.get(),
        std::vector<glm::vec3>{{px,py,r*kWindowWidth*0.5f}}
    );

    Ball ball;
    ball.center=center;
    ball.r = r;
    ball.v = velocity;
    ball.drawable = drawable.get();

    shapes.push_back(std::move(drawable));
    balls.push_back(ball);
}

void App::updateBalls(){
    float dt = static_cast<float>(timeElapsedSinceLastFrame);
    dt = std::min(dt,0.016f);
    for (Ball &b:balls){
        b.center+=b.v*dt;
        
        // wall collisions
        if (b.center.x-b.r<-1.0f){
            b.center.x=-1.0f+b.r;
            b.v.x*=-1.0f;
        }
        if(b.center.x+b.r>1.0f){
            b.center.x=1.0f-b.r;
            b.v.x*=-1.0f;
        }
        if(b.center.y-b.r<-1.0f){
            b.center.y=-1.0f+b.r;
            b.v.y*=-1.0f;
        }
        if(b.center.y+b.r>1.0f){
            b.center.y=1.0f-b.r;
            b.v.y*=-1.0f;
        }
    }

    for(int i=0;i<balls.size()-1;i++){
        for(int j=i+1;j<balls.size();j++){
            Ball &a = balls[i];
            Ball &b = balls[j];

            float dist = glm::distance(a.center,b.center);
            if (dist<a.r+b.r){
                glm::vec2 normal = glm::normalize(b.center - a.center);
                glm::vec2 relative_velocity=b.v-a.v;

                float velAlongNormal = glm::dot(relative_velocity,normal);
                if(velAlongNormal>0) continue; // moving apart

                // elastic collision
                float jImpulse = -2.0f * velAlongNormal/2.0f;
                glm::vec2 impulse = jImpulse *normal;

                a.v-=impulse;
                b.v+=impulse;

                float overlap = (a.r+b.r)-dist;
                a.center-=0.5f*overlap*normal;
                b.center+=0.5f*overlap*normal;
            }
        }
    }


    for(int i=0;i<balls.size();i++){
        Ball &b = balls[i];

        float px = (b.center.x*0.5f+0.5f)* App::kWindowWidth;
        float py = (b.center.y * -0.5f + 0.5f)*App::kWindowHeight;
        float pr = b.r*App::kWindowWidth*0.5f;
        static_cast<Circle *>(b.drawable)->setCircleGeometry(px, py, pr);
    }
}

void App::run()
{
    while (!glfwWindowShouldClose(pWindow))
    {
        // Per-frame logic
        perFrameTimeLogic(pWindow);
        processKeyInput(pWindow);

        // Send render commands to OpenGL server
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        if(animationEnabled){
            updateBalls();
        };
        render();

        // Check and call events and swap the buffers
        glfwSwapBuffers(pWindow);
        glfwPollEvents();
    }
}


void App::cursorPosCallback(GLFWwindow * window, double xpos, double ypos)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

    app.mousePos.x = xpos;
    app.mousePos.y = App::kWindowHeight - ypos;

    if (app.mousePressed)
    {
        // // Note: Must calculate offset first, then update lastMouseLeftPressPos.
        // glm::dvec2 offset = app.mousePos - app.lastMouseLeftPressPos;
        app.lastMouseLeftPressPos = app.mousePos;
    }
}


void App::framebufferSizeCallback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void App::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_A && action == GLFW_RELEASE)
    {
        app.animationEnabled = !app.animationEnabled;
    }
    if(key==GLFW_KEY_1){
        std::cout<<"Key pressed is 1 mode should now be 1\n";
        app.mode = 1;
        app.shapes.clear();
        app.balls.clear();

        

    }
}


void App::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            app.mousePressed = true;
            app.lastMouseLeftClickPos = app.mousePos;
            app.lastMouseLeftPressPos = app.mousePos;
        }
        else if (action == GLFW_RELEASE)
        {
            app.mousePressed = false;

            //glm::vec2 ndc=app.screenToNDC(app.mousePos.x,app.mousePos.y);
            switch(app.mode){
                case 0:
                    break;
                case 1:
                {
                    std::cout<<"entering mode 1\n";
                    float r,vx,vy;
                    
                    // read in from etc/config.txt
                    std::ifstream fin("etc/config.txt");

                    if(!fin.is_open()){
                        std::cerr<<"Error: couldn't open file\n";
                        break;
                    }

                    fin>>r>>vx>>vy;

                    std::cout<<"r is "<<r<<" vx is "<<vx<<" vy is "<<vy<<"\n";
                    glm::vec2 ndc = app.screenToNDC(app.mousePos.x, app.mousePos.y);
                    glm::vec2 vel(vx,vy);

                    if(app.canPlaceBall(ndc,r)){
                        app.spawnBall(ndc,r,vel);
                    }
                    else{
                        std::cout<<"Cannot place ball interferes or goes out\n";
                    }
                    break;
                }
            }

            #ifdef DEBUG_MOUSE_POS
            std::cout << "[ " << app.mousePos.x << ' ' << app.mousePos.y << " ]\n";
            #endif
        }
    }
}


void App::scrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{

}


void App::perFrameTimeLogic(GLFWwindow * window)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

    double currentFrame = glfwGetTime();
    app.timeElapsedSinceLastFrame = currentFrame - app.lastFrameTimeStamp;
    app.lastFrameTimeStamp = currentFrame;
}


void App::processKeyInput(GLFWwindow * window)
{

}


App::App() : Window(kWindowWidth, kWindowHeight, kWindowName, nullptr, nullptr)
{
    // GLFW boilerplate.
    glfwSetWindowUserPointer(pWindow, this);
    glfwSetCursorPosCallback(pWindow, cursorPosCallback);
    glfwSetFramebufferSizeCallback(pWindow, framebufferSizeCallback);
    glfwSetKeyCallback(pWindow, keyCallback);
    glfwSetMouseButtonCallback(pWindow, mouseButtonCallback);
    glfwSetScrollCallback(pWindow, scrollCallback);

    // Global OpenGL pipeline settings
    glViewport(0, 0, kWindowWidth, kWindowHeight);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1.0f);
    glPointSize(1.0f);

    // Initialize shaders and objects-to-render;
    pTriangleShader = std::make_unique<Shader>("src/shader/triangle.vert.glsl",
                                               "src/shader/triangle.frag.glsl");
    pCircleShader = std::make_unique<Shader>("src/shader/circle.vert.glsl",
                                             "src/shader/circle.tesc.glsl",
                                             "src/shader/circle.tese.glsl",
                                             "src/shader/circle.frag.glsl");
    
                                       
    shapes.emplace_back(
            std::make_unique<Triangle>(
                    pTriangleShader.get(),
                    std::vector<Triangle::Vertex> {
                            // Vertex coordinate (screen-space coordinate), Vertex color
                            {{200.0f, 326.8f}, {1.0f, 0.0f, 0.0f}},
                            {{800.0f, 326.8f}, {0.0f, 1.0f, 0.0f}},
                            {{500.0f, 846.4f}, {0.0f, 0.0f, 1.0f}},
                    }
            )
    );
    

    shapes.emplace_back(
            std::make_unique<Circle>(
                    pCircleShader.get(),
                    std::vector<glm::vec3> {
                            // Coordinate (x, y) of the center and the radius (screen-space)
                            {200.0f, 326.8f, 200.0f},
                            {800.0f, 326.8f, 300.0f},
                            {500.0f, 846.4f, 400.0f}
                    }
            )
    );
}


void App::render()
{
    auto t = static_cast<float>(timeElapsedSinceLastFrame);

    // Update all shader uniforms.
    pTriangleShader->use();
    pTriangleShader->setFloat("windowWidth", kWindowWidth);
    pTriangleShader->setFloat("windowHeight", kWindowHeight);

    pCircleShader->use();
    pCircleShader->setFloat("windowWidth", kWindowWidth);
    pCircleShader->setFloat("windowHeight", kWindowHeight);

    // Render all shapes.
    for (auto & s : shapes)
    {
        s->render(t, animationEnabled);
    }
}
