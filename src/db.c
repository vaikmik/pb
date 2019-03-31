#include <stdio.h>
#include <stdlib.h>

#include "db.h"
#include "macros.h"

static FILE *s_fpidx;
static FILE *s_fprec;

static b32 index_range_check(usize id);

int
db_init(const char *dbidx_path, const char *dbrec_path)
{
    ASSERT(dbidx_path);
    ASSERT(dbrec_path);

    s_fpidx = fopen(dbidx_path, "a+");
    if (!s_fpidx)
    {
        return ERR;
    }
    s_fprec = fopen(dbrec_path, "a+");
    if (!s_fprec)
    {
        fclose(s_fpidx);
        return ERR;
    }

    return OK;
}

void
db_destroy(void)
{
    fclose(s_fpidx);
    fclose(s_fprec);
}

int
db_get(struct dbrec *result, usize id)
{
    struct dbidx idx;
    usize idpos;

    /* Range check for safety */
    if (!index_range_check(id))
    {
        return ERR;
    }

    /* Get the index */
    idpos = id * sizeof(struct dbidx);
    if (fseek(s_fpidx, idpos, SEEK_SET) < 0)
    {
        /* TODO: Error handling, logging */
        return ERR;
    }
    if (fread(&idx, sizeof idx, 1, s_fpidx) < 1)
    {
        /* TODO: Error handling, logging */
        return ERR;
    }

    /* Get the data */
    if (fseek(s_fprec, idx.offset, SEEK_SET) < 0)
    {
        /* TODO: Error handling, logging */
        return ERR;
    }
    result->data = calloc(1, idx.len);
    if (!result->data)
    {
        /* TODO: Error handling, logging */
        return ERR;
    }
    if (fread(result->data, idx.len, 1, s_fprec) < 1)
    {
        free(result->data);

        /* TODO: Error handling, logging */
        return ERR;
    }
    result->len = idx.len;

    return OK;
}

int
db_insert(struct dbidx *result, const u8 *data, usize len)
{
    isize pos;

    /* Get the current index & position */
    if (fseek(s_fpidx, 0, SEEK_END) < 0)
    {
        /* TODO: Error handling, logging */
        return ERR;
    }
    pos = ftell(s_fpidx);
    if (pos < 0)
    {
        /* TODO: Error handling, logging */
        return ERR;
    }

    result->id = pos / sizeof(struct dbidx);
    result->len = len;
    
    /* Get the current rec position & offset */
    if (fseek(s_fprec, 0, SEEK_END) < 0)
    {
        /* TODO: Error handling, logging */
        return ERR;
    }
    pos = ftell(s_fprec);
    if (pos < 0)
    {
        /* TODO: Error handling, logging */
        return ERR;
    }
    result->offset = pos;

    /* Write the index */
    if (fwrite(result, sizeof *result, 1, s_fpidx) < 0)
    {
        /* TODO: Error handling, logging */
        return ERR;
    }

    /* Write the record */
    if (fwrite(data, len, 1, s_fprec) < 0)
    {
        /* TODO: At this point, we have to roll back the index write in the database. */
        return ERR;
    }

    /* Writes ok, return success */
    return OK;
}

void
dbrec_free(struct dbrec *rec)
{
    ASSERT(rec);

    if (rec->data)
    {
        free(rec->data);
    }
}

static b32
index_range_check(usize id)
{
    isize pos;

    if (fseek(s_fpidx, 0, SEEK_END) < 0)
    {
        /* TODO: Error handling, logging */
        return FALSE;
    }

    pos = ftell(s_fpidx);
    if (pos < 0)
    {
        /* TODO: Error handling, logging */
        return FALSE;
    }

    return pos > id * sizeof(struct dbidx);
}
