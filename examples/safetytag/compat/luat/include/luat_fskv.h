#ifndef __LUAT_FSKV_H__
#define __LUAT_FSKV_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the flash KV store.
 *        On Lierda this is a no-op (filesystem is ready at boot).
 * @return 0 always.
 */
int luat_fskv_init(void);

/**
 * @brief Write/overwrite a key-value pair.
 * @param key   null-terminated key string.
 * @param value pointer to value data.
 * @param len   number of bytes to store.
 * @return 0 on success, negative on error.
 */
int luat_fskv_set(const char *key, const void *value, size_t len);

/**
 * @brief Read a key-value pair.
 * @param key   null-terminated key string.
 * @param buf   output buffer.
 * @param len   size of output buffer.
 * @return number of bytes read on success, negative on error or key not found.
 */
int luat_fskv_get(const char *key, void *buf, size_t len);

/**
 * @brief Delete a single key.
 * @return 0 on success, negative if key not found or error.
 */
int luat_fskv_del(const char *key);

/**
 * @brief Delete all KV entries.
 * @return 0 on success.
 */
int luat_fskv_clear(void);

#ifdef __cplusplus
}
#endif

#endif /* __LUAT_FSKV_H__ */
