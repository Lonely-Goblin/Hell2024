#pragma once
#include "../Common.h"
#include "GameObject.h"
#include "AnimatedGameObject.h"
#include "../ThreadPool.h"

#define WALL_HEIGHT 2.4f

inline glm::vec3 NormalFromThreePoints(glm::vec3 pos0, glm::vec3 pos1, glm::vec3 pos2) {
    return glm::normalize(glm::cross(pos1 - pos0, pos2 - pos0));
}

inline void SetNormalsAndTangentsFromVertices(Vertex* vert0, Vertex* vert1, Vertex* vert2) {
    // Shortcuts for UVs
    glm::vec3& v0 = vert0->position;
    glm::vec3& v1 = vert1->position;
    glm::vec3& v2 = vert2->position;
    glm::vec2& uv0 = vert0->uv;
    glm::vec2& uv1 = vert1->uv;
    glm::vec2& uv2 = vert2->uv;
    // Edges of the triangle : postion delta. UV delta
    glm::vec3 deltaPos1 = v1 - v0;
    glm::vec3 deltaPos2 = v2 - v0;
    glm::vec2 deltaUV1 = uv1 - uv0;
    glm::vec2 deltaUV2 = uv2 - uv0;
    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
    glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;
    glm::vec3 normal = NormalFromThreePoints(vert0->position, vert1->position, vert2->position);
    vert0->normal = normal;
    vert1->normal = normal;
    vert2->normal = normal;
    vert0->tangent = tangent;
    vert1->tangent = tangent;
    vert2->tangent = tangent;
    vert0->bitangent = bitangent;
    vert1->bitangent = bitangent;
    vert2->bitangent = bitangent;
}

#define PROPOGATION_SPACING 1
#define PROPOGATION_WIDTH (MAP_WIDTH / PROPOGATION_SPACING)
#define PROPOGATION_HEIGHT (MAP_HEIGHT / PROPOGATION_SPACING)
#define PROPOGATION_DEPTH (MAP_DEPTH / PROPOGATION_SPACING)

struct CloudPoint {
    glm::vec4 position = glm::vec4(0);
    glm::vec4 normal = glm::vec4(0);
    glm::vec4 directLighting = glm::vec4(0);
};

struct Door {
    glm::vec3 position;
    float rotation;
    float openRotation;
    enum State {CLOSED = 0, CLOSING, OPEN, OPENING} state;
    Door(glm::vec3 position, float rotation);
    void Interact();
    void Update(float deltaTime);
    glm::mat4 GetFrameModelMatrix();
    glm::mat4 GetDoorModelMatrix();
    glm::vec3 GetVertFrontLeft(float padding = 0);
    glm::vec3 GetVertFrontRight(float padding = 0);
    glm::vec3 GetVertBackLeft(float padding = 0);
    glm::vec3 GetVertBackRight(float padding = 0);
    bool IsInteractable();
};

struct Wall {
    glm::vec3 begin;
    glm::vec3 end;
    float height;
    GLuint VAO = 0;
    GLuint VBO = 0;
    std::vector<Vertex> vertices;
    int materialIndex = 0;
    float wallHeight = 2.4f;
    void CreateMesh();
    Wall(glm::vec3 begin, glm::vec3 end, float height, int materialIndex);
    glm::vec3 GetNormal();
    glm::vec3 GetMidPoint();
    void Draw();
    std::vector<Transform> ceilingTrims;
    std::vector<Transform> floorTrims;
};

struct Floor {
    float x1, z1, x2, z2, height; // remove these. you are only using them to create the bathroom ceiling. 
    Vertex v1, v2, v3, v4;
    GLuint VAO = 0;
    GLuint VBO = 0;
    std::vector<Vertex> vertices;
    float textureScale = 1.0f;
    int materialIndex = 0;
    Floor(float x1, float z1, float x2, float z2, float height, int materialIndex, float textureScale);
    Floor(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4, int materialIndex, float textureScale);
    void Draw();
    void CreateMesh();
};

struct Ceiling {
    float x1, z1, x2, z2, height;
    GLuint VAO = 0;
    GLuint VBO = 0;
    std::vector<Vertex> vertices;
    int materialIndex = 0;
    Ceiling(float x1, float z1, float x2, float z2, float height, int materialIndex);
    void Draw();
};

struct RTMesh {
    GLuint baseVertex = 0;
    GLuint vertexCount = 0;
    GLuint padding0 = 0;
    GLuint padding1 = 0;
};

struct RTInstance {
    glm::mat4 modelMatrix = glm::mat4(1);
    glm::mat4 inverseModelMatrix = glm::mat4(1);
    GLuint meshIndex = 0;
    GLuint padding0 = 0;
    GLuint padding1 = 0;
    GLuint padding2 = 0;
};

namespace Scene {

    inline std::vector<Wall> _walls;
    inline std::vector<Door> _doors;
    inline std::vector<Floor> _floors;
    inline std::vector<Ceiling> _ceilings;
    inline std::vector<CloudPoint> _cloudPoints;
    inline std::vector<GameObject> _gameObjects;
    inline std::vector<AnimatedGameObject> _animatedGameObjects;
    inline std::vector<Light> _lights;
    inline std::vector<glm::vec3> _rtVertices;
    inline std::vector<RTMesh> _rtMesh;
    inline std::vector<RTInstance> _rtInstances;
    inline RayCastResult _cameraRayData;

    void Init();
    void NewScene();
    void Update(float deltaTime);
    void LoadLightSetup(int index);
    GameObject* GetGameObjectByName(std::string);
    AnimatedGameObject* GetAnimatedGameObjectByName(std::string);
    std::vector<AnimatedGameObject>& GetAnimatedGameObjects();
    void CreatePointCloud();
    void CreateMeshData();
    void AddDoor(Door& door);
    void AddWall(Wall& wall);
    void AddFloor(Floor& floor);
    void CreateRTInstanceData();
    bool CursorShouldBeInterect();
    void RecreateDataStructures();
}