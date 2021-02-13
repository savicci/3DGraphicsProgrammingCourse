#define STB_IMAGE_IMPLEMENTATION
#include "3rdParty/src/stb/stb_image.h"
#include "quad.h"
#include <vector>
#include <string>
#include <iostream>

Quad::Quad() {
    // load texture
    auto texture_filename = std::string(PROJECT_DIR) + "/Textures/silver.png";
    int width, height, n_channels;
    uint8_t  *data = stbi_load(texture_filename.c_str(), &width, &height, &n_channels, 0);
    if(data != nullptr) {
        std::cout << "Loaded texture with dimensions " << width << "x" << height << " and n_channels " << n_channels << std::endl;
    } else {
        std::cerr << "Could not load texture from file in " << texture_filename << std::endl;
    }

    // initialize texture
    glGenTextures(1, &diffuse_texture_);
    glBindTexture(GL_TEXTURE_2D, diffuse_texture_);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, width, height,0, GL_RGBA, GL_UNSIGNED_BYTE,data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    std::vector<GLfloat> vertices{
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

    };

    unsigned int indices[] = {
            0,1,2,
            0,2,3
    };

    glGenBuffers(1, &buffer_[0]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vao_);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));

    // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          reinterpret_cast<GLvoid *>(5 * sizeof(GLfloat)));


    glGenBuffers(1, &buffer_[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_[1]);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Quad::~Quad() {
    glDeleteBuffers(2, buffer_);
    glDeleteVertexArrays(1, &vao_);
}

void Quad::draw() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,diffuse_texture_);

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) nullptr);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}