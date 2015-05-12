#include "Problem.hpp"

using namespace std;

struct Options {
    int n_problems;
    bool use_time;
};

Options parse_args(int argc, const char **argv)
{
    Options res = {1, false};
    while (argc){
        cout << *argv << endl;
        if (string(*argv) == "-n" && argc > 1){
            res.n_problems = strtol(argv[1], NULL, 10);
            argc--;
            argv++;
        }
        else if (string(*argv) == "-t"){
            res.use_time = true;
        }
        argc--;
        argv++;
    }
    return res;
}

int main(int argc, const char **argv)
{
    Options opts = parse_args(argc-1, argv+1);

    for (int i=0; i<opts.n_problems; i++){
        Problem p(cin, opts.use_time);
        p.solver.solve();

        if (! p.solver.okay()){
            cout << "Le probleme n'a pas de solution" << endl;
        }
        else {
            cout << "Le probleme a une solution" << endl;
            p.print_solution(cout);
        }
        cout << "\033[1;3";

        if (p.solver.okay())
            cout << "2m";
        else
            cout << "1m";
        cout << "==========================================================\033[0m" << endl;
        
    }
    return 0;
}
