#pragma once
#include "scenegraph.h"
#include "Transform.h"

#include <unordered_map>
#include "UUID.h"

// class is an encapsulation of both scenegraph and transform as a means of testing
// not reflective of the final implementation and shouldnt be used for production.

using Entity = size_t;

class GameObject
{
private:
    static Entity index;
    Entity m_entt = index++;

    // every entity has a transform and scenegraph properties
    scenenode::shared_pointer m_node;
    Transform m_transform;

public:
    
    GameObject();
    // for root creation only
    GameObject(scenenode::shared_pointer root_node);

    explicit GameObject(Entity entt);
    GameObject(std::string const& name);
    GameObject(std::string const& name, UUID uuid);
    GameObject(GameObject const& other)
        : m_entt { other.m_entt }
        , m_node { other.m_node }
        , m_transform { other.m_transform }
    {
    }

    void Destroy();

    void Print() const;
    Entity GetEntity() const;
    Transform GetTransform() const { return m_transform; }
    scenenode::shared_pointer GetSceneNode() const { return m_node; }
    
    Transform& Transform() { return m_transform; }
    scenenode::shared_pointer& SceneNode() { return m_node; }

    void AddChild(GameObject& target);

    GameObject GetParent();
    std::vector<GameObject> GetChilds(bool includeItself = false);
};

void PrintGo(GameObject const* go);

extern "C"
{
    // global scenegraph
    extern scenegraph g_sceneGraph;
    extern GameObject g_root;
    extern std::unordered_map<scenegraph::handle_type, GameObject*> g_sceneData;
}
