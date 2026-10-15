#ifndef STATEVECTOR_H
#define STATEVECTOR_H

#include <complex.h>
#include <stddef.h>

typedef double complex cplx;
typedef cplx gate1[4];

typedef enum {
    SV_OK = 0,
    SV_ERR_INVALID_ARG,
    SV_ERR_INVALID_QUBIT,
    SV_ERR_OUT_OF_RANGE,
    SV_ERR_NO_MEMORY
} sv_status;

const char *sv_status_str(sv_status s);

typedef struct statevector statevector;
sv_status sv_create(int num_qubits, statevector **out);
void sv_destroy(statevector *sv);
int sv_num_qubits(const statevector *sv);
size_t sv_size(const statevector *sv); /* 2^n */
sv_status sv_amplitude(const statevector *sv, size_t index, cplx *out);
sv_status sv_probabilities(const statevector *sv, double *out, size_t out_len);
sv_status sv_apply_1q(statevector *sv, const gate1 u, int target);
sv_status sv_x(statevector *sv, int target);
sv_status sv_h(statevector *sv, int target);
sv_status sv_z(statevector *sv, int target);
sv_status sv_cnot(statevector *sv, int control, int target);

#endif
