/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "./cdocs/cdocs.h"
#include "arraylist.h"

/*
_choco_arraylist_allocator _choco_arraylist_heap_allocator(void);
_choco_arraylist_header* _choco_arraylist_get_header(_choco_arraylist arrlist);
_choco_arraylist_result _choco_arraylist_destroy(_choco_arraylist arrlist);
_choco_arraylist_result _choco_arraylist_remove(_choco_arraylist arrlist);
_choco_arraylist_result _choco_arraylist_swap(_choco_arraylist arrlist, size_t a, size_t b);
_choco_arraylist_result _choco_arraylist_is_full(_choco_arraylist arrlist);
_choco_arraylist _choco_arraylist_create(_choco_arraylist_allocator allocator, size_t size, size_t allocated);
_choco_arraylist _choco_arraylist_resize(_choco_arraylist arrlist, size_t desired);
_choco_arraylist _choco_arraylist_add(_choco_arraylist arrlist);
size_t _choco_arraylist_element_size(_choco_arraylist arrlist);
size_t _choco_arraylist_sizeof(_choco_arraylist arrlist);
size_t _choco_arraylist_length(_choco_arraylist arrlist);
void* _choco_arraylist_at(_choco_arraylist arrlist, size_t index);
*/

void _choco_arraylist_generate_doc(void)
{
    _cdocs_fn_struct functions[] = {
        _cdocs_fn(
            _cdocs_fn_f(_choco_arraylist_add, "Inserts an element at the back of the list."),
            _cdocs_fn_r(_choco_arraylist_result,
                "- If arraylist is null, " _cdocs_code(_CHOCO_ARRAYLIST_RESULT_ERROR) " is returned.\n"
                                                                                      "- If size of element is different to size of elements in arraylist, " _cdocs_code(_CHOCO_ARRAYLIST_RESULT_NO) " is returned.\n"
                                                                                                                                                                                                     "- If arraylist is full and resize fails, " _cdocs_code(_CHOCO_ARRAYLIST_RESULT_ERROR) "is returned\n"
                                                                                                                                                                                                                                                                                            "- If element is successfully inserted, " _cdocs_code(_CHOCO_ARRAYLIST_RESULT_OK) " is returned\n"),
            _cdocs_fn_p(_choco_arraylist arraylist, "The arraylist to insert an element."),
            _cdocs_fn_p(void* data, "The element's reference to insert."),
            _cdocs_fn_p(size_t data_size, "The size of the element, in bytes."))
    };

    _cdocs_generate_documentation("choco arraylist", functions);
}
