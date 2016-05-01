
#ifdef __cplusplus
extern "C" {
#endif

#ifndef ZEND_ENGINE_3

typedef int strsize_t;
typedef long zend_long;

#define PHP5TO7_STRL ZEND_STRS
#define PHP5TO7_RETVAL_STRING(a) RETVAL_STRING(a, 1)
#define PHP5TO7_RETVAL_STRINGL(a, b) RETVAL_STRINGL(a, b, 1)
#define PHP5TO7_ZVAL_STRING(z, s) ZVAL_STRING(z, s, 1)
#define PHP5TO7_ZVAL_STRINGL(z, s, l) ZVAL_STRINGL(z, s, l, 1)

#define PHP5TO7_Z_IS_TRUE_P(a) (Z_TYPE_P(a) == IS_BOOL && Z_BVAL_P(a))
#define PHP5TO7_Z_IS_BOOL_P(a) (Z_TYPE_P(a) == IS_BOOL)

#define php5to7_add_next_index_string(...) add_next_index_string(__VA_ARGS__, 1)
#define php5to7_add_next_index_stringl(...) add_next_index_stringl(__VA_ARGS__, 1)
#define php5to7_add_assoc_string(...) add_assoc_string(__VA_ARGS__, 1)
#define php5to7_add_assoc_string_ex(...) add_assoc_string_ex(__VA_ARGS__, 1)
#define php5to7_add_assoc_stringl_ex(...) add_assoc_stringl_ex(__VA_ARGS__, 1)

#define php5to7_register_internal_class_ex(class, parent) zend_register_internal_class_ex(class, parent, NULL TSRMLS_CC)

#define php5to7_zend_read_property(a, b, c, d) zend_read_property(a, b, c, d TSRMLS_CC)
#define php5to7_zend_read_property2(a, b, c, d, e) zend_read_property(a, b, c, d, e TSRMLS_CC)

#define php5to7_zend_hash_exists(ht, str, len) zend_hash_exists(ht, str, len + 1)

#define _DECLARE_ZVAL(name) zval * name
#define _INIT_ZVAL INIT_ZVAL
#define _ALLOC_INIT_ZVAL(name) ALLOC_INIT_ZVAL(name)
#define php5to7_zval_ptr_dtor(z) zval_ptr_dtor((&z))

#define PHP5TO7_MAKE_STD_ZVAL MAKE_STD_ZVAL

#else /* ZEND_ENGINE_3 */

typedef size_t strsize_t;

#define PHP5TO7_STRL ZEND_STRL
#define PHP5TO7_RETVAL_STRING RETVAL_STRING
#define PHP5TO7_RETVAL_STRINGL RETVAL_STRINGL
#define PHP5TO7_ZVAL_STRING ZVAL_STRING
#define PHP5TO7_ZVAL_STRINGL ZVAL_STRINGL

#define PHP5TO7_Z_IS_TRUE_P(a) (Z_TYPE_P(a) == IS_TRUE)
#define PHP5TO7_Z_IS_BOOL_P(a) (Z_TYPE_P(a) == IS_TRUE || Z_TYPE_P(a) == IS_FALSE)

#define php5to7_add_next_index_string add_next_index_string
#define php5to7_add_next_index_stringl add_next_index_stringl
#define php5to7_add_assoc_string(z, k, s) add_assoc_string_ex(z, k, strlen(k)+1, s)
#define php5to7_add_assoc_string_ex add_assoc_string_ex
#define php5to7_add_assoc_stringl_ex add_assoc_stringl_ex

#define php5to7_register_internal_class_ex(class, parent) zend_register_internal_class_ex(class, parent)

#define php5to7_zend_read_property(a, b, c, d) zend_read_property(a, b, c, d, NULL)
#define php5to7_zend_read_property2(a, b, c, d, e) zend_read_property(a, b, c, d, e, NULL)

#define _DECLARE_ZVAL(name) zval name ## _v; zval * name = &name ## _v
#define _INIT_ZVAL ZVAL_NULL
#define _ALLOC_INIT_ZVAL(name) ZVAL_NULL(name)
#define php5to7_zval_ptr_dtor(z) zval_ptr_dtor(z)

#define PHP5TO7_MAKE_STD_ZVAL(name) zval name ## _v; name = &name ## _v; ZVAL_UNDEF(name);

#endif

//#define _DECLARE_ALLOC_INIT_ZVAL(name) _DECLARE_ZVAL(name); _ALLOC_INIT_ZVAL(name)

#ifdef __cplusplus
} // extern "C"
#endif
