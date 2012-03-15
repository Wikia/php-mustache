
#include "php_mustache.hpp"

#include "mustache_mustache.hpp"
#include "mustache_template.hpp"

// Module Entry ----------------------------------------------------------------

zend_module_entry mustache_module_entry = {
  STANDARD_MODULE_HEADER,
  PHP_MUSTACHE_NAME,        // Define PHP extension name  
  NULL,        /* Functions */  
  PHP_MINIT(mustache),        /* MINIT */  
  NULL,        /* MSHUTDOWN */  
  NULL,        /* RINIT */  
  NULL,        /* RSHUTDOWN */  
  PHP_MINFO(mustache),        /* MINFO */  
  PHP_MUSTACHE_VERSION,
  STANDARD_MODULE_PROPERTIES  
};  

#ifdef COMPILE_DL_MUSTACHE 
extern "C" {
  ZEND_GET_MODULE(mustache)      // Common for all PHP extensions which are build as shared modules  
}
#endif

// MINIT -----------------------------------------------------------------------
  
PHP_MINIT_FUNCTION(mustache)
{
  PHP_MINIT(mustache_mustache)(INIT_FUNC_ARGS_PASSTHRU);
  PHP_MINIT(mustache_template)(INIT_FUNC_ARGS_PASSTHRU);
  
  return SUCCESS;
}

// MINFO -----------------------------------------------------------------------
  
PHP_MINFO_FUNCTION(mustache)
{
  php_info_print_table_start();
  php_info_print_table_row(2, "Version", PHP_MUSTACHE_VERSION);
  php_info_print_table_row(2, "Released", PHP_MUSTACHE_RELEASE);
  php_info_print_table_row(2, "Revision", PHP_MUSTACHE_BUILD);
  php_info_print_table_row(2, "Authors", PHP_MUSTACHE_AUTHORS);
  php_info_print_table_end();
}

// Utils -----------------------------------------------------------------------

void mustache_node_to_zval(mustache::Node * node, zval * current)
{
  array_init(current);
  
  // Basic data
  add_assoc_long(current, "type", node->type);
  add_assoc_long(current, "flags", node->flags);
  if( NULL != node->data && node->data->length() > 0 ) {
    add_assoc_stringl(current, "data", (char *) node->data->c_str(), node->data->length(), 1);
  }
  
  // Children
  if( node->children.size() > 0 ) {
    zval * children;
    ALLOC_INIT_ZVAL(children);
    array_init(children);
    
    mustache::Node::Children::iterator it;
    for ( it = node->children.begin() ; it != node->children.end(); it++ ) {
      zval * child;
      ALLOC_INIT_ZVAL(child);
      mustache_node_to_zval(*it, child);
      add_next_index_zval(children, child);
    }
    
    add_assoc_zval(current, "children", children);
  }
}

void mustache_data_from_zval(mustache::Data * node, zval * current)
{
  HashTable * data_hash = NULL;
  HashPosition data_pointer = NULL;
  zval **data_entry = NULL;
  long data_count;
  
  int key_type;
  char * key_str;
  uint key_len;
  ulong key_nindex;
  std::string ckey;
  
  int ArrayPos = 0;
  mustache::Data * child = NULL;
  
  switch( Z_TYPE_P(current) ) {
      case IS_NULL:
      case IS_LONG:
      case IS_BOOL:
      case IS_DOUBLE:
      case IS_STRING:
        convert_to_string(current);
        node->type = mustache::Data::TypeString;
        node->val = new std::string(Z_STRVAL_P(current));
        break;
      case IS_ARRAY: // START IS_ARRAY -----------------------------------------
        node->type = mustache::Data::TypeNone;
        
        data_hash = HASH_OF(current);
        data_count = zend_hash_num_elements(data_hash);
        zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
        while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
          // Get current key
          key_type = zend_hash_get_current_key_ex(data_hash, &key_str, &key_len, &key_nindex, true, &data_pointer);
          // Check key type
          if( key_type == HASH_KEY_IS_LONG ) {
            if( node->type == mustache::Data::TypeNone ) {
              node->init(mustache::Data::TypeArray, data_count);
              child = node->array;
            } else if( node->type != mustache::Data::TypeArray ) {
              php_error(E_WARNING, "Mixed numeric and associative arrays are not supported");
              return; // EXIT
            }
          } else {
            if( node->type == mustache::Data::TypeNone ) {
              node->type = mustache::Data::TypeMap;
            } else if( node->type != mustache::Data::TypeMap ) {
              php_error(E_WARNING, "Mixed numeric and associative arrays are not supported");
              return; // EXIT
            }
          }
          
          // Store value
          if( node->type == mustache::Data::TypeArray ) {
            mustache_data_from_zval(child, *data_entry);
            child++;
          } else if( node->type == mustache::Data::TypeMap ) {
            child = new mustache::Data();
            mustache_data_from_zval(child, *data_entry);
            ckey.assign(key_str);
            node->data.insert(std::pair<std::string,mustache::Data*>(ckey,child));
          } else {
            // Whoops
          }
          zend_hash_move_forward_ex(data_hash, &data_pointer);
        }
  
        break; // END IS_ARRAY -------------------------------------------------
    case IS_OBJECT:
      // @todo
    default:
      php_error(E_WARNING, "Invalid data type: %d", Z_TYPE_P(current));
      break;
  }
}

zval * mustache_data_to_zval(mustache::Data * node)
{
  mustache::Data::List::iterator l_it;
  mustache::Data::Map::iterator m_it;
  mustache::Data::Array childNode;
  int pos;
  zval * current;
  zval * child;
  
  ALLOC_INIT_ZVAL(current);
  
  switch( node->type ) {
    case mustache::Data::TypeString:
      Z_TYPE_P(current) = IS_STRING;
      Z_STRVAL_P(current) = (char *) estrdup(node->val->c_str());
      Z_STRLEN_P(current) = node->val->length();
      break;
    case mustache::Data::TypeArray:
      array_init(current);
      childNode = node->array;
      for( pos = 0; pos < node->length; pos++, childNode++ ) {
        child = mustache_data_to_zval(childNode);
        add_next_index_zval(current, child);
      }
      break;
    case mustache::Data::TypeList:
      array_init(current);
      for ( l_it = node->children.begin() ; l_it != node->children.end(); l_it++ ) {
        child = mustache_data_to_zval(*l_it);
        add_next_index_zval(current, child);
      }
      break;
    case mustache::Data::TypeMap:
      array_init(current);
      for ( m_it = node->data.begin() ; m_it != node->data.end(); m_it++ ) {
        child = mustache_data_to_zval((*m_it).second);
        add_assoc_zval(current, (*m_it).first.c_str(), child);
      }
      break;
    default:
      Z_TYPE_P(current) = IS_NULL;
      php_error(E_WARNING, "Invalid data type");
      break;
  }
  
  return current;
}

void mustache_partials_from_zval(mustache::Mustache * mustache, 
        mustache::Node::Partials * partials, zval * current)
{
  // Ignore if not an array
  if( current == NULL || Z_TYPE_P(current) != IS_ARRAY ) {
    return;
  }
  
  HashTable * data_hash = NULL;
  HashPosition data_pointer = NULL;
  zval **data_entry = NULL;
  long data_count;
  
  int key_type;
  char * key_str;
  uint key_len;
  ulong key_nindex;
  std::string ckey;
  
  std::string tmpl;
  mustache::Node node;
  
  data_hash = HASH_OF(current);
  data_count = zend_hash_num_elements(data_hash);
  zend_hash_internal_pointer_reset_ex(data_hash, &data_pointer);
  while( zend_hash_get_current_data_ex(data_hash, (void**) &data_entry, &data_pointer) == SUCCESS ) {
    // Get current key
    key_type = zend_hash_get_current_key_ex(data_hash, &key_str, &key_len, &key_nindex, true, &data_pointer);
    // Check key type
    if( key_type != HASH_KEY_IS_STRING ) {
      // Non-string key
      php_error(E_WARNING, "Partial array contains a non-string key");
    } else if( Z_TYPE_PP(data_entry) != IS_STRING ) {
      // Non-string value
      php_error(E_WARNING, "Partial array contains a non-string value");
    } else {
      // String key, string value
      ckey.assign(key_str);
      tmpl.assign(Z_STRVAL_PP(data_entry));
      partials->insert(make_pair(ckey, node));
      mustache->tokenize(&tmpl, &(*partials)[ckey]);
    }
    zend_hash_move_forward_ex(data_hash, &data_pointer);
  }
}
