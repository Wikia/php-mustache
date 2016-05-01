
#ifndef PHP_MUSTACHE_DATA_HPP
#define PHP_MUSTACHE_DATA_HPP

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ZEND_ENGINE_3
struct php_obj_MustacheData {
    zend_object std;
    mustache::Data * data;
};
#else
struct php_obj_MustacheData {
    mustache::Data * data;
    zend_object std;
};
#endif

extern zend_class_entry * MustacheData_ce_ptr;

extern PHPAPI struct php_obj_MustacheData * php_mustache_data_object_fetch_object(zval * zv TSRMLS_DC);
extern PHPAPI void mustache_data_from_zval(mustache::Data * node, zval * current TSRMLS_DC);
extern PHPAPI void mustache_data_to_zval(mustache::Data * node, zval * current TSRMLS_DC);

PHP_MINIT_FUNCTION(mustache_data);

PHP_METHOD(MustacheData, __construct);
PHP_METHOD(MustacheData, toValue);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* PHP_MUSTACHE_DATA_HPP */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: et sw=4 ts=4
 */
