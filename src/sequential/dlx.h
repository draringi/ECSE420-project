#ifndef DLX_H
#define DLX_H

#include "common/matrix.h"

struct DLX_STACK {
  int id;
  struct DLX_STACK* next;
};

struct DLX_STACK* dlx(struct MATRIX* matrix);

#endif
