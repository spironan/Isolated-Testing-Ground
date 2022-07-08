#include <iostream>
#include "src/GameObject.h"
#include "src/TransformWrapper.h"

void printvalues(Transform const& transform)
{
    std::cout << "===========================================================\n";

    std::cout << "Local Pos " << transform.GetPosition() << "\n";
    std::cout << "Local Rot " << transform.GetEulerAngles() << "\n";
    std::cout << "Local Sca " << transform.GetScale() << "\n";
    std::cout << "Local Mat " << transform.GetLocalMatrix() << "\n";

    std::cout << "Global Pos " << transform.GetGlobalPosition() << "\n";
    std::cout << "Global Rot(Deg) " << transform.GetGlobalRotationDeg() << "\n";
    std::cout << "Global Rot(Rad)" << transform.GetGlobalRotationRad() << "\n";
    std::cout << "Global Rot(Mat)" << transform.GetGlobalRotationMatrix() << "\n";
    std::cout << "Global Sca" << transform.GetGlobalScale() << "\n";
    std::cout << "Global Mat" << transform.GetGlobalMatrix() << "\n";

    std::cout << "Has Changed " << transform.HasChanged() << "\n";
    std::cout << "Is Dirty " << transform.IsDirty() << "\n";

    std::cout << "Local Right " << transform.LocalRight() << "\n";
    std::cout << "Local Up " << transform.LocalUp() << "\n";
    std::cout << "Local Forward" << transform.LocalForward() << "\n";

    std::cout << "===========================================================\n";
}

int main()
{ 
    // Memory Leak Checker in Debug builds
#if not defined (OO_PRODUCTION)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    // Uncomment to cause a break on allocation for debugging
    //_CrtSetBreakAlloc(/*Allocation Number here*/);
#endif

    oo::TransformWrapper wrapper;
    wrapper.SetPosition(glm::vec3{ 10.f });
    printvalues(wrapper.m_transform);

    // Combined GameObject-Transform-SceneGraph Test
    Transform test_tf;
    test_tf.SetPosition(glm::vec3{ 10.0f });
    printvalues(test_tf);
    
    GameObject go("go");
    go.Transform().Position() = { 1, 0, 0 };
    GameObject go1("go1");
    go1.Transform().Position() = { 0, 1, 0 };
    GameObject go2("go2");
    go2.Transform().Position() = { 0, 0, 1 };
    
    go.AddChild(go1);
    go1.AddChild(go2);
    
    // simulate transform update
    for (auto& elem : g_sceneGraph.get_root_childs())
    {
        auto& tf = g_sceneData.at(elem)->Transform();
        if (tf.IsDirty())
        {
            tf.CalculateLocalTransform();
        }
        
        scenenode& sceneNode = *g_sceneData.at(elem)->SceneNode().get();
        
        GameObject parent = *g_sceneData.at(sceneNode.get_parent_handle());
        auto parentTf = parent.GetTransform();
        tf.m_globalTransform = parentTf.m_globalTransform * tf.m_localTransform;

        for (auto& child : g_sceneGraph.get_childs(&sceneNode))
        {
            g_sceneData.at(child)->Transform().m_dirty = true;
        }
        
    }

    //print values
    for (auto& child : g_sceneGraph.get_root_childs())
    {
        PrintGo(g_sceneData.at(child));
        printvalues(g_sceneData.at(child)->GetTransform());
    }
    
    g_sceneGraph.print();
}
