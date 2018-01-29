#ifndef PYCADICAL_HPP
#define PYCADICAL_HPP
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "cadical.hpp"

namespace py = pybind11;

class PyCadical
{
    CaDiCaL::Solver solver;

    bool unfinished_clause = false;
    unsigned int lit_in_clause_counter = 0;

    std::map<unsigned int, unsigned int> clause_hist;

    long last_solve_time = -1;
    int last_solve_status = -1;

public:

    // Native CaDiCal methods
    void init(int new_max);
    int max() const;

    bool has (const char * name);
    double get (const char * name);
    bool set (const char * name, double val);

    void add(int lit);
    int val(int lit);
    int solve();

    const char * version();

    void banner ();
    void usage ();
    void options ();
    void statistics ();

    // Convencience functions
    void add_clause(std::vector<int>& clause);
    void fix_vars(std::vector<int>& lits);
    std::vector<int> get_sol();
    py::array_t<int> get_sol_np();

    std::vector<std::vector<int> > get_all_sol();  // modifies problem !

    long get_last_solve_time();
    int get_last_solve_status();

    std::map<unsigned int, unsigned int> get_clause_hist();

};

#endif
