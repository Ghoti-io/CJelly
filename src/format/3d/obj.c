#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cjelly/format/3d/obj.h>

#define LINE_SIZE 256

/**
 * @brief Loads an OBJ file and fills the CJellyFormat3dObjModel structure.
 *
 * Reads an OBJ file from disk, parses its contents, and allocates a
 * CJellyFormat3dObjModel structure with vertices, texture coordinates, normals,
 * faces, and groups. The parsed model is returned via the outModel output parameter.
 *
 * @param filename Path to the OBJ file.
 * @param outModel Output pointer that will point to the allocated CJellyFormat3dObjModel on success.
 * @return CJellyFormat3dObjError Error code indicating success or the type of failure.
 */
CJellyFormat3dObjError cjelly_format_3d_obj_load(const char * filename, CJellyFormat3dObjModel * * outModel) {
  CJellyFormat3dObjError err = CJELLY_FORMAT_3D_OBJ_SUCCESS;

  // Check for invalid input.
  if (!filename || !outModel) {
    return CJELLY_FORMAT_3D_OBJ_ERR_INVALID_FORMAT;
  }

  // Open the file for reading.
  FILE* fp = fopen(filename, "r");
  if (!fp) {
    fprintf(stderr, "Cannot open file %s\n", filename);
    return CJELLY_FORMAT_3D_OBJ_ERR_FILE_NOT_FOUND;
  }

  // Allocate memory for the model structure.
  CJellyFormat3dObjModel * model = (CJellyFormat3dObjModel *)malloc(sizeof(CJellyFormat3dObjModel));
  if (!model) {
    goto ERROR_OUT_OF_MEMORY;
  }

  // Initialize all counts and capacities, and set pointers to NULL.
  model->vertex_count = 0;
  model->vertex_capacity = 128;
  model->vertices = (CJellyFormat3dObjVertex*)malloc(model->vertex_capacity * sizeof(CJellyFormat3dObjVertex));
  if (!model->vertices) { goto ERROR_OUT_OF_MEMORY; }

  model->texcoord_count = 0;
  model->texcoord_capacity = 128;
  model->texcoords = (CJellyFormat3dObjTexCoord*)malloc(model->texcoord_capacity * sizeof(CJellyFormat3dObjTexCoord));
  if (!model->texcoords) { goto ERROR_OUT_OF_MEMORY; }

  model->normal_count = 0;
  model->normal_capacity = 128;
  model->normals = (CJellyFormat3dObjNormal*)malloc(model->normal_capacity * sizeof(CJellyFormat3dObjNormal));
  if (!model->normals) { goto ERROR_OUT_OF_MEMORY; }

  model->face_count = 0;
  model->face_capacity = 128;
  model->faces = (CJellyFormat3dObjFace*)malloc(model->face_capacity * sizeof(CJellyFormat3dObjFace));
  if (!model->faces) { goto ERROR_OUT_OF_MEMORY; }

  model->group_count = 0;
  model->group_capacity = 16;
  model->groups = (CJellyFormat3dObjGroup*)malloc(model->group_capacity * sizeof(CJellyFormat3dObjGroup));
  if (!model->groups) { goto ERROR_OUT_OF_MEMORY; }

  model->mtllib[0] = '\0';

  int current_group = -1; // Index of the current active group

  char line[LINE_SIZE];
  while (fgets(line, LINE_SIZE, fp)) {
    // Remove newline characters.
    line[strcspn(line, "\r\n")] = 0;

    if (strncmp(line, "v ", 2) == 0) {
      // Read a vertex line.
      CJellyFormat3dObjVertex v;
      if (sscanf(line + 2, "%f %f %f", &v.x, &v.y, &v.z) == 3) {
        if (model->vertex_count >= model->vertex_capacity) {
          model->vertex_capacity *= 2;
          CJellyFormat3dObjVertex* temp = realloc(model->vertices, model->vertex_capacity * sizeof(CJellyFormat3dObjVertex));
          if (!temp) {
            fclose(fp);
            cjelly_format_3d_obj_free(model);
            return CJELLY_FORMAT_3D_OBJ_ERR_OUT_OF_MEMORY;
          }
          model->vertices = temp;
        }
        model->vertices[model->vertex_count++] = v;
      }
    } else if (strncmp(line, "vt ", 3) == 0) {
      // Read a texture coordinate line.
      CJellyFormat3dObjTexCoord vt;
      if (sscanf(line + 3, "%f %f", &vt.u, &vt.v) == 2) {
        if (model->texcoord_count >= model->texcoord_capacity) {
          model->texcoord_capacity *= 2;
          CJellyFormat3dObjTexCoord* temp = realloc(model->texcoords, model->texcoord_capacity * sizeof(CJellyFormat3dObjTexCoord));
          if (!temp) {
            fclose(fp);
            cjelly_format_3d_obj_free(model);
            return CJELLY_FORMAT_3D_OBJ_ERR_OUT_OF_MEMORY;
          }
          model->texcoords = temp;
        }
        model->texcoords[model->texcoord_count++] = vt;
      }
    } else if (strncmp(line, "vn ", 3) == 0) {
      // Read a normal line.
      CJellyFormat3dObjNormal vn;
      if (sscanf(line + 3, "%f %f %f", &vn.x, &vn.y, &vn.z) == 3) {
        if (model->normal_count >= model->normal_capacity) {
          model->normal_capacity *= 2;
          CJellyFormat3dObjNormal* temp = realloc(model->normals, model->normal_capacity * sizeof(CJellyFormat3dObjNormal));
          if (!temp) {
            fclose(fp);
            cjelly_format_3d_obj_free(model);
            return CJELLY_FORMAT_3D_OBJ_ERR_OUT_OF_MEMORY;
          }
          model->normals = temp;
        }
        model->normals[model->normal_count++] = vn;
      }
    } else if (strncmp(line, "f ", 2) == 0) {
      // Read a face line.
      CJellyFormat3dObjFace face;
      face.count = 0;
      // Tokenize the line after "f ".
      char * token = strtok(line + 2, " ");
      while (token != NULL && face.count < 4) {
        int vIndex = 0, vtIndex = 0, vnIndex = 0;
        if (strchr(token, '/')) {
          /* Replace '/' with space for easier parsing */
          char temp[64];
          strncpy(temp, token, 63);
          temp[63] = '\0';
          for (int i = 0; i < (int)strlen(temp); i++) {
            if (temp[i] == '/')
              temp[i] = ' ';
          }
          // Parse the indices (some may be missing).
          int count = sscanf(temp, "%d %d %d", &vIndex, &vtIndex, &vnIndex);
          if (count < 3)
            vtIndex = 0; // If texture coordinate is missing, set to 0 (or use -1).
        } else {
          vIndex = atoi(token);
        }
        // Convert from 1-based to 0-based indices.
        face.vertex[face.count] = vIndex - 1;
        face.texcoord[face.count] = vtIndex ? (vtIndex - 1) : -1;
        face.normal[face.count] = vnIndex ? (vnIndex - 1) : -1;
        face.count++;
        token = strtok(NULL, " ");
      }
      if (model->face_count >= model->face_capacity) {
        model->face_capacity *= 2;
        CJellyFormat3dObjFace * temp = realloc(model->faces, model->face_capacity * sizeof(CJellyFormat3dObjFace));
        if (!temp) {
          fclose(fp);
          cjelly_format_3d_obj_free(model);
          return CJELLY_FORMAT_3D_OBJ_ERR_OUT_OF_MEMORY;
        }
        model->faces = temp;
      }
      model->faces[model->face_count++] = face;
      if (current_group >= 0) {
        model->groups[current_group].face_count++;
      }
    }
    else if (strncmp(line, "g ", 2) == 0 || strncmp(line, "o ", 2) == 0) {
      // Read a group or object name line.
      char name[128];
      if (sscanf(line + 2, "%127s", name) == 1) {
        if (model->group_count >= model->group_capacity) {
          model->group_capacity *= 2;
          CJellyFormat3dObjGroup* temp = realloc(model->groups, model->group_capacity * sizeof(CJellyFormat3dObjGroup));
          if (!temp) {
            fclose(fp);
            cjelly_format_3d_obj_free(model);
            return CJELLY_FORMAT_3D_OBJ_ERR_OUT_OF_MEMORY;
          }
          model->groups = temp;
        }
        strcpy(model->groups[model->group_count].name, name);
        model->groups[model->group_count].start_face = model->face_count;
        model->groups[model->group_count].face_count = 0;
        current_group = model->group_count;
        model->group_count++;
      }
    }
    else if (strncmp(line, "mtllib", 6) == 0) {
      // Read the material library name.
      sscanf(line + 6, "%255s", model->mtllib);
    }
  }
  fclose(fp);
  *outModel = model;
  return CJELLY_FORMAT_3D_OBJ_SUCCESS;

  // Error handling.
ERROR_OUT_OF_MEMORY:
  if (err == CJELLY_FORMAT_3D_OBJ_SUCCESS) {
    err = CJELLY_FORMAT_3D_OBJ_ERR_OUT_OF_MEMORY;
  }
  cjelly_format_3d_obj_free(model);
  fclose(fp);
  return err;
}

void cjelly_format_3d_obj_free(CJellyFormat3dObjModel* model) {
  if (!model) return;
  if (model->vertices) free(model->vertices);
  if (model->texcoords) free(model->texcoords);
  if (model->normals) free(model->normals);
  if (model->faces) free(model->faces);
  if (model->groups) free(model->groups);
  free(model);
}

const char* cjelly_format_3d_obj_strerror(CJellyFormat3dObjError err) {
  switch (err) {
    case CJELLY_FORMAT_3D_OBJ_SUCCESS:
      return "No error";
    case CJELLY_FORMAT_3D_OBJ_ERR_FILE_NOT_FOUND:
      return "OBJ file not found";
    case CJELLY_FORMAT_3D_OBJ_ERR_OUT_OF_MEMORY:
      return "Out of memory";
    case CJELLY_FORMAT_3D_OBJ_ERR_INVALID_FORMAT:
      return "Invalid OBJ file format";
    default:
      return "Unknown error";
  }
}
