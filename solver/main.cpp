#include <iostream>
#include "Solver.hpp"

using namespace std;


static inline bool skip(istream & input)
{
    char c;
    while (1) {
        input >> c;
        if (c == ',') return false;
        else if (c == ';') return true;
    }
}

struct Problem {
    /* Problem input */
    int T, S, E, P, X;
    int **Ae, **Bp;
    int *Cs;

    /* Problem working structures */
    Solver solver;
    int ***mu;

    Problem(istream & input){
        parse(input);
        addConstraints();
    }

    ~Problem(){
        for (int e=0; e<E; e++)
            delete[] Ae[e];
        delete[] Ae;

        for (int p=0; p<P; p++)
            delete[] Bp[p];
        delete[] Bp;

        for (int x=0; x<X; x++){
            for (int s=0; s<S; s++){
                delete[] mu[x][s];
            }
            delete[] mu[x];
        }
        delete[] mu;
        delete[] Cs;
    }

    void parse(istream & input){
        int n;

        input >> T;
        cout << "Nombre de tranches horaires: " << T << endl;
        skip(input);

        input >> S;
        cout << "Nombre de salles: " << S << endl;
        skip(input);

        Cs = new int[S];
        for (int i=0; i<S; i++){
            input >> Cs[i];
            skip(input);
            cout << "    Salle " << i+1 << ": " << Cs[i] << " places" << endl;
        }

        input >> E;
        cout << "Nombre d'etudiants: " << E << endl;
        skip(input);

        input >> P;
        cout << "Nombre de professeurs: " << P << endl;
        skip(input);

        input >> X;
        cout << "Nombre d'examens: " << X << endl;
        skip(input);

        Ae = new int*[E];
        for (int i=0; i<E; i++){
            Ae[i] = new int[X];
            for (int j=0; j<X; j++){
                Ae[i][j] = 0;
            }

            cout << "    L'etudiant " << i+1 << " passe les examens:";
            do {
                input >> n;
                Ae[i][n-1] = 1;
                cout << " " << n;
            } while (! skip(input));
            cout << endl;
        }

        cout << endl << endl;

        Bp = new int*[P];
        for (int i=0; i<P; i++){
            Bp[i] = new int[X];
            for (int j=0; j<X; j++){
                Bp[i][j] = 0;
            }

            cout << "    Le prof " << i+1 << " surveille les examens:";
            do {
                input >> n;
                Bp[i][n-1] = 1;
                cout << " " << n;
            } while (! skip(input));
            cout << endl;
        }
        cout << "....................................." << endl;
    }

    void addConstraints(){
        /* Creation de la matrice 3D (X,S,T) */
        mu = new int**[X];
        for (int x=0; x<X; x++){
            vec<Lit> solution_exists;
            mu[x] = new int*[S];
            for (int s=0; s<S; s++){
                mu[x][s] = new int[T];
                for (int t=0; t<T; t++){
                    mu[x][s][t] = solver.newVar();
                    solution_exists.push(Lit(mu[x][s][t]));
                }
            }

            /* Chaque examen doit avoir lieu */
            solver.addClause(solution_exists);
        }

        /* Contrainte: un exam a lieu dans une et une seule salle */
        for (int x=0; x<X; x++){
            for (int s1=1; s1<S; s1++){
                for (int s2=0; s2<s1; s2++){
                    for (int t=0; t<T; t++){
                        solver.addBinary(~Lit(mu[x][s1][t]), ~Lit(mu[x][s2][t]));
                    }
                }
            }
        }

        /* Contrainte: un exam a lieu à une et une seule heure */
        for (int x=0; x<X; x++){
            for (int s=0; s<S; s++){
                for (int t1=1; t1<T; t1++){
                    for (int t2=0; t2<t1; t2++){
                        solver.addBinary(~Lit(mu[x][s][t1]), ~Lit(mu[x][s][t2]));
                    }
                }
            }
        }

        /* Contrainte : un étudiant ne peut passer qu'un examen à la fois */
        for (int e=0; e<E; e++){
            for (int x1=1; x1<X; x1++){
                for (int x2=0; x2<x1; x2++){
                    if (pass_both_exams(e, x1, x2)){
                        cout << "L'examen " << x1+1
                             << " et l'examen " << x2+1
                             << " ne peuvent avoir lieu simultanement "
                             << "(Etudiant " << e+1 << ")" << endl;
                        for (int s1=1; s1<S; s1++){
                            for (int s2=0; s2<s1; s2++){
                                for (int t=0; t<T; t++){
                                    solver.addBinary(~Lit(mu[x1][s1][t]), ~Lit(mu[x2][s2][t]));
                                }
                            }
                        }
                    }
                }
            }
        }

        /* Contrainte: un prof ne peut surveiller qu'un examen à la fois */
        for (int p=0; p<P; p++){
            for (int x1=1; x1<X; x1++){
                for (int x2=0; x2<x1; x2++){
                    if (supervise_both_exams(p, x1, x2)){
                        cout << "L'examen " << x1+1
                             << " et l'examen " << x2+1
                             << " ne peuvent avoir lieu simultanement "
                             << "(Prof " << p+1 << ")" << endl;
                        for (int s1=1; s1<S; s1++){
                            for (int s2=0; s2<s1; s2++){
                                for (int t=0; t<T; t++){
                                    solver.addBinary(~Lit(mu[x1][s1][t]), ~Lit(mu[x2][s2][t]));
                                }
                            }
                        }
                    }
                }
            }
        }

        /* Contrainte: capacité de la salle */
        for (int x=0; x<X; x++){
            for (int s=0; s<S; s++){
                if (students_for_exam(x) > Cs[s]){
                    cout << "L'examen " << x+1
                         << " ne peut avoir lieu dans la salle " << s+1
                         << endl;
                    for (int t=0; t<T; t++){
                        solver.addUnit(~Lit(mu[x][s][t]));
                    }
                }
            }
        }
    }

    bool supervise_both_exams(int p, int x1, int x2){return Bp[p][x1] && Bp[p][x2];}
    bool pass_both_exams(int e, int x1, int x2){return Ae[e][x1] && Ae[e][x2];}

    /* -> number of students passing exam x */
    int students_for_exam(int x){
        int res = 0;
        for (int e=0; e<E; e++){
            res += Ae[e][x];
        }
        return res;
    }

    void print_solution(ostream & out)
    {
        printf("%7s | %7s | %7s\n", "Horaire", "Salle", "Examen");
        printf("--------+---------+---------\n");
        for (int t=0; t<T; t++){
            for (int x=0; x<X; x++){
                for (int s=0; s<S; s++){
                    if (solver.model[mu[x][s][t]] == l_True)
                        printf("%7d | %7d | %7d\n", t+1, s+1, x+1);
                }
            }
        }
    }
};

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
        cout << "\033[1;33m==========================================================\033[0m" << endl;
    }
    return 0;
}
