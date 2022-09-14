#include "..\include\Transform.h"
#include "Quaternion/include/Transform.h"
#include <glm/ext/matrix_transform.hpp>
#include <iomanip>

#include <glm/gtx/matrix_decompose.hpp>

/*-----------------------------------------------------------------------------*/
/* Getter Functions                                                            */
/*-----------------------------------------------------------------------------*/

Transform::vec3 Transform::GetPosition() const { return m_position; }

Transform::quat Transform::GetRotationQuat() const { return m_orientation.value; }

Transform::vec3 Transform::GetEulerAngles() const { return m_eulerRotation; }

Transform::vec3 Transform::GetScale() const { return m_scale; }

Transform::vec3 Transform::LocalRight() const { return m_localTransform[0]; }

Transform::vec3 Transform::LocalUp() const { return m_localTransform[1]; }

Transform::vec3 Transform::LocalForward() const { return m_localTransform[2]; }

Transform::vec3 Transform::GlobalRight() const { return m_globalTransform[0]; }

Transform::vec3 Transform::GlobalUp() const { return m_globalTransform[1]; }

Transform::vec3 Transform::GlobalForward() const { return m_globalTransform[2]; }

Transform::mat4 Transform::GetLocalMatrix() const { return m_localTransform; }

Transform::mat4 Transform::GetGlobalMatrix() const { return m_globalTransform; }

/*-----------------------------------------------------------------------------*/
/* Setter Functions                                                            */
/*-----------------------------------------------------------------------------*/

Transform::vec3& Transform::Position() { return m_position; }

Transform::vec3& Transform::Scale() { return m_scale; }

void Transform::SetPosition(vec3 const& pos) { m_position = pos; }

void Transform::SetRotation(vec3 const& euler_angles_degrees)
{
    m_eulerRotation = euler_angles_degrees;
    m_orientation = quaternion::from_euler(glm::radians(m_eulerRotation));
}

void Transform::SetScale(vec3 const& scale) { m_scale = scale; }

void Transform::SetGlobalPosition(vec3 const& position)
{
    m_globalPosition = position;
    CalculateGlobalTransform();
}

void Transform::SetGlobalRotation(vec3 const& euler_angles_degrees)
{
    m_globalEulerRotation = euler_angles_degrees;
    m_globalOrientation = quaternion::from_euler(glm::radians(m_globalEulerRotation));
    CalculateGlobalTransform();
}

void Transform::SetGlobalScale(vec3 const& scale)
{
    m_globalScale = scale;
    CalculateGlobalTransform();
}

void Transform::SetGlobalTransform(vec3 const& position, vec3 const& euler_angles_degrees, vec3 const& scale)
{
    m_globalPosition = position;

    m_globalEulerRotation = euler_angles_degrees;
    m_globalOrientation = quaternion::from_euler(glm::radians(m_globalEulerRotation));

    m_globalScale = scale;

    CalculateGlobalTransform();

    //m_dirty = true;

    //auto t = glm::translate(glm::mat4{ 1.f }, position);
    //auto [axis, angle] = quaternion::to_axis_angle(quaternion::from_euler(glm::radians(euler_angles_degrees)));
    //auto r = glm::rotate(glm::mat4{ 1.f }, angle, axis);
    //auto s = glm::scale(glm::mat4{ 1.f }, scale);

    //glm::mat4 desiredGlobal = t * r * s; //glm::trs_matrix(position, euler_angles, scale);
    //m_globalTransform = desiredGlobal;
    ////glm::mat4 parentTf = GameObject::FindWithInstanceID(m_parent).Transform().GetGlobalMatrix();
    ////if (parentTf == glm::mat4::identity())
    ////{
    ////    m_localTransform = desiredGlobal;
    ////}
    ////else
    ////{
    ////    m_localTransform = glm::inverse(parentTf).value() * desiredGlobal;
    ////}

    //RecalculateLocalValues();
}

void Transform::SetGlobalTransform(mat4 const& desired_global_transform)
{
    m_globalTransform = desired_global_transform;
    DecomposeValues(m_globalTransform, m_globalScale, m_globalEulerRotation, m_globalOrientation.value, m_globalPosition);

    //glm::mat4 parentGlobalTf = static_cast<GameObject>(m_parent).Transform().GetGlobalMatrix();
    //if (parentGlobalTf == glm::mat4::identity())
    //{
    //    m_localTransform = desiredGlobalTransform;
    //}
    //else
    //{
    //    m_localTransform = glm::inverse(parentGlobalTf) * desiredGlobalTransform;
    //}

    //RecalculateLocalValues();
}

Transform::vec3 Transform::GetGlobalPosition() const 
{
    return m_globalPosition;
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
Transform::mat4 Transform::GetGlobalRotationMatrix() const
{
    //return quaternion::to_matrix(m_orientation);
    return quaternion::to_matrix(m_globalOrientation);
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
Transform::vec3 Transform::GetGlobalRotationDeg() const
{
    return glm::degrees(GetGlobalRotationRad());
}

Transform::quat Transform::GetGlobalRotationQuat() const
{
    return m_globalOrientation;
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
Transform::vec3 Transform::GetGlobalRotationRad() const
{
    return quaternion::to_euler(m_globalOrientation);
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
Transform::vec3 Transform::GetGlobalScale() const
{
    ////calculate global scale by calculating the length of each row which represents
    ////the scale of that particular axis.
    //glm::vec3 scale = glm::vec3{ glm::length(m_globalTransform[0]), glm::length(m_globalTransform[1]), glm::length(m_globalTransform[2]) };

    //// support for negative scaling
    //glm::vec3 negativeScaling{ m_scale.x < 0.f ? -1.f : 1.f, m_scale.y < 0.f ? -1.f : 1.f , m_scale.z < 0.f ? -1.f : 1.f };

    //return scale * negativeScaling;

    return m_globalScale;
}

void Transform::CalculateLocalTransform()
{
    auto t = glm::translate(glm::mat4{ 1.f }, m_position);
    m_orientation = quaternion::from_euler(glm::radians(m_eulerRotation));
    auto [axis, angle] = quaternion::to_axis_angle(m_orientation);
    auto r = glm::rotate(glm::mat4{ 1.f }, angle, axis);
    auto s = glm::scale(glm::mat4{ 1.f }, m_scale);

    m_localTransform = t * r * s;
}

void Transform::CalculateGlobalTransform()
{
    auto t = glm::translate(glm::mat4{ 1.f }, m_globalPosition);
    m_globalOrientation = quaternion::from_euler(glm::radians(m_globalEulerRotation));
    auto [axis, angle] = quaternion::to_axis_angle(m_globalOrientation);
    auto r = glm::rotate(glm::mat4{ 1.f }, angle, axis);
    auto s = glm::scale(glm::mat4{ 1.f }, m_globalScale);

    m_globalTransform = t * r * s;

    DecomposeValues(m_globalTransform, m_scale, m_eulerRotation, m_orientation.value, m_position);
    CalculateLocalTransform();
}

void Transform::DecomposeValues(mat4 const& matrix, glm::vec3& scale, glm::vec3& euler_angle, glm::quat& orientation, glm::vec3& position)
{
    glm::vec3 unusedSkew;
    glm::vec4 unusedPerspective;
    glm::decompose(matrix, scale, orientation, position, unusedSkew, unusedPerspective);
    euler_angle = glm::degrees(quaternion::to_euler(orientation));

    //RecalculateLocalValues();

    //// recalculate local values given global transform.
    //mat4 result{ m_globalTransform };
    //m_globalPosition = result[3];
    //result[3] = glm::vec4{ 0.f, 0.f, 0.f, 1.0f };
    //// perform symmetric diagonalization
    //glm::mat3 a = { result };
    //// first find ATranposed * A
    //glm::mat3 ata = glm::transpose(a) * a;
    ////

    //m_globalScale = vec3{ glm::length(result[0]), glm::length(result[1]), glm::length(result[2]) };

    //result[0] /= m_globalScale[0];
    //result[1] /= m_globalScale[1];
    //result[2] /= m_globalScale[2];

    //m_globalOrientation = quaternion::from_matrix({ result[0], result[1], result[2] });
    //m_globalEulerRotation = glm::degrees(quaternion::to_euler(m_globalOrientation));

    ////Calulate the new local transform
    //CalculateLocalTransform();
}


std::ostream& operator<< (std::ostream& os, Transform::vec3 const& vector)
{
    os << std::fixed << std::setprecision(4);

    os << "(" << vector.x << "," << vector.y << "," << vector.z << ")";
    return os;
}

std::ostream& operator<< (std::ostream& os, Transform::mat4 const& matrix)
{
    os << std::fixed << std::setprecision(4) << "\n";
    auto mat = glm::transpose(matrix);
    for (int i = 0; i < 4; ++i)
    {
        os << "[";
        for (int j = 0; j < 4; ++j)
        {
            os << " " << mat[i][j];
        }
        os << "]\n";
    }

    return os;
}