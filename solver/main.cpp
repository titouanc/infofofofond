#include "Problem.hpp"

using namespace std;

int main(int argc, const char **argv)
{
    int nProbs = 1;
    if (argc > 1){
        nProbs = strtol(argv[1], NULL, 10);
    }

    for (int i=0; i<nProbs; i++){
        Problem p(cin);
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
