# Cache-Implementation in C

Emulating the operations of a Set Associative Cache using C

## Functions Implemented:

- cache_line_check_validity_and_tag() - Determine whether or not a cache line is valid for a given tag
- cache_line_retrieve_data() - Return long integer data from a cache line
- cache_set_find_matching_line() - Retrieve a matching cache line from a set, if one exists
- find_available_cache_line() - Function to find an unused cache line to use for new data or uses a suitable replacement policy
- cache_read() - Read a single long integer from the cache
