#ifndef CJELLY_FORMAT_IMAGE_BMP_H
#define CJELLY_FORMAT_IMAGE_BMP_H

#include <cjelly/macros.h>
#include <cjelly/format/image.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @file cjelly_format_image_bmp.h
 * @brief BMP format-specific image loader for the CJelly library.
 */

/**
 * @brief Structure representing a BMP image.
 *
 * This structure "inherits" from CJellyFormatImage by including it as its base.
 * Additional BMP-specific members (e.g., header fields) can be added here.
 */
typedef struct CJellyFormatImageBMP {
    CJellyFormatImage base; /**< Base image structure. */
    // BMP-specific fields can be added here if needed.
} CJellyFormatImageBMP;

/**
 * @brief Loads a BMP image from a file.
 *
 * This function reads a BMP image from the specified file, parses the BMP header,
 * allocates the necessary memory for the raw image data, and populates a CJellyFormatImage.
 * This implementation currently supports only uncompressed 24-bit BMP images.
 *
 * @param filename The path to the BMP image file.
 * @param out_image Output pointer that will point to the allocated CJellyFormatImage on success.
 * @return CJellyFormatImageError CJELLY_FORMAT_IMAGE_SUCCESS on success,
 *         or an appropriate error code on failure.
 */
CJellyFormatImageError cjelly_format_image_bmp_load(const char * filename, CJellyFormatImage * * out_image);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // CJELLY_FORMAT_IMAGE_BMP_H
