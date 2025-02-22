#ifndef HITTABLE_H
#define HITTABLE_H

#include "rt_weekend.h"

// tell the compiler that the material class will be defined later
// (we are trying to avoid circular dependencies between this and the material header)
class material;

struct hit_record
{
    point3 p;
    vec3 normal;
    shared_ptr<material> mat;
    double t;
    bool front_face;

    void set_face_normal(const ray& r, const vec3& outward_normal)
    {
        // sets the hit record normal vector
        // NOTE: the parameter 'outward_normal' is assumed to have unit length

        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
public:
    virtual ~hittable() = default;
    
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif // !HITTABLE_H
