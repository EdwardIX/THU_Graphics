#include <iostream>
#include "ppm.hpp"
#include "utils.hpp"
#include "omp.h"

void PPM::run() {
    printf("Start PPM Algorithm\n");
    image->SetAllPixels(Vector3f::ZERO);

    // Setup ViewPoints
    for(int x=0; x<image->Width(); x++) 
        for(int y=0; y<image->Height(); y++) {
            Ray ray = camera->generateRay(Vector2f(x, y));
            traceRay(ray, Vector3f(1.0), 0, x, y);
        }
    
    printf("ViewPoints Has been Set up: total %d ViewPoints\n", (int)vps.size());

    // Main Iteration 
    for(int iter=0; iter < PPM_ITER; iter++) {
        printf("Starting Iteration %d / %d\n", iter+1, PPM_ITER);

        #ifdef USE_OPENMP
        #pragma omp parallel num_threads(8)
        #pragma omp for
        #endif
        // Emit Photons
        for(int photons=0; photons < PPM_PHOTON_PER_ITER; photons++) {
            if(photons % (PPM_PHOTON_PER_ITER/20) == 0) 
                printf("     (%d / %d) Emit Photons Complete %d%c\n", iter+1, PPM_ITER, photons / (PPM_PHOTON_PER_ITER/20)*5,'%');

            Vector3f color;
            Ray ray = light->getPhoton(color);
            tracePhoton(ray, color, 0, 0);
        }

        // Build KDTree
        printf("     (%d / %d) Build KDTree....\n", iter+1, PPM_ITER);
        kdt.build();

        #ifdef USE_OPENMP
        #pragma omp parallel num_threads(8)
        #pragma omp for
        #endif
        // Process all view points
        for(int vpi=0; vpi<(int)vps.size(); vpi++) {
            if(vpi % (vps.size()/20) == 0) 
                printf("     (%d / %d) Update ViewPoints Complete %d%c\n", iter+1, PPM_ITER, vpi / (vps.size()/20)*5,'%');
            // query all photons from KDTree
            ViewPoint &vp = vps[vpi];
            std::vector<Photon*> P;
            kdt.search(P, vp.hit.getPosition(), vp.radius); 

            // use BRDF to calculate energy
            Vector3f update(0);
            for(auto pho : P) {
                update += vp.hit.Shade(-vp.direction, -pho->direction, pho->color);
            }
            
            // update result & radius ...
            if(iter != 0) { // not the first iter
                float init_photon_num = vp.photon_num;
                vp.photon_num += PPM_ALPHA * P.size();
                if(!O(vp.photon_num)) continue; // ignore those do not receive photons
                float rate = vp.photon_num / (init_photon_num + P.size());
                vp.radius *= sqrt(rate);
                vp.result += update;
                vp.result *= rate;
            } else { // the first iter
                vp.photon_num += P.size();
                vp.result += update;
            }
        }

        kdt.clear();
    }

    // Render Image
    // TODO: Include Power for View Points that hit lights...
    printf("Start Render To Image...\n");
    for(auto &vp : vps) {
        vp.result *= 1 / (pi * vp.radius * vp.radius * PPM_PHOTON_NUM);
        image->AddPixel(vp.pixel_x, vp.pixel_y, vp.result * vp.color);
    }
    printf("Finish PPM Algorithm\n");
}

void PPM::traceRay(Ray ray, Vector3f color, int depth, int pixel_x, int pixel_y) {
    if(color.length() < eps || depth >= PPM_TRACERAY_MAXDEPTH) return;

    // First, find the hit point (with group or light);
    Hit hit;
    bool result_group = group->intersect(ray, hit, eps);
    Vector3f light_color; float light_thit;
    bool result_light = light->intersect(ray, eps, light_thit, light_color);

    // Then, judge the type of intersection...
    if(!result_group) { // not hit object
        if(result_light) image->AddPixel(pixel_x, pixel_y, color * light_color);
        else             image->AddPixel(pixel_x, pixel_y, color * background_color);
        return;
    } else { // find a hit object
        if(result_light && light_thit < hit.getT()) { // hit light first
            image->AddPixel(pixel_x, pixel_y, color * light_color);
            return;
        }
        if(hit.getMaterial()->coef_reflect > eps) { // reflect
            Vector3f next_color = color * hit.getMaterial()->coef_reflect;
            traceRay(hit.reflection(ray), next_color, depth+1, pixel_x, pixel_y);
        }
        if(hit.getMaterial()->coef_refract > eps) { // refract
            Vector3f next_color = 
                hit.getIn(ray) ? color : color * hit.getMaterial()->coef_refract * hit.getRefractionColor(hit.getT());
            traceRay(hit.refraction(ray), next_color, depth+1, pixel_x, pixel_y);
        }
        if(hit.getMaterial()->coef_diff > eps) { // diffusion (record)
            vps.push_back(ViewPoint(hit, ray.getDirection(), color, pixel_x, pixel_y));
        }
    }
}

void PPM::tracePhoton(Ray ray, Vector3f color, int depth, int diff_count) {
    if(color.length() < eps || depth >= PPM_TRACEPHOTON_MAXDEPTH) return;

    // First, find the hit point;
    Hit hit;
    bool result = group->intersect(ray, hit, eps);
    if(!result) return;
    else {
        if(hit.getMaterial()->coef_reflect > eps) { // reflect
            Vector3f next_color = color * hit.getMaterial()->coef_reflect; // the part for reflect
            tracePhoton(hit.reflection(ray), next_color, depth+1, diff_count);
        }
        if(hit.getMaterial()->coef_refract > eps) { // refract
            Vector3f next_color = 
                hit.getIn(ray) ? color : color * hit.getRefractionColor(hit.getT());
            next_color *= hit.getMaterial()->coef_refract; // the part for refract
            tracePhoton(hit.refraction(ray), next_color, depth+1, diff_count);
        }
        if(hit.getMaterial()->coef_diff > eps) { // diffusion (record + absorb/bounce)
            #pragma omp critical
            kdt.push(Photon(hit.getPosition(), ray.getDirection(), color)); // record

            Vector3f diffuse_color = hit.getDiffuseColor();
            float p = std::max(diffuse_color[0], std::max(diffuse_color[1], diffuse_color[2])); // calc probability for absorb

            if(rand_range(0, 1) >= p) return; // absorb
            else { // bounce
                Vector3f next_color = color * hit.getDiffuseColor() / p;
                next_color *= hit.getMaterial()->coef_diff;
                tracePhoton(hit.diffusion(ray), next_color, depth+1, diff_count+1);
            }
        }
    }

}