#include "../renderer/model.h"

int main()
{
    sr::Model model;
    sr::ObjReader reader;
    return reader.ReadModel("palm.obj", model);
}
