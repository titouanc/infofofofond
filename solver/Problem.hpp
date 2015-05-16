#include "Solver.hpp"
#include <iostream>

struct Problem {
    /* Problem input */
    int T, S, E, P, X, I;
    int **Ae, **Bp;
    int *Cs;
    int **forbiddenTimes;

    bool exam_duration;
    int *Dx;

    /* Problem working structures */
    Solver solver;
    int ***mu;

    /* Read problem data from input */
    Problem(std::istream & input, bool use_exam_duration=false);

    ~Problem();

    void add_constraints();

    /* -> true if professor p supervise exams x1 and x2 */
    bool supervise_both_exams(int p, int x1, int x2);

    /* -> true if student e pass both exams x1 and x2 */
    bool pass_both_exams(int e, int x1, int x2);

    /* -> number of students passing exam x */
    int students_for_exam(int x);

    /* -> print report to stdout and expected output on out */
    void print_solution(std::ostream & out);

private:
    /* Parse a problem dataset from input */
    void parse(std::istream & input);
};