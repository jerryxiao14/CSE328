#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "app/App.h"
#include "shape/Pixel.h"
#include "util/Shader.h"



App & App::getInstance()
{
    static App instance;
    return instance;
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

    auto pixel = dynamic_cast<Pixel*>(app.shapes.front().get());

    if (app.mousePressed)
    {
        // // Note: Must calculate offset first, then update lastMouseLeftPressPos.
        // glm::dvec2 offset = app.mousePos - app.lastMouseLeftPressPos;
        app.lastMouseLeftPressPos = app.mousePos;
    }
    


    if(app.currentMode==1 && app.showPreview){
        
        auto x0 = static_cast<int>(app.lastMouseLeftClickPos.x);
        auto y0 = static_cast<int>(app.lastMouseLeftClickPos.y);
        auto x1 = static_cast<int>(app.mousePos.x);
        auto y1 = static_cast<int>(app.mousePos.y);
        
        pixel->path.clear();
        bresenhamLine(pixel->path,x0,y0,x1,y1);
        pixel->dirty=true;

    }   
    else if(app.currentMode==3&&!app.polylinePoints.empty()){
        pixel->path.clear();
        for(size_t i=0;i+1<app.polylinePoints.size();++i){
            int x0 = static_cast<int>(app.polylinePoints[i].x);
            int y0 = static_cast<int>(app.polylinePoints[i].y);
            int x1 = static_cast<int>(app.polylinePoints[i+1].x);
            int y1 = static_cast<int>(app.polylinePoints[i+1].y);
            bresenhamLine(pixel->path,x0,y0,x1,y1);
        }
        auto x0 = static_cast<int>(app.lastMouseLeftClickPos.x);
        auto y0 = static_cast<int>(app.lastMouseLeftClickPos.y);
        auto x1 = static_cast<int>(app.mousePos.x);
        auto y1 = static_cast<int>(app.mousePos.y);
        bresenhamLine(pixel->path,x0,y0,x1,y1);
        if(app.closePolygon){
            x0 = x1;
            y0 = y1;
            x1 = static_cast<int>(app.polylinePoints[0].x);
            y1 = static_cast<int>(app.polylinePoints[0].y);
            bresenhamLine(pixel->path,x0,y0,x1,y1);
        }
        pixel->dirty=true;
    }
    else if(app.currentMode==4&&app.showPreview){
        pixel->path.clear();
            auto x0 = static_cast<int>(app.lastMouseLeftClickPos.x);
            auto y0 = static_cast<int>(app.lastMouseLeftClickPos.y);
            auto x1 = static_cast<int>(app.mousePos.x);
            auto y1 = static_cast<int>(app.mousePos.y);
        if(app.circleMode){
            bresenhamCircle(pixel->path,x0,y0,x1,y1);
        }
        else{
            bresenhamEllipse(pixel->path,x0,y0,x1,y1);
        }
        pixel->dirty = true;
    }
    // Display a preview line which moves with the mouse cursor iff.
    // the most-recent mouse click is left click.
    // showPreview is controlled by mouseButtonCallback.
    
    /*
    if (app.showPreview)
    {
        auto pixel = dynamic_cast<Pixel *>(app.shapes.front().get());

        auto x0 = static_cast<int>(app.lastMouseLeftPressPos.x);
        auto y0 = static_cast<int>(app.lastMouseLeftPressPos.y);
        auto x1 = static_cast<int>(app.mousePos.x);
        auto y1 = static_cast<int>(app.mousePos.y);

        pixel->path.clear();
        bresenhamLine(pixel->path, x0, y0, x1, y1);
        pixel->dirty = true;
    }
    */
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
        app.currentMode=1;// single line
        app.showPreview = false;
        app.mousePressed = false;
        app.polylinePoints.clear();
    }
    else if(key==GLFW_KEY_3){
        app.currentMode = 3;
        app.showPreview = false;
        app.mousePressed = false;
        app.polylinePoints.clear();
    }
    else if(key==GLFW_KEY_4){
        app.currentMode = 4;
        app.showPreview = false;
        app.mousePressed = false;
        app.polylinePoints.clear();
    }
    else if(key==GLFW_KEY_C){
        // logic for closing polygon
        if(app.currentMode==3){
            if(action!=GLFW_RELEASE){
                app.closePolygon=true;
                /*
                int n = app.polylinePoints.size();
                if(n){
                    int x0 = app.polylinePoints[n-1].x;
                    int y0 = app.polylinePoints[n-1].y;
                    int x1 = app.polylinePoints[0].x;
                    int y1 = app.polylinePoints[0].y;
                    auto pixel = dynamic_cast<Pixel*>(app.shapes.front().get());
                    bresenhamLine(pixel->path,x0,y0,x1,y1);
                    pixel->dirty = true;
                }
                */
            }
            else app.closePolygon=false;
        }
    
    }
    else if(key==GLFW_KEY_LEFT_SHIFT||key==GLFW_KEY_RIGHT_SHIFT){
        if(action!=GLFW_RELEASE) app.circleMode=true;
        else app.circleMode = false;
    }

}


void App::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            if(app.currentMode==3){
                app.polylinePoints.push_back(app.mousePos);
            }
            app.mousePressed = true;
            app.lastMouseLeftClickPos = app.mousePos;
            app.lastMouseLeftPressPos = app.mousePos;
        }
        else if (action == GLFW_RELEASE)
        {
            app.mousePressed = false;
            app.showPreview = true;

            #ifdef DEBUG_MOUSE_POS
            std::cout << "[ " << app.mousePos.x << ' ' << app.mousePos.y << " ]\n";
            #endif
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_RELEASE && app.currentMode==1)
        {
            app.showPreview = false;
        }
        else if(action==GLFW_RELEASE && app.currentMode==3){
            app.polylinePoints.push_back(app.mousePos);
            if(app.closePolygon)
            {
                if(!app.polylinePoints.empty()){
                    app.polylinePoints.push_back(app.polylinePoints.front());
                }
            }
            auto pixel = dynamic_cast<Pixel*>(app.shapes.front().get());
            pixel->path.clear();
            for(size_t i=0;i+1<app.polylinePoints.size();++i){
                int x0 = static_cast<int>(app.polylinePoints[i].x);
                int y0 = static_cast<int>(app.polylinePoints[i].y);
                int x1 = static_cast<int>(app.polylinePoints[i+1].x);
                int y1 = static_cast<int>(app.polylinePoints[i+1].y);
                bresenhamLine(pixel->path,x0,y0,x1,y1);
            }
            pixel->dirty = true;

            app.mousePressed = false;
            app.polylinePoints.clear();
        }
        else if(action==GLFW_RELEASE && app.currentMode==4){
            app.showPreview=false;
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

void App::bresenhamEllipse(std::vector<Pixel::Vertex> &path, int xc, int yc, int x, int y){
    int a = abs(xc-x);
    int b = abs(yc-y);

    int x0 = 0;
    int y0 = b;

    int a2 = a*a;
    int b2 = b*b;

    int twoa2 = a2<<1;
    int twob2 = b2<<1;

    int px = 0;
    int py = twoa2*y0;

    auto plot = [&](int xc, int yc, int x, int y){
        path.emplace_back(xc+x,yc+y,1.0f,1.0f,1.0f);
        path.emplace_back(xc-x,yc+y,1.0f,1.0f,1.0f);
        path.emplace_back(xc+x,yc-y,1.0f,1.0f,1.0f);
        path.emplace_back(xc-x,yc-y,1.0f,1.0f,1.0f);
    };

    plot(xc,yc,x0,y0);

    // region 1 
    int p = std::round(b2-a2*b+0.25*a2);
    while(px<py){
        x0++;
        px+=twob2;
        if(p<0) p+=b2+px;
        else{
            y0--;
            py-=twoa2;
            p+=b2+px-py;
        }
        plot(xc,yc,x0,y0);
    }

    // region 2
    p = round(b2*(x0+0.5)*(x0+0.5)+a2*(y0-1)*(y0-1)-a2*b2);
    while(y0>=0){
        y0--;
        py-=twoa2;
        if(p>0){
            p+=a2-py;
        }
        else{
            x0++;
            px+=twob2;
            p+=a2-py+px;
        }
        plot(xc,yc,x0,y0);
    }
}

void App::bresenhamCircle(std::vector<Pixel::Vertex> &path, int xc, int yc, int x, int y)
{
    auto dist = [&](int dx, int dy){
        return static_cast<int>(std::round(std::sqrt(dx*dx+dy*dy)));
    };

    int r = dist(xc-x,yc-y);
    int x0 = 0;
    int y0 = r;
    int d = 1-r;


    auto plot = [&](int dx, int dy){
        path.emplace_back(xc+dx,yc+dy,1.0f,1.0f,1.0f);
        path.emplace_back(xc+dx,yc-dy,1.0f,1.0f,1.0f);
        path.emplace_back(xc-dx,yc+dy,1.0f,1.0f,1.0f);
        path.emplace_back(xc-dx,yc-dy,1.0f,1.0f,1.0f);
        path.emplace_back(xc+dy,yc+dx,1.0f,1.0f,1.0f);
        path.emplace_back(xc+dy,yc-dx,1.0f,1.0f,1.0f);
        path.emplace_back(xc-dy,yc+dx,1.0f,1.0f,1.0f);
        path.emplace_back(xc-dy,yc-dx,1.0f,1.0f,1.0f);
    };
    
    plot(x0,y0); 
    while(x0<y0){
        x0+=1;
        if(d<0){
            d+=((x0<<1)+1);
        }
        else{
            y0-=1;
            d+=(((x0-y0)<<1)+1);
        }
        plot(x0,y0);
    }

}
void App::bresenhamLine(std::vector<Pixel::Vertex> & path, int x0, int y0, int x1, int y1)
{
    if(x0>x1){
        std::swap(x0,x1);
        std::swap(y0,y1);
    }

    int ystep = (y1>y0)? 1:-1;

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int p = 2 * dy - dx;
    int twoDy = 2 * dy;
    int twoDyMinusDx = 2 * (dy - dx);

    int twoDx = dx<<1;
    int twoDxMinusDy = (dx-dy)<<1;

    int x = x0;
    int y = y0;

    path.emplace_back(x, y, 1.0f, 1.0f, 1.0f);

    if(dx>=dy){
        while (x < x1)
        {
            ++x;

            if (p < 0)
            {
                p += twoDy;
            }
            else
            {
                y+=ystep;
                p += twoDyMinusDx;
            }

            path.emplace_back(x, y, 1.0f, 1.0f, 1.0f);
        }
    }
    else{
        p = 2*dx-dy;
        while (y != y1)
        {
            y+=ystep;

            if (p < 0)
            {
                p += twoDx;
            }
            else
            {
                ++x;
                p += twoDxMinusDy;
            }

            path.emplace_back(x, y, 1.0f, 1.0f, 1.0f);
        }
    }
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    glLineWidth(1.0f);
    glPointSize(1.0f);

    // Initialize shaders and objects-to-render;
    pPixelShader = std::make_unique<Shader>("src/shader/pixel.vert.glsl",
                                            "src/shader/pixel.frag.glsl");

    shapes.emplace_back(std::make_unique<Pixel>(pPixelShader.get()));
}


void App::render()
{
    // Update all shader uniforms.
    pPixelShader->use();
    pPixelShader->setFloat("windowWidth", kWindowWidth);
    pPixelShader->setFloat("windowHeight", kWindowHeight);

    // Render all shapes.
    for (auto & s : shapes)
    {
        s->render();
    }
}
