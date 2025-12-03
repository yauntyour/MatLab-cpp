// main.cpp - 示波器功能（固定时间窗口滚动显示）
#include "matlab.hpp"
#include <cmath>
#include <vector>
#include <iostream>

int main()
{
    using namespace matplot;

    Figure *fig = figure();
    auto axes = fig->gca();
    axes->setTitle("Oscilloscope - Real-time Sine Wave");
    axes->setXLabel("Time (s)");
    axes->setYLabel("Amplitude");
    axes->grid(true);
    axes->setDataRange(-1.8f, 1.8f, -3.0f, 3.0f);

    std::vector<float> initX, initY;
    auto line = plot(axes, initX, initY, "b-");
    line->setMaxPoints(50);
    line->setAutoUpdateAxes(false);

    float currentTime = axes->getdataXMin();
    const float dt = 0.05f;
    const float updateTime = 0.02f;
    auto lastUpdateTime = std::chrono::steady_clock::now();
    float b = 0;

    while (!glfwWindowShouldClose(fig->window))
    {
        auto now = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration<float>(now - lastUpdateTime).count();

        if (elapsed >= updateTime)
        {
            float y = pow(abs(currentTime + b), 0.666666) + 0.9 * pow(3.36 - (abs(currentTime + b) * abs(currentTime + b)), 0.5) * sin(3.141592 * abs(currentTime + b) / 0.1314);
            line->appendData(currentTime, y);
            currentTime += dt;
            if (currentTime >= axes->getdataXMax())
            {
                currentTime = axes->getdataXMin();
            }

            lastUpdateTime = now;
        }

        // 渲染
        fig->render();
        glfwSwapBuffers(fig->window);
        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    delete fig;
    glfwTerminate();
    return 0;
}