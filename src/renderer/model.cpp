#include "model.h"

namespace sr
{

void Model::Normalize()
{
    float maxNorm = 0;
    for (auto face = faces.begin(); face != faces.end(); ++face)
    {
        for (size_t v = 0; v < 3; ++v)
        {
            float vertexNorm = face->v[v].coord.Norm();
            if (vertexNorm > maxNorm)
                maxNorm = vertexNorm;
        }
    }

    if (maxNorm == 0)
        return;

    float invMaxNorm = 1.0f / maxNorm;

    for (auto face = faces.begin(); face != faces.end(); ++face)
        for (size_t i = 0; i < 3; ++i)
            face->v[i].coord = face->v[i].coord * invMaxNorm;
}

std::vector<std::string> ObjReader::Split(std::string& string)
{
    auto pred = [](char ch) {
        return ch == ' ' || ch == '\\' || ch == '/' || ch == '\t' || ch == '\r';
    };

    std::vector<std::string> parts;
    auto iter = string.begin();
    auto prevIter = iter;

    while (iter != string.end())
    {
        iter = find_if(iter, string.end(), pred);
        if (prevIter != string.end() && iter != prevIter)
            parts.push_back(std::string(prevIter, iter));
        if (iter != string.end())
            ++iter;
        prevIter = iter;
    }

    return parts;
}

template <class T>
bool ObjReader::TryParse(std::string& str, T& res)
{
    std::istringstream stream(str);
    return bool(stream >> res);
}

template bool ObjReader::TryParse<uint32_t>(std::string& str, uint32_t& res);
template bool ObjReader::TryParse<float>(std::string& str, float& res);

template <size_t n, class T>
Vec<n, T> ObjReader::Eject(std::vector<Vec<n, T>>& vector, size_t index)
{
    if (index == undefined)
    {
        Vec<n, T> res;
        res.Fill((T)(0));
        return res;
    }
    return vector[index - 1];
}

template Vec3f ObjReader::Eject(std::vector<Vec3f>& vector, size_t index);
template Vec2f ObjReader::Eject(std::vector<Vec2f>& vector, size_t index);
template Vec3i ObjReader::Eject(std::vector<Vec3i>& vector, size_t index);
template Vec2i ObjReader::Eject(std::vector<Vec2i>& vector, size_t index);

int ObjReader::ParseVLine(std::vector<std::string>& tokens)
{
    if (tokens.size() != 4)
        return -1;

    Vec3f coord;
    for (size_t i = 0; i < 3; ++i)
        if (!TryParse<float>(tokens[1 + i], coord[i]))
            return -1;

    verts_.push_back(coord);
    return 0;
}

int ObjReader::ParseVtLine(std::vector<std::string>& tokens)
{
    if (tokens.size() != 3 && tokens.size() != 4)
        return -1;

    Vec2f tex;
    for (size_t i = 0; i < 2; ++i)
        if (!TryParse<float>(tokens[1 + i], tex[i]))
            return -1;

    texs_.push_back(tex);
    return 0;
}

int ObjReader::ParseVnLine(std::vector<std::string>& tokens)
{
    if (tokens.size() != 4)
        return -1;

    Vec3f norm;
    for (size_t i = 0; i < 3; ++i)
        if (!TryParse<float>(tokens[1 + i], norm[i]))
            return -1;

    norms_.push_back(norm);
    return 0;
}

inline Vec3f BuildNormal(Vec3f v1, Vec3f v2, Vec3f v3)
{
    Vec3f d1 = v2 - v1;
    Vec3f d2 = v3 - v1;
    return Normalize(Cross(d1, d2));
}

int ObjReader::ParseFLine(std::string& line, std::vector<std::string>& tokens, Model& model)
{
    uint32_t v1 = undefined, t1 = undefined, n1 = undefined;
    uint32_t v2 = undefined, t2 = undefined, n2 = undefined;
    uint32_t v3 = undefined, t3 = undefined, n3 = undefined;

    std::regex pattern1("\\s*f\\s+\\d+\\/\\d+\\s+\\d+\\/\\d+\\s+\\d+\\/\\d+\\s*");
    std::regex pattern2(
        "\\s*f\\s+\\d+\\/\\d+\\/\\d+\\s+\\d+\\/\\d+\\/\\d+\\s+\\d+\\/\\d+\\/\\d+\\s*");
    std::regex pattern3("\\s*f\\s+\\d+\\/\\/\\d+\\s+\\d+\\/\\/\\d+\\s+\\d+\\/\\/\\d+\\s*");

    if (regex_match(line, pattern1) && tokens.size() == 7)
    {
        if (!TryParse<uint32_t>(tokens[1], v1) || !TryParse<uint32_t>(tokens[2], t1) ||
            !TryParse<uint32_t>(tokens[3], v2) || !TryParse<uint32_t>(tokens[4], t2) ||
            !TryParse<uint32_t>(tokens[5], v3) || !TryParse<uint32_t>(tokens[6], t3))
            return -1;
    }
    else if (regex_match(line, pattern2) && tokens.size() == 10)
    {
        if (!TryParse<uint32_t>(tokens[1], v1) || !TryParse<uint32_t>(tokens[2], t1) ||
            !TryParse<uint32_t>(tokens[3], n1) || !TryParse<uint32_t>(tokens[4], v2) ||
            !TryParse<uint32_t>(tokens[5], t2) || !TryParse<uint32_t>(tokens[6], n2) ||
            !TryParse<uint32_t>(tokens[7], v3) || !TryParse<uint32_t>(tokens[8], t3) ||
            !TryParse<uint32_t>(tokens[9], n3))
            return -1;
    }
    else if (regex_match(line, pattern3) && tokens.size() == 7)
    {
        if (!TryParse<uint32_t>(tokens[1], v1) || !TryParse<uint32_t>(tokens[2], n1) ||
            !TryParse<uint32_t>(tokens[3], v2) || !TryParse<uint32_t>(tokens[4], n2) ||
            !TryParse<uint32_t>(tokens[5], v3) || !TryParse<uint32_t>(tokens[6], n3))
            return -1;
    }
    else
        return -1;

    if ((v1 - 1) >= verts_.size() || (v2 - 1) >= verts_.size() || (v3 - 1) >= verts_.size() ||
        t1 != undefined && (t1 - 1) >= texs_.size() || t2 != undefined && (t2 - 1) >= texs_.size() ||
        t3 != undefined && (t3 - 1) >= texs_.size() || n1 != undefined && (n1 - 1) >= norms_.size() ||
        n2 != undefined && (n2 - 1) >= norms_.size() || n3 != undefined && (n3 - 1) >= norms_.size())
        return -2;

    Face face;
    face.v[0].coord = Eject(verts_, v1);
    face.v[1].coord = Eject(verts_, v2);
    face.v[2].coord = Eject(verts_, v3);
    face.v[0].tex = Eject(texs_, t1);
    face.v[1].tex = Eject(texs_, t2);
    face.v[2].tex = Eject(texs_, t3);

    if (n1 != undefined && n2 != undefined && n3 != undefined)
    {
        face.v[0].norm = Eject(norms_, n1);
        face.v[1].norm = Eject(norms_, n2);
        face.v[2].norm = Eject(norms_, n3);
    }
    else
    {
        Vec3f norm = BuildNormal(face.v[0].coord, face.v[1].coord, face.v[2].coord);
        face.v[0].norm = norm;
        face.v[1].norm = norm;
        face.v[2].norm = norm;
    }

    model.faces.push_back(face);

    return 0;
}

int ObjReader::ParseLine(std::string& line, Model& model)
{
    std::vector<std::string> tokens = Split(line);
    if (tokens.empty())
        return 0;

    if (tokens[0] == "v")
        return ParseVLine(tokens);
    else if (tokens[0] == "vt")
        return ParseVtLine(tokens);
    else if (tokens[0] == "vn")
        return ParseVnLine(tokens);
    else if (tokens[0] == "f")
        return ParseFLine(line, tokens, model);

    return 0;
}

void ObjReader::Clear()
{
    verts_.clear();
    norms_.clear();
    texs_.clear();
}

int ObjReader::ReadModel(const char* filename, Model& model)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        ERROR("ObjReader::ReadModel could not read model %s\n", filename);
        return -1;
    }

    int status = 0;

    std::string line;
    uint32_t lineNumber = 1;

    while (std::getline(file, line))
    {
        status = ParseLine(line, model);
        if (status == -1)
        {
            ERROR("Line %d has unknown format: %s\n", lineNumber, line.c_str());
            break;
        }
        else if (status == -2)
        {
            ERROR("Model is corrupted. Line %d has invalid indexes: %s\n", lineNumber,
                  line.c_str());
            break;
        }
        ++lineNumber;
    }

    Clear();
    return status;
}

} // namespace sr
