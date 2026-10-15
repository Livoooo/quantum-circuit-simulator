#pragma once
#include <array>
#include <complex>
#include <cstddef>
#include <map>
#include <random>
#include <string>
#include <vector>

namespace qbit {
    using cplx = std::complex<double>;
    using Gate1 = std::array<cplx, 4>; // matrice 2x2 ligne par ligne : {U00, U01, U10, U11}

    class StateVector {
    public:
        explicit StateVector(int num_qubits); // initialise à |00...0⟩

        int num_qubits() const;
        std::size_t size() const;
        cplx amplitude(std::size_t index) const;
        std::vector<double> probabilities() const;

        // Portes
        void apply_1q(const Gate1& u, int target);
        void x(int target);
        void h(int target);
        void z(int target);
        void cnot(int control, int target);

        // Mesure : renvoie {"bitstring" -> nombre d'occurrences}, q0 à droite
        std::map<std::string, int> sample(int shots, unsigned seed = std::random_device{}()) const;

    private:
        void check_qubit(int q) const;

        int n_;
        std::vector<cplx> ampl_;
    };

}