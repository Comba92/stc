#include "stc_matrix.h"
#include <stdio.h>
#include <time.h>

int main() {
  srand(time(NULL));
  IntMatrix grid = matrix_new(int, 10, 20);
  IntMatrix copy = matrix_new(int, 20, 30);

  matrix_set(grid, 0, 0, 69);
  printf("Hello? %d\n", matrix_get(grid, 0,0));
  printf("Hello? %d\n", matrix_get(grid, 10,20));
  printf("Hello? %d\n", matrix_get(grid, 20,40));

  matrix_foreach(grid, { matrix_set(grid, x, y, rand()%70); }, {});
  matrix_foreach(grid, { printf("%d ", matrix_get(grid, x, y)); }, { printf("\n"); });
  printf("\n\n");

  matrix_foreach(grid, {
    printf("Setting %d,%d (%d) to %d\n", x, y, x+copy.width*y, matrix_get(grid, x, y));
    matrix_set(copy, x, y, matrix_get(grid, x, y));
  }, {})

  printf("\n\n");
  matrix_foreach(copy, { matrix_set(copy, x, y, 5); }, {});
  matrix_foreach(copy, { printf("(%d, %d): %d\n", x, y, matrix_get(copy, x, y)); }, { printf("\n"); });
}