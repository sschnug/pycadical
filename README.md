pycadical
=========

This is a simple wrapper for the [CaDiCaL](https://github.com/arminbiere/cadical) SAT-solver by [Armin Biere](http://fmv.jku.at/biere/index.html) using [pybind11](https://github.com/pybind/pybind11).

For some other (bigger) project i'm trying out pybind11 and re-discover C++, which effected in this code.

The basic setup.py template is from [pybind/python_example](https://github.com/pybind/python_example) by [Sylvain Corlay](https://github.com/SylvainCorlay).

Installation
------------
- Install was only tested on (Ubuntu-)Linux and probably does not work for other systems
- Only Python 3 was tested and the example will need [numpy](https://github.com/numpy/numpy)
- The thirdparty-step will copy ```libcadical``` to ```/usr/lib``` (which probably is not best-practice)

Steps:
- clone this repository
- cd to ```/thirdparty```
- ```sudo ./install_cadical.sh``` (or whatever is needed to write to ```/usr/lib```)
- cd to base-repository (which has ```setup.py```)
- ```sudo python3 setup.py install```

Status
------
*Prototype!*

What's wrapped

- Most methods in ```cadical.hpp``` are wrapped
- There are some additional helpers & small features to make usage easier or to obtain
more infos
  - See ```PyCadical.hpp```

What's working

- Basic non-incremental usage looks good (see example)
- Some cadical-methods which do printing actually do nothing (needs more pybind11 magic currently not tried)
- Incremental-usage probably does not work as expected when reaching UNSAT
  - [see here](https://github.com/arminbiere/cadical/issues/2)

Example
-------
There is a basic *sudoku*-example in ```/examples/sudoku.py```:

```python
from pyCadical import PyCadical
import itertools
import numpy as np

"""
Small example solving a Sudoku.

Warning:
    The numpy-usage here is sub-optimal in terms of vectorization, but should
    be easier to read!

Example instance and basic approach follows:
    Lynce, Inês, and Joël Ouaknine. "Sudoku as a SAT Problem." ISAIM. 2006.

- 1 8 - - - 7 - -
- - - 3 - - 2 - -
- 7 - - - - - - -
- - - - 7 1 - - -
6 - - - - - - 4 -
3 - - - - - - - -
4 - - 5 - - - - 3
- 2 - - 8 - - - -
- - - - - - - 6 -
"""

sudoku = np.array([[0,1,8,0,0,0,7,0,0],
                   [0,0,0,3,0,0,2,0,0],
                   [0,7,0,0,0,0,0,0,0],
                   [0,0,0,0,7,1,0,0,0],
                   [6,0,0,0,0,0,0,4,0],
                   [3,0,0,0,0,0,0,0,0],
                   [4,0,0,5,0,0,0,0,3],
                   [0,2,0,0,8,0,0,0,0],
                   [0,0,0,0,0,0,0,6,0]])
print('Incomplete Sudoku')
print(sudoku)

""" Variables """
vars = np.arange(9*9*9).reshape(9,9,9) + 1

""" Create solver instance """
cad = PyCadical()

""" Add clauses """

# a-priori known entries
known_pos = np.where(sudoku > 0)
known_val = sudoku[known_pos]
cad.fix_vars(vars[known_pos[0], known_pos[1], known_val-1])  # -1!

# at least one number in each cell
for row in range(9):
    for col in range(9):
        cad.add_clause(vars[row, col, :])

# each number appears at most once in each row
for row in range(9):
    for val in range(9):
        for a, b, in itertools.combinations(vars[row, :, val], 2):
            cad.add_clause([-a, -b])

# each number appears at most once in each col
for col in range(9):
    for val in range(9):
        for a, b, in itertools.combinations(vars[:, col, val], 2):
            cad.add_clause([-a, -b])

# each number appears at most once in each 3x3 sub-grid
for srow in range(3):
    for scol in range(3):
         for val in range(9):
             for a, b in itertools.combinations(vars[srow*3:(srow+1)*3,
                                                     scol*3:(scol+1)*3, val].ravel(), 2):
                cad.add_clause([-a, -b])

""" Solve """
cad.solve()

""" Read out solution """
sol = cad.get_sol_np()

output_matrix = np.empty((9,9), dtype=int)
for row in range(9):
    for col in range(9):
        val_index = np.flatnonzero(sol.reshape(9,9,9)[row, col]>0)
        output_matrix[row, col] = val_index[0] + 1
print('Solution')
print(output_matrix)

""" Statistics """
print('Solver time used (milliseconds): ', cad.get_last_solve_time())

print('Clause histogram')
for key, val in cad.get_clause_hist().items():
    print(key, val)

```

 which will output:

```python
Incomplete Sudoku
[[0 1 8 0 0 0 7 0 0]
 [0 0 0 3 0 0 2 0 0]
 [0 7 0 0 0 0 0 0 0]
 [0 0 0 0 7 1 0 0 0]
 [6 0 0 0 0 0 0 4 0]
 [3 0 0 0 0 0 0 0 0]
 [4 0 0 5 0 0 0 0 3]
 [0 2 0 0 8 0 0 0 0]
 [0 0 0 0 0 0 0 6 0]]
Solution
[[5 1 8 9 2 6 7 3 4]
 [9 6 4 3 5 7 2 8 1]
 [2 7 3 1 4 8 6 5 9]
 [8 5 2 4 7 1 3 9 6]
 [6 9 7 2 3 5 1 4 8]
 [3 4 1 8 6 9 5 2 7]
 [4 8 6 5 1 2 9 7 3]
 [7 2 9 6 8 3 4 1 5]
 [1 3 5 7 9 4 8 6 2]]
Solver time used (milliseconds):  27
Clause histogram
1 17
2 8748
9 81

```
