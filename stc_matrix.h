#ifndef STC_MATRIX_H
#define STC_MATRIX_H

#include "stc_list.h"

#define matrix_define(type,name) \
  typedef struct { \
    type* data; \
    size_t width; \
    size_t height; \
  } name;
#endif

matrix_define(int, IntMatrix);
matrix_define(int, FloatMatrix);
matrix_define(int, DoubleMatrix);

#define matrix_new(type, _width, _height) \
  { \
    .data = malloc(sizeof(type) * (_width) * (_height)), \
    .width = (_width), \
    .height = (_height), \
  } \

#define matrix_init(type, _width, _height) \
  { \
    .data = calloc((_width) * (_height), sizeof(type)), \
    .width = (_width), \
    .height = (_height), \
  } \

#define matrix_in_bounds(m, x, y) (x) >= 0 && (x) < (m).width && (y) >= 0 && (y) < (m).height

#define matrix_get(m, x, y) (m).data[(y) * (m).width + (x)]
#define matrix_set(m, x, y, value) (m).data[(y) * (m).width + (x)] = (value)

#define matrix_foreach(m, column_body, after_row_body) \
  for (int y=0; y<(m).height; ++y) { \
    for (int x=0; x<(m).width; ++x) { \
      column_body \
    } \
    after_row_body; \
  } \

#define matrix_copy(m_old, m_new) matrix_foreach(m_old, { matrix_set(m_new, x, y, matrix_get(m_old, x, y)); }, {})