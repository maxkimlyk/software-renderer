#include "../common/program.h"
#include <cmath>
#include <list>

const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const std::string CAPTION = "Triangles";

const size_t ELEMENTS_AMOUNT = 100;
const uint32_t CHANCE_FACTOR = 8;
const uint32_t ELEMENT_SIZE_FACTOR = 150;
const uint8_t MIN_INTENSITY = 10;
const uint8_t FADE_SPEED = 3;

struct Element
{
    Vec2i verts[3];
    Color color;
};

std::list<Element> Elements;

inline int randSize()
{
    return rand() % ELEMENT_SIZE_FACTOR - ELEMENT_SIZE_FACTOR / 2;
}

void Process()
{
    for (auto elem = Elements.begin(); elem != Elements.end();)
    {
        if (elem->color.r < MIN_INTENSITY || elem->color.g < MIN_INTENSITY || elem->color.b < MIN_INTENSITY)
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
        newElem.verts[0] = Vec2i {rand() % WIDTH, rand() % HEIGHT};
        newElem.verts[1] = newElem.verts[0] + Vec2i {randSize(), randSize()};
        newElem.verts[2] = newElem.verts[0] + Vec2i {randSize(), randSize()};
        newElem.color = Color(rand() % 128 + 128, rand() % 128 + 128, rand() % 128 + 128);
        Elements.push_back(newElem);
    }
}

void Draw(Renderer &renderer)
{
    renderer.Clear();
    for (auto elem = Elements.begin(); elem != Elements.end(); ++elem)
        renderer.Triangle(elem->verts[0], elem->verts[1], elem->verts[2], elem->color);
}

int main()
{
    Program program;
    program.ProcessCallback = Process;
    program.DrawCallback = Draw;
    return program.Run(WIDTH, HEIGHT, CAPTION);
}
