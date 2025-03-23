#ifndef CJELLY_FORMAT_3D_OBJ_H
#define CJELLY_FORMAT_3D_OBJ_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @file obj.h
 * @brief Header file for loading and parsing OBJ files.
 *
 * This file defines the structures and functions needed to parse and represent
 * 3D models in the OBJ file format with standardized error handling.
 */

/**
 * @brief Enumeration of error codes for the OBJ parser.
 */
typedef enum {
    CJELLY_FORMAT_3D_OBJ_SUCCESS = 0,         /**< No error */
    CJELLY_FORMAT_3D_OBJ_ERR_FILE_NOT_FOUND,    /**< Unable to open the file */
    CJELLY_FORMAT_3D_OBJ_ERR_OUT_OF_MEMORY,     /**< Memory allocation failure */
    CJELLY_FORMAT_3D_OBJ_ERR_INVALID_FORMAT     /**< File contains an invalid format */
} CJellyFormat3dObjError;

/**
 * @brief Structure representing a 3D vertex.
 */
typedef struct {
    float x; /**< X coordinate */
    float y; /**< Y coordinate */
    float z; /**< Z coordinate */
} CJellyFormat3dObjVertex;

/**
 * @brief Structure representing a 2D texture coordinate.
 */
typedef struct {
    float u; /**< U coordinate */
    float v; /**< V coordinate */
} CJellyFormat3dObjTexCoord;

/**
 * @brief Structure representing a vertex normal.
 */
typedef struct {
    float x; /**< X component of the normal */
    float y; /**< Y component of the normal */
    float z; /**< Z component of the normal */
} CJellyFormat3dObjNormal;

/**
 * @brief Structure representing a face in the OBJ model.
 *
 * This structure stores up to 4 vertex indices along with corresponding texture
 * and normal indices. The count field indicates the number of vertices that form the face.
 */
typedef struct {
    int vertex[4];      /**< Vertex indices (0-based) */
    int texcoord[4];    /**< Texture coordinate indices (0-based or -1 if missing) */
    int normal[4];      /**< Normal indices (0-based or -1 if missing) */
    int count;          /**< Number of vertices in the face */
} CJellyFormat3dObjFace;

/**
 * @brief Structure representing a group or object in the OBJ model.
 *
 * Groups help organize subsets of faces within the model.
 */
typedef struct {
    char name[128];     /**< Group or object name */
    int start_face;     /**< Index of the first face in this group */
    int face_count;     /**< Number of faces in this group */
} CJellyFormat3dObjGroup;

/**
 * @brief Main structure for storing an OBJ model.
 *
 * This structure contains dynamically allocated arrays for vertices, texture coordinates,
 * normals, faces, and groups. It also includes a reference to an external material library if present.
 */
typedef struct {
    CJellyFormat3dObjVertex * vertices;    /**< Array of vertices */
    int vertex_count;       /**< Number of vertices */
    int vertex_capacity;    /**< Allocated capacity for vertices */

    CJellyFormat3dObjTexCoord * texcoords;   /**< Array of texture coordinates */
    int texcoord_count;     /**< Number of texture coordinates */
    int texcoord_capacity;  /**< Allocated capacity for texture coordinates */

    CJellyFormat3dObjNormal * normals;       /**< Array of normals */
    int normal_count;       /**< Number of normals */
    int normal_capacity;    /**< Allocated capacity for normals */

    CJellyFormat3dObjFace * faces;           /**< Array of faces */
    int face_count;         /**< Number of faces */
    int face_capacity;      /**< Allocated capacity for faces */

    CJellyFormat3dObjGroup* groups;          /**< Array of groups/objects */
    int group_count;        /**< Number of groups */
    int group_capacity;     /**< Allocated capacity for groups */

    char mtllib[256];       /**< CJellyFormat3dMtlMaterial library filename, if any */
} CJellyFormat3dObjModel;

/**
 * @brief Loads an OBJ file and parses its contents.
 *
 * This function reads an OBJ file from disk, parses its contents, and allocates a
 * CJellyFormat3dObjModel structure with vertices, texture coordinates, normals, faces,
 * and groups. The parsed model is returned via the outModel output parameter.
 *
 * @param filename Path to the OBJ file.
 * @param outModel Output pointer that will point to the allocated CJellyFormat3dObjModel on success.
 * @return CJellyFormat3dObjError Error code indicating success or the type of failure.
 */
CJellyFormat3dObjError cjelly_format_3d_obj_load(const char* filename,
                                                 CJellyFormat3dObjModel** outModel);

/**
 * @brief Frees the memory allocated for an OBJ model.
 *
 * @param model Pointer to the CJellyFormat3dObjModel to free.
 */
void cjelly_format_3d_obj_free(CJellyFormat3dObjModel* model);

/**
 * @brief Converts an OBJ error code to a human-readable error message.
 *
 * @param err The CJellyFormat3dObjError code.
 * @return A constant string describing the error.
 */
const char* cjelly_format_3d_obj_strerror(CJellyFormat3dObjError err);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CJELLY_FORMAT_3D_OBJ_H
