#include <stdio.h>
#include <math.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_version.h>
#include "qr-fixture.h"
#define CHECK(call) do { int status = (call); if (status) { fprintf(stderr, "%s: %s\n", #call, gsl_strerror(status)); return 2; } } while (0)
int main(void) {
    const size_t n=20, m=24, k=18;
    const double tolerance=1.0658141036401503e-9;
    const double reference=-1.0911000254544585121910287386601588e-6;
    gsl_matrix_view bmat=gsl_matrix_view_array(input_a,m+n,n);
    gsl_vector_view rhs=gsl_vector_view_array(input_b,m+n);
    gsl_matrix_view top=gsl_matrix_submatrix(&bmat.matrix,0,0,n,n);
    gsl_matrix_view bottom=gsl_matrix_submatrix(&bmat.matrix,n,0,m,n);
    gsl_matrix *r=gsl_matrix_alloc(n,n), *y=gsl_matrix_alloc(m,n), *t=gsl_matrix_alloc(n,n);
    gsl_matrix *u=gsl_matrix_alloc(m+n,n), *v=gsl_matrix_alloc(n,n);
    gsl_vector *qr=gsl_vector_alloc(m+n), *work=gsl_vector_alloc(n);
    gsl_vector *svd=gsl_vector_alloc(n), *s=gsl_vector_alloc(n);
    gsl_set_error_handler_off();
    gsl_matrix_memcpy(r,&top.matrix); gsl_matrix_memcpy(y,&bottom.matrix);
    CHECK(gsl_linalg_QR_UR_decomp(r,y,t));
    CHECK(gsl_linalg_QR_UR_lssolve(r,y,t,&rhs.vector,qr,work));
    double x=gsl_vector_get(qr,k);
    printf("GSL %s; high-precision reference rounded to double: %.17g\n",gsl_version,reference);
    printf("QR %.17g; relative reference error %.17g\n",x,fabs((x-reference)/reference));
    int original_failed=0;
    for (int jacobi=0;jacobi<3;++jacobi) {
        gsl_matrix_memcpy(u,&bmat.matrix);
        if(jacobi==2) { gsl_matrix *aux=gsl_matrix_alloc(n,n); CHECK(gsl_linalg_SV_decomp_mod(u,aux,v,s,work)); gsl_matrix_free(aux); }
        else if(jacobi==1) { CHECK(gsl_linalg_SV_decomp_jacobi(u,v,s)); }
        else { CHECK(gsl_linalg_SV_decomp(u,v,s,work)); }
        CHECK(gsl_linalg_SV_solve(u,v,s,&rhs.vector,svd));
        double z=gsl_vector_get(svd,k), difference=fabs((x-z)/z);
        printf("%s SVD %.17g; relative reference error %.17g; QR comparison %.17g > %.17g: %s\n",
               jacobi==2?"Modified":jacobi==1?"Jacobi":"Original",z,fabs((z-reference)/reference),difference,tolerance,
               difference>tolerance?"FAIL":"PASS");
        if(!jacobi) original_failed=difference>tolerance;
    }
    gsl_matrix_free(r); gsl_matrix_free(y); gsl_matrix_free(t);
    gsl_matrix_free(u); gsl_matrix_free(v);
    gsl_vector_free(qr); gsl_vector_free(work); gsl_vector_free(svd); gsl_vector_free(s);
    return original_failed;
}
