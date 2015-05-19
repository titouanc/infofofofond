#include "Problem.hpp"

using namespace std;

/* Consume chars from stdin, return true if ';' or false if ',' */
static inline bool skip(istream & input)
{
    char c;
    while (1) {
        input >> c;
        if (c == ',') return false;
        else if (c == ';') return true;
    }
}

static inline void next_int(istream & input, int & dest, const char *title)
{
    input >> dest;
    cout << title << ": " << dest << endl;
    skip(input);
}

static inline void next_set(istream & input, size_t n_elems, int * & dest, const char *title)
{
    size_t n;
    for (size_t i=0; i<n_elems; i++){
        dest[i] = 0;
    }

    cout << title << ": ";
    do {
        input >> n;
        if (n <= n_elems){
            dest[n-1] = 1;
            cout << " " << n;
        }
    } while (! skip(input));
    cout << endl;
}

Problem::Problem(istream & input, bool use_exam_duration, bool use_forbidden_times, bool use_switch_hour, int k) :
    exam_duration(use_exam_duration), use_forbidden_times(use_forbidden_times), use_switch_hour(use_switch_hour),
    mu(NULL), rc(NULL), k(k)
{
    parse(input);
    add_constraints();
    if (k >= 0){
        add_roomchanges_constraint();
    }
}

Problem::~Problem()
{
    for (int e=0; e<E; e++)
        delete[] Ae[e];
    delete[] Ae;

    for (int p=0; p<P; p++)
        delete[] Bp[p];
    delete[] Bp;

    if (mu != NULL){
        for (int x=0; x<X; x++){
            for (int s=0; s<S; s++){
                delete[] mu[x][s];
            }
            delete[] mu[x];
        }
        delete[] mu;
    }

    if (rc != NULL){
        for (int e=0; e<E; e++){
            for (int s=0; s<S; s++){
                delete[] rc[e][s];
            }
            delete[] rc[e];
        }
        delete[] rc;
    }
    delete[] Cs;

    for (int i=0; i<I; i++)
        delete[] forbiddenTimes[i];
    delete[] forbiddenTimes;
}

void Problem::parse(istream & input)
{
    next_int(input, T, "Nombre de tranches horaires");
    next_int(input, S, "Nombre de salles");

    Cs = new int[S];
    for (int i=0; i<S; i++){
        next_int(input, Cs[i], "    Capacite");
    }

    next_int(input, E, "Nombre d'etudiants");
    next_int(input, P, "Nombre de professeurs");
    next_int(input, X, "Nombre d'examens");

    Ae = new int*[E];
    for (int i=0; i<E; i++){
        Ae[i] = new int[X];
        next_set(input, X, Ae[i], "    Passe les examens");
    }

    Bp = new int*[P];
    for (int i=0; i<P; i++){
        Bp[i] = new int[X];
        next_set(input, X, Bp[i], "    Surveille les examens");
    }

    Dx = new int[X];
    if (exam_duration){
        for (int x=0; x<X; x++){
            next_int(input, Dx[x], "    Duree de l'examen");
        }
    } else {
        for (int x=0; x<X; x++){
            Dx[x] = 1;
        }
    }

    if (use_forbidden_times){
        next_int(input, I, "Nombre de periodes interdites");
        forbiddenTimes = new int*[I];
        for (int i=0; i<I; i++){
            forbiddenTimes[i] = new int[2];
            next_int(input, forbiddenTimes[i][0], "    Debut de la periode");
            next_int(input, forbiddenTimes[i][1], "    Fin de la periode");
        }
    } else {
        I = 0;
        forbiddenTimes = new int*[I];
    }

    cout << "....................................." << endl;
}

int Problem::duration(int x)
{
    return Dx[x] - 1;
}

void Problem::add_constraints()
{
    /* Creation de la matrice 3D (X,S,T) */
    mu = new int**[X];
    for (int x=0; x<X; x++){
        vec<Lit> solution_exists;
        mu[x] = new int*[S];

        for (int s=0; s<S; s++){
            mu[x][s] = new int[T];
            for (int t=0; t<T; t++){
                mu[x][s][t] = solver.newVar();
            }
            for (int t=0; t<T-duration(x); t++){
                solution_exists.push(Lit(mu[x][s][t]));
            }

            /* Il doit rester assez de périodes après le début d'un exam pour
               qu'il ait entièrement lieu */
            for (int t=T-duration(x); t<T; t++){
                solver.addUnit(~Lit(mu[x][s][t]));
            }
        }

        /* Chaque examen doit avoir lieu */
        solver.addClause(solution_exists);
    }

    /* Contrainte: deux examens ne peuvent avoir lieu en même temps
                   dans la même salle */
    for (int x1=0; x1<X; x1++){
        for (int x2=0; x2<X; x2++){
            if (x1 == x2)
                continue;
            for (int s=0; s<S; s++){
                for (int t0=0; t0<T-duration(x1); t0++){
                    for (int t=0; t<=duration(x1); t++)
                        solver.addBinary(~Lit(mu[x1][s][t0]), ~Lit(mu[x2][s][t+t0]));
                }
            }
        }
    }

    /* Contrainte: un exam a lieu dans une et une seule salle */
    for (int x=0; x<X; x++){
        for (int s1=1; s1<S; s1++){
            for (int s2=0; s2<s1; s2++){
                for (int t0=0; t0<T-duration(x); t0++){
                    for (int t=0; t<=duration(x); t++)
                        solver.addBinary(~Lit(mu[x][s1][t0]), ~Lit(mu[x][s2][t0+t]));
                }
            }
        }
    }

    /* Contrainte: un exam commence à une seule période */
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
        for (int x1=0; x1<X; x1++){
            for (int x2=0; x2<X; x2++){
                if (x2 == x1)
                    continue;
                if (pass_both_exams(e, x1, x2)){
                    cout << "L'examen " << x1+1
                         << " et l'examen " << x2+1
                         << " ne peuvent avoir lieu simultanement "
                         << "(Etudiant " << e+1 << ")" << endl;
                    for (int s1=0; s1<S; s1++){
                        for (int s2=0; s2<S; s2++){
                            if (s2 == s1)
                                continue;
                            for (int t0=0; t0<T-duration(x1); t0++){
                                for (int t=0; t<=duration(x1); t++)
                                    solver.addBinary(~Lit(mu[x1][s1][t0]), ~Lit(mu[x2][s2][t0+t]));

                                /* Q9 : one hour between exams in different rooms */
                                if (use_switch_hour){
                                    int sh = t0+duration(x1) + 1;
                                    if (sh < T)
                                        solver.addBinary(~Lit(mu[x1][s1][t0]), ~Lit(mu[x2][s2][sh]));
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /* Contrainte: un prof ne peut surveiller qu'un examen à la fois */
    for (int p=0; p<P; p++){
        for (int x1=0; x1<X; x1++){
            for (int x2=0; x2<X; x2++){
                if (x2 == x1)
                    continue;
                if (supervise_both_exams(p, x1, x2)){
                    cout << "L'examen " << x1+1
                         << " et l'examen " << x2+1
                         << " ne peuvent avoir lieu simultanement "
                         << "(Prof " << p+1 << ")" << endl;
                    for (int s1=0; s1<S; s1++){
                        for (int s2=0; s2<S; s2++){
                            if (s2 == s1)
                                continue;
                            for (int t0=0; t0<T-duration(x1); t0++){
                                for (int t=0; t<=duration(x1); t++)
                                    solver.addBinary(~Lit(mu[x1][s1][t0]), ~Lit(mu[x2][s2][t0+t]));
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
                     << " (trop petite)" << endl;
                for (int t=0; t<T; t++){
                    solver.addUnit(~Lit(mu[x][s][t]));
                }
            }
        }
    }

    /* Contrainte: certaines tranches d'horaire ne sont pas admissibles (Q6) */
    for (int i=0; i<I; i++){
        for (int x=0; x<X; x++){
            for (int s=0; s<S; s++){
                for (int d=forbiddenTimes[i][0]-duration(x); d<=forbiddenTimes[i][1]; d++){
                    if (d > 0)
                        solver.addUnit(~Lit(mu[x][s][d-1])); // d-1 because input is 1-based
                }
            }
        }
    }

    /*
    csab = (a ET non b) OU (non a ET b)
    FNC(csab) = (a OU b) ET (non a OU non b)
    for salleDeDépart = a in Salles:
        for autreSalle(salleDeDépart) = b: (for autreSalle à exécuter k fois)
            for autreSalle(b) = c:
                non (csab ET csbc ET csca)
                FNC ^ : non csab OU non csbc OU non csca
    */
}

void Problem::add_roomchanges_constraint_rec(int e, int t, int s1, int allowed_changes)
{
    assert(0 <= e && e < E);
    assert(0 <= t && t < T);
    assert(0 <= allowed_changes);

    /* Si on a encore droit à plus de changements qu'il reste de périodes
       antérieures, pas besoin d'imposer de contrainte */
    if (t < allowed_changes){
        return;
    }

    /* Sinon, pour toutes les autres salles */
    for (int s2=0; s2<S; s2++){
        /* Si plus aucun chgmt, alors on ne peut plus jamais changer de salle */
        if (allowed_changes == 0){
            if (s1 == s2)
                continue;
            solver.addBinary(~Lit(rc[e][s1][t-1]), ~Lit(rc[e][s2][t]));
            printf("e%d ne peut pas avoir exam en (s%d, t%d) puis (s%d, t%d)\n", e+1, s1+1, t-1, s2+1, t);
        }

        /* Sinon on génère les autres combinaisons impossibles */
        else {
            for (int dt=1; t-dt>=0; dt++){
                /* En restant dans la même salle */
                if (s1 == s2){
                    add_roomchanges_constraint_rec(e, t-dt, s2, allowed_changes);
                }

                /* Ou en ayant changé de salle */
                else {
                    add_roomchanges_constraint_rec(e, t-dt, s2, allowed_changes-1);
                }
            }
        }
    }
}

void Problem::add_roomchanges_constraint()
{
    if (k >= T-1){
        return;
    }

    rc = new int**[E];
    for (int e=0; e<E; e++){
        rc[e] = new int*[S];

        for (int s=0; s<S; s++){
            rc[e][s] = new int[T];
            for (int t=0; t<T; t++){
                rc[e][s][t] = solver.newVar();
                for (int x=0; x<X; x++){
                    if (Ae[x]){
                        solver.addBinary(~Lit(rc[e][s][t]), Lit(mu[x][s][t]));
                        solver.addBinary(Lit(rc[e][s][t]), ~Lit(mu[x][s][t]));
                    }
                }
            }
        }
        for (int s=0; s<S; s++){
            add_roomchanges_constraint_rec(e, T-1, s, k);
            printf("-----\n");
        }
    }
}

bool Problem::supervise_both_exams(int p, int x1, int x2)
{
    return Bp[p][x1] && Bp[p][x2];
}

bool Problem::pass_both_exams(int e, int x1, int x2)
{
    return Ae[e][x1] && Ae[e][x2];
}

/* -> number of students passing exam x */
int Problem::students_for_exam(int x)
{
    int res = 0;
    for (int e=0; e<E; e++){
        res += Ae[e][x];
    }
    return res;
}

void Problem::print_solution(ostream & out)
{
    printf("  t | ");
    for (int s=0; s<S; s++){
        printf("%2d (%4d) | ", s+1, Cs[s]);
    }
    printf("\n----+");
    for (int s=0; s<S; s++){
        printf("-----------+");
    }
    printf("\n");
    
    for (int t0=0; t0<T; t0++){
        printf(" %2d | ", t0);
        for (int s=0; s<S; s++){
            bool exam = false;
            for (int x=0; x<X; x++){
                for (int t=t0; t>=0; t--){
                    if (solver.model[mu[x][s][t]] == l_True){
                        if (duration(x) >= t0-t){
                            int bg = x%8;
                            int fg = (bg == 7) ? 0 : 7;
                            printf("\033[3%d;4%dmExamen %2d\033[0m | ", fg, bg, x+1);
                            exam = true;
                        }
                        break;
                    }
                }
            }
            if (! exam){
                printf("   ----   | ");
            }
        }
        printf("\n");
    }


    if (k >= 0){
        for (int e=0; e<E; e++){
            printf("Planning de l'etudiant %d: ", e+1);
            for (int t=0; t<T; t++){
                for (int s=0; s<S; s++){
                    if (solver.model[rc[e][s][t]] == l_True)
                        printf("t%d: salle %d    ", t, s+1);
                }
            }
            printf("\n");
        }
    }
}
