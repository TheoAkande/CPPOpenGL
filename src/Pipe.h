#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Pipe
{
    private:
        std::vector<float> distances;
        float totalDistance;

        // Both
        int numVertices;
        std::vector<glm::vec3> vertices;    
        std::vector<glm::vec3> faceNormals;
        std::vector<glm::vec2> texCoords;
        std::vector<glm::vec3> normals;

        // Main pipe
        int numIndicesPipe;
        std::vector<int> pipeIndices;

        // End faces
        int numIndicesFace;
        std::vector<int> startIndices;
        std::vector<int> endIndices;

        void init(std::vector<glm::vec3>, int, float);
        float toRadians(float degrees);
    public:
        Pipe(std::vector<glm::vec3>, int, float);
        int getNumVertices();
        int getNumIndicesPipe();
        int getNumIndicesFace();
        std::vector<int> getPipeIndices();
        std::vector<int> getStartIndices();
        std::vector<int> getEndIndices();
        std::vector<glm::vec3> getVertices();
        std::vector<glm::vec2> getTexCoords();
        std::vector<glm::vec3> getNormals();
};