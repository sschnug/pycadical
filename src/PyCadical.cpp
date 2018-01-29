#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <chrono>
#include <algorithm>
#include <functional>
#include "PyCadical.hpp"

namespace py = pybind11;

using namespace std::chrono;

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;

// Native CaDiCal methods

void PyCadical::init(int new_max)
{
    this->solver.init(new_max);
}

int PyCadical::max() const
{
    return this->solver.max();
}

bool PyCadical::has(const char * name)
{
    return this->solver.has(name);
}

double PyCadical::get(const char * name)
{
    return this->solver.get(name);
}

bool PyCadical::set(const char * name, double val)
{
    return this->solver.set(name, val);
}

void PyCadical::add(int lit)
{
    if(lit == 0)
    {
        this->unfinished_clause = false;

        std::map<unsigned int, unsigned int>::iterator map_it =
            this->clause_hist.find(this->lit_in_clause_counter);
        if(map_it == this->clause_hist.end())
        {
            this->clause_hist.insert(
                std::pair<unsigned int, unsigned int>(this->lit_in_clause_counter, 1));
        }
        else
        {
            map_it->second++;
        }

        this->lit_in_clause_counter = 0;
    }
    else
    {
        this->unfinished_clause = true;
        this->lit_in_clause_counter++;
    }
    this->solver.add(lit);
}

int PyCadical::val(int lit)
{
    return this->solver.val(lit);
}

int PyCadical::solve()
{
    auto t0 = Time::now();
    int result = this->solver.solve();
    auto t1 = Time::now();
    ms d = std::chrono::duration_cast<ms>(t1 - t0);
    this->last_solve_time  = d.count();
    this-> last_solve_status = result;
    return result;
}

const char * PyCadical::version()
{
    return this->solver.version();
}

void PyCadical::banner()
{
    this->solver.banner();
}

void PyCadical::usage()
{
    this->solver.usage();
}

void PyCadical::options()
{
    this->solver.options();
}

void PyCadical::statistics()
{
    this->solver.statistics();
}

// Convencience functions

void PyCadical::add_clause(std::vector<int>& clause)
{
    // TODO: assert that we have no clause open!
    unsigned int n = clause.size();

    std::map<unsigned int, unsigned int>::iterator map_it =
        this->clause_hist.find(n);
    if(map_it == this->clause_hist.end())
    {
        this->clause_hist.insert(
            std::pair<unsigned int, unsigned int>(n, 1));
    }
    else
    {
        map_it->second++;
    }

    for (const int& l : clause)
        this->solver.add(l);
    this->solver.add(0);
}

void PyCadical::fix_vars(std::vector<int>& lits)
{
    unsigned int n = lits.size();

    // TODO check for this globally -> more effcient than logn lookup each time!
    std::map<unsigned int, unsigned int>::iterator map_it =
        this->clause_hist.find(1);
    if(map_it == this->clause_hist.end())
    {
        this->clause_hist.insert(
            std::pair<unsigned int, unsigned int>(1, n));
    }
    else
    {
        map_it->second += n;
    }

    // TODO: assert that we have no clause open
    for (const int& l : lits)
    {
        this->solver.add(l);
        this->solver.add(0);
    }
}

std::vector<int> PyCadical::get_sol()
{
    // TODO: assert solved!
    int n = this->max();
    std::vector<int> sol;
    sol.reserve(n);

    for(int i=1; i<=n; ++i)
    {
        if(this->val(i) == 1)
            sol.push_back(i);
        else
            sol.push_back(-i);
    }
    return sol;
}

py::array_t<int> PyCadical::get_sol_np()
{
    // TODO: assert solved!
    int n = this->max();
    std::vector<int> sol;
    sol.reserve(n);

    for(int i=1; i<=n; ++i)
    {
        if(this->val(i) == 1)
            sol.push_back(i);
        else
            sol.push_back(-i);
    }
    return py::array(n, sol.data());
}

long PyCadical::get_last_solve_time()
{
    return this->last_solve_time;
}

int PyCadical::get_last_solve_status()
{
    return this->last_solve_status;
}

std::map<unsigned int, unsigned int> PyCadical::get_clause_hist()
{
    return this->clause_hist;
}

std::vector<std::vector<int> > PyCadical::get_all_sol()
{
    // TODO WARNING -> not working: see https://github.com/arminbiere/cadical/issues/2
    this->set("check", true);

    // TODO
    std::vector<std::vector<int> > sols;

    bool finished = false;
    while(!finished)
    {
        int status = this->solve();

        py::print(status);

        if(status == 10)
        {
            // sol found
            std::vector<int> sol = this->get_sol();
            sols.push_back(sol);  // add to solution-pool

            std::vector<int> sol_copy = sol;
            std::transform(sol_copy.cbegin(),sol_copy.cend(),sol_copy.begin(),std::negate<int>());  // negate
            this->add_clause(sol_copy); // forbid sol in problem
        }
        else if (status == 20)
        {
            // no sol found
            finished = true;
        }
        else
        {
            // TODO something bad happened
        }
    }
    return sols;
}
