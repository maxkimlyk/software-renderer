#include "../common/program.h"
#include <cmath>
#include <list>

using namespace sr;

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const std::string CAPTION = "Random Triangles";

const size_t ELEMENTS_AMOUNT = 100;
const uint32_t CHANCE_FACTOR = 3;
const float ELEMENT_SIZE_FACTOR = 0.4f;
const uint8_t MIN_INTENSITY = 10;
const uint8_t FADE_SPEED = 3;
const float Z = -1.0f;

struct Element
{
    Vec3f verts[3];
    Color color;
};

std::list<Element> Elements;

void Init(Renderer& renderer)
{
    renderer.SetProjMatrix(Mat4f::Identity());
}

float RandFloat()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

float RandSize()
{
    return (RandFloat() - 0.5f) * ELEMENT_SIZE_FACTOR;
}

void Process(Renderer& renderer, Input& input)
{
    for (auto elem = Elements.begin(); elem != Elements.end();)
    {
        if (elem->color.r < MIN_INTENSITY || elem->color.g < MIN_INTENSITY ||
            elem->color.b < MIN_INTENSITY)
        {
            elem = Elements.erase(elem);
        }
        else
        {
            elem->color.r -= FADE_SPEED;
            elem->color.g -= FADE_SPEED;
            elem->color.b -= FADE_SPEED;
            ++elem;
        }
    }

    if (Elements.size() < ELEMENTS_AMOUNT && rand() % CHANCE_FACTOR == 0)
    {
        Element newElem;
        newElem.verts[0] = Vec3f{2.0f * (RandFloat() - 0.5f), 2.0f * (RandFloat() - 0.5f), Z};
        newElem.verts[1] = newElem.verts[0] + Vec3f{RandSize(), RandSize(), 0};
        newElem.verts[2] = newElem.verts[0] + Vec3f{RandSize(), RandSize(), 0};
        newElem.color = Color(rand() % 128 + 128, rand() % 128 + 128, rand() % 128 + 128);
        Elements.push_back(newElem);
    }
}

void Draw(Renderer& renderer)
{
    renderer.Clear();
    for (const auto& elem : Elements)
        renderer.Triangle(elem.verts[0], elem.verts[1], elem.verts[2], elem.color);
}

int main()
{
    Program program(Init, Process, Draw);
    return program.Run(WIDTH, HEIGHT, CAPTION);
}
