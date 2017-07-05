#ifndef VYPOCETNI_HPP
#define VYPOCETNI_HPP
#include<vector>
#include<fstream>
#include<iostream>

using namespace std;

extern int N;

extern double z[6], d[5], K[5];
extern double Q[2], r[2], R[2], s[2];
extern double L, T, H;

extern ofstream logfile;

enum EndPoint {
    well,
    range,
    watershed,
    other
};

void simple_track_point(double x0, double y0, double krok, vector<double> *X, vector<double> *Y);
void track_point(double x0, double y0, double z0, double krok, vector<double> *X, vector<double> *Y, double *T, EndPoint *ep = NULL);
double track_point(double x0, double y0, double z0, double krok, vector<double> *X, vector<double> *Y, EndPoint *ep = NULL);
double hydraulic_head(double x, double y);
void testovaci_input();
double wellYield(int idx);
double wellDrawdown(int idx);
int sgn(double x);
int getLayer(double Z);
double getK(double Z);
void logInput();
double extrem();
#endif
