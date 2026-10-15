#include "../include/statevector.h"
#include <stdlib.h>

struct statevector {
    int num_qubits;
    size_t size;
    cplx *amplitudes;
};

const char *sv_status_str(sv_status s)
{
    switch (s) {
    case SV_OK:                return "ok";
    case SV_ERR_INVALID_ARG:   return "invalid argument";
    case SV_ERR_INVALID_QUBIT: return "invalid qubit";
    case SV_ERR_OUT_OF_RANGE:  return "index out of range";
    case SV_ERR_NO_MEMORY:     return "out of memory";
    }
    return "unknown error";
}

sv_status sv_create(int num_qubits, statevector **out)
{
    if (out == NULL)
        return SV_ERR_INVALID_ARG;
    *out = NULL;

    if (num_qubits < 1 || num_qubits > 30)
        return SV_ERR_INVALID_ARG;

    statevector *sv = malloc(sizeof *sv);
    if (sv == NULL)
        return SV_ERR_NO_MEMORY;

    sv->num_qubits = num_qubits;
    sv->size = (size_t)1 << num_qubits;
    sv->amplitudes = calloc(sv->size, sizeof *sv->amplitudes);
    if (sv->amplitudes == NULL) {
        free(sv);
        return SV_ERR_NO_MEMORY;
    }

    sv->amplitudes[0] = 1.0;
    *out = sv;
    return SV_OK;
}

void sv_destroy(statevector *sv)
{
    if (sv == NULL)
        return;
    free(sv->amplitudes);
    free(sv);
}

int sv_num_qubits(const statevector *sv)
{
    return sv != NULL ? sv->num_qubits : 0;
}

size_t sv_size(const statevector *sv)
{
    return sv != NULL ? sv->size : 0;
}

sv_status sv_amplitude(const statevector *sv, size_t index, cplx *out)
{
    if (sv == NULL || out == NULL)
        return SV_ERR_INVALID_ARG;
    if (index >= sv->size)
        return SV_ERR_OUT_OF_RANGE;
    *out = sv->amplitudes[index];
    return SV_OK;
}

sv_status sv_probabilities(const statevector *sv, double *out, size_t out_len)
{
    if (sv == NULL || out == NULL)
        return SV_ERR_INVALID_ARG;
    if (out_len < sv->size)
        return SV_ERR_INVALID_ARG;
    for (size_t i = 0; i < sv->size; i++) {
        double re = creal(sv->amplitudes[i]);
        double im = cimag(sv->amplitudes[i]);
        out[i] = re * re + im * im;
    }
    return SV_OK;
}
