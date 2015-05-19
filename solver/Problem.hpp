#include "Solver.hpp"
#include <iostream>

struct Problem {
    /* Problem input */
    int T, S, E, P, X, I;
    int **Ae, **Bp;
    int *Cs;
    int **forbiddenTimes;

    bool exam_duration;
    bool use_forbidden_times;
    bool use_switch_hour;
    int *Dx;

    /* Problem working structures */
    Solver solver;

    /* 3D (X,S,T) matrix */
    int ***mu;

    /* Maximum number of room switches for a student, -1 if not taken into account */
    int k;

    bool limit_room_changes;

    /* Read problem data from input */
    Problem(std::istream & input,   bool use_exam_duration=false,
                                    bool use_forbidden_times=false,
                                    bool use_switch_hour=false,
                                    int k=-1);

    ~Problem();

    /* -> true if professor p supervise exams x1 and x2 */
    bool supervise_both_exams(int p, int x1, int x2);

    /* -> true if student e pass both exams x1 and x2 */
    bool pass_both_exams(int e, int x1, int x2);

    /* -> Number of supplementary periods for exam x */
    int duration(int x);

    /* -> number of students passing exam x */
    int students_for_exam(int x);

    /* -> print report to stdout and expected output on out */
    void print_solution(std::ostream & out);

private:
    /* Parse a problem dataset from input */
    void parse(std::istream & input);

    void add_constraints();

    void add_roomchanges_constraint_rec(vec<Lit> & klaus, int e, int x1, int s1, int t, int changes);

    void add_roomchanges_constraint();
};