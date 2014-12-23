//
// Copyright 2012-2013, Syoyo Fujita.
//
// Licensed under 2-clause BSD liecense.
//
#ifndef _TINY_OBJ_LOADER_H
#define _TINY_OBJ_LOADER_H

#include <string>
#include <vector>
#include <map>

namespace tinyobj {

typedef struct
{
    std::string name;

    float ambient[3];
    float diffuse[3];
    float specular[3];
    float transmittance[3];
    float emission[3];		// psdeo:not required
    float shininess;		// psdeo:not required	
    float ior;              // psdeo:not required  // index of refraction
    float dissolve;         // psdeo:not required  // 1 == opaque; 0 == fully transparent
    // illumination model (see http://www.fileformat.info/format/material/)
    int illum;

    std::string ambient_texname;
    std::string diffuse_texname;
    std::string specular_texname;
    std::string normal_texname;
    std::map<std::string, std::string> unknown_parameter;
} material_t;

typedef struct
{
    std::vector<float>          positions; //psdeo: x1,y1,z1,x2,y2,z2,x3,y3,z3 i.e. one triangle
    std::vector<float>          normals;
    std::vector<float>          texcoords; //psdeo:irrelevent for current assignment
    std::vector<unsigned int>   indices;
} mesh_t;

typedef struct
{
    std::string  name;
    material_t   material;
    mesh_t       mesh;
} shape_t;

class MaterialReader
{
public:
    MaterialReader(){}
    virtual ~MaterialReader(){}

    virtual std::string operator() (
        const std::string& matId,
        std::map<std::string, material_t>& matMap) = 0;
};

class MaterialFileReader:
  public MaterialReader
{
    public:
        MaterialFileReader(const std::string& mtl_basepath): m_mtlBasePath(mtl_basepath) {}
        virtual ~MaterialFileReader() {}
        virtual std::string operator() (
          const std::string& matId,
          std::map<std::string, material_t>& matMap);

    private:
        std::string m_mtlBasePath;
};

/// Loads .obj from a file.
/// 'shapes' will be filled with parsed shape data
/// The function returns error string.
/// Returns empty string when loading .obj success.
/// 'mtl_basepath' is optional, and used for base path for .mtl file.
std::string LoadObj(
    std::vector<shape_t>& shapes,   // [output]
    const char* filename,
    const char* mtl_basepath = NULL);

/// Loads object from a std::istream, uses GetMtlIStreamFn to retrieve
/// std::istream for materials.
/// Returns empty string when loading .obj success.
std::string LoadObj(
    std::vector<shape_t>& shapes,   // [output]
    std::istream& inStream,
    MaterialReader& readMatFn);

/// Loads materials into std::map
/// Returns an empty string if successful
std::string LoadMtl (
  std::map<std::string, material_t>& material_map,
  std::istream& inStream);
}

#endif  // _TINY_OBJ_LOADER_H