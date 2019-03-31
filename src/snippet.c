#include "macros.h"
#include "snippet.h"

int
snippet_init(struct dbidx *result, const u8 *data, usize len)
{
    ASSERT(result);

    if (len == 0)
    {
        return ERR;
    }
    if (db_insert(result, data, len) != OK)
    {
        return ERR;
    }

    return OK;
}

int
snippet_get(struct dbrec *result, usize id)
{
    return db_get(result, id);
}
