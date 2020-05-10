#ifndef _MODEL_H_
#define _MODEL_H_

#include "../common/logging.h"
#include "geometry.h"
#include <fstream>
#include <limits>
#include <regex>
#include <sstream>

#include "vertex.h"

namespace sr
{

struct Face
{
    Vertex v[3];
};

struct Model
{
    std::vector<Face> faces;

    void Normalize();
};

class ObjReader
{
    std::vector<Vec3f> verts_;
    std::vector<Vec3f> norms_;
    std::vector<Vec2f> texs_;

    static const uint32_t undefined = 0;

    std::vector<std::string> Split(std::string& string);

    template <class T>
    bool TryParse(std::string& str, T& res);

    template <size_t n, class T>
    Vec<n, T> Eject(std::vector<Vec<n, T>>& vector, size_t index);

    int ParseVLine(std::vector<std::string>& tokens);

    int ParseVtLine(std::vector<std::string>& tokens);

    int ParseVnLine(std::vector<std::string>& tokens);

    int ParseFLine(std::string& line, std::vector<std::string>& tokens, Model& model);
    int ParseLine(std::string& line, Model& model);

    void Clear();

  public:
    int ReadModel(const char* filename, Model& model);
};

} // namespace sr

#endif
