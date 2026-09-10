#include <stdio.h>
#include <math.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

int main(int argv, char* argc[])
{
  // Regression test for https://github.com/conda-forge/ipopt-feedstock/issues/57
  gsl_rng *r = gsl_rng_alloc(gsl_rng_mt19937);
  
  if (r == NULL) 
  {
    return 1;
  }
  
  gsl_rng_free(r);

  /* Exercise the installed GSL DLL and its external CBLAS dependency. */
  double matrix[] = {4.0, 1.0, 1.0, 3.0};
  double rhs[] = {1.0, 2.0};
  double solution[2] = {0.0, 0.0};
  double product[2] = {0.0, 0.0};
  gsl_matrix_view a = gsl_matrix_view_array(matrix, 2, 2);
  gsl_vector_view b = gsl_vector_view_array(rhs, 2);
  gsl_vector_view x = gsl_vector_view_array(solution, 2);
  gsl_vector_view y = gsl_vector_view_array(product, 2);
  gsl_permutation *p = gsl_permutation_alloc(2);
  int sign;
  if (p == NULL) return 2;
  if (gsl_linalg_LU_decomp(&a.matrix, p, &sign) != 0) return 3;
  if (gsl_linalg_LU_solve(&a.matrix, p, &b.vector, &x.vector) != 0) return 4;
  gsl_permutation_free(p);
  if (fabs(solution[0] - 1.0 / 11.0) > 1e-12 ||
      fabs(solution[1] - 7.0 / 11.0) > 1e-12) return 5;
  matrix[0] = 4.0; matrix[1] = 1.0; matrix[2] = 1.0; matrix[3] = 3.0;
  if (gsl_blas_dgemv(CblasNoTrans, 1.0, &a.matrix, &x.vector,
                    0.0, &y.vector) != 0) return 6;
  if (fabs(product[0] - rhs[0]) > 1e-12 ||
      fabs(product[1] - rhs[1]) > 1e-12) return 7;
  puts("GSL installed-package RNG, LU solve, and BLAS residual checks passed");

  return 0;
}
