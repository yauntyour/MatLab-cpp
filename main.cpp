// main.cpp - 单窗口正确绘制多条曲线
#include "matlab.hpp"
#include <cmath>
#include <vector>
#include <iostream>

int main()
{
    using namespace matplot;

    std::vector<float> x = linspace(-4 * acos(-1), 4 * acos(-1), 100);
    std::vector<float> y1, y2;
    for (float xi : x)
    {
        y1.push_back(sin(xi));
        y2.push_back(cos(xi));
    }

    // === 创建一个窗口 ===
    Figure *fig1 = figure();
    auto axes1 = fig1->gca();
    axes1->setTitle("Sine and Cosine Waves");
    axes1->setXLabel("X");
    axes1->setYLabel("Y");
    axes1->grid(true);

    // === 向该 axes 添加曲线（关键修改！）===
    plot(axes1, x, y1, "b-");  // 蓝色实线
    plot(axes1, x, y2, "r--"); // 红色虚线

    // === 主渲染循环 ===
    std::vector<Figure *> figures = {fig1};
    bool anyOpen = true;

    while (anyOpen)
    {
        anyOpen = false;
        for (auto fig : figures)
        {
            if (fig && fig->window && !glfwWindowShouldClose(fig->window))
            {
                fig->render();
                glfwSwapBuffers(fig->window);
                anyOpen = true;
            }
        }
        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    // === 清理 ===
    delete fig1;
    glfwTerminate();

    return 0;
}