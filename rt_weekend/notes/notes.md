# Raytracing In One Weekend

This is called raytracing, but what we're really making is a pathtracer!

Think of a ray as the following function:

> P(t) = A + tB
* `P` is a 3D position along a line in 3D.
* `A` is the ray origin
* `B` is the ray direction
* `t` is a real number, increasing or decreasing this moves the ray along the ray

The steps of a raytracer are as follows:
1. Calculate the ray from the "eye" through the pixel
2. Determine which objects the ray intersects
3. Compute a color for the closest interaction point

We are going to use a 16:9 aspect ratio for our rendered image. Aspect ratios can be used to calculate a value when the width is divided by the height:

> 16:9 = width / height = 16/9 = 1.7778

## Viewports

Viewports are a virtual rectangle in the 3D world that contains the grid of image pixel locations. If the pixels are spaced the same distance horizontally as they are vertically, the viewport that bounds them will have the same aspect ratio as the rendered image.

That distance between pixels is what's known as *pixel spacing*. Square pixels is the standard.

## The Linear Blue Gradient

We use a simple function to determine a linear blue gradient image:

> blended_value = (1 - a) * start_value + a * end_value

## Ray-Sphere Intersection!

To render a sphere, we need to use a function that calculates whether or not a ray hits a sphere:

> x^2 + y^2 + z^2 = r^2

You can also think of this as saying that if a given point (x,y,z) is on the surface of the sphere, then `x^2 + y^2 + z^2 = r^2`. If a given point (x,y,z) is inside the sphere, then `x^2 + y^2 + z^2 < r^2`, and if a given point (x,y,z) is outside the sphere, then `x^2 + y^2 + z^2 > r^2`.

However, once we decide we want that sphere at an arbitrary point in space, the function changes:

> (Cx − x)^2 + (Cy − y)^2 + (Cz − z)^2 = r^2

Developing this function a little leads to the following in C++ code:

```c++
bool hit_sphere(const point3& center, double radius, const ray& r)
{
    vec3 oc = center - r.origin();
    double a = dot(r.direction(), r.direction());
    double b = -2.0 * dot(r.direction(), oc);
    double c = dot(oc, oc) - radius * radius;
    double discriminant = b * b - 4 * a * c;
    return discriminant >= 0;
}
```

## Surface Normals

Obviously, we know that surface normals are vectors (usually of length 1) that point perpedicularly out from the surface of a given object.

For a sphere the outward normal is the direction of the hit point minus the center.

## Antialiasing

A single ray through the center of each pixel indicates that we are doing something called *point sampling*.

The problem with this method is illustrated by the case in which we would try to render a checkerboard from far away. Only four or so rays might hit it, and those rays may only hit white tiles, meaning all we would see is a white box in the distance.

In real life, however, our eyes would naturally interpolate the black and white colors of the checkerboard and it would appear gray.

## Diffuse Materials

Diffuse lighting, a.k.a. matte lighting. With rays, the darker the color of the object the more rays will be absorbed by the object. This works in much the way light does in the real world. Color is really just light bands being either absorbed or reflected by the object, and remember that the color we perceive is the combination of whatever light bands get reflected off the surface of the object!

Something to be cognizant of in rendering code especially is floating point errors at the lower ends of a floating point number. When something could possibly be very close to zero, it might create visual artifacting due to precision errors at that tiny scale, so it's important to account for that by setting proper epsilon limitations if you can!

'Albedo' is latin for 'whiteness'. It is used to define some form of *fractional reflectance*.