#ifndef NACTENIDAT_H
#define NACTENIDAT_H

const int N = 5;
class nactenidat
{
    bool Initialized = false;
    MainWindow *ui;
public:
     // pocet vrstev
    static double z[N+1], d[N], K[N], R[2], r[2], Q[2], s[2], H, L, T;
    nactenidat(MainWindow *ptr);
    void refresh();
};

#endif // NACTENIDAT_H
