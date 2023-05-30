/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */


#ifndef _OPENDAL_H
#define _OPENDAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * The error code for all opendal APIs in C binding
 * \todo The error handling is not complete, the error with error message will be
 * added in the future.
 */
typedef enum opendal_code {
  /**
   * All is well
   */
  OPENDAL_OK,
  /**
   * General error
   */
  OPENDAL_ERROR,
  /**
   * returning it back. For example, s3 returns an internal service error.
   */
  OPENDAL_UNEXPECTED,
  /**
   * Underlying service doesn't support this operation.
   */
  OPENDAL_UNSUPPORTED,
  /**
   * The config for backend is invalid.
   */
  OPENDAL_CONFIG_INVALID,
  /**
   * The given path is not found.
   */
  OPENDAL_NOT_FOUND,
  /**
   * The given path doesn't have enough permission for this operation
   */
  OPENDAL_PERMISSION_DENIED,
  /**
   * The given path is a directory.
   */
  OPENDAL_IS_A_DIRECTORY,
  /**
   * The given path is not a directory.
   */
  OPENDAL_NOT_A_DIRECTORY,
  /**
   * The given path already exists thus we failed to the specified operation on it.
   */
  OPENDAL_ALREADY_EXISTS,
  /**
   * Requests that sent to this path is over the limit, please slow down.
   */
  OPENDAL_RATE_LIMITED,
  /**
   * The given file paths are same.
   */
  OPENDAL_IS_SAME_FILE,
} opendal_code;

/**
 * BlockingOperator is the entry for all public blocking APIs.
 *
 * Read [`concepts`][docs::concepts] for know more about [`Operator`].
 *
 * # Examples
 *
 * Read more backend init examples in [`services`]
 *
 * ```
 * # use anyhow::Result;
 * use opendal::services::Fs;
 * use opendal::BlockingOperator;
 * use opendal::Operator;
 * #[tokio::main]
 * async fn main() -> Result<()> {
 *     // Create fs backend builder.
 *     let mut builder = Fs::default();
 *     // Set the root for fs, all operations will happen under this root.
 *     //
 *     // NOTE: the root must be absolute path.
 *     builder.root("/tmp");
 *
 *     // Build an `BlockingOperator` to start operating the storage.
 *     let _: BlockingOperator = Operator::new(builder)?.finish().blocking();
 *
 *     Ok(())
 * }
 * ```
 */
typedef struct BlockingOperator BlockingOperator;

typedef struct HashMap_String__String HashMap_String__String;

/**
 * Metadata carries all metadata associated with an path.
 *
 * # Notes
 *
 * mode and content_length are required metadata that all services
 * should provide during `stat` operation. But in `list` operation,
 * a.k.a., `Entry`'s content length could be `None`.
 */
typedef struct Metadata Metadata;

/**
 * \brief The opendal_operator_ptr is used to access almost all OpenDAL
 * APIs. It represents a operator that provides the unified interfaces provided
 * by OpenDAL.
 *
 * @see opendal_operator_new This function construct the operator
 * @see opendal_operator_free This function frees the heap memory of the operator
 *
 * \note The opendal_operator_ptr actually owns a pointer to
 * a opendal::BlockingOperator, which is inside the Rust core code.
 */
typedef struct opendal_operator_ptr {
  const struct BlockingOperator *ptr;
} opendal_operator_ptr;

/**
 * [`opendal_operator_options`] represents a series of string type key-value pairs, it may be used for initialization
 */
typedef struct opendal_operator_options {
  struct HashMap_String__String *inner;
} opendal_operator_options;

/**
 * The [`opendal_bytes`] type is a C-compatible substitute for [`Vec`]
 * in Rust, it will not be deallocated automatically like what
 * has been done in Rust. Instead, you have to call [`opendal_free_bytes`]
 * to free the heap memory to avoid memory leak.
 */
typedef struct opendal_bytes {
  const uint8_t *data;
  uintptr_t len;
} opendal_bytes;

/**
 * The Result type of read operation in opendal C binding, it contains
 * the data that the read operation returns and a error code.
 * If the read operation failed, the `data` fields should be a nullptr
 * and the error code is **NOT** OPENDAL_OK.
 */
typedef struct opendal_result_read {
  struct opendal_bytes *data;
  enum opendal_code code;
} opendal_result_read;

/**
 * The result type for opendal_operator_is_exist(), the field `is_exist`
 * contains whether the path exists, and the field `code` contains the
 * corresponding error code.
 */
typedef struct opendal_result_is_exist {
  bool is_exist;
  enum opendal_code code;
} opendal_result_is_exist;

/**
 * Metadata carries all metadata associated with an path.
 *
 * # Notes
 *
 * mode and content_length are required metadata that all services
 * should provide during `stat` operation. But in `list` operation,
 * a.k.a., `Entry`'s content length could be NULL.
 */
typedef struct opendal_metadata {
  const struct Metadata *inner;
} opendal_metadata;

/**
 * The result type for opendal_operator_stat(), the field `meta` contains the metadata
 * of the path, the field `code` represents whether the stat operation is successful.
 */
typedef struct opendal_result_stat {
  struct opendal_metadata meta;
  enum opendal_code code;
} opendal_result_stat;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Uses an array of key-value pairs to initialize the operator based on provided `scheme`
 * and `options`. For each scheme, i.e. Backend, different options could be set, you may
 * reference the [documentation](https://opendal.apache.org/docs/category/services/) for
 * each service, especially for the **Configuration Part**.
 *
 * @param scheme the service scheme you want to specify, e.g. "fs", "s3", "supabase"
 * @param options the options for this operators
 * @see opendal_operator_options
 * @return A valid opendal_operator_ptr setup with the `scheme` and `options` is the construction
 * succeeds. A null opendal_operator_ptr if any error happens.
 *
 * # Example
 *
 * Following is an example.
 * ```C
 * // Allocate a new options
 * opendal_operator_options options = opendal_operator_options_new();
 * // Set the options you need
 * opendal_operator_options_set(&options, "root", "/myroot");
 *
 * // Construct the operator based on the options and scheme
 * opendal_operator_ptr ptr = opendal_operator_new("memory", options);
 *
 * // you could free the options right away since the options is not used afterwards
 * opendal_operator_options_free(&options);
 *
 * // ... your operations
 * ```
 *
 * # Safety
 *
 * It is **safe** under two cases below
 * * The memory pointed to by `scheme` contain a valid nul terminator at the end of
 *   the string.
 * * The `scheme` points to NULL, this function simply returns you a null opendal_operator_ptr.
 */
struct opendal_operator_ptr opendal_operator_new(const char *scheme,
                                                 struct opendal_operator_options options);

/**
 * Write the `bytes` into the `path` blockingly by `op_ptr`, returns the opendal_code OPENDAL_OK
 * if succeeds, others otherwise
 *
 * @param ptr The opendal_operator_ptr created previously
 * @param path The designated path you want to write your bytes in
 * @param bytes The opendal_byte typed bytes to be written
 * @see opendal_operator_ptr
 * @see opendal_bytes
 * @see opendal_code
 * @return OPENDAL_OK if succeeds others otherwise
 *
 * # Example
 *
 * Following is an example
 * ```C
 * //...prepare your opendal_operator_ptr, named ptr for example
 *
 * // prepare your data
 * char* data = "Hello, World!";
 * opendal_bytes bytes = opendal_bytes { .data = (uint8_t*)data, .len = 13 };
 *
 * // now you can write!
 * opendal_code code = opendal_operator_blocking_write(ptr, "/testpath", bytes);
 *
 * // Assert that this succeeds
 * assert(code == OPENDAL_OK)
 * ```
 *
 * # Safety
 *
 * It is **safe** under the cases below
 * * The memory pointed to by `path` must contain a valid nul terminator at the end of
 *   the string.
 * * The `bytes` provided has valid byte in the `data` field and the `len` field is set
 *   correctly.
 *
 * # Panic
 *
 * * If the `path` points to NULL, this function panics, i.e. exits with information
 */
enum opendal_code opendal_operator_blocking_write(struct opendal_operator_ptr ptr,
                                                  const char *path,
                                                  struct opendal_bytes bytes);

/**
 * Read the data out from `path` blockingly by operator, returns
 * an opendal_result_read with error code.
 *
 * @param ptr The opendal_operator_ptr created previously
 * @param path The path you want to read the data out
 * @see opendal_operator_ptr
 * @see opendal_result_read
 * @see opendal_code
 * @return Returns opendal_result_read, the `data` field is a pointer to a newly allocated
 * opendal_bytes, the `code` field contains the error code. If the `code` is not OPENDAL_OK,
 * the `data` field points to NULL.
 *
 * \note If the read operation succeeds, the returned opendal_bytes is newly allocated on heap.
 * After your usage of that, please call opendal_bytes_free() to free the space.
 *
 * # Example
 *
 * Following is an example
 * ```C
 * // ... you have write "Hello, World!" to path "/testpath"
 *
 * opendal_result_read r = opendal_operator_blocking_read(ptr, "testpath");
 * assert(r.code == OPENDAL_OK);
 *
 * opendal_bytes *bytes = r.data;
 * assert(bytes->len == 13);
 * ```
 *
 * # Safety
 *
 * It is **safe** under the cases below
 * * The memory pointed to by `path` must contain a valid nul terminator at the end of
 *   the string.
 *
 * # Panic
 *
 * * If the `path` points to NULL, this function panics, i.e. exits with information
 */
struct opendal_result_read opendal_operator_blocking_read(struct opendal_operator_ptr ptr,
                                                          const char *path);

/**
 * \brief Check whether the path exists.
 *
 * If the operation succeeds, no matter the path exists or not,
 * the error code should be opendal_code::OPENDAL_OK. Otherwise,
 * the field `is_exist` is filled with false, and the error code
 * is set correspondingly.
 *
 * @param ptr The opendal_operator_ptr created previously
 * @param path The path you want to check existence
 * @see opendal_operator_ptr
 * @see opendal_result_is_exist
 * @see opendal_code
 * @return Returns opendal_result_is_exist, the `is_exist` field contains whether the path exists.
 * However, it the operation fails, the `is_exist` will contains false and the error code will be
 * stored in the `code` field.
 *
 * # Example
 *
 * ```C
 * // .. you previously wrote some data to path "/mytest/obj"
 * opendal_result_is_exist e = opendal_operator_is_exist(ptr, "/mytest/obj");
 * assert(e.code == OPENDAL_OK);
 * assert(e.is_exist);
 *
 * // but you previously did **not** write any data to path "/yourtest/obj"
 * opendal_result_is_exist e = opendal_operator_is_exist(ptr, "yourtest/obj");
 * assert(e.code == OPENDAL_OK);
 * assert(!e.is_exist);
 * ```
 *
 * # Safety
 *
 * It is **safe** under the cases below
 * * The memory pointed to by `path` must contain a valid nul terminator at the end of
 *   the string.
 *
 * # Panic
 *
 * * If the `path` points to NULL, this function panics, i.e. exits with information
 */
struct opendal_result_is_exist opendal_operator_is_exist(struct opendal_operator_ptr ptr,
                                                         const char *path);

/**
 * \brief Stat the path, return its metadata.
 *
 * If the operation succeeds, the error code should be
 * OPENDAL_OK. Otherwise, the field `meta` is filled with
 * a NULL pointer, and the error code is set correspondingly.
 *
 * @param ptr The opendal_operator_ptr created previously
 * @param path The path you want to stat
 * @see opendal_operator_ptr
 * @see opendal_result_stat
 * @see opendal_metadata
 * @return Returns opendal_result_stat, containing a metadata and a opendal_code.
 * If the operation succeeds, the `meta` field would holds a valid metadata and
 * the `code` field should hold OPENDAL_OK. Otherwise the metadata will contain a
 * NULL pointer, i.e. invalid, and the `code` will be set correspondingly.
 *
 * # Example
 *
 * ```C
 * // ... previously you wrote "Hello, World!" to path "/testpath"
 * opendal_result_stat s = opendal_operator_stat(ptr, "/testpath");
 * assert(s.code == OPENDAL_OK);
 *
 * opendal_metadata meta = s.meta;
 *
 * // ... you could now use your metadata, notice that please only access metadata
 * // using the APIs provided by OpenDAL
 * ```
 *
 * # Safety
 *
 * It is **safe** under the cases below
 * * The memory pointed to by `path` must contain a valid nul terminator at the end of
 *   the string.
 *
 * # Panic
 *
 * * If the `path` points to NULL, this function panics, i.e. exits with information
 */
struct opendal_result_stat opendal_operator_stat(struct opendal_operator_ptr ptr, const char *path);

/**
 * \brief Free the heap-allocated operator pointed by opendal_operator_ptr.
 *
 * Please only use this for a pointer pointing at a valid opendal_operator_ptr.
 * Calling this function on NULL does nothing, but calling this function on pointers
 * of other type will lead to segfault.
 */
void opendal_operator_free(const struct opendal_operator_ptr *self);

/**
 * Frees the heap memory used by the [`opendal_bytes`]
 */
void opendal_bytes_free(const struct opendal_bytes *self);

/**
 * Free the allocated metadata
 */
void opendal_metadata_free(const struct opendal_metadata *self);

/**
 * Return the content_length of the metadata
 */
uint64_t opendal_metadata_content_length(const struct opendal_metadata *self);

/**
 * Return whether the path represents a file
 */
bool opendal_metadata_is_file(const struct opendal_metadata *self);

/**
 * Return whether the path represents a directory
 */
bool opendal_metadata_is_dir(const struct opendal_metadata *self);

/**
 * Construct a heap-allocated opendal_operator_options
 */
struct opendal_operator_options opendal_operator_options_new(void);

/**
 * Set a Key-Value pair inside opendal_operator_options
 *
 * # Safety
 *
 * This function is unsafe because it dereferences and casts the raw pointers
 * Make sure the pointer of `key` and `value` point to a valid string.
 *
 * # Example
 *
 * ```C
 * opendal_operator_options options = opendal_operator_options_new();
 * opendal_operator_options_set(&options, "root", "/myroot");
 *
 * // .. use your opendal_operator_options
 *
 * opendal_operator_options_free(options);
 * ```
 */
void opendal_operator_options_set(struct opendal_operator_options *self,
                                  const char *key,
                                  const char *value);

/**
 * Free the allocated memory used by [`opendal_operator_options`]
 */
void opendal_operator_options_free(const struct opendal_operator_options *self);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* _OPENDAL_H */
