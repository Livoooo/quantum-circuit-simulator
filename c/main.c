#include "statevector.h"

#include <math.h>
#include <stdio.h>

static int failures = 0;

#define CHECK(cond)                                                 \
    do {                                                            \
        if (!(cond)) {                                              \
            printf("  ECHEC ligne %d : %s\n", __LINE__, #cond);     \
            failures++;                                             \
        }                                                           \
    } while (0)

static int approx(double a, double b)
{
    return fabs(a - b) < 1e-12;
}

static void test_create_initial_state(void)
{
    printf("test_create_initial_state\n");
    statevector *sv = NULL;
    CHECK(sv_create(3, &sv) == SV_OK);
    CHECK(sv != NULL);
    CHECK(sv_num_qubits(sv) == 3);
    CHECK(sv_size(sv) == 8);

    cplx a;
    CHECK(sv_amplitude(sv, 0, &a) == SV_OK);
    CHECK(approx(creal(a), 1.0) && approx(cimag(a), 0.0));
    for (size_t i = 1; i < 8; i++) {
        CHECK(sv_amplitude(sv, i, &a) == SV_OK);
        CHECK(approx(creal(a), 0.0) && approx(cimag(a), 0.0));
    }
    sv_destroy(sv);
}

static void test_probabilities(void)
{
    printf("test_probabilities\n");
    statevector *sv = NULL;
    CHECK(sv_create(3, &sv) == SV_OK);

    double p[8];
    CHECK(sv_probabilities(sv, p, 8) == SV_OK);
    double sum = 0.0;
    for (size_t i = 0; i < 8; i++)
        sum += p[i];
    CHECK(approx(p[0], 1.0));
    CHECK(approx(sum, 1.0));

    CHECK(sv_probabilities(sv, p, 4) == SV_ERR_INVALID_ARG);
    CHECK(sv_probabilities(sv, NULL, 8) == SV_ERR_INVALID_ARG);
    sv_destroy(sv);
}

static void test_errors(void)
{
    printf("test_errors\n");
    statevector *sv = NULL;
    CHECK(sv_create(0, &sv) == SV_ERR_INVALID_ARG);
    CHECK(sv == NULL);
    CHECK(sv_create(-1, &sv) == SV_ERR_INVALID_ARG);
    CHECK(sv_create(31, &sv) == SV_ERR_INVALID_ARG);
    CHECK(sv_create(3, NULL) == SV_ERR_INVALID_ARG);

    CHECK(sv_create(3, &sv) == SV_OK);
    cplx a;
    CHECK(sv_amplitude(sv, 8, &a) == SV_ERR_OUT_OF_RANGE);
    CHECK(sv_amplitude(sv, 0, NULL) == SV_ERR_INVALID_ARG);
    CHECK(sv_amplitude(NULL, 0, &a) == SV_ERR_INVALID_ARG);
    sv_destroy(sv);

    sv_destroy(NULL); /* ne doit pas planter */
    CHECK(sv_num_qubits(NULL) == 0);
    CHECK(sv_size(NULL) == 0);
}

static void test_status_str(void)
{
    printf("test_status_str\n");
    printf("  SV_OK               -> %s\n", sv_status_str(SV_OK));
    printf("  SV_ERR_OUT_OF_RANGE -> %s\n", sv_status_str(SV_ERR_OUT_OF_RANGE));
    CHECK(sv_status_str(SV_OK) != NULL);
}

int main(void)
{
    test_create_initial_state();
    test_probabilities();
    test_errors();
    test_status_str();

    if (failures == 0) {
        printf("\nTous les tests passent\n");
        return 0;
    }
    printf("\n%d echec(s)\n", failures);
    return 1;
}
