#include "Common.h"
#include "OpenGLCommon.h"
#include "OpenGLMarkerObjects.h"
#include "OpenGLBgEffect.h"
#include "OpenGLMesh.h"
#include "OpenGLViewer.h"
#include "OpenGLWindow.h"
#include "TinyObjLoader.h"
#include "OpenGLSkybox.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <unordered_set>
#include <vector>
#include <string>

#ifndef __Main_cpp__
#define __Main_cpp__

#ifdef __APPLE__
#define CLOCKS_PER_SEC 100000
#endif

class MyDriver : public OpenGLViewer
{
    std::vector<OpenGLTriangleMesh *> mesh_object_array;
    clock_t startTime;
    float last_time;
    
    // Flame animation data (no class, just arrays)
    std::vector<Matrix4f> flame_transforms;
    std::vector<float> flame_offsets;
    int flame_start_index;
    
    // Starfield animation data
    std::vector<Matrix4f> star_transforms;
    std::vector<float> star_twinkle_offsets;
    int star_start_index;

public:
    MyDriver() : last_time(0.0f), flame_start_index(0), star_start_index(0) {}

    virtual void Initialize()
    {
        draw_axes = false;
        startTime = clock();
        last_time = 0.0f;
        OpenGLViewer::Initialize();
    }

    virtual void Initialize_Data()
    {
        // Load shaders
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/spaceship.frag", "spaceship");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/billboard.vert", "shaders/alphablend.frag", "billboard");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/basic.vert", "shaders/planet.frag", "planet");
        OpenGLShaderLibrary::Instance()->Add_Shader_From_File("shaders/starfield.vert", "shaders/starfield.frag", "starfield");
        
        // Load textures
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/metal_color.jpg", "metal_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/metal_normal.jpg", "metal_normal");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/star.png", "star_color");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/earth.jpg", "earth");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/lava.jpg", "lava");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/asteroidone.jpg", "asteroidone");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/asteroidtwo.jpg", "asteroidtwo");
        OpenGLTextureLibrary::Instance()->Add_Texture_From_File("tex/sun.png", "sun");
        
        // Add lights
        opengl_window->Add_Light(Vector3f(3.0f, 2.0f, 5.0f), Vector3f(0.1f, 0.1f, 0.1f), Vector3f(0.8f, 0.8f, 0.8f), Vector3f(0.5f, 0.5f, 0.5f));
        
        // Black background
        {
            auto bg = Add_Interactive_Object<OpenGLBackground>();
            bg->Set_Color(OpenGLColor(0.0f, 0.0f, 0.0f, 1.0f), OpenGLColor(0.0f, 0.0f, 0.0f, 1.0f));
            bg->Initialize();
        }
        
        // YOUR SPACESHIP
        {
            auto spaceship = Add_Obj_Mesh_Object("obj/spaceship.obj");
            
            Matrix4f t = Matrix4f::Identity();
            t << 0.3f, 0.0f, 0.0f, 0.0f,
                 0.0f, 0.3f, 0.0f, 0.0f,
                 0.0f, 0.0f, 0.3f, 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f;
            spaceship->Set_Model_Matrix(t);
            
            spaceship->Set_Ka(Vector3f(0.1f, 0.1f, 0.1f));
            spaceship->Set_Kd(Vector3f(0.7f, 0.7f, 0.7f));
            spaceship->Set_Ks(Vector3f(0.5f, 0.5f, 0.5f));
            spaceship->Set_Shininess(32.0f);
            
            spaceship->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("metal_color"));
            spaceship->Add_Texture("tex_normal", OpenGLTextureLibrary::Get_Texture("metal_normal"));
            spaceship->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("spaceship"));
        }

        // ============ ENGINE FLAMES (Simple Implementation) ============
        flame_start_index = (int)mesh_object_array.size();
        
        for(int i = 0; i < 25; i++) {
            auto flame = Add_Obj_Mesh_Object("obj/sqad.obj");
            
            // Position at back of spaceship (-X direction)
            float z = (rand() / (float)RAND_MAX - 0.5f) * 0.15f;
            float y = (rand() / (float)RAND_MAX - 0.5f) * 0.08f;
            float x = -0.6f - (rand() / (float)RAND_MAX) * 0.5f;
            
            // Flame size - bigger near engine
            float size = 0.04f + (0.02f * (-x - 0.6f));
            
            Matrix4f t = Matrix4f::Identity();
            t << size, 0.0f, 0.0f, x,
                 0.0f, size, 0.0f, y,
                 0.0f, 0.0f, size, z,
                 0.0f, 0.0f, 0.0f, 1.0f;
            flame->Set_Model_Matrix(t);
            
            // Store for animation
            flame_transforms.push_back(t);
            flame_offsets.push_back((rand() / (float)RAND_MAX) * 3.14159f * 2.0f);
            
            // Flame colors
            float r = 1.0f;
            float g = 0.5f + (rand() / (float)RAND_MAX) * 0.4f;
            float b = 0.1f;
            
            flame->Set_Ka(Vector3f(r * 0.3f, g * 0.3f, b * 0.3f));
            flame->Set_Kd(Vector3f(r, g, b));
            flame->Set_Ks(Vector3f(r * 0.5f, g * 0.5f, b * 0.5f));
            flame->Set_Shininess(12.0f);
            
            flame->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("star_color"));
            flame->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("planet"));
            
            Set_Polygon_Mode(flame, PolygonMode::Fill);
            Set_Shading_Mode(flame, ShadingMode::TexAlpha);
            flame->Set_Data_Refreshed();
            flame->Initialize();
        }

        // ============ PROCEDURAL STARFIELD ============
        star_start_index = (int)mesh_object_array.size();
        
        // Create procedural starfield (100 stars)
        for(int i = 0; i < 100; i++) {
            auto star = Add_Obj_Mesh_Object("obj/sqad.obj");
            
            // Procedural positioning in a sphere
            float theta = (rand() / (float)RAND_MAX) * 2.0f * 3.14159f;
            float phi = acos(2.0f * (rand() / (float)RAND_MAX) - 1.0f);
            float radius = 30.0f + (rand() / (float)RAND_MAX) * 70.0f; // 30-100 units away
            
            float x = radius * sin(phi) * cos(theta);
            float y = radius * sin(phi) * sin(theta);
            float z = radius * cos(phi);
            
            // Star properties
            float brightness = 0.3f + (rand() / (float)RAND_MAX) * 0.7f;
            float size = 0.008f + brightness * 0.02f;
            
            // Star color based on "temperature"
            float r, g, b;
            float temp = rand() / (float)RAND_MAX;
            if(temp < 0.7f) { // Orange/yellow (most common)
                r = 1.0f; g = 0.8f + (rand() / (float)RAND_MAX)*0.2f; b = 0.6f;
            } else if(temp < 0.9f) { // White/blue
                r = 0.8f; g = 0.9f; b = 1.0f;
            } else { // Red
                r = 1.0f; g = 0.6f; b = 0.5f;
            }
            
            Matrix4f t = Matrix4f::Identity();
            t << size, 0.0f, 0.0f, x,
                 0.0f, size, 0.0f, y,
                 0.0f, 0.0f, size, z,
                 0.0f, 0.0f, 0.0f, 1.0f;
            star->Set_Model_Matrix(t);
            
            // Store for animation
            star_transforms.push_back(t);
            star_twinkle_offsets.push_back((rand() / (float)RAND_MAX) * 3.14159f * 2.0f);
            
            star->Set_Ka(Vector3f(r*0.3f, g*0.3f, b*0.3f));
            star->Set_Kd(Vector3f(r, g, b));
            star->Set_Ks(Vector3f(0.0f, 0.0f, 0.0f));
            star->Set_Shininess(1.0f);
            
            star->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("star_color"));
            star->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("starfield"));
            
            Set_Polygon_Mode(star, PolygonMode::Fill);
            Set_Shading_Mode(star, ShadingMode::TexAlpha);
            star->Set_Data_Refreshed();
            star->Initialize();
        }

        //// PLANET 1: Earth
        {
            auto planet = Add_Obj_Mesh_Object("obj/sphere.obj");
            
            Matrix4f t = Matrix4f::Identity();
            t << 1.0f, 0.0f, 0.0f, 4.0f,
                 0.0f, 1.0f, 0.0f, 0.0f,
                 0.0f, 0.0f, 1.0f, -3.0f,
                 0.0f, 0.0f, 0.0f, 1.0f;
            planet->Set_Model_Matrix(t);
            
            planet->Set_Ka(Vector3f(0.1f, 0.1f, 0.1f));
            planet->Set_Kd(Vector3f(0.2f, 0.4f, 0.8f));
            planet->Set_Ks(Vector3f(0.3f, 0.3f, 0.3f));
            planet->Set_Shininess(32.0f);
            
            planet->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("earth"));
            planet->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("planet"));
        }

        //// PLANET 2: Lava planet
        {
            auto lava_planet = Add_Obj_Mesh_Object("obj/sphere.obj");
            
            Matrix4f t = Matrix4f::Identity();
            t << 0.7f, 0.0f, 0.0f, -3.5f,
                 0.0f, 0.7f, 0.0f, 1.0f,
                 0.0f, 0.0f, 0.7f, -6.0f,
                 0.0f, 0.0f, 0.0f, 1.0f;
            lava_planet->Set_Model_Matrix(t);
            
            lava_planet->Set_Ka(Vector3f(0.15f, 0.1f, 0.05f));
            lava_planet->Set_Kd(Vector3f(0.9f, 0.5f, 0.2f));
            lava_planet->Set_Ks(Vector3f(0.4f, 0.2f, 0.1f));
            lava_planet->Set_Shininess(24.0f);
            
            lava_planet->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("lava"));
            lava_planet->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("planet"));
        }
        
        //// ASTEROID
        {
            auto asteroid = Add_Obj_Mesh_Object("obj/asteroidone.obj");
            
            Matrix4f t = Matrix4f::Identity();
            t << 0.2f, 0.0f, 0.0f, 2.0f,
                 0.0f, 0.2f, 0.0f, 1.5f,
                 0.0f, 0.0f, 0.2f, 3.0f,
                 0.0f, 0.0f, 0.0f, 1.0f;
            asteroid->Set_Model_Matrix(t);
            
            asteroid->Set_Ka(Vector3f(0.1f, 0.1f, 0.1f));
            asteroid->Set_Kd(Vector3f(0.5f, 0.5f, 0.5f));
            asteroid->Set_Ks(Vector3f(0.2f, 0.2f, 0.2f));
            asteroid->Set_Shininess(8.0f);
            
            asteroid->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("asteroidone"));
            asteroid->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("planet"));
        }

        //// ASTEROID 2 (bottom left)
        {
            auto asteroid2 = Add_Obj_Mesh_Object("obj/asteroidtwo.obj");
            
            Matrix4f t = Matrix4f::Identity();
            t << 0.15f, 0.0f, 0.0f, -2.5f,
                 0.0f, 0.15f, 0.0f, -1.5f,
                 0.0f, 0.0f, 0.15f, 2.5f,
                 0.0f, 0.0f, 0.0f, 1.0f;
            asteroid2->Set_Model_Matrix(t);
            
            asteroid2->Set_Ka(Vector3f(0.1f, 0.1f, 0.1f));
            asteroid2->Set_Kd(Vector3f(0.4f, 0.4f, 0.4f));
            asteroid2->Set_Ks(Vector3f(0.1f, 0.1f, 0.1f));
            asteroid2->Set_Shininess(6.0f);
            
            asteroid2->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("asteroidtwo"));
            asteroid2->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("planet"));
        }

        //// SUN
        {
            auto sun = Add_Obj_Mesh_Object("obj/sphere.obj");
            
            Matrix4f t = Matrix4f::Identity();
            t << 2.0f, 0.0f, 0.0f, 0.0f,
                 0.0f, 2.0f, 0.0f, 2.0f,
                 0.0f, 0.0f, 2.0f, -10.0f,
                 0.0f, 0.0f, 0.0f, 1.0f;
            sun->Set_Model_Matrix(t);
            
            sun->Set_Ka(Vector3f(0.2f, 0.15f, 0.05f));
            sun->Set_Kd(Vector3f(1.0f, 0.8f, 0.3f));
            sun->Set_Ks(Vector3f(1.0f, 0.9f, 0.5f));
            sun->Set_Shininess(64.0f);
            
            sun->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("sun"));
            sun->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("planet"));
        }
        
        // Original background stars (keep some for compatibility)
        for(int i = 0; i < 15; i++) {
            auto star = Add_Obj_Mesh_Object("obj/sqad.obj");
            
            float x = (rand() / (float)RAND_MAX - 0.5f) * 15.0f;
            float y = (rand() / (float)RAND_MAX - 0.5f) * 15.0f;
            float z = -8.0f - (rand() / (float)RAND_MAX) * 10.0f;
            
            Matrix4f t = Matrix4f::Identity();
            t << 0.02f, 0.0f, 0.0f, x,
                 0.0f, 0.02f, 0.0f, y,
                 0.0f, 0.0f, 0.02f, z,
                 0.0f, 0.0f, 0.0f, 1.0f;
            star->Set_Model_Matrix(t);
            star->Add_Texture("tex_color", OpenGLTextureLibrary::Get_Texture("star_color"));
            star->Add_Shader_Program(OpenGLShaderLibrary::Get_Shader("billboard"));
            
            Set_Polygon_Mode(star, PolygonMode::Fill);
            Set_Shading_Mode(star, ShadingMode::TexAlpha);
            star->Set_Data_Refreshed();
            star->Initialize();
        }
        
        // Setup rendering for all objects
        for (auto &mesh_obj : mesh_object_array){
            Set_Polygon_Mode(mesh_obj, PolygonMode::Fill);
            Set_Shading_Mode(mesh_obj, ShadingMode::TexAlpha);
            mesh_obj->Set_Data_Refreshed();
            mesh_obj->Initialize();
        }
        
        Toggle_Play();
    }

    OpenGLTriangleMesh *Add_Obj_Mesh_Object(std::string obj_file_name)
    {
        auto mesh_obj = Add_Interactive_Object<OpenGLTriangleMesh>();
        Array<std::shared_ptr<TriangleMesh<3>>> meshes;
        Obj::Read_From_Obj_File_Discrete_Triangles(obj_file_name, meshes);

        mesh_obj->mesh = *meshes[0];
        std::cout << "load tri_mesh from obj file, #vtx: " << mesh_obj->mesh.Vertices().size() << ", #ele: " << mesh_obj->mesh.Elements().size() << std::endl;

        mesh_object_array.push_back(mesh_obj);
        return mesh_obj;
    }

    virtual void Toggle_Next_Frame()
    {
        float current_time = GLfloat(clock() - startTime) / CLOCKS_PER_SEC;
        float delta_time = current_time - last_time;
        if (delta_time > 0.1f) delta_time = 0.0167f;
        last_time = current_time;
        
        // ============ ANIMATE FLAMES ============
        for (int i = 0; i < flame_transforms.size(); i++) {
            int flame_index = flame_start_index + i;
            if (flame_index >= mesh_object_array.size()) break;
            
            auto& flame = mesh_object_array[flame_index];
            
            // Update transform
            Matrix4f& t = flame_transforms[i];
            
            // Move flame backward in -X direction
            t(0, 3) -= delta_time * 3.0f;
            
            // Make flame pulse in size
            float pulse = sin(current_time * 8.0f + flame_offsets[i]) * 0.3f + 0.7f;
            float base_size = 0.04f;
            float pulse_size = base_size * pulse;
            
            t(0, 0) = t(1, 1) = t(2, 2) = pulse_size;
            
            // Reset if flame goes too far back
            if (t(0, 3) < -2.0f) {
                t(0, 3) = -0.6f;
                // Randomize position slightly
                t(1, 3) = (rand() / (float)RAND_MAX - 0.5f) * 0.08f;
                t(2, 3) = (rand() / (float)RAND_MAX - 0.5f) * 0.15f;
            }
            
            // Apply updated transform
            flame->Set_Model_Matrix(t);
        }
        
        // ============ ANIMATE STARFIELD ============
        for (int i = 0; i < star_transforms.size(); i++) {
            int star_index = star_start_index + i;
            if (star_index >= mesh_object_array.size()) break;
            
            auto& star = mesh_object_array[star_index];
            
            // Update transform
            Matrix4f& t = star_transforms[i];
            
            // Star twinkling effect
            float twinkle = sin(current_time * 3.0f + star_twinkle_offsets[i]) * 0.3f + 0.7f;
            float base_size = t(0, 0) / twinkle; // Get original size
            t(0, 0) = t(1, 1) = t(2, 2) = base_size * twinkle;
            
            // Very slow rotation/orbit
            float orbit_speed = 0.01f;
            float angle = delta_time * orbit_speed;
            float cosA = cos(angle);
            float sinA = sin(angle);
            
            // Simple circular orbit around origin
            float old_x = t(0, 3);
            float old_z = t(2, 3);
            t(0, 3) = old_x * cosA - old_z * sinA;
            t(2, 3) = old_x * sinA + old_z * cosA;
            
            // Apply updated transform
            star->Set_Model_Matrix(t);
        }
        
        // Update time for all objects
        for (auto &mesh_obj : mesh_object_array) {
            mesh_obj->setTime(current_time);
        }

        OpenGLViewer::Toggle_Next_Frame();
    }

    virtual void Run()
    {
        OpenGLViewer::Run();
    }
};

int main(int argc, char *argv[])
{
    MyDriver driver;
    driver.Initialize();
    driver.Run();
}

#endif