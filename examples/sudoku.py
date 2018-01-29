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
