#include "statevector.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace qbit {

    // ---------- (a) Construction et accès ----------

    StateVector::StateVector(int num_qubits) : n_(num_qubits)
    {
        if (num_qubits < 1 || num_qubits > 30)
            throw std::invalid_argument("StateVector: num_qubits doit être entre 1 et 30");
        ampl_.assign(std::size_t{1} << n_, cplx{0.0, 0.0});
        ampl_[0] = cplx{1.0, 0.0}; // |00...0⟩
    }

    int StateVector::num_qubits() const { return n_; }

    std::size_t StateVector::size() const { return ampl_.size(); }

    cplx StateVector::amplitude(std::size_t index) const
    {
        return ampl_.at(index); // .at() lève std::out_of_range si index invalide
    }

    std::vector<double> StateVector::probabilities() const
    {
        std::vector<double> p(ampl_.size());
        for (std::size_t i = 0; i < ampl_.size(); ++i)
            p[i] = std::norm(ampl_[i]); // |a|^2
        return p;
    }

    void StateVector::check_qubit(int q) const
    {
        if (q < 0 || q >= n_)
            throw std::out_of_range("StateVector: qubit " + std::to_string(q) + " invalide");
    }

    // ---------- (b) Porte à 1 qubit : algorithme des paires ----------

    void StateVector::apply_1q(const Gate1& u, int target)
    {
        check_qubit(target);
        const std::size_t stride = std::size_t{1} << target;

        for (std::size_t i = 0; i < ampl_.size(); ++i) {
            if ((i >> target) & 1)
                continue; // on ne traite chaque paire qu'une fois, depuis l'indice où target = 0
            const std::size_t j = i | stride; // même indice avec target = 1
            const cplx a0 = ampl_[i];
            const cplx a1 = ampl_[j];
            ampl_[i] = u[0] * a0 + u[1] * a1;
            ampl_[j] = u[2] * a0 + u[3] * a1;
        }
    }

    // ---------- (c) X, H, Z ----------

    void StateVector::x(int target)
    {
        apply_1q({cplx{0}, cplx{1},
                  cplx{1}, cplx{0}}, target);
    }

    void StateVector::h(int target)
    {
        const double s = 1.0 / std::sqrt(2.0);
        apply_1q({cplx{s}, cplx{s},
                  cplx{s}, cplx{-s}}, target);
    }

    void StateVector::z(int target)
    {
        apply_1q({cplx{1}, cplx{0},
                  cplx{0}, cplx{-1}}, target);
    }

    // ---------- (d) CNOT ----------

    void StateVector::cnot(int control, int target)
    {
        check_qubit(control);
        check_qubit(target);
        if (control == target)
            throw std::invalid_argument("StateVector::cnot: control == target");

        const std::size_t tbit = std::size_t{1} << target;
        for (std::size_t i = 0; i < ampl_.size(); ++i) {
            if (((i >> control) & 1) && !((i >> target) & 1))
                std::swap(ampl_[i], ampl_[i | tbit]);
        }
    }

    // ---------- (e) Échantillonnage ----------

    std::map<std::string, int> StateVector::sample(int shots, unsigned seed) const
    {
        if (shots < 0)
            throw std::invalid_argument("StateVector::sample: shots négatif");

        // Probabilités cumulées
        std::vector<double> cum(ampl_.size());
        double acc = 0.0;
        for (std::size_t i = 0; i < ampl_.size(); ++i) {
            acc += std::norm(ampl_[i]);
            cum[i] = acc;
        }

        std::mt19937 rng(seed);
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        std::map<std::string, int> counts;

        for (int s = 0; s < shots; ++s) {
            const double r = dist(rng);
            // premier indice dont la proba cumulée dépasse r
            auto it = std::upper_bound(cum.begin(), cum.end(), r);
            std::size_t idx = (it == cum.end()) ? cum.size() - 1  // sécurité arrondis flottants
                                                : static_cast<std::size_t>(it - cum.begin());

            // Indice -> chaîne binaire de n_ caractères, q0 à droite
            std::string bits(n_, '0');
            for (int k = 0; k < n_; ++k)
                if ((idx >> k) & 1)
                    bits[n_ - 1 - k] = '1';

            ++counts[bits];
        }
        return counts;
    }

}