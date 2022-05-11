#pragma once

#include "glm/glm.hpp"
#include "Quaternion.h"

class Transform final
{
public:
    using vec3 = glm::vec3;
    using mat4 = glm::mat4;

    Transform();

    /*-----------------------------------------------------------------------------*/
    /* Getter Functions                                                            */
    /*-----------------------------------------------------------------------------*/
    vec3 GetPosition()         const { return m_position; }
    //glm::vec4 GetRotationQuat()const { return { m_orientation.x, m_orientation.y, m_orientation.z, m_orientation.w }; }
    //vec3 GetEulerAngles()      const { auto res = quaternion::to_euler(m_orientation);  return { glm::degrees(res.x), glm::degrees(res.y), glm::degrees(res.z) }; }
    quaternion  GetRotationQuat() const { return m_orientation.value; }
    vec3        GetEulerAngles()  const { auto res = quaternion::to_euler(m_orientation);  return { glm::degrees(res.x), glm::degrees(res.y), glm::degrees(res.z) }; }
    vec3 GetScale()            const { return m_scale; }

    vec3 LocalRight()          const { return m_localTransform[0]; }
    vec3 LocalUp()             const { return m_localTransform[1]; }
    vec3 LocalForward()        const { return m_localTransform[2]; }

    vec3 GlobalRight()         const { return m_globalTransform[0]; }
    vec3 GlobalUp()            const { return m_globalTransform[1]; }
    vec3 GlobalForward()       const { return m_globalTransform[2]; }

    mat4 GetLocalMatrix()      const { return m_localTransform; }
    mat4 GetGlobalMatrix()     const { return m_globalTransform; }

    bool HasChanged()          const { return m_hasChanged; }

    /*-----------------------------------------------------------------------------*/
    /* Setter Functions                                                            */
    /*-----------------------------------------------------------------------------*/
    vec3& Position() { m_dirty = true; return m_position; }
    vec3& Scale() { m_dirty = true; return m_scale; }

    void SetPosition(vec3 const& pos) { m_dirty = true; m_position = pos; }
    void SetEulerAngles(vec3 eulerAngle) 
    {
        m_dirty = true; 
        m_orientation = quaternion::from_euler({ glm::radians(eulerAngle.x), glm::radians(eulerAngle.y), glm::radians(eulerAngle.z) });
    }
    void SetScale(vec3 const& scale) { m_dirty = true; m_scale = scale; }

    void SetGlobalPosition(vec3 const& position);
    void SetGlobalScale(vec3 const& scale);
    /*void SetGlobalAngle(float angle);*/
    void SetGlobalAngle(vec3 const& euler_angles);
    /*void SetGlobalTransform(vec3 const& position, float angle, vec3 const& scale);*/
    void SetGlobalTransform(vec3 const& position, vec3 const& euler_angles, vec3 const& scale);
    void SetGlobalTransform(glm::mat4 const& targetGlobalTransform);

    /****************************************************************************//*!
        @brief     Retrieves the global position of this Component from the global
                transformation matrix.

        @warning   Take note that in most cases, unless being called after the Transform
                Runtime has been executed, this will be the position from the previous
                frame.

        @return    An vec3 that represents the previous position of this Component in
                global coordinates.
    *//*****************************************************************************/
    vec3 GetGlobalPosition() const { return m_globalTransform[3]; }

    /****************************************************************************//*!
        @brief     Retrieves the global rotation matrix of this object from the global
                transformation matrix.
                Link to explaination : https://tinyurl.com/deriveTRS

        @warning   Take note that in most cases, unless being called after the Transform
                Runtime has been executed, this will be the rotation matrix
                from the previous frame.

        @return    An mat4 that represents the rotation matrix from the previous frame
                of this Component in global coordinates.
    *//*****************************************************************************/
    mat4 GetGlobalRotationMatrix() const;
    vec3 GetGlobalRotationRad() const;
    vec3 GetGlobalRotationDeg() const;

    /****************************************************************************//*!
        @brief     Retrieves the global scale of this object from the global
                transformation matrix.
                Link to explaination : https://tinyurl.com/deriveTRS

        @warning   Take note that in most cases, unless being called after the Transform
                Runtime has been executed, this will be the scale from the previous
                frame.

        @return    An vec3 that represents the previous scale of this GameObject in
                global coordinates.
    *//*****************************************************************************/
    vec3 GetGlobalScale() const;

private:

    mat4                m_globalTransform;
    mat4                m_localTransform;

    vec3                m_position;
    quaternion          m_orientation;
    vec3                m_scale;

    bool                m_dirty;
    bool                m_hasChanged;
};
