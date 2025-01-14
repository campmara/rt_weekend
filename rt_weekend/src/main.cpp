#include "color.h"
#include "vec3.h"
#include "ray.h"

#include <iostream>

double hit_sphere(const point3& center, double radius, const ray& r)
{
    vec3 oc = center - r.origin();
    double a = dot(r.direction(), r.direction());
    double b = -2.0 * dot(r.direction(), oc);
    double c = dot(oc, oc) - radius * radius;
    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
    {
        return -1.0;
    }
    else
    {
        return (-b - std::sqrt(discriminant)) / (2.0 * a);
    }
}

color ray_color(const ray& r)
{
    double t = hit_sphere(point3(0, 0, -1), 0.5, r); 
    if (t > 0.0)
    {
        vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1));
        return 0.5 * color(N.x() + 1.0, N.y() + 1.0, N.z() + 1.0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    double a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main()
{
    // image
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    // calculate the image height from the above values and ensure that it's at least 1
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // camera
    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * (double(image_width) / image_height);
    point3 camera_center = point3(0, 0, 0);

    // calculate the vectors across the horizontal and down the vertical viewport edges
    vec3 viewport_u = vec3(viewport_width, 0, 0);
    vec3 viewport_v = vec3(0, -viewport_height, 0);

    // calculate the horizontal and vertical delta vectors from pixel to pixel
    vec3 pixel_delta_u = viewport_u / image_width;
    vec3 pixel_delta_v = viewport_v / image_height;

    // calculate the location of the upper left pixel
    vec3 viewport_upper_left =
        camera_center - vec3(0, 0, focal_length) - viewport_u / 2.0 - viewport_v / 2.0;
    vec3 pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int y = 0; y < image_height; ++y)
    {
        std::clog << "\rScanlines remaining: " << (image_height - y) << ' ' << std::flush;
        for (int x = 0; x < image_width; ++x)
        {
            vec3 pixel_center = pixel00_loc + (x * pixel_delta_u) + (y * pixel_delta_v);
            vec3 ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);

            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }

    std::clog << "\rDone.                 \n";
}