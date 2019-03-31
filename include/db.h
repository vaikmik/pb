#ifndef DB_H
#define DB_H

#include "types.h"

// DB design:
// Two files, DB index and DB data.
// DB index file maps keys to offsets and lengths within the DB data file.
// i.e.: [ID] [OFFSET] [LEN] -> 8 + 8 + (4 or 8) = 20 or 24 bytes
// This way, to get to an entry in the index file,
// is a simple matter of calculating an offset to the file, i.e. ID * ENTRY_SIZE (20/24)

typedef usize dbid;

#pragma pack(push, 1)
struct dbidx
{
    dbid id;
    usize offset;
    u32 len;
};
#pragma pack(pop)

struct dbrec
{
    usize len;
    u8 *data;
};

int db_init(const char *dbidx_path, const char *dbrec_path);
void db_destroy(void);
int db_get(struct dbrec *result, usize id);
int db_insert(struct dbidx *result, const u8 *data, usize len);

void dbrec_free(struct dbrec *rec);

#endif
