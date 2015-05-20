#include "Problem.hpp"
#include <fstream>

using namespace std;

struct Options {
    int n_problems;

    bool use_time;
    bool forbidden_times;
    bool switch_hour;
    int k = 0;

    Options(int argc, const char **argv) : 
        n_problems(1), use_time(false), forbidden_times(false), k(-1)
    {
        cerr << tmp_res;
        while (argc){
            if (string(*argv) == "-h"){
                printf("OPTIONS: -n N: lire N problemes consecutivement depuis l'entree\n"
                       "         -k K: Autoriser au plus K changements de salle\n"
                       "         -s  : Minimum 1h entre 2 exams pas dans la meme salle pour un etudiant\n"
                       "         -f  : Lire les periodes qui ne devraient pas avoir d'exams dans le fichier d'entree\n"
                       "         -t  : Lire la duree de chaque exam dans le fichier d'entree\n");
                exit(0);
            }
            if (string(*argv) == "-n" && argc > 1){
                n_problems = strtol(argv[1], NULL, 10);
                argc--;
                argv++;
            }
            if (string(*argv) == "-k" && argc > 1){
                k = strtol(argv[1], NULL, 10);
                argc--;
                argv++;
            }
            else if (string(*argv) == "-t"){
                use_time = true;
            }
            else if (string(*argv) == "-f"){
                forbidden_times = true;
            }
            else if (string(*argv) == "-s"){
                switch_hour = true;
            }
            argc--;
            argv++;
        }
    }
};

int main(int argc, const char **argv)
{
    Options opts(argc-1, argv+1);

    // istream & input = (opts.use_in_file) ? ifstream(opts.in_file) : cin;
    istream & input = cin;

    for (int i=0; i<opts.n_problems; i++){
        Problem p(input, opts.use_time, opts.forbidden_times, opts.switch_hour, opts.k);
        p.solver.solve();

        if (! p.solver.okay()){
            cerr << "Le probleme n'a pas de solution" << endl;
        }
        else {
            cerr << "Le probleme a une solution" << endl;
            p.print_solution(cerr);
        }
        cerr << "\033[1;3";

        if (p.solver.okay())
            cerr << "2m";
        else
            cerr << "1m";
        cerr << "==========================================================\033[0m" << endl;
        
    }
    return 0;
}
