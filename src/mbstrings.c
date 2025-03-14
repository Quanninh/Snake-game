#include "mbstrings.h"
#include <stddef.h>

/* mbslen - multi-byte string length
 * - Description: returns the number of UTF-8 code points ("characters")
 * in a multibyte string. If the argument is NULL or an invalid UTF-8
 * string is passed, returns -1.
 *
 * - Arguments: A pointer to a character array (`bytes`), consisting of UTF-8
 * variable-length encoded multibyte code points.
 *
 * - Return: returns the actual number of UTF-8 code points in `src`. If an
 * invalid sequence of bytes is encountered, return -1.
 *
 * - Hints:
 * UTF-8 characters are encoded in 1 to 4 bytes. The number of leading 1s in the
 * highest order byte indicates the length (in bytes) of the character. For
 * example, a character with the encoding 1111.... is 4 bytes long, a character
 * with the encoding 1110.... is 3 bytes long, and a character with the encoding
 * 1100.... is 2 bytes long. Single-byte UTF-8 characters were designed to be
 * compatible with ASCII. As such, the first bit of a 1-byte UTF-8 character is
 * 0.......
 *
 * You will need bitwise operations for this part of the assignment!
 */
size_t mbslen(const char* bytes) {
    // TODO: implement!
    if (!bytes) {
        return -1; // Null input, return error
    }

    size_t count = 0; // UTF-8 code point count
    while (*bytes) {
        unsigned char byte = (unsigned char)*bytes;

        // Determine the length of the UTF-8 character from the leading byte
        size_t char_len;
        if ((byte & 0x80) == 0) {
            // 1-byte character (ASCII: 0xxxxxxx)
            char_len = 1;
        } else if ((byte & 0xE0) == 0xC0) {
            // 2-byte character (110xxxxx)
            char_len = 2;
        } else if ((byte & 0xF0) == 0xE0) {
            // 3-byte character (1110xxxx)
            char_len = 3;
        } else if ((byte & 0xF8) == 0xF0) {
            // 4-byte character (11110xxx)
            char_len = 4;
        } else {
            // Invalid leading byte
            return -1;
        }

        // Validate the continuation bytes
        for (size_t i = 1; i < char_len; i++) {
            if ((bytes[i] & 0xC0) != 0x80) {
                // Continuation bytes must have the format 10xxxxxx
                return -1;
            }
        }

        // Advance the pointer and increase the count
        bytes += char_len;
        count++;
    }

    return count;  
}
