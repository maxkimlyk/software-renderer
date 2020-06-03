#include "../common/program.h"
#include <cmath>
#include <list>

using namespace sr;

namespace
{
struct Element
{
    Vec3f verts[3];
    Color color;
};

float RandFloat()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

float RandSize(float size_factor)
{
    return (RandFloat() - 0.5f) * size_factor;
}
} // namespace

class Demo
{
    inline static const size_t ELEMENTS_AMOUNT = 100;
    inline static const uint32_t CHANCE_FACTOR = 3;
    inline static const float ELEMENT_SIZE_FACTOR = 0.4f;
    inline static const uint8_t MIN_INTENSITY = 10;
    inline static const uint8_t FADE_SPEED = 3;
    inline static const float Z = -1.0f;

  public:
    const size_t Width = 800;
    const size_t Height = 600;
    inline static const std::string Caption = "Random Triangles";

    void Init(Renderer& renderer)
    {
        renderer.Matrices.SetProjection(Mat4f::Identity());
    }

    void Process(Renderer& renderer, Input& input)
    {
        for (auto elem = elements_.begin(); elem != elements_.end();)
        {
            if (elem->color.r < MIN_INTENSITY || elem->color.g < MIN_INTENSITY ||
                elem->color.b < MIN_INTENSITY)
            {
                elem = elements_.erase(elem);
            }
            else
            {
                elem->color.r -= FADE_SPEED;
                elem->color.g -= FADE_SPEED;
                elem->color.b -= FADE_SPEED;
                ++elem;
            }
        }

        if (elements_.size() < ELEMENTS_AMOUNT && rand() % CHANCE_FACTOR == 0)
        {
            Element newElem;
            newElem.verts[0] = Vec3f{2.0f * (RandFloat() - 0.5f), 2.0f * (RandFloat() - 0.5f), Z};
            newElem.verts[1] = newElem.verts[0] + Vec3f{RandSize(ELEMENT_SIZE_FACTOR),
                                                        RandSize(ELEMENT_SIZE_FACTOR), 0};
            newElem.verts[2] = newElem.verts[0] + Vec3f{RandSize(ELEMENT_SIZE_FACTOR),
                                                        RandSize(ELEMENT_SIZE_FACTOR), 0};
            newElem.color = Color(rand() % 128 + 128, rand() % 128 + 128, rand() % 128 + 128);
            elements_.push_back(newElem);
        }
    }

    void Draw(Renderer& renderer)
    {
        renderer.Clear();
        for (const auto& elem : elements_)
            renderer.Triangle(elem.verts[0], elem.verts[1], elem.verts[2], elem.color);
    }

  private:
    std::list<Element> elements_;
};

int main()
{
    Demo demo;

    auto Init = [&demo](Renderer& renderer) { demo.Init(renderer); };
    auto Process = [&demo](Renderer& renderer, Input& input) { demo.Process(renderer, input); };
    auto Draw = [&demo](Renderer& renderer) { demo.Draw(renderer); };

    return Program(Init, Process, Draw).Run(demo.Width, demo.Height, demo.Caption);
}
