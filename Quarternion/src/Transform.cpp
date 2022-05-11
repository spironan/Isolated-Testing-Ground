#include "Transform.h"
#include <glm/ext/matrix_transform.hpp>

Transform::Transform()
: m_position{ 0.f, 0.f, 0.f }
, m_orientation{ quaternion::identity() }
, m_scale{ 1.f, 1.f, 1.f }
, m_dirty{ false }
, m_hasChanged{ false }
, m_globalTransform{ glm::mat4{ 1.f } }
, m_localTransform{ glm::mat4{ 1.f } }
{
}

void Transform::SetGlobalPosition(vec3 const& position)
{
    SetGlobalTransform(position, GetGlobalRotationRad(), GetGlobalScale());
}

void Transform::SetGlobalAngle(vec3 const& euler_angles)
{
    SetGlobalTransform(GetGlobalPosition(), euler_angles, GetGlobalScale());
}

void Transform::SetGlobalScale(vec3 const& scale)
{
    SetGlobalTransform(GetGlobalPosition(), GetGlobalRotationRad(), scale);
}

void Transform::SetGlobalTransform(vec3 const& position, vec3 const& euler_angles, vec3 const& scale)
{
    m_dirty = true;
    auto t = glm::translate(glm::mat4{ 1.f }, position);
    auto [axis, angle] = quaternion::to_axis_angle(quaternion::from_euler(euler_angles));
    auto r = glm::rotate(glm::mat4{ 1.f }, angle, axis);
    auto s = glm::scale(glm::mat4{ 1.f }, scale);
    

    glm::mat4 desiredGlobal = t * r * s; //glm::trs_matrix(position, euler_angles, scale);
    //glm::mat4 parentTf = GameObject::FindWithInstanceID(m_parent).Transform().GetGlobalMatrix();
    //if (parentTf == glm::mat4::identity())
    //{
    //    m_localTransform = desiredGlobal;
    //}
    //else
    //{
    //    m_localTransform = glm::inverse(parentTf).value() * desiredGlobal;
    //}

    {
        mat4 result{ m_localTransform };
        m_position = result[3];
        result[3] = glm::vec4{ 0.f, 0.f, 0.f, 1.0f };
        m_scale = vec3{ length(result[0]), length(result[1]), length(result[2]) };

        result[0] /= m_scale[0];
        result[1] /= m_scale[1];
        result[2] /= m_scale[2];

        m_orientation = quaternion::from_matrix({ result[0], result[1], result[2] });
    }
}

void Transform::SetGlobalTransform(glm::mat4 const& desiredGlobalTransform)
{
    m_dirty = true;
    //glm::mat4 parentGlobalTf = static_cast<GameObject>(m_parent).Transform().GetGlobalMatrix();
    //if (parentGlobalTf == glm::mat4::identity())
    //{
    //    m_localTransform = desiredGlobalTransform;
    //}
    //else
    //{
    //    m_localTransform = glm::inverse(parentGlobalTf) * desiredGlobalTransform;
    //}

    {
        mat4 result{ m_localTransform };
        m_position = result[3];
        result[3] = glm::vec4{ 0.f, 0.f, 0.f, 1.0f };
        m_scale = vec3{ length(result[0]), length(result[1]), length(result[2]) };

        result[0] /= m_scale[0];
        result[1] /= m_scale[1];
        result[2] /= m_scale[2];

        m_orientation = quaternion::from_matrix({ result[0], result[1], result[2] });
    }
}

/****************************************************************************//*!
    @brief    Retrieves the global rotation matrix of this object from the global
            transformation matrix.

    @warning  Take note that in most cases, unless being called after the Transform
            Runtime has been executed, this will be the position from the previous
            frame.

    @return   An glm::mat4 that represents the current rotation matrix
            of this GameObject in global coordinates.
*//*****************************************************************************/
glm::mat4 Transform::GetGlobalRotationMatrix() const
{
    return quaternion::to_matrix(m_orientation);
}

/****************************************************************************//*!
    @brief    Retrieves the global rotation of this object in degress
            from the global transformation matrix.
            link to explaination : https://tinyurl.com/DeriveTRSfrom2dMat

    @warning  Take note that in most cases, unless being called after the Transform
            Runtime has been executed, this will be the rotation from the previous
            frame.

    @return   A float in degrees that represents the rotation angle of the previous
            frame of this Component in global coordinates.
*//*****************************************************************************/
glm::vec3 Transform::GetGlobalRotationDeg() const
{
    return glm::degrees(GetGlobalRotationRad());
    //auto res = GetGlobalRotationRad();
    //return { glm::degrees(res.x), glm::degrees(res.y), glm::degrees(res.z) };
}

/****************************************************************************//*!
    @brief    Retrieves the global rotation of this object in radians
            from the global transformation matrix.
            link to explaination : https://tinyurl.com/DeriveTRSfrom2dMat

    @warning  Take note that in most cases, unless being called after the Transform
            Runtime has been executed, this will be the rotation from the previous
            frame.

    @return   A float in radians that represents the rotation angle of the previous
            frame of this Component in global coordinates.
*//*****************************************************************************/
glm::vec3 Transform::GetGlobalRotationRad() const
{
    return quaternion::to_euler(m_orientation);
}

/****************************************************************************//*!
    @brief    Retrieves the global scale of this object from the global
            transformation matrix.

    @warning  Take note that in most cases, unless being called after the Transform
            Runtime has been executed, this will be the position from the previous
            frame.

    @return   An glm::vec3 that represents the current scale of this GameObject in
            global coordinates.
*//*****************************************************************************/
glm::vec3 Transform::GetGlobalScale() const
{
    //calculate global scale by calculating the length of each row which represents
    //the scale of that particular axis.
    glm::vec3 scale = glm::vec3{ glm::length(m_globalTransform[0]), glm::length(m_globalTransform[1]), glm::length(m_globalTransform[2]) };

    // support for negative scaling
    glm::vec3 negativeScaling{ m_scale.x < 0.f ? -1.f : 1.f, m_scale.y < 0.f ? -1.f : 1.f , m_scale.z < 0.f ? -1.f : 1.f };

    return scale * negativeScaling;
}

