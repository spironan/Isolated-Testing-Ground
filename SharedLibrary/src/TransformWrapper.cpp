#include "TransformWrapper.h"

namespace oo
{
    TransformWrapper::vec3 TransformWrapper::GetPosition()                const { return m_transform.GetPosition(); }
    TransformWrapper::quat TransformWrapper::GetRotationQuat()            const { return m_transform.GetRotationQuat(); }
    TransformWrapper::vec3 TransformWrapper::GetEulerAngles()             const { return m_transform.GetEulerAngles(); }
    TransformWrapper::vec3 TransformWrapper::GetScale()                   const { return m_transform.GetScale(); }

    TransformWrapper::vec3 TransformWrapper::LocalRight()                 const { return m_transform.LocalRight(); }
    TransformWrapper::vec3 TransformWrapper::LocalUp()                    const { return m_transform.LocalUp(); }
    TransformWrapper::vec3 TransformWrapper::LocalForward()               const { return m_transform.LocalForward(); }

    TransformWrapper::vec3 TransformWrapper::GlobalRight()                const { return m_transform.GlobalRight(); }
    TransformWrapper::vec3 TransformWrapper::GlobalUp()                   const { return m_transform.GlobalUp(); }
    TransformWrapper::vec3 TransformWrapper::GlobalForward()              const { return m_transform.GlobalForward(); }

    TransformWrapper::mat4 TransformWrapper::GetLocalMatrix()             const { return m_transform.GetLocalMatrix(); }
    TransformWrapper::mat4 TransformWrapper::GetGlobalMatrix()            const { return m_transform.GetGlobalMatrix(); }

    bool TransformWrapper::HasChanged()                                    const { return m_transform.HasChanged(); }
    bool TransformWrapper::IsDirty()                                       const { return m_transform.IsDirty(); }

    TransformWrapper::vec3 TransformWrapper::GetGlobalPosition()          const { return m_transform.GetGlobalPosition(); }
    TransformWrapper::mat4 TransformWrapper::GetGlobalRotationMatrix()    const { return m_transform.GetGlobalRotationMatrix(); }
    TransformWrapper::vec3 TransformWrapper::GetGlobalRotationRad()       const { return m_transform.GetGlobalRotationRad(); }
    TransformWrapper::vec3 TransformWrapper::GetGlobalRotationDeg()       const { return m_transform.GetGlobalRotationDeg(); }
    TransformWrapper::vec3 TransformWrapper::GetGlobalScale()             const { return m_transform.GetGlobalScale(); }

    TransformWrapper::vec3& TransformWrapper::Position() { return m_transform.Position(); }
    TransformWrapper::vec3& TransformWrapper::Scale() { return m_transform.Scale(); }
    // note : scale must be set using setEulerAngle (internally uses quaternions)

    // Local Setters
    void TransformWrapper::SetPosition(vec3 const& pos) { m_transform.SetPosition(pos); }
    void TransformWrapper::SetEulerAngles(vec3 const& eulerAngle) { m_transform.SetEulerAngles(eulerAngle); }
    void TransformWrapper::SetScale(vec3 const& scale) { m_transform.SetScale(scale); }

    // Global Setters
    void TransformWrapper::SetGlobalPosition(vec3 const& position) { m_transform.SetGlobalPosition(position); }
    void TransformWrapper::SetGlobalScale(vec3 const& scale) { m_transform.SetGlobalScale(scale); }
    void TransformWrapper::SetGlobalAngle(vec3 const& euler_angles) { m_transform.SetGlobalAngle(euler_angles); }
    void TransformWrapper::SetGlobalTransform(vec3 const& position, vec3 const& euler_angles, vec3 const& scale) { m_transform.SetGlobalTransform(position, euler_angles, scale); }
    void SetGlobalTransform(glm::mat4 const& targetGlobalTransform) { }
}