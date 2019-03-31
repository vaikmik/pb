#ifndef SNIPPET_H
#define SNIPPET_H

#include "db.h"

int snippet_init(struct dbidx *result, const u8 *data, usize len);
int snippet_get(struct dbrec *result, usize id);

#endif
