/** CSci-4611 Assignment 3:  Earthquake
 */

#include "earth.h"
#include "config.h"

#include <vector>

// for M_PI constant
#define _USE_MATH_DEFINES
#include <math.h>


Earth::Earth() {
}

Earth::~Earth() {
}

void Earth::Init(const std::vector<std::string> &search_path) {
    // init shader program
    shader_.Init();
    
    // init texture: you can change to a lower-res texture here if needed
    earth_tex_.InitFromFile(Platform::FindFile("earth-2k.png", search_path));

    // init geometry
    const int nslices = 360;    //split world into 360x180 mesh
    const int nstacks = 180;    //so we have an (x,y) for each degree
    const int nindices = (nslices + 1) * (nstacks + 1);

    const float pi_180 = M_PI / 180;

    std::vector<unsigned int> indices;

    // Initialize globe/map vertices and texture coordinates
    float yt = nstacks;
    for (float y = -(M_PI_2); y <= M_PI_2 + pi_180; y += pi_180) {   //added an extra pi_180 to the bound to account
        float v = yt / nstacks;                                       //for rounding errors - else it was 1 short 
        float xt = 0.0;
        for (float x = -(M_PI); x <= M_PI + pi_180; x += pi_180) {
            float u = xt / nslices;
            plane_vertices.push_back(Point3(x, y, 0));
            plane_normals.push_back(Vector3(0, 0, 1));

            globe_vertices.push_back(LatLongToSphere(y, x));
            globe_normals.push_back((LatLongToSphere(y, x) - Point3()).ToUnit());
            
            morph_verts.push_back(LatLongToSphere(y, x));
            morph_norms.push_back((LatLongToSphere(y, x) - Point3()).ToUnit());

            tex_coords.push_back(Point2(u, v));
            xt += 1.0;
        }
        yt -= 1.0;
    }
    
    // Such a clean algorithm
    for (int i = 1; i < (nindices - nslices) - 1; i++) {
        indices.push_back(i - 1);
        indices.push_back(i);
        indices.push_back(i + nslices);

        indices.push_back(i + nslices);
        indices.push_back(i);
        indices.push_back(i + nslices + 1);
    }

    
    // Set everything into mesh
    earth_mesh_.SetVertices(globe_vertices);
    earth_mesh_.SetNormals(globe_normals);
    earth_mesh_.SetIndices(indices);
    earth_mesh_.SetTexCoords(0, tex_coords);
    earth_mesh_.UpdateGPUMemory();
}



void Earth::Draw(const Matrix4 &model_matrix, const Matrix4 &view_matrix, const Matrix4 &proj_matrix) {
    // Define a really bright white light.  Lighting is a property of the "shader"
    DefaultShader::LightProperties light;
    light.position = Point3(10,10,10);
    light.ambient_intensity = Color(1,1,1);
    light.diffuse_intensity = Color(1,1,1);
    light.specular_intensity = Color(1,1,1);
    shader_.SetLight(0, light);

    // Adust the material properties, material is a property of the thing
    // (e.g., a mesh) that we draw with the shader.  The reflectance properties
    // affect the lighting.  The surface texture is the key for getting the
    // image of the earth to show up.
    DefaultShader::MaterialProperties mat;
    mat.ambient_reflectance = Color(0.5, 0.5, 0.5);
    mat.diffuse_reflectance = Color(0.75, 0.75, 0.75);
    mat.specular_reflectance = Color(0.75, 0.75, 0.75);
    mat.surface_texture = earth_tex_;

    // Draw the earth mesh using these settings
    if (earth_mesh_.num_triangles() > 0) {
        shader_.Draw(model_matrix, view_matrix, proj_matrix, &earth_mesh_, mat);
    }
}


Point3 Earth::LatLongToSphere(double latitude, double longitude) const {

    float x = cos(latitude) * sin(longitude);
    float y = sin(latitude);
    float z = cos(latitude) * cos(longitude);
    
    return Point3(x,y,z);
}

Point3 Earth::LatLongToPlane(double latitude, double longitude) const {

    latitude = GfxMath::ToRadians(latitude);
    longitude = GfxMath::ToRadians(longitude);

    return Point3(longitude,latitude,0);
}



void Earth::DrawDebugInfo(const Matrix4 &model_matrix, const Matrix4 &view_matrix, const Matrix4 &proj_matrix) {
    // This draws a cylinder for each line segment on each edge of each triangle in your mesh.
    // So it will be very slow if you have a large mesh, but it's quite useful when you are
    // debugging your mesh code, especially if you start with a small mesh.
    for (int t=0; t<earth_mesh_.num_triangles(); t++) {
        std::vector<unsigned int> indices = earth_mesh_.triangle_vertices(t);
        std::vector<Point3> loop;
        loop.push_back(earth_mesh_.vertex(indices[0]));
        loop.push_back(earth_mesh_.vertex(indices[1]));
        loop.push_back(earth_mesh_.vertex(indices[2]));
        quick_shapes_.DrawLines(model_matrix, view_matrix, proj_matrix,
            Color(1,1,0), loop, QuickShapes::LinesType::LINE_LOOP, 0.005);
    }
}


void Earth::UpdateEarth(double alpha) {
    // Switch between globe and plane

    // Reached globe mode
    if (alpha == 0.0) {
        earth_mesh_.SetVertices(globe_vertices);
        earth_mesh_.SetNormals(globe_normals);
    }
    // Reached plane mode
    else if (alpha == 1.0){
        earth_mesh_.SetVertices(plane_vertices);
        earth_mesh_.SetNormals(plane_normals);
    }
    // Interpolate between globe/plane
    else {

        for (int i = 0; i < morph_verts.size(); i++) {
            morph_verts[i] = (globe_vertices[i].Lerp(plane_vertices[i], alpha));
            morph_norms[i] = (globe_normals[i].Lerp(plane_normals[i], alpha));
        }

        earth_mesh_.SetVertices(morph_verts);
        earth_mesh_.SetNormals(morph_norms);
    }
    
    earth_mesh_.UpdateGPUMemory();
}