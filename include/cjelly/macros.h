#ifndef CJELLY_MACROS_H
#define CJELLY_MACROS_H

#include <cjelly/libver.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Typedef prototypes.
 */
typedef struct CJellyFormatImageRaw CJellyFormatImageRaw;
typedef struct CJellyFormatImage CJellyFormatImage;
typedef struct CJellyFormat3dMtlMaterial CJellyFormat3dMtlMaterial;
typedef struct CJellyFormat3dMtl CJellyFormat3dMtl;
typedef struct CJellyFormat3dObjVertex CJellyFormat3dObjVertex;
typedef struct CJellyFormat3dObjTexCoord CJellyFormat3dObjTexCoord;
typedef struct CJellyFormat3dObjNormal CJellyFormat3dObjNormal;
typedef struct CJellyFormat3dObjFaceOverflow CJellyFormat3dObjFaceOverflow;
typedef struct CJellyFormat3dObjFace CJellyFormat3dObjFace;
typedef struct CJellyFormat3dObjGroup CJellyFormat3dObjGroup;
typedef struct CJellyFormat3dObjMaterialMapping CJellyFormat3dObjMaterialMapping;
typedef struct CJellyFormat3dObjModel CJellyFormat3dObjModel;

/**
 * A cross-compiler macro for marking a function parameter as unused.
 */
#if defined(__GNUC__) || defined(__clang__)
#define GCJ_MAYBE_UNUSED(X) __attribute__((unused)) X

#elif defined(_MSC_VER)
#define GCJ_MAYBE_UNUSED(X) (void)(X)

#else
#define GCJ_MAYBE_UNUSED(X) X

#endif


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CJELLY_MACROS_H
