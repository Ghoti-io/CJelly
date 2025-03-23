#ifndef CJELLY_FORMAT_3D_MTL_H
#define CJELLY_FORMAT_3D_MTL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @file mtl.h
 * @brief Header file for loading and parsing MTL files.
 *
 * This file defines the structures and functions needed to parse and represent
 * material definitions in the MTL file format.
 */

/**
 * @brief Enumeration of error codes for the MTL parser.
 */
typedef enum {
  CJELLY_FORMAT_3D_MTL_SUCCESS = 0,           /**< No error */
  CJELLY_FORMAT_3D_MTL_ERR_FILE_NOT_FOUND,    /**< Unable to open the file */
  CJELLY_FORMAT_3D_MTL_ERR_OUT_OF_MEMORY,     /**< Memory allocation failure */
  CJELLY_FORMAT_3D_MTL_ERR_INVALID_FORMAT     /**< File contains an invalid format */
} CJellyFormat3dMtlError;

/**
 * @brief Structure representing a single material definition.
 *
 * This structure contains common material properties such as ambient, diffuse,
 * and specular colors, as well as the specular exponent and illumination model.
 */
typedef struct CJellyFormat3dMtlMaterial {
    char name[128];  /**< Name of the material */
    float Ka[3];     /**< Ambient color (RGB) */
    float Kd[3];     /**< Diffuse color (RGB) */
    float Ks[3];     /**< Specular color (RGB) */
    float Ns;        /**< Specular exponent */
    float d;         /**< Dissolve (transparency) */
    int illum;       /**< Illumination model */
} CJellyFormat3dMtlMaterial;

/**
 * @brief Structure representing a material file.
 *
 * This structure contains an array of CJellyFormat3dMtlMaterial structures and
 * the number of materials in the library.
 */
typedef struct CJellyFormat3dMtl {
    CJellyFormat3dMtlMaterial * materials;  /**< Array of materials */
    int material_count;                    /**< Number of materials */
} CJellyFormat3dMtl;

/**
 * @brief Loads materials from an MTL file.
 *
 * This function reads an MTL file from disk, parses its contents, and allocates an array
 * of CJellyFormat3dMtlMaterial structures. The loaded materials are returned via the
 * materials output parameter.
 *
 * @param filename Path to the MTL file.
 * @param materials Output pointer that will point to the allocated array of materials on success.
 * @return CJellyFormat3dMtlError An error code indicating success or the type of failure.
 */
CJellyFormat3dMtlError cjelly_format_3d_mtl_load(const char * filename, CJellyFormat3dMtl * materials);

/**
 * @brief Frees the allocated memory of the materials struct.
 *
 * @param materials Pointer to the CJellyFormat3dMtl structure.
 * @param material_count The number of materials in the array.
 */
void cjelly_format_3d_mtl_free(CJellyFormat3dMtl * materials);

/**
 * @brief Converts an MTL error code to a human-readable error message.
 *
 * @param err The CJellyFormat3dMtlError code.
 * @return A constant string describing the error.
 */
const char* cjelly_format_3d_mtl_strerror(CJellyFormat3dMtlError err);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CJELLY_FORMAT_3D_MTL_H
