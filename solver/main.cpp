#include "Problem.hpp"
#include <fstream>

using namespace std;

struct Options {
    int n_problems;

    bool use_time;
    bool forbidden_times;
    bool switch_hour;
    int k = 0;

    bool use_in_file;
    const char *in_file;

    Options(int argc, const char **argv) : 
        n_problems(1), use_time(false), use_in_file(false), in_file(""), forbidden_times(false), k(-1)
    {
        while (argc){
            if (string(*argv) == "-h"){
                printf("OPTIONS: -n N: lire N problemes depuis l'entree\n"
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
                printf("MAX ROOM CHANGES: %d\n", k);
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

ostream & operator<<(ostream & out, Options opts)
{
    out << "<Options problems=" << opts.n_problems
        << " in_file=\"" << opts.in_file << "\"";
    if (opts.use_time)
        out << " using time";
    out << ">";
    return out;
}

int main(int argc, const char **argv)
{
    Options opts(argc-1, argv+1);

    cout << opts << endl;

    // istream & input = (opts.use_in_file) ? ifstream(opts.in_file) : cin;
    istream & input = cin;

    for (int i=0; i<opts.n_problems; i++){
        Problem p(input, opts.use_time, opts.forbidden_times, opts.switch_hour, opts.k);
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
