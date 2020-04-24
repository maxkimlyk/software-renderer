#include "../renderer/model.h"

int main()
{
    Model model;
    ObjReader reader;
    return reader.ReadModel("palm.obj", model);
}
