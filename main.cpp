// Luka Stancev, RA48/2022

#define _CRT_SECURE_NO_WARNINGS

// --- IMPLEMENTACIJA BIBLIOTEKA ---
#include "stb_image.h"
#include "tiny_obj_loader.h" 

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Util.h" 

// ============================================================================
// ==================== KONFIGURACIJA - KONSTANTE =============================
// ============================================================================

const double TARGET_FPS = 75.0;
const float PI = 3.14159265359f;

// --- DŽOJSTIK KONSTANTE ---
const float DZOJSTIK_X = -0.05f;
const float DZOJSTIK_Y = -0.35f;
const float DZOJSTIK_Z = 0.4f;
const float DZOJSTIK_BAZA_SIRINA = 0.08f;
const float DZOJSTIK_BAZA_VISINA = 0.01f;
const float DZOJSTIK_STAP_SIRINA = 0.015f;
const float DZOJSTIK_STAP_VISINA = 0.12f;
const float DZOJSTIK_LOPTA_RADIUS = 0.035f;

// DONJA KUTIJA
const float DONJA_KUTIJA_X = 0.0f;
const float DONJA_KUTIJA_Y = -0.5f;
const float DONJA_KUTIJA_Z = 0.0f;
const float DONJA_KUTIJA_A = 0.8f;
const float DONJA_KUTIJA_B = 0.6f;
const float DONJA_KUTIJA_C = 0.8f;

// POD
const float POD_X = 0.0f;
const float POD_Y = -0.21f;
const float POD_Z = 0.0f;
const float POD_A = 0.78f;
const float POD_B = 0.01f;
const float POD_C = 0.78f;

// RUPA
const float RUPA_MIN_X = 0.10f;
const float RUPA_MAX_X = 0.35f;
const float RUPA_MIN_Z = 0.10f;
const float RUPA_MAX_Z = 0.35f;

// STAKLO
const float STAKLO_X = 0.0f;
const float STAKLO_Y = 0.15f;
const float STAKLO_Z = 0.0f;
const float STAKLO_A = 0.8f;
const float STAKLO_B = 0.7f;
const float STAKLO_C = 0.8f;

// GORNJA KUTIJA
const float GORNJA_KUTIJA_X = 0.0f;
const float GORNJA_KUTIJA_Y = 0.65f;
const float GORNJA_KUTIJA_Z = 0.0f;
const float GORNJA_KUTIJA_A = 0.8f;
const float GORNJA_KUTIJA_B = 0.3f;
const float GORNJA_KUTIJA_C = 0.8f;

// LAMPICA
const float LAMPICA_X = 0.0f;
const float LAMPICA_Y = 0.67f;
const float LAMPICA_Z = 0.4f;
const float LAMPICA_RADIUS = 0.05f;

// SLOT ZA ŽETON
const float SLOT_X = -0.27f;
const float SLOT_Y = -0.4f;
const float SLOT_Z = 0.401f;
const float SLOT_A = 0.1f;
const float SLOT_B = 0.2f;
const float SLOT_C = 0.01f;

// KANDŽA
const float KANDZA_POCETNA_X = 0.0f;
const float KANDZA_POCETNA_Y = 0.4f;
const float KANDZA_POCETNA_Z = 0.0f;
const float KANDZA_BRZINA_X = 0.015f;
const float KANDZA_BRZINA_Z = 0.015f;
const float KANDZA_BRZINA_Y = 0.008f;
const float KANDZA_MAX_X = 0.28f;
const float KANDZA_MAX_Z = 0.245f;

// IGRAČKE
const float SLON_POCETNA_X = -0.3f;
const float SLON_POCETNA_Z = -0.1f;
const float IGRACKA_SCALE = 0.043f;

// --- BEAVER (DABAR) ---
const float BEAVER_POCETNA_X = 0.07f;
const float BEAVER_POCETNA_Z = -0.07f;
const float BEAVER_SCALE = 0.00014f;

const float IGRACKA_OFFSET_Y = 0.06f;

// FIOKA
const float FIOKA_CLICK_X = 0.225f;
const float FIOKA_CLICK_Y = -0.43f;
const float FIOKA_CLICK_WIDTH = 0.135f;
const float FIOKA_CLICK_HEIGHT = 0.17f;
const float FIOKA_Y = -0.495f;
const float FIOKA_POD_Y = -0.55f;
const float FIOKA_VRH_Y = -0.21f;
const float FIOKA_X = 0.225f;
const float FIOKA_Z = 0.205f;
const float FIOKA_SIRINA = 0.27f;
const float FIOKA_DUBINA = 0.39f;

// KONOPAC
const float KONOPAC_PLAFON_Y = 0.8f;
const float BRZINA_PADANJA = 0.0008f;

// KAMERA
const float CAMERA_DISTANCE = 2.25f;
const float CAMERA_HEIGHT = 0.9f;
float camHeight = CAMERA_HEIGHT;

// BOJE
const glm::vec4 COLOR_WHITE = glm::vec4(1, 1, 1, 1);
const glm::vec4 COLOR_GRAY = glm::vec4(0.5, 0.5, 0.5, 1);
const glm::vec4 COLOR_DARK_GRAY = glm::vec4(0.4, 0.4, 0.4, 1);
const glm::vec4 COLOR_METAL = glm::vec4(0.8, 0.8, 0.8, 1);
const glm::vec4 COLOR_GLASS_LIGHT = glm::vec4(0.9f, 0.9f, 0.9f, 0.2f);
const glm::vec4 COLOR_YELLOW = glm::vec4(1, 1, 0, 1);
const glm::vec4 COLOR_GREEN = glm::vec4(0, 1, 0, 1);
const glm::vec4 COLOR_RED = glm::vec4(1, 0, 0, 1);
const glm::vec4 COLOR_PURPLE = glm::vec4(0.8f, 0.2f, 1.0f, 1.0f);

// GLOBALNE PROMENLJIVE
enum Stanje {
    CEKANJE_ZETONA, CILJANJE, SPUSTANJE, VRACANJE_PRAZNA, VRACANJE_PUNA, PADANJE_ZIVOTINJE, POBEDA
};
Stanje stanje = CEKANJE_ZETONA;

float kandzaX = KANDZA_POCETNA_X;
float kandzaY = KANDZA_POCETNA_Y;
float kandzaZ = KANDZA_POCETNA_Z;

float slonX = SLON_POCETNA_X;
float slonY = POD_Y + IGRACKA_OFFSET_Y;
float slonZ = SLON_POCETNA_Z;

float beaverX = BEAVER_POCETNA_X;
float beaverY = POD_Y + IGRACKA_OFFSET_Y;
float beaverZ = BEAVER_POCETNA_Z;

int kojiJeUhvacen = 0; // 1 = medved, 2 = beaver
float trenutnaBrzinaPada = 0.0f;
float lampicaTajmer = 0.0f;
bool lampicaZelena = true;
bool kliknutTasterMisa = false;
float camAngle = 0.0f;
float joyRotX = 0.0f;
float joyRotZ = 0.0f;

// --- MOUSE CAMERA GLOBALS ---
float lastX = 500, lastY = 500;
bool firstMouse = true;

float yaw = -90.0f;
float pitch = -24.0f;
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

//DEPT TEST I CULLING
bool enableDepthTest = true;
bool enableCullFace = true;
bool keyPressedP = false;
bool keyPressedO = false;

// EKRAN GLOBALS (za rezoluciju)
int scrWidth;
int scrHeight;

struct Animal {
    glm::vec3 position;
    glm::vec4 color;
    float scale;
};

// --- STRUKTURA ZA MULTI-MATERIAL MODELE (Dabar i Medved) ---
struct MaterialGroup {
    std::vector<float> vertices;
    int vertexCount;
    unsigned int VAO, VBO;

    // Podrska za vise tekstura
    unsigned int albedoID;
    unsigned int normalID;
    unsigned int roughnessID;

    glm::vec3 color;
};

// Liste delova za modele
std::vector<MaterialGroup> beaverGroups;
std::vector<MaterialGroup> bearGroups;

// ============================================================================
// POMOCNE FUNKCIJE
// ============================================================================

unsigned int generateSphere(float radius, int sectors, int stacks, std::vector<float>& vertices) {
    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / radius;
    float s, t;
    float sectorStep = 2 * PI / sectors;
    float stackStep = PI / stacks;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stacks; ++i) {
        stackAngle = PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);
        for (int j = 0; j <= sectors; ++j) {
            sectorAngle = j * sectorStep;
            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            float px = x; float py = z; float pz = y;
            nx = px * lengthInv; ny = py * lengthInv; nz = pz * lengthInv;
            s = (float)j / sectors; t = (float)i / stacks;
            vertices.push_back(px); vertices.push_back(py); vertices.push_back(pz);
            vertices.push_back(s);  vertices.push_back(t);
            vertices.push_back(nx); vertices.push_back(ny); vertices.push_back(nz);
        }
    }
    std::vector<float> finalVertices;
    int k1, k2;
    for (int i = 0; i < stacks; ++i) {
        k1 = i * (sectors + 1);
        k2 = k1 + sectors + 1;
        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                for (int m = 0; m < 8; m++) finalVertices.push_back(vertices[(k1 * 8) + m]);
                for (int m = 0; m < 8; m++) finalVertices.push_back(vertices[(k2 * 8) + m]);
                for (int m = 0; m < 8; m++) finalVertices.push_back(vertices[((k1 + 1) * 8) + m]);
            }
            if (i != (stacks - 1)) {
                for (int m = 0; m < 8; m++) finalVertices.push_back(vertices[((k1 + 1) * 8) + m]);
                for (int m = 0; m < 8; m++) finalVertices.push_back(vertices[(k2 * 8) + m]);
                for (int m = 0; m < 8; m++) finalVertices.push_back(vertices[((k2 + 1) * 8) + m]);
            }
        }
    }
    vertices = finalVertices;
    return vertices.size() / 8;
}

// --- UNIVERZALNA FUNKCIJA ZA UCITAVANJE MODELA ---
bool loadModelWithMaterials(const char* objPath, const char* mtlBasePath, std::vector<MaterialGroup>& outGroups) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath, mtlBasePath);

    if (!err.empty()) std::cerr << "TINYOBJ ERR: " << err << std::endl;
    if (!warn.empty()) std::cout << "TINYOBJ WARN: " << warn << std::endl;
    if (!ret) return false;

    std::map<int, std::vector<float>> materialVertices;

    for (const auto& shape : shapes) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            int matId = shape.mesh.material_ids[f];
            for (size_t v = 0; v < 3; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                // 1. Pos
                materialVertices[matId].push_back(attrib.vertices[3 * idx.vertex_index + 0]);
                materialVertices[matId].push_back(attrib.vertices[3 * idx.vertex_index + 1]);
                materialVertices[matId].push_back(attrib.vertices[3 * idx.vertex_index + 2]);

                // 2. Tex
                if (idx.texcoord_index >= 0) {
                    materialVertices[matId].push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
                    materialVertices[matId].push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
                }
                else {
                    materialVertices[matId].push_back(0.0f); materialVertices[matId].push_back(0.0f);
                }

                // 3. Norm
                if (idx.normal_index >= 0) {
                    materialVertices[matId].push_back(attrib.normals[3 * idx.normal_index + 0]);
                    materialVertices[matId].push_back(attrib.normals[3 * idx.normal_index + 1]);
                    materialVertices[matId].push_back(attrib.normals[3 * idx.normal_index + 2]);
                }
                else {
                    materialVertices[matId].push_back(0.0f); materialVertices[matId].push_back(1.0f); materialVertices[matId].push_back(0.0f);
                }
            }
            index_offset += 3;
        }
    }

    for (auto& entry : materialVertices) {
        int matId = entry.first;
        std::vector<float>& data = entry.second;

        MaterialGroup group;
        group.vertices = data;
        group.vertexCount = data.size() / 8;

        group.albedoID = 0;
        group.normalID = 0;
        group.roughnessID = 0;

        if (matId >= 0 && matId < materials.size()) {
            std::string texName = materials[matId].diffuse_texname;
            if (!texName.empty()) {
                std::string fullPath = std::string(mtlBasePath) + texName;
                group.albedoID = loadImageToTexture(fullPath.c_str());
                group.color = glm::vec3(1.0f);
            }
            else {
                group.albedoID = 0;
                group.color = glm::vec3(materials[matId].diffuse[0],
                    materials[matId].diffuse[1],
                    materials[matId].diffuse[2]);
            }
        }
        else {
            group.albedoID = 0;
            group.color = glm::vec3(0.5f);
        }

        glGenVertexArrays(1, &group.VAO);
        glGenBuffers(1, &group.VBO);
        glBindVertexArray(group.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, group.VBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

        size_t stride = 8 * sizeof(float);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        outGroups.push_back(group);
    }
    return true;
}

bool proveriSudar(glm::vec3 kandza, glm::vec3 zivotinja) {
    float dist = glm::distance(kandza, zivotinja);
    return dist < 0.25f;
}

bool proveriKlikZeton(GLFWwindow* window, int screenW, int screenH) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!kliknutTasterMisa) {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            float glX = (float)mouseX / screenW * 2.0f - 1.0f;
            float glY = -((float)mouseY / screenH * 2.0f - 1.0f);
            float vizuelniOffsetX = 0.08f;
            float vizuelniOffsetY = -0.15f;
            if (glX > (SLOT_X + vizuelniOffsetX - 0.35f) && glX < (SLOT_X + vizuelniOffsetX + 0.35f) &&
                glY >(SLOT_Y + vizuelniOffsetY - 0.3f) && glY < (SLOT_Y + vizuelniOffsetY + 0.3f)) {
                kliknutTasterMisa = true; return true;
            }
        }
    }
    else { kliknutTasterMisa = false; }
    return false;
}

bool proveriKlikFioka(GLFWwindow* window, int screenW, int screenH) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!kliknutTasterMisa) {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            float glX = (float)mouseX / screenW * 2.0f - 1.0f;
            float glY = -((float)mouseY / screenH * 2.0f - 1.0f);
            if (glX > (FIOKA_CLICK_X - FIOKA_CLICK_WIDTH) && glX < (FIOKA_CLICK_X + FIOKA_CLICK_WIDTH) &&
                glY >(FIOKA_CLICK_Y - FIOKA_CLICK_HEIGHT) && glY < (FIOKA_CLICK_Y + FIOKA_CLICK_HEIGHT)) {
                kliknutTasterMisa = true; return true;
            }
        }
    }
    else { kliknutTasterMisa = false; }
    return false;
}

// --- CALLBACK ZA ROTACIJU KAMERE MISEM ---
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }
}

void drawBox(unsigned int shader, unsigned int VAO, glm::mat4 model, unsigned int modelLoc,
    unsigned int viewLoc, unsigned int projLoc, glm::mat4 view, glm::mat4 projection,
    glm::vec4 color, unsigned int texID, bool useTex) {
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4f(glGetUniformLocation(shader, "uColor"), color.r, color.g, color.b, color.a);
    glUniform1i(glGetUniformLocation(shader, "uUseTex"), useTex);
    glUniform1i(glGetUniformLocation(shader, "uUseNormalMap"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    glBindVertexArray(VAO);
    for (int i = 0; i < 6; ++i) { glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4); }
}

// Funkcija za crtanje 2D elementa (Ime u uglu)
void drawImage2D(unsigned int shader, unsigned int VAO, unsigned int texID, float x, float y, float w, float h, int scrW, int scrH) {
    // Iskljucujemo depth test da bi slika bila uvek preko svega
    glDisable(GL_DEPTH_TEST);

    glm::mat4 projection = glm::ortho(0.0f, (float)scrW, 0.0f, (float)scrH);
    glm::mat4 view = glm::mat4(1.0f); // Identity
    
    // Model matrica: Pozicionira kvadrat
    // Transliramo do x,y, zatim skaliramo na w,h. 
    // Posto je osnovna kocka 1x1 centrirana na 0,0, moramo je pomeriti za 0.5 da bi cosak bio na x,y
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x + w/2, y + h/2, 0.0f));
    model = glm::scale(model, glm::vec3(w, h, 0.0f)); // Z skala 0 da bude pljosnato

    unsigned int modelLoc = glGetUniformLocation(shader, "uM");
    unsigned int viewLoc = glGetUniformLocation(shader, "uV");
    unsigned int projLoc = glGetUniformLocation(shader, "uP");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    glUniform4f(glGetUniformLocation(shader, "uColor"), 1, 1, 1, 1);
    glUniform1i(glGetUniformLocation(shader, "uUseTex"), 1);
    glUniform1i(glGetUniformLocation(shader, "uUseNormalMap"), 0);
    glUniform1i(glGetUniformLocation(shader, "uIsLightSource"), 1); // Da se vidi jasno, bez senke

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    glBindVertexArray(VAO);
    
    // Crtamo samo prednju stranu kocke (koja simulira 2D quad)
    glDrawArrays(GL_TRIANGLE_FAN, 0 * 4, 4);

    glEnable(GL_DEPTH_TEST);
    glUniform1i(glGetUniformLocation(shader, "uIsLightSource"), 0); // Vracamo normalno
}

void drawBoxCustom(unsigned int shader, unsigned int VAO, glm::mat4 model, unsigned int modelLoc,
    unsigned int viewLoc, unsigned int projLoc, glm::mat4 view, glm::mat4 projection,
    glm::vec4 color, unsigned int texID, bool useTex,
    bool front, bool back, bool left, bool right, bool top, bool bottom) {
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4f(glGetUniformLocation(shader, "uColor"), color.r, color.g, color.b, color.a);
    glUniform1i(glGetUniformLocation(shader, "uUseTex"), useTex);
    glUniform1i(glGetUniformLocation(shader, "uUseNormalMap"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    glBindVertexArray(VAO);
    if (front)  glDrawArrays(GL_TRIANGLE_FAN, 0 * 4, 4);
    if (left)   glDrawArrays(GL_TRIANGLE_FAN, 1 * 4, 4);
    if (bottom) glDrawArrays(GL_TRIANGLE_FAN, 2 * 4, 4);
    if (top)    glDrawArrays(GL_TRIANGLE_FAN, 3 * 4, 4);
    if (right)  glDrawArrays(GL_TRIANGLE_FAN, 4 * 4, 4);
    if (back)   glDrawArrays(GL_TRIANGLE_FAN, 5 * 4, 4);
}

void drawBoxDouble(unsigned int shader, unsigned int VAO, glm::mat4 model, unsigned int modelLoc,
    unsigned int viewLoc, unsigned int projLoc, glm::mat4 view, glm::mat4 projection,
    glm::vec4 color, unsigned int texID, bool useTex,
    bool front, bool back, bool left, bool right, bool top, bool bottom) {

    drawBoxCustom(shader, VAO, model, modelLoc, viewLoc, projLoc, view, projection,
        color, texID, useTex, front, back, left, right, top, bottom);

    glCullFace(GL_FRONT);
    drawBoxCustom(shader, VAO, model, modelLoc, viewLoc, projLoc, view, projection,
        color, texID, useTex, front, back, left, right, top, bottom);
    glCullFace(GL_BACK);
}

void drawSphere(unsigned int shader, unsigned int VAO, int vertexCount, glm::mat4 model, unsigned int modelLoc,
    unsigned int viewLoc, unsigned int projLoc, glm::mat4 view, glm::mat4 projection,
    glm::vec4 color, unsigned int texID, bool useTex) {
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4f(glGetUniformLocation(shader, "uColor"), color.r, color.g, color.b, color.a);
    glUniform1i(glGetUniformLocation(shader, "uUseTex"), useTex);
    glUniform1i(glGetUniformLocation(shader, "uUseNormalMap"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void drawOBJModel(OBJModel model, unsigned int shader, glm::mat4 modelMatrix,
    unsigned int modelLoc, unsigned int viewLoc, unsigned int projLoc,
    glm::mat4 view, glm::mat4 projection, glm::vec4 color,
    unsigned int texBaseID, bool useTex) {

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4f(glGetUniformLocation(shader, "uColor"), color.r, color.g, color.b, color.a);
    glUniform1i(glGetUniformLocation(shader, "uUseTex"), useTex);
    glUniform1i(glGetUniformLocation(shader, "uUseNormalMap"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texBaseID);
    glBindVertexArray(model.VAO);
    glDrawArrays(GL_TRIANGLES, 0, model.vertexCount);
}

int main(void) {
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // --- FULLSCREEN I REZOLUCIJA ---
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
    scrWidth = videoMode->width;
    scrHeight = videoMode->height;
    // Kreiramo prozor u fullscreen modu sa native rezolucijom
    GLFWwindow* window = glfwCreateWindow(scrWidth, scrHeight, "3D Kandza - Projekat", primaryMonitor, NULL);
    
    if (!window) { glfwTerminate(); return 2; }
    glfwMakeContextCurrent(window);
    glewInit();

    // --- REGISTRACIJA MOUSE CALLBACK-A ---
    glfwSetCursorPosCallback(window, mouse_callback);

    glEnable(GL_DEPTH_TEST);

    // --- CULL FACE ENABLE ---
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");

    unsigned int modelLoc = glGetUniformLocation(unifiedShader, "uM");
    unsigned int viewLoc = glGetUniformLocation(unifiedShader, "uV");
    unsigned int projLoc = glGetUniformLocation(unifiedShader, "uP");
    unsigned int rupaAktivnaLoc = glGetUniformLocation(unifiedShader, "uRupaAktivna");
    unsigned int rupaMinLoc = glGetUniformLocation(unifiedShader, "uRupaMin");
    unsigned int rupaMaxLoc = glGetUniformLocation(unifiedShader, "uRupaMax");
    unsigned int texScaleLoc = glGetUniformLocation(unifiedShader, "uTexScale");

    unsigned int uLightPosLoc = glGetUniformLocation(unifiedShader, "uLightPos");
    unsigned int uViewPosLoc = glGetUniformLocation(unifiedShader, "uViewPos");
    unsigned int uLightColorLoc = glGetUniformLocation(unifiedShader, "uLightColor");
    unsigned int uIsLightSourceLoc = glGetUniformLocation(unifiedShader, "uIsLightSource");

    // LAMPICA
    unsigned int uLampPosLoc = glGetUniformLocation(unifiedShader, "uLampPos");
    unsigned int uLampColorLoc = glGetUniformLocation(unifiedShader, "uLampColor");
    unsigned int uLampOnLoc = glGetUniformLocation(unifiedShader, "uLampOn");
    unsigned int uReceiveLampLightLoc = glGetUniformLocation(unifiedShader, "uReceiveLampLight");

    glUniform3f(uLampPosLoc, LAMPICA_X, LAMPICA_Y, LAMPICA_Z);
    glUniform1i(uLampOnLoc, 1);
    glUniform1i(uReceiveLampLightLoc, 1);

    // ================= TEKSTURE =================
    unsigned int texAutomat = loadImageToTexture("res/metal_ljub.jpg");
    unsigned int texPod = loadImageToTexture("res/metal.jpg");
    unsigned int texZeton = loadImageToTexture("res/zeton.png");
    unsigned int texMetal = loadImageToTexture("res/metal.jpg");
    unsigned int texZid = loadImageToTexture("res/zid5.jpg");
    unsigned int texPodSoba = loadImageToTexture("res/pod5.jpg");
    unsigned int texTunel = loadImageToTexture("res/tkanina_bela.jpg");
    unsigned int texKandza = loadImageToTexture("res/kandza_tekstura.png");
    unsigned int texIme = loadImageToTexture("res/luka.png"); // TEKSTURA ZA IME

    // --- TEKSTURE ZA MEDVEDA  ---
    unsigned int texBearAlbedo = loadImageToTexture("res/Material_albedo.jpg");
    unsigned int texBearNormal = loadImageToTexture("res/Material_normal.png");
    unsigned int texBearRoughness = loadImageToTexture("res/Material_roughness.jpg");

    glBindTexture(GL_TEXTURE_2D, texZid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, texPodSoba);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // ================= MODELI =================
    OBJModel kandzaOtvorena = loadOBJ("res/otvorena_kandza.obj");
    OBJModel kandzaZatvorena = loadOBJ("res/zatvorena_kandza.obj");

    // ================= UCITAVANJE DABRA =================
    std::cout << "Ucitavam dabra..." << std::endl;
    if (!loadModelWithMaterials("res/beaver_stuffed_animal.obj", "res/", beaverGroups)) {
        std::cout << "GRESKA: Dabar nije ucitan!" << std::endl;
    }

    // ================= UCITAVANJE MEDVEDA =================
    std::cout << "Ucitavam medveda..." << std::endl;
    if (!loadModelWithMaterials("res/teddy.obj", "res/", bearGroups)) {
        std::cout << "GRESKA: Medved nije ucitan!" << std::endl;
    }
    else {

        for (auto& group : bearGroups) {
            group.albedoID = texBearAlbedo;
            group.normalID = texBearNormal;
            group.roughnessID = texBearRoughness;
            group.color = glm::vec3(1.0f);
        }
    }

    // ================= GENERISANJE KOCKE =================
    float vertices[] = {
        // FRONT
          0.5,  0.5,  0.5,   0.0, 0.0, 1.0,   1.0, 1.0,
         -0.5,  0.5,  0.5,   0.0, 0.0, 1.0,   0.0, 1.0,
         -0.5, -0.5,  0.5,   0.0, 0.0, 1.0,   0.0, 0.0,
          0.5, -0.5,  0.5,   0.0, 0.0, 1.0,   1.0, 0.0,
          // LEFT
          -0.5,  0.5,  0.5,  -1.0, 0.0, 0.0,   1.0, 1.0,
          -0.5,  0.5, -0.5,  -1.0, 0.0, 0.0,   0.0, 1.0,
          -0.5, -0.5, -0.5,  -1.0, 0.0, 0.0,   0.0, 0.0,
          -0.5, -0.5,  0.5,  -1.0, 0.0, 0.0,   1.0, 0.0,
          // BOTTOM
           0.5, -0.5,  0.5,   0.0, -1.0, 0.0,  1.0, 1.0,
          -0.5, -0.5,  0.5,   0.0, -1.0, 0.0,  0.0, 1.0,
          -0.5, -0.5, -0.5,   0.0, -1.0, 0.0,  0.0, 0.0,
           0.5, -0.5, -0.5,   0.0, -1.0, 0.0,  1.0, 0.0,
           // TOP
            0.5,  0.5, -0.5,   0.0, 1.0, 0.0,   1.0, 1.0,
           -0.5,  0.5, -0.5,   0.0, 1.0, 0.0,   0.0, 1.0,
           -0.5,  0.5,  0.5,   0.0, 1.0, 0.0,   0.0, 0.0,
            0.5,  0.5,  0.5,   0.0, 1.0, 0.0,   1.0, 0.0,
            // RIGHT
             0.5,  0.5, -0.5,   1.0, 0.0, 0.0,   1.0, 1.0,
             0.5,  0.5,  0.5,   1.0, 0.0, 0.0,   0.0, 1.0,
             0.5, -0.5,  0.5,   1.0, 0.0, 0.0,   0.0, 0.0,
             0.5, -0.5, -0.5,   1.0, 0.0, 0.0,   1.0, 0.0,
             // BACK
             -0.5,  0.5, -0.5,   0.0, 0.0, -1.0,  1.0, 1.0,
              0.5,  0.5, -0.5,   0.0, 0.0, -1.0,  0.0, 1.0,
              0.5, -0.5, -0.5,   0.0, 0.0, -1.0,  0.0, 0.0,
             -0.5, -0.5, -0.5,   0.0, 0.0, -1.0,  1.0, 0.0
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // ================= GENERISANJE SFERE =================
    std::vector<float> sphereVerts;
    unsigned int sphereVertexCount = generateSphere(1.0f, 20, 20, sphereVerts);
    unsigned int sphereVAO, sphereVBO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVerts.size() * sizeof(float), sphereVerts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);


    std::vector<Animal> animals = {
        { glm::vec3(slonX, slonY, slonZ), COLOR_WHITE, IGRACKA_SCALE },
        { glm::vec3(beaverX, beaverY, beaverZ), COLOR_WHITE, BEAVER_SCALE }
    };

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);



    while (!glfwWindowShouldClose(window)) {
        double startTime = glfwGetTime();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

        // Ažuriranje viewport-a na pun ekran (bitno ako se menja rezolucija)
        glViewport(0, 0, scrWidth, scrHeight);

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            if (!keyPressedP) {
                enableDepthTest = !enableDepthTest;
                if (enableDepthTest) glEnable(GL_DEPTH_TEST);
                else glDisable(GL_DEPTH_TEST);
                std::cout << "Depth Test: " << (enableDepthTest ? "ON" : "OFF") << std::endl;
                keyPressedP = true;
            }
        }
        else {
            keyPressedP = false;
        }

        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
            if (!keyPressedO) {
                enableCullFace = !enableCullFace;
                if (enableCullFace) {
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_BACK);
                }
                else glDisable(GL_CULL_FACE);
                std::cout << "Cull Face: " << (enableCullFace ? "ON" : "OFF") << std::endl;
                keyPressedO = true;
            }
        }
        else {
            keyPressedO = false;
        }

        bool kameraJeIspred = cos(camAngle) > 0.3f;
        joyRotX = 0.0f;
        joyRotZ = 0.0f;

        // LOGIKA
        if (stanje == CEKANJE_ZETONA) {
            if (kameraJeIspred) {
                if (proveriKlikZeton(window, scrWidth, scrHeight)) {
                    stanje = CILJANJE;
                }
            }
        }
        else if (stanje == CILJANJE) {
            if (kameraJeIspred) {
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                    if (kandzaX > -KANDZA_MAX_X) kandzaX -= KANDZA_BRZINA_X;
                    joyRotZ = 15.0f;
                }
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                    if (kandzaX < KANDZA_MAX_X) kandzaX += KANDZA_BRZINA_X;
                    joyRotZ = -15.0f;
                }
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                    if (kandzaZ > -KANDZA_MAX_Z) kandzaZ -= KANDZA_BRZINA_Z;
                    joyRotX = -15.0f;
                }
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                    if (kandzaZ < KANDZA_MAX_Z) kandzaZ += KANDZA_BRZINA_Z;
                    joyRotX = 15.0f;
                }
                if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) stanje = SPUSTANJE;
            }
        }
        else if (stanje == SPUSTANJE) {
            kandzaY -= KANDZA_BRZINA_Y;
            if (kandzaY <= POD_Y + 0.28f) {
                glm::vec3 clawPos(kandzaX, kandzaY, kandzaZ);
                bool uhvatioSlona = proveriSudar(clawPos, glm::vec3(slonX, slonY, slonZ));
                bool uhvatioBeavera = proveriSudar(clawPos, glm::vec3(beaverX, beaverY, beaverZ));

                if (uhvatioSlona) { kojiJeUhvacen = 1; stanje = VRACANJE_PUNA; }
                else if (uhvatioBeavera) { kojiJeUhvacen = 2; stanje = VRACANJE_PUNA; }
                else { kojiJeUhvacen = 0; stanje = VRACANJE_PRAZNA; }
            }
        }
        else if (stanje == VRACANJE_PRAZNA) {
            kandzaY += KANDZA_BRZINA_Y;
            if (kandzaY >= KANDZA_POCETNA_Y) kandzaY = KANDZA_POCETNA_Y;
            if (kandzaY >= KANDZA_POCETNA_Y - 0.05f) {
                float distX = abs(kandzaX - KANDZA_POCETNA_X);
                float distZ = abs(kandzaZ - KANDZA_POCETNA_Z);
                if (distX > 0.01f) kandzaX += (KANDZA_POCETNA_X > kandzaX ? KANDZA_BRZINA_X : -KANDZA_BRZINA_X);
                if (distZ > 0.01f) kandzaZ += (KANDZA_POCETNA_Z > kandzaZ ? KANDZA_BRZINA_Z : -KANDZA_BRZINA_Z);
                if (distX <= 0.01f && distZ <= 0.01f) { kandzaX = KANDZA_POCETNA_X; kandzaZ = KANDZA_POCETNA_Z; stanje = CILJANJE; }
            }
        }
        else if (stanje == VRACANJE_PUNA) {
            if (kameraJeIspred) {
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                    if (kandzaX > -KANDZA_MAX_X) kandzaX -= KANDZA_BRZINA_X;
                    joyRotZ = 15.0f;
                }
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                    if (kandzaX < KANDZA_MAX_X) kandzaX += KANDZA_BRZINA_X;
                    joyRotZ = -15.0f;
                }
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                    if (kandzaZ > -KANDZA_MAX_Z) kandzaZ -= KANDZA_BRZINA_Z;
                    joyRotX = -15.0f;
                }
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                    if (kandzaZ < KANDZA_MAX_Z) kandzaZ += KANDZA_BRZINA_Z;
                    joyRotX = 15.0f;
                }
            }

            if (kandzaY < KANDZA_POCETNA_Y) kandzaY += KANDZA_BRZINA_Y;
            else kandzaY = KANDZA_POCETNA_Y;

            float offsetIspodKandze = -0.17f;
            if (kojiJeUhvacen == 1) {
                slonX = kandzaX; slonY = kandzaY + offsetIspodKandze; slonZ = kandzaZ;
                animals[0].position = glm::vec3(slonX, slonY, slonZ);
            }
            else if (kojiJeUhvacen == 2) {
                beaverX = kandzaX; beaverY = kandzaY + offsetIspodKandze; beaverZ = kandzaZ;
                animals[1].position = glm::vec3(beaverX, beaverY, beaverZ);
            }

            if (kandzaY >= KANDZA_POCETNA_Y - 0.05f && kameraJeIspred && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                stanje = PADANJE_ZIVOTINJE; trenutnaBrzinaPada = 0.0f; std::cout << "KANDZA OTVORENA!" << std::endl;
            }
        }
        else if (stanje == PADANJE_ZIVOTINJE) {
            trenutnaBrzinaPada += BRZINA_PADANJA;
            if (kojiJeUhvacen == 1) {
                slonY -= trenutnaBrzinaPada; animals[0].position.y = slonY;
                bool uRupi = (slonX > RUPA_MIN_X && slonX < RUPA_MAX_X && slonZ > RUPA_MIN_Z && slonZ < RUPA_MAX_Z);
                if (uRupi) {
                    if (slonY < FIOKA_Y) { slonY = FIOKA_Y; animals[0].position.y = slonY; stanje = POBEDA; }
                }
                else if (slonY <= POD_Y) { slonY = POD_Y + IGRACKA_OFFSET_Y; animals[0].position.y = slonY; stanje = CILJANJE; kojiJeUhvacen = 0; }
            }
            else if (kojiJeUhvacen == 2) {
                beaverY -= trenutnaBrzinaPada; animals[1].position.y = beaverY;
                bool uRupi = (beaverX > RUPA_MIN_X && beaverX < RUPA_MAX_X && beaverZ > RUPA_MIN_Z && beaverZ < RUPA_MAX_Z);
                if (uRupi) {
                    if (beaverY < FIOKA_Y) { beaverY = FIOKA_Y; animals[1].position.y = beaverY; stanje = POBEDA; }
                }
                else if (beaverY <= POD_Y) { beaverY = POD_Y + IGRACKA_OFFSET_Y; animals[1].position.y = beaverY; stanje = CILJANJE; kojiJeUhvacen = 0; }
            }
        }
        else if (stanje == POBEDA) {
            lampicaTajmer += (1.0f / TARGET_FPS);
            if (lampicaTajmer >= 0.5f) { lampicaZelena = !lampicaZelena; lampicaTajmer = 0.0f; }
            if (kameraJeIspred) {
                if (proveriKlikFioka(window, scrWidth, scrHeight)) {
                    slonX = SLON_POCETNA_X; slonY = POD_Y + IGRACKA_OFFSET_Y; slonZ = SLON_POCETNA_Z;
                    beaverX = BEAVER_POCETNA_X; beaverY = POD_Y + IGRACKA_OFFSET_Y; beaverZ = BEAVER_POCETNA_Z;
                    animals[0].position = glm::vec3(slonX, slonY, slonZ);
                    animals[1].position = glm::vec3(beaverX, beaverY, beaverZ);
                    kandzaX = KANDZA_POCETNA_X; kandzaY = KANDZA_POCETNA_Y; kandzaZ = KANDZA_POCETNA_Z;
                    kojiJeUhvacen = 0; stanje = CEKANJE_ZETONA; std::cout << "RESET!" << std::endl;
                }
            }
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) camAngle -= 0.02f;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) camAngle += 0.02f;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            camHeight += 0.02f;
            if (camHeight > 1.12f) camHeight = 1.12f; 
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            camHeight -= 0.02f;
            if (camHeight < -0.5f) camHeight = -0.5f;
        }

        // --- RENDERIRANJE ---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(unifiedShader);

        if (enableDepthTest) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);

        // KOREKCIJA ASPEKTA ZA FULLSCREEN
        float aspect = (float)scrWidth / (float)scrHeight;
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        
        glm::vec3 cameraPos = glm::vec3(sin(camAngle) * CAMERA_DISTANCE, camHeight, cos(camAngle) * CAMERA_DISTANCE);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, glm::vec3(0, 1, 0));

        // SVETLO
        glm::vec3 lightPos = glm::vec3(0.0f, 1.1f, 0.0f);
        glm::vec3 lightColor = glm::vec3(0.7f, 0.2f, 1.0f);
        glUniform3f(uLightPosLoc, lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(uViewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
        glUniform3f(uLightColorLoc, lightColor.x, lightColor.y, lightColor.z);
        glUniform1i(uIsLightSourceLoc, 0);

        // SOBA
        float sobaSirina = 4.8f;
        float sobaDubina = 4.8f;
        float sobaVisina = 2.0f;
        float nivoPoda = -0.81f;

        glUniform1f(texScaleLoc, 7.0f);
        // POD
        drawBox(unifiedShader, VAO, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, nivoPoda, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(sobaSirina, 0.01f, sobaDubina)), modelLoc, viewLoc, projLoc, view, projection, COLOR_WHITE, texPodSoba, texPodSoba != 0);

        glUniform1f(texScaleLoc, 1.0f);
        // ZADNJI ZID (PLAFON zapravo po poziciji, ali neka bude)
        drawBox(unifiedShader, VAO, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, nivoPoda + sobaVisina, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(sobaSirina, 0.01f, sobaDubina)), modelLoc, viewLoc, projLoc, view, projection, COLOR_WHITE, texZid, texZid != 0);
        // LEVI ZID (Z minus)
        drawBox(unifiedShader, VAO, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, nivoPoda + (sobaVisina / 2.0f), -sobaDubina / 2.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(sobaSirina, sobaVisina, 0.01f)), modelLoc, viewLoc, projLoc, view, projection, COLOR_WHITE, texZid, texZid != 0);
        // DESNI ZID (X minus)
        drawBox(unifiedShader, VAO, glm::translate(glm::mat4(1.0f), glm::vec3(-sobaSirina / 2.0f, nivoPoda + (sobaVisina / 2.0f), 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, sobaVisina, sobaDubina)), modelLoc, viewLoc, projLoc, view, projection, COLOR_WHITE, texZid, texZid != 0);
        // PREDNJI ZID (X plus)
        drawBox(unifiedShader, VAO, glm::translate(glm::mat4(1.0f), glm::vec3(sobaSirina / 2.0f, nivoPoda + (sobaVisina / 2.0f), 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, sobaVisina, sobaDubina)), modelLoc, viewLoc, projLoc, view, projection, COLOR_WHITE, texZid, texZid != 0);
        
        // --- NOVI ZID (FALI ZID) ---
        // Zid na Z plus poziciji da zatvori sobu
        drawBoxDouble(unifiedShader, VAO, 
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, nivoPoda + (sobaVisina / 2.0f), sobaDubina / 2.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(sobaSirina, sobaVisina, 0.01f)), 
            modelLoc, viewLoc, projLoc, view, projection, COLOR_WHITE, texZid, texZid != 0,
            true, true, true, true, true, true); 
        // Koristimo drawBoxDouble da se vidi i spolja i iznutra

        glUniform1f(texScaleLoc, 1.0f);

        // SIJALICA
        drawBox(unifiedShader, VAO,
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.475f, 0.0f))*
            glm::scale(glm::mat4(1.0f), glm::vec3(0.03f, 0.65f, 0.03f)),
            modelLoc, viewLoc, projLoc, view, projection,
            COLOR_DARK_GRAY, 0, false);
        glUniform1i(uIsLightSourceLoc, 1);
        drawSphere(unifiedShader, sphereVAO, sphereVertexCount,
            glm::translate(glm::mat4(1.0f), lightPos) *
            glm::scale(glm::mat4(1.0f), glm::vec3(0.05f)),
            modelLoc, viewLoc, projLoc, view, projection,
            glm::vec4(lightColor, 1.0), 0, false);
        glUniform1i(uIsLightSourceLoc, 0);

        // AUTOMAT
        glUniform1i(rupaAktivnaLoc, 0);
        drawBoxDouble(unifiedShader, VAO,
            glm::translate(glm::mat4(1.0f), glm::vec3(DONJA_KUTIJA_X, DONJA_KUTIJA_Y, DONJA_KUTIJA_Z))*
            glm::scale(glm::mat4(1.0f), glm::vec3(DONJA_KUTIJA_A, DONJA_KUTIJA_B, DONJA_KUTIJA_C)),
            modelLoc, viewLoc, projLoc, view, projection,
            COLOR_WHITE, texAutomat, texAutomat != 0,
            false, true, true, true, false, true);

        glUniform1i(rupaAktivnaLoc, 1);
        glUniform2f(rupaMinLoc, 0.62f, 0.41f);
        glUniform2f(rupaMaxLoc, 0.94f, 0.83f);
        drawBoxDouble(unifiedShader, VAO,
            glm::translate(glm::mat4(1.0f), glm::vec3(DONJA_KUTIJA_X, DONJA_KUTIJA_Y, DONJA_KUTIJA_Z))*
            glm::scale(glm::mat4(1.0f), glm::vec3(DONJA_KUTIJA_A, DONJA_KUTIJA_B, DONJA_KUTIJA_C)),
            modelLoc, viewLoc, projLoc, view, projection,
            COLOR_WHITE, texAutomat, texAutomat != 0,
            true, false, false, false, false, false);
        glUniform1i(rupaAktivnaLoc, 0);

        // POD I RUPE
        float rupaMinX = RUPA_MIN_X, rupaMaxX = RUPA_MAX_X, rupaMinZ = RUPA_MIN_Z, rupaMaxZ = RUPA_MAX_Z;
        float podMinX = -POD_A / 2.0f, podMaxX = POD_A / 2.0f, podMinZ = -POD_C / 2.0f, podMaxZ = POD_C / 2.0f;
        float centarRupeX = (rupaMinX + rupaMaxX) / 2.0f;
        drawBox(unifiedShader, VAO, glm::translate(glm::mat4(1.0f), glm::vec3(podMinX + ((rupaMinX - podMinX) / 2.0f), POD_Y, POD_Z)) * glm::scale(glm::mat4(1.0f), glm::vec3(rupaMinX - podMinX, POD_B, POD_C)), modelLoc, viewLoc, projLoc, view, projection, COLOR_WHITE, texPod, texPod != 0);
        drawBox(unifiedShader, VAO, glm::translate(glm::mat4(1.0f), glm::vec3(rupaMaxX + ((podMaxX - rupaMaxX) / 2.0f), POD_Y, POD_Z)) * glm::scale(glm::mat4(1.0f), glm::vec3(podMaxX - rupaMaxX, POD_B, POD_C)), modelLoc, viewLoc, projLoc, view, projection, COLOR_WHITE, texPod, texPod != 0);
        drawBox(unifiedShader, VAO, glm::translate(glm::mat4(1.0f), glm::vec3(centarRupeX, POD_Y, podMinZ + ((rupaMinZ - podMinZ) / 2.0f))) * glm::scale(glm::mat4(1.0f), glm::vec3(rupaMaxX - rupaMinX, POD_B, rupaMinZ - podMinZ)), modelLoc, viewLoc, projLoc, view, projection, COLOR_WHITE, texPod, texPod != 0);
        drawBox(unifiedShader, VAO, glm::translate(glm::mat4(1.0f), glm::vec3(centarRupeX, POD_Y, rupaMaxZ + ((podMaxZ - rupaMaxZ) / 2.0f))) * glm::scale(glm::mat4(1.0f), glm::vec3(rupaMaxX - rupaMinX, POD_B, podMaxZ - rupaMaxZ)), modelLoc, viewLoc, projLoc, view, projection, COLOR_WHITE, texPod, texPod != 0);

        // TUNEL - SA DVOSTRUKIM ZIDOVIMA
        float tunelVisina = FIOKA_VRH_Y - FIOKA_POD_Y;
        drawBoxDouble(unifiedShader, VAO,
            glm::translate(glm::mat4(1.0f), glm::vec3(FIOKA_X, FIOKA_POD_Y + (tunelVisina / 2.0f), FIOKA_Z)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(FIOKA_SIRINA, tunelVisina, FIOKA_DUBINA)),
            modelLoc, viewLoc, projLoc, view, projection,
            COLOR_WHITE, texTunel, texTunel != 0,
            false, true, true, true, false, true);

        // GORNJA KUTIJA
        drawBox(unifiedShader, VAO, glm::translate(glm::mat4(1.0f), glm::vec3(GORNJA_KUTIJA_X, GORNJA_KUTIJA_Y, GORNJA_KUTIJA_Z)) * glm::scale(glm::mat4(1.0f), glm::vec3(GORNJA_KUTIJA_A, GORNJA_KUTIJA_B, GORNJA_KUTIJA_C)), modelLoc, viewLoc, projLoc, view, projection, COLOR_WHITE, texAutomat, texAutomat != 0);

        // LAMPICA
        glm::vec4 lampColor;

        if (stanje == CEKANJE_ZETONA) {
            lampColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
        }
        else if (stanje == POBEDA) {
            lampColor = (!lampicaZelena) ? COLOR_RED : COLOR_GREEN;
        }
        else {
            lampColor = COLOR_YELLOW;
        }

        glUniform3f(uLampPosLoc, LAMPICA_X, LAMPICA_Y, LAMPICA_Z);
        glUniform3f(uLampColorLoc, lampColor.r, lampColor.g, lampColor.b);
        glUniform1i(uLampOnLoc, 1);

        // Crtamo samu sijalicu - SA DVOSTRUKOM SFEROM
        glUniform1i(uIsLightSourceLoc, 1);

        // Spoljna sfera
        drawSphere(unifiedShader, sphereVAO, sphereVertexCount,
            glm::translate(glm::mat4(1.0f), glm::vec3(LAMPICA_X, LAMPICA_Y, LAMPICA_Z)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(LAMPICA_RADIUS)),
            modelLoc, viewLoc, projLoc, view, projection, lampColor, 0, false);

        // Unutrasnja sfera
        glCullFace(GL_FRONT);
        drawSphere(unifiedShader, sphereVAO, sphereVertexCount,
            glm::translate(glm::mat4(1.0f), glm::vec3(LAMPICA_X, LAMPICA_Y, LAMPICA_Z)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(LAMPICA_RADIUS)),
            modelLoc, viewLoc, projLoc, view, projection, lampColor, 0, false);
        glCullFace(GL_BACK);

        glUniform1i(uIsLightSourceLoc, 0);

        // ZETON SLOT - SA DVOSTRUKIM ZIDOVIMA
        drawBoxDouble(unifiedShader, VAO,
            glm::translate(glm::mat4(1.0f), glm::vec3(SLOT_X, SLOT_Y, SLOT_Z)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(SLOT_A, SLOT_B, SLOT_C)),
            modelLoc, viewLoc, projLoc, view, projection,
            COLOR_DARK_GRAY, texZeton, texZeton != 0,
            true, true, true, true, true, true);

        // DŽOJSTIK
        glm::mat4 baseMat = glm::translate(glm::mat4(1.0f), glm::vec3(DZOJSTIK_X, DZOJSTIK_Y, DZOJSTIK_Z));
        baseMat = glm::rotate(baseMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        drawBox(unifiedShader, VAO,
            baseMat * glm::scale(glm::mat4(1.0f), glm::vec3(DZOJSTIK_BAZA_SIRINA, DZOJSTIK_BAZA_VISINA, DZOJSTIK_BAZA_SIRINA)),
            modelLoc, viewLoc, projLoc, view, projection, COLOR_DARK_GRAY, texMetal, texMetal != 0);
        glm::mat4 stickMat = baseMat;
        stickMat = glm::rotate(stickMat, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        stickMat = glm::rotate(stickMat, glm::radians(joyRotX), glm::vec3(1.0f, 0.0f, 0.0f));
        stickMat = glm::rotate(stickMat, glm::radians(joyRotZ), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 stickVisualMat = glm::translate(stickMat, glm::vec3(0.0f, DZOJSTIK_STAP_VISINA / 2.0f, 0.0f));
        drawBox(unifiedShader, VAO,
            stickVisualMat * glm::scale(glm::mat4(1.0f), glm::vec3(DZOJSTIK_STAP_SIRINA, DZOJSTIK_STAP_VISINA, DZOJSTIK_STAP_SIRINA)),
            modelLoc, viewLoc, projLoc, view, projection, COLOR_METAL, texMetal, texMetal != 0);
        glm::mat4 ballMat = glm::translate(stickMat, glm::vec3(0.0f, DZOJSTIK_STAP_VISINA, 0.0f));

        // Spoljna sfera (normalna)
        drawSphere(unifiedShader, sphereVAO, sphereVertexCount,
            ballMat * glm::scale(glm::mat4(1.0f), glm::vec3(DZOJSTIK_LOPTA_RADIUS)),
            modelLoc, viewLoc, projLoc, view, projection, COLOR_RED, texMetal, texMetal != 0);

        // Unutrasnja sfera (za culling)
        glCullFace(GL_FRONT);
        drawSphere(unifiedShader, sphereVAO, sphereVertexCount,
            ballMat * glm::scale(glm::mat4(1.0f), glm::vec3(DZOJSTIK_LOPTA_RADIUS)),
            modelLoc, viewLoc, projLoc, view, projection, COLOR_RED, texMetal, texMetal != 0);
        glCullFace(GL_BACK);

        // ================= IGRAČKE (CRTANJE) =================

        // 1. SLON / MEDVED (Koristi isti loader kao dabar)
        {
            glm::mat4 bearModelMat = glm::translate(glm::mat4(1.0f), animals[0].position) *
                glm::scale(glm::mat4(1.0f), glm::vec3(IGRACKA_SCALE));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bearModelMat));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform1f(texScaleLoc, 1.0f);

            for (const auto& group : bearGroups) {
                if (group.albedoID > 0) {
                    // ALBEDO
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, group.albedoID);
                    glUniform1i(glGetUniformLocation(unifiedShader, "uUseTex"), 1);
                    glUniform4f(glGetUniformLocation(unifiedShader, "uColor"), 1, 1, 1, 1);
                }
                else {
                    glUniform1i(glGetUniformLocation(unifiedShader, "uUseTex"), 0);
                    glUniform4f(glGetUniformLocation(unifiedShader, "uColor"),
                        group.color.r, group.color.g, group.color.b, 1.0f);
                }

                // NORMAL MAP
                if (group.normalID > 0) {
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, group.normalID);
                    glUniform1i(glGetUniformLocation(unifiedShader, "uTexNormal"), 1);
                    glUniform1i(glGetUniformLocation(unifiedShader, "uUseNormalMap"), 1);
                }
                else {
                    glUniform1i(glGetUniformLocation(unifiedShader, "uUseNormalMap"), 0);
                }

                // METALLIC / ROUGHNESS
                if (group.roughnessID > 0) {
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, group.roughnessID);
                    glUniform1i(glGetUniformLocation(unifiedShader, "uTexMetallic"), 2);
                    glUniform1i(glGetUniformLocation(unifiedShader, "uUseMetallicMap"), 1);
                }
                else {
                    glUniform1i(glGetUniformLocation(unifiedShader, "uUseMetallicMap"), 0);
                }

                glBindVertexArray(group.VAO);
                glDrawArrays(GL_TRIANGLES, 0, group.vertexCount);
            }

            // Resetuj slotove da ne utice na dabra
            glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, 0);
            glUniform1i(glGetUniformLocation(unifiedShader, "uUseNormalMap"), 0);
            glUniform1i(glGetUniformLocation(unifiedShader, "uUseMetallicMap"), 0);
        }

        // 2. BEAVER (DABAR)
        {

            float korekcijaX = -0.018f;
            float korekcijaZ = 0.0315f;

            glm::mat4 beaverModelMat = glm::translate(glm::mat4(1.0f), animals[1].position);

            beaverModelMat = glm::translate(beaverModelMat, glm::vec3(korekcijaX, 0.0f, korekcijaZ));

            beaverModelMat = glm::scale(beaverModelMat, glm::vec3(BEAVER_SCALE));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(beaverModelMat));
            glUniform1f(texScaleLoc, 1.0f);

            for (const auto& group : beaverGroups) {
                if (group.albedoID > 0) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, group.albedoID);
                    glUniform1i(glGetUniformLocation(unifiedShader, "uUseTex"), 1);
                    glUniform4f(glGetUniformLocation(unifiedShader, "uColor"), 1, 1, 1, 1);
                }
                else {
                    glUniform1i(glGetUniformLocation(unifiedShader, "uUseTex"), 0);
                    glUniform4f(glGetUniformLocation(unifiedShader, "uColor"),
                        group.color.r, group.color.g, group.color.b, 1.0f);
                }
                glUniform1i(glGetUniformLocation(unifiedShader, "uUseNormalMap"), 0); 
                glUniform1i(glGetUniformLocation(unifiedShader, "uUseMetallicMap"), 0);

                glBindVertexArray(group.VAO);
                glDrawArrays(GL_TRIANGLES, 0, group.vertexCount);
            }
        }

        // KANDZA
        glUniform1i(uReceiveLampLightLoc, 0);
        float duzinaKonopca = KONOPAC_PLAFON_Y - kandzaY;
        drawBox(unifiedShader, VAO,
            glm::translate(glm::mat4(1.0f), glm::vec3(kandzaX, kandzaY + (duzinaKonopca / 2.0f), kandzaZ)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, duzinaKonopca, 0.01f)),
            modelLoc, viewLoc, projLoc, view, projection, COLOR_DARK_GRAY, 0, false);

        glUniform1i(uReceiveLampLightLoc, 1);
        OBJModel* trenutniModel = (stanje == VRACANJE_PUNA || stanje == VRACANJE_PRAZNA) ? &kandzaZatvorena : &kandzaOtvorena;
        glm::mat4 kandzaMat = glm::translate(glm::mat4(1.0f), glm::vec3(kandzaX, kandzaY, kandzaZ));
        kandzaMat = glm::rotate(kandzaMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        kandzaMat = glm::scale(kandzaMat, glm::vec3(0.003f));
        drawOBJModel(*trenutniModel, unifiedShader, kandzaMat, modelLoc, viewLoc, projLoc, view, projection,
            COLOR_WHITE, texKandza, true);

        // STAKLO
        drawBoxDouble(unifiedShader, VAO,
            glm::translate(glm::mat4(1.0f), glm::vec3(STAKLO_X, STAKLO_Y, STAKLO_Z))*
            glm::scale(glm::mat4(1.0f), glm::vec3(STAKLO_A, STAKLO_B, STAKLO_C)),
            modelLoc, viewLoc, projLoc, view, projection,
            COLOR_GLASS_LIGHT, 0, false,
            true, true, true, true, false, false);

        // --- HUD / IME (LUKA) ---
        // Crtamo na kraju, preko svega
        float imgWidth = 300.0f; // Prilagodi velicinu
        float imgHeight = 100.0f; 
        float margin = 20.0f;
        // Donji desni ugao: X = sirina - slika - margina, Y = margina
        drawImage2D(unifiedShader, VAO, texIme, scrWidth - imgWidth - margin, margin, imgWidth, imgHeight, scrWidth, scrHeight);

        glfwSwapBuffers(window);
        glfwPollEvents();
        double endTime = glfwGetTime();
        double frameTime = endTime - startTime;
        if (frameTime < 1.0 / TARGET_FPS) std::this_thread::sleep_for(std::chrono::milliseconds((int)((1.0 / TARGET_FPS - frameTime) * 1000)));
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteProgram(unifiedShader);
    glfwTerminate();
    return 0;
}