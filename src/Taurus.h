#include <cmath>
#include <vector>
#include <glm/glm.hpp>
class Taurus
{
    private:
        int numVertices;
        int numIndices;
        float scaleFactor;
        std::vector<int> indices;
        std::vector<glm::vec3> base2D;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> texCoords;
        std::vector<glm::vec3> normals;
        void init(int, int, float, float);
        float toRadians(float degrees);
    public:
        Taurus(int vert, int horiz, float outer, float inner);
        int getNumVertices();
        int getNumIndices();
        std::vector<int> getIndices();
        std::vector<glm::vec3> getVertices();
        std::vector<glm::vec2> getTexCoords();
        std::vector<glm::vec3> getNormals();
};