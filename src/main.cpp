#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PyCadical.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pyCadical, m)
{
  m.doc() = "pyCadical";

  py::class_<PyCadical>(m, "PyCadical")
  .def(py::init<>())
  .def("init", &PyCadical::init, "init")
  .def("max", &PyCadical::max, "max")
  .def("has", &PyCadical::has, "has")
  .def("get", &PyCadical::get, "get")
  .def("set", &PyCadical::set, "set")
  .def("add", &PyCadical::add, "add")
  .def("val", &PyCadical::val, "val")
  .def("solve", &PyCadical::solve, "solve")
  .def("version", &PyCadical::version, "version")
  .def("banner", &PyCadical::banner, "banner")
  .def("usage", &PyCadical::usage, "usage")
  .def("options", &PyCadical::options, "options")
  .def("statistics", &PyCadical::statistics, "statistics")
  .def("add_clause", &PyCadical::add_clause, "add_clause")
  .def("fix_vars", &PyCadical::fix_vars, "fix_vars")
  .def("get_sol", &PyCadical::get_sol, "get_sol")
  .def("get_sol_np", &PyCadical::get_sol_np, "get_sol_np")
  .def("get_last_solve_time", &PyCadical::get_last_solve_time, "get_last_solve_time")
  .def("get_last_solve_status", &PyCadical::get_last_solve_status, "get_last_solve_status")
  .def("get_clause_hist", &PyCadical::get_clause_hist, "get_clause_hist")
  .def("get_all_sol", &PyCadical::get_all_sol, "get_all_sol")
  ;
}
