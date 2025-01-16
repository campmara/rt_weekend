#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"

class camera
{
public:
    double aspect_ratio = 1.0;  // ratio of image width over height
    int image_width = 100;      // rendered image width in pixel count
    int samples_per_pixel = 10; // count of random samples for each pixel
    int max_depth = 10;         // maximum number of ray bounces into scene

    double vfov = 90.0;                // vertical view angle (field of view)
    point3 lookfrom = point3(0, 0, 0); // point camera is looking from
    point3 lookat = point3(0, 0, -1);  // point camera is looking at
    vec3 vup = vec3(0, 1, 0);          // camera-relative "up" direction

    double defocus_angle = 0; // variation angle of rays through each pixel
    double focus_dist = 10;   // distance from camera lookfrom point to plane of perfect focus

    void render(const hittable &world)
    {
        initialize();

        // render
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int y = 0; y < image_height; ++y)
        {
            std::clog << "\rScanlines remaining: " << (image_height - y) << ' ' << std::flush;
            for (int x = 0; x < image_width; ++x)
            {
                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; ++sample)
                {
                    ray r = get_ray(x, y);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }

        std::clog << "\rDone.                 \n";
    }
private:
    int    image_height;        // Rendered image height
    double pixel_samples_scale; // Color scale factor for a sum of pixel samples
    point3 center;              // Camera center
    point3 pixel00_loc;         // Location of pixel 0, 0
    vec3   pixel_delta_u;       // Offset to pixel to the right
    vec3   pixel_delta_v;       // Offset to pixel below
    vec3 u, v, w;               // camera frame basis vectors
    vec3 defocus_disk_u;        // defocus disk horizontal radius
    vec3 defocus_disk_v;        // defocus disk vertical radius

    void initialize()
    {
        // calculate the image height from image_width and aspect_ratio and ensure that it's at least 1
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // determine viewport dimensions
        double theta = degrees_to_radians(vfov);
        double h = std::tan(theta / 2.0);
        double viewport_height = 2 * h * focus_dist;
        double viewport_width = viewport_height * (double(image_width) / image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = viewport_width * u;   // vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -v; // vector down viewport vertical edge

        // calculate the horizontal and vertical delta vectors from pixel to pixel
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // calculate the location of the upper left pixel
        vec3 viewport_upper_left = center - (focus_dist * w) - viewport_u / 2.0 - viewport_v / 2.0;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // calculate the camera defocus disk basis vectors
        double defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const
    {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j.
        vec3 offset = sample_square();
        vec3 pixel_sample = pixel00_loc
            + ((i + offset.x()) * pixel_delta_u)
            + ((j + offset.y()) * pixel_delta_v);

        vec3 ray_origin = (defocus_angle <= 0) ? center : sample_defocus_disk();
        vec3 ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const
    {
        // returns the vector to a random point in the [-0.5, -0.5] - [+0.5, +0.5] unit square
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 sample_defocus_disk() const
    {
        // returns a random point in the camera defocus disk
        vec3 p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    color ray_color(const ray &r, int depth, const hittable &world) const
    {
        if (depth <= 0)
        {   // if we've exceeded the ray bounce limit, no more light is gathered
            return color(0, 0, 0);
        }

        hit_record rec;
        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
            {
                return attenuation * ray_color(scattered, depth - 1, world);
            }
            return color(0, 0, 0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }

    // diffuse material
    vec3 hit_direction_diffuse(const vec3 &normal) const
    {
        return random_on_hemisphere(normal);
    }

    // lambert material
    vec3 hit_direction_lambert(const vec3 &normal) const
    {
        return normal + random_unit_vector();
    }
};

#endif // !CAMERA_H
