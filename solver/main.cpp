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
    int T, S, E, P, X;
    int **Ae, **Bp;
    int *Cs;

    Problem(istream & input){
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

            cout << "    L'etudiant " << i+1 << endl;

            do {
                input >> n;
                Ae[i][n-1] = 1;
                cout << "        passe l'examen " << n << endl;
            } while (! skip(input));
        }

        Bp = new int*[P];
        for (int i=0; i<P; i++){
            Bp[i] = new int[X];
            for (int j=0; j<X; j++){
                Bp[i][j] = 0;
            }

            cout << "    Le prof " << i+1 << endl;

            do {
                input >> n;
                Bp[i][n-1] = 1;
                cout << "        surveille l'examen " << n << endl;
            } while (! skip(input));
        }
    }

    ~Problem(){
        int i;
        for (i=0; i<E; i++)
            delete Ae[i];
        delete Ae;
        for (i=0; i<P; i++)
            delete Bp[i];
        delete Bp;
        delete Cs;
    }
};

int main(int argc, const char **argv)
{
    Solver s;
    Problem p(cin);
    return 0;
}
