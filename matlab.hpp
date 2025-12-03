#pragma once

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <thread>
#include <chrono>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace matplot
{

    struct Color
    {
        float r, g, b, a;
        Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f)
            : r(r), g(g), b(b), a(a) {}

        static Color Red() { return Color(1.0f, 0.0f, 0.0f); }
        static Color Green() { return Color(0.0f, 1.0f, 0.0f); }
        static Color Blue() { return Color(0.0f, 0.0f, 1.0f); }
        static Color Black() { return Color(0.0f, 0.0f, 0.0f); }
        static Color White() { return Color(1.0f, 1.0f, 1.0f); }

        static Color fromHSV(float h, float s, float v, float alpha = 1.0f)
        {
            h = std::fmod(h, 360.0f);
            if (h < 0)
                h += 360.0f;
            if (s == 0.0f)
                return Color(v, v, v, alpha);

            float c = v * s;
            float hp = h / 60.0f;
            float x = c * (1.0f - std::abs(std::fmod(hp, 2.0f) - 1.0f));

            float r1, g1, b1;
            if (hp >= 0 && hp < 1)
            {
                r1 = c;
                g1 = x;
                b1 = 0;
            }
            else if (hp >= 1 && hp < 2)
            {
                r1 = x;
                g1 = c;
                b1 = 0;
            }
            else if (hp >= 2 && hp < 3)
            {
                r1 = 0;
                g1 = c;
                b1 = x;
            }
            else if (hp >= 3 && hp < 4)
            {
                r1 = 0;
                g1 = x;
                b1 = c;
            }
            else if (hp >= 4 && hp < 5)
            {
                r1 = x;
                g1 = 0;
                b1 = c;
            }
            else if (hp >= 5 && hp < 6)
            {
                r1 = c;
                g1 = 0;
                b1 = x;
            }
            else
            {
                r1 = 0;
                g1 = 0;
                b1 = 0;
            }

            float m = v - c;
            return Color(r1 + m, g1 + m, b1 + m, alpha);
        }
    };

    enum LineStyle
    {
        LINE_SOLID,
        LINE_DASHED,
        LINE_DOTTED,
        LINE_DASH_DOT
    };
    enum MarkerStyle
    {
        MARKER_NONE,
        MARKER_CIRCLE,
        MARKER_SQUARE,
        MARKER_TRIANGLE,
        MARKER_CROSS,
        MARKER_PLUS
    };

    class PlotObject
    {
    public:
        virtual ~PlotObject() = default;
        virtual void draw() const = 0;
        virtual void update() = 0;
    };

    class Axes
    {
    private:
        float dataXMin = -10, dataXMax = 10;
        float dataYMin = -10, dataYMax = 10;

        std::string title, xLabel, yLabel;
        bool gridEnabled = true;
        Color backgroundColor = Color::White();
        std::vector<std::shared_ptr<PlotObject>> children;

    public:
        void setDataRange(float xMin, float xMax, float yMin, float yMax)
        {
            dataXMin = xMin;
            dataXMax = xMax;
            dataYMin = yMin;
            dataYMax = yMax;
        }

        float getdataXMin() { return dataXMin; };
        float getdataXMax() { return dataXMax; };
        float getdataYMin() { return dataYMin; };
        float getdataYMax() { return dataYMax; };

        void setTitle(const std::string &t) { title = t; }
        void setXLabel(const std::string &l) { xLabel = l; }
        void setYLabel(const std::string &l) { yLabel = l; }
        void grid(bool on) { gridEnabled = on; }

        void addPlotObject(std::shared_ptr<PlotObject> obj)
        {
            children.push_back(obj);
        }

        void draw(int winWidth, int winHeight) const
        {
            if (winWidth <= 0 || winHeight <= 0)
                return;

            float dataWidth = dataXMax - dataXMin;
            float dataHeight = dataYMax - dataYMin;
            float dataAspect = dataWidth / dataHeight;
            float winAspect = static_cast<float>(winWidth) / static_cast<float>(winHeight);

            float viewWidth, viewHeight;
            if (winAspect > dataAspect)
            {
                viewHeight = dataHeight;
                viewWidth = dataHeight * winAspect;
            }
            else
            {
                viewWidth = dataWidth;
                viewHeight = dataWidth / winAspect;
            }

            float left = -viewWidth / 2.0f;
            float right = viewWidth / 2.0f;
            float bottom = -viewHeight / 2.0f;
            float top = viewHeight / 2.0f;

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(left, right, bottom, top, -1, 1);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            if (gridEnabled)
                drawGrid(viewWidth, viewHeight);
            drawAxes(viewWidth, viewHeight);

            for (const auto &child : children)
            {
                child->draw();
            }
        }

    private:
        void drawGrid(float viewWidth, float viewHeight) const
        {
            glColor3f(0.9f, 0.9f, 0.9f);
            glLineWidth(1.0f);
            glBegin(GL_LINES);
            for (int i = 0; i <= 10; ++i)
            {
                float x = -viewWidth / 2 + viewWidth * i / 10.0f;
                float y = -viewHeight / 2 + viewHeight * i / 10.0f;
                glVertex2f(x, -viewHeight / 2);
                glVertex2f(x, viewHeight / 2);
                glVertex2f(-viewWidth / 2, y);
                glVertex2f(viewWidth / 2, y);
            }
            glEnd();
        }

        void drawAxes(float viewWidth, float viewHeight) const
        {
            glColor3f(0.0f, 0.0f, 0.0f);
            glLineWidth(2.0f);
            glBegin(GL_LINES);
            glVertex2f(-viewWidth / 2, 0);
            glVertex2f(viewWidth / 2, 0);
            glVertex2f(0, -viewHeight / 2);
            glVertex2f(0, viewHeight / 2);
            glEnd();

            glLineWidth(1.0f);
            glBegin(GL_LINES);
            for (int i = 0; i <= 10; ++i)
            {
                float x = -viewWidth / 2 + viewWidth * i / 10.0f;
                float y = -viewHeight / 2 + viewHeight * i / 10.0f;
                glVertex2f(x, -0.01f * viewHeight);
                glVertex2f(x, 0.01f * viewHeight);
                glVertex2f(-0.01f * viewWidth, y);
                glVertex2f(0.01f * viewWidth, y);
            }
            glEnd();
        }
    };

    class LinePlot : public PlotObject
    {
    private:
        std::vector<float> xData, yData;
        Color lineColor = Color::Blue();
        float lineWidth = 2.0f;
        LineStyle lineStyle = LINE_SOLID;
        MarkerStyle markerStyle = MARKER_NONE;
        Color markerColor = Color::Red();
        float markerSize = 5.0f;

        size_t maxPoints = 200;
        bool autoUpdateAxesRange = true;
        std::weak_ptr<Axes> linkedAxes; // 弱引用，避免循环引用

    public:
        LinePlot(const std::vector<float> &x, const std::vector<float> &y, size_t maxPts = 200)
            : xData(x), yData(y), maxPoints(maxPts)
        {
            if (xData.size() > maxPoints)
            {
                xData.erase(xData.begin(), xData.end() - maxPoints);
                yData.erase(yData.begin(), yData.end() - maxPoints);
            }
        }

        void setColor(const Color &c) { lineColor = c; }
        void setLineWidth(float w) { lineWidth = w; }
        void setLineStyle(LineStyle style) { lineStyle = style; }
        void setMarkerStyle(MarkerStyle style) { markerStyle = style; }
        void setMarkerColor(const Color &c) { markerColor = c; }
        void setMarkerSize(float size) { markerSize = size; }
        void setMaxPoints(size_t n) { maxPoints = n; }
        void setAutoUpdateAxes(bool enable) { autoUpdateAxesRange = enable; }

        void appendData(float x, float y, std::shared_ptr<Axes> ax = nullptr)
        {
            if (ax)
                linkedAxes = ax;
            xData.push_back(x);
            yData.push_back(y);
            if (xData.size() > maxPoints)
            {
                xData.erase(xData.begin());
                yData.erase(yData.begin());
            }

            if (autoUpdateAxesRange && !linkedAxes.expired())
            {
                auto axes = linkedAxes.lock();
                if (!xData.empty())
                {
                    auto xMinMax = std::minmax_element(xData.begin(), xData.end());
                    auto yMinMax = std::minmax_element(yData.begin(), yData.end());
                    float xMin = static_cast<float>(*xMinMax.first);
                    float xMax = static_cast<float>(*xMinMax.second);
                    float yMin = static_cast<float>(*yMinMax.first);
                    float yMax = static_cast<float>(*yMinMax.second);

                    float marginX = (xMax - xMin) * 0.05f;
                    float marginY = (yMax - yMin) * 0.05f;
                    axes->setDataRange(xMin - marginX, xMax + marginX, yMin - marginY, yMax + marginY);
                }
            }
        }

        void draw() const override
        {
            glColor3f(lineColor.r, lineColor.g, lineColor.b);
            glLineWidth(lineWidth);

            switch (lineStyle)
            {
            case LINE_DASHED:
                glEnable(GL_LINE_STIPPLE);
                glLineStipple(1, 0xF0F0);
                break;
            case LINE_DOTTED:
                glEnable(GL_LINE_STIPPLE);
                glLineStipple(1, 0xAAAA);
                break;
            case LINE_DASH_DOT:
                glEnable(GL_LINE_STIPPLE);
                glLineStipple(1, 0xF6F6);
                break;
            default:
                glDisable(GL_LINE_STIPPLE);
                break;
            }

            glBegin(GL_LINE_STRIP);
            for (size_t i = 0; i < xData.size() && i < yData.size(); ++i)
                glVertex2f(xData[i], yData[i]);
            glEnd();

            glDisable(GL_LINE_STIPPLE);

            if (markerStyle != MARKER_NONE)
            {
                glColor3f(markerColor.r, markerColor.g, markerColor.b);
                for (size_t i = 0; i < xData.size() && i < yData.size(); ++i)
                    drawSingleMarker(xData[i], yData[i]);
            }
        }

        void update() override {}

    private:
        void drawSingleMarker(float x, float y) const
        {
            float size = markerSize / 20.0f;
            switch (markerStyle)
            {
            case MARKER_CIRCLE:
            {
                const int seg = 20;
                glBegin(GL_POLYGON);
                for (int i = 0; i < seg; ++i)
                {
                    float th = 2.0f * acos(-1) * i / seg;
                    glVertex2f(x + size * cosf(th), y + size * sinf(th));
                }
                glEnd();
                break;
            }
            case MARKER_SQUARE:
                glBegin(GL_QUADS);
                glVertex2f(x - size, y - size);
                glVertex2f(x + size, y - size);
                glVertex2f(x + size, y + size);
                glVertex2f(x - size, y + size);
                glEnd();
                break;
            case MARKER_TRIANGLE:
                glBegin(GL_TRIANGLES);
                glVertex2f(x, y + size);
                glVertex2f(x - size, y - size);
                glVertex2f(x + size, y - size);
                glEnd();
                break;
            case MARKER_CROSS:
                glBegin(GL_LINES);
                glVertex2f(x - size, y - size);
                glVertex2f(x + size, y + size);
                glVertex2f(x - size, y + size);
                glVertex2f(x + size, y - size);
                glEnd();
                break;
            case MARKER_PLUS:
                glBegin(GL_LINES);
                glVertex2f(x - size, y);
                glVertex2f(x + size, y);
                glVertex2f(x, y - size);
                glVertex2f(x, y + size);
                glEnd();
                break;
            default:
                break;
            }
        }
    };
    class Figure
    {
    public:
        GLFWwindow *window = nullptr;

    private:
        std::shared_ptr<Axes> currentAxes;
        int width, height;

        static void framebuffer_size_callback(GLFWwindow *window, int w, int h)
        {
            glViewport(0, 0, w, h);
        }

    public:
        Figure(int w = 800, int h = 600) : width(w), height(h)
        {
            static bool glfwInitDone = false;
            if (!glfwInitDone)
            {
                if (!glfwInit())
                {
                    std::cerr << "Failed to initialize GLFW\n";
                    return;
                }
                glfwInitDone = true;
            }

            window = glfwCreateWindow(width, height, "MATPlot Figure", nullptr, nullptr);
            if (!window)
            {
                std::cerr << "Failed to create GLFW window\n";
                return;
            }

            glfwMakeContextCurrent(window);
            glViewport(0, 0, width, height);
            currentAxes = std::make_shared<Axes>();

            glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
            glfwSetWindowUserPointer(window, this);
        }

        ~Figure()
        {
            if (window)
                glfwDestroyWindow(window);
        }

        std::shared_ptr<Axes> gca() { return currentAxes; }

        void render()
        {
            if (!window)
                return;
            int w, h;
            glfwGetFramebufferSize(window, &w, &h);
            glfwMakeContextCurrent(window);
            currentAxes->draw(w, h);
        }
    };

    inline Figure *figure(int /*id*/ = -1)
    {
        return new Figure();
    }

    template <typename T>
    std::shared_ptr<LinePlot> plot(std::shared_ptr<Axes> ax,
                                   const std::vector<T> &x,
                                   const std::vector<T> &y,
                                   const std::string &style = "")
    {
        auto line = std::make_shared<LinePlot>(
            std::vector<float>(x.begin(), x.end()),
            std::vector<float>(y.begin(), y.end()));

        if (!x.empty() && !y.empty())
        {
            auto xMinMax = std::minmax_element(x.begin(), x.end());
            auto yMinMax = std::minmax_element(y.begin(), y.end());
            float xMin = static_cast<float>(*xMinMax.first);
            float xMax = static_cast<float>(*xMinMax.second);
            float yMin = static_cast<float>(*yMinMax.first);
            float yMax = static_cast<float>(*yMinMax.second);

            float marginX = (xMax - xMin) * 0.05f;
            float marginY = (yMax - yMin) * 0.05f;
            ax->setDataRange(xMin - marginX, xMax + marginX, yMin - marginY, yMax + marginY);
        }

        if (!style.empty())
        {
            if (style.find('r') != std::string::npos)
                line->setColor(Color::Red());
            else if (style.find('g') != std::string::npos)
                line->setColor(Color::Green());
            else if (style.find('b') != std::string::npos)
                line->setColor(Color::Blue());

            if (style.find("--") != std::string::npos)
                line->setLineStyle(LINE_DASHED);
            else if (style.find(":") != std::string::npos)
                line->setLineStyle(LINE_DOTTED);
            else if (style.find("-.") != std::string::npos)
                line->setLineStyle(LINE_DASH_DOT);

            if (style.find('o') != std::string::npos)
                line->setMarkerStyle(MARKER_CIRCLE);
            else if (style.find('s') != std::string::npos)
                line->setMarkerStyle(MARKER_SQUARE);
            else if (style.find('^') != std::string::npos)
                line->setMarkerStyle(MARKER_TRIANGLE);
        }

        ax->addPlotObject(line);
        return line;
    }

    template <typename T>
    void plot(const std::vector<T> &x, const std::vector<T> &y, const std::string &style = "")
    {
        Figure *fig = figure();
        plot(fig->gca(), x, y, style);
    }

    inline std::vector<float> linspace(float start, float end, int num)
    {
        std::vector<float> res(num);
        float step = (end - start) / std::max(1, num - 1);
        for (int i = 0; i < num; ++i)
            res[i] = start + i * step;
        return res;
    }

} // namespace matplot