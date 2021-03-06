#include<cmath>
#include<iostream>
#include"vypocetni.hpp"
//#define DEBUG
#include<iostream>

using namespace std;

int N;

double z[6], d[5], K[5];
double Q[2], r[2], R[2], s[2];
double L, T, H;
ofstream logfile;//("log.txt",ios_base::ate);

// bool first_case = true;

//int getLayer(double Z);
void gradG(double x, double y, double *dGdx, double *dGdy);
double IntegralOverOneLayer(double a, double b, double k, double h);
double Integral(double h);
double GirPot(double r1, double r2);
void gradh(double x, double y, double *dhdx, double *dhdy);
void v(double x, double y, double k, double *vx, double *vy);

/* vraci x lokalniho extremu hydraulicke vysky na spojnici mezi vrty
 * extrem hleda numericky, gradientovou metodou s adaptivnim krokem... inteligentne
 */

/* extrem bude bliz studne s mensi |Q|
 * smer, kterym se od vrtu ubiram ma znamenko:    - sgn(Q)
 */

double extrem()
{ // bisekce intervalu - hledame nulu gradientu hydraulicke vysky
    double a = 0 + r[0];
    double b = L - r[1];
    double x; // pro stred intervalu

    double da, db, dx;
    double odpad;

    gradh(a, 0, &da, &odpad);
    gradh(b, 0, &db, &odpad);

    if(sgn(da) == sgn(db))
        return 0;

    if(sgn(da) == 0)
        return a;

    if(sgn(db) == 0)
        return b;

    do
    {
        x = (a + b) / 2;

        gradh(a, 0, &da, &odpad);
        gradh(x, 0, &dx, &odpad);
        gradh(b, 0, &db, &odpad);

        if(sgn(dx) == sgn(da))
            a = x;

        if(sgn(dx) == sgn(db))
            b = x;

        if(sgn(dx) == 0)
            return x;

    } while( fabs(b-a) > 1e-3 ); // dokud je interval delsi nez 1 mm

    // kdyz uz jsme dostatecne blizko, prohlasime stred intervalu za reseni
    return ((a + b) / 2);
}

int sgn(double x)
{
    return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}

// vypocitat cerpani ze snizeni
double wellYield(int idx)
{
    if((idx!=0) && (idx!=1))
    {
        logfile << "wellYield: bad parameter value!" << endl;
        return 0.0;
    }

    //pocitam cerpani z dane studny - ze snizeni v te studne a vydatnosti druhe studny
    int id2 = 1 - idx; // index druhe studny

    double ro[] = {fabs(idx*L - r[idx]), fabs(id2*L - r[idx])}; // vzdalenosti pro bod na plasti dane studny

    for(int k = 0; k < 2; k++)
        if(ro[k] > R[k]) ro[k] = R[k]; // zkratit vzdalenosti pro pripad, ze by depresni kuzel nektere studny nezasahoval az ke druhe

    // nechci hned prepsat Q-cko, proto NE "Q[idx] = ..."
    //Q[idx] = 1.0/log(r[idx]/R[idx]) * (2*M_PI * (Integral(H-s[idx]) - Integral(H)) - Q[id2]*log(r[id2]/R[id2]));
    return (2*M_PI * (Integral(H-s[idx]) - Integral(H)) - Q[id2]*log(r[id2]/R[id2])) / log(r[idx]/R[idx]);
}

// snizeni ve studnach, pokud jsou zadana Q[]
double wellDrawdown(int idx)
{
    if((idx!=0) && (idx!=1))
    {
        logfile << "wellDrawdown: bad parameter value!" << endl;
        return 0.0;
    }

    //if(Q[idx] == 0)
    //    return 0.0;

    return (H - hydraulic_head(fabs(idx*L - r[idx]),0)); // mwahaha! ... :) vyuzivam toho, ze studna[0] ma vzdycky souradnice [0,0] a druha [L,0]
}

// pocitame hladinu v bode [x,y]
double hydraulic_head(double x, double y)
{
	double r1 = sqrt(x*x + y*y);
	double r2 = sqrt((x-L)*(x-L) + y*y);
    
    if(r1 < r[0])
        return hydraulic_head(r[0],0);

    if(r2 < r[1])
        return hydraulic_head(L-r[1],0);

    if((r1 > R[0]) && (r2 > R[1]))
        return H;

    double Gh = GirPot(r1, r2); // vyzaduje Q-cka
    
    if(Gh >= 0)
        return 0;

    double h_odhad;
    double a,b;

    a = 0;
    b = 50;

    // pro pripad, ze by h > 50:
    while(Gh < Integral(b))
    {
        a = b - 1; // nastavim a kousek pod b
        b+= 10; // postrcim b o kus nahoru
    }

/*    cout << "Gh = " << Gh << endl;
    cout << "sa = " << sa << endl;
    cout << "sb = " << sb << endl;
    cout << "r1 = " << r1 << endl;
    cout << "r2 = " << r2 << endl;
    cout << "IH = " << Integral(H) << endl;
*/
    do {
		h_odhad = (a + b)/2;
        double fh = Gh - Integral(h_odhad);
        
        if( sgn(fh) == -1 )
            a = h_odhad;
        
        if( sgn(fh) ==  1 )
            b = h_odhad;

        if( sgn(fh) ==  0 )
            return h_odhad;

    } while(fabs(b-a) > 1e-4); // hyd. vyska s chybou .1 mm musi stacit kazdymu

    return (a + b)/2;
}

double GirPot(double r1, double r2)
{
    if((sgn(r1) < 1) || (sgn(r2) < 1))
    {
        if(!logfile.is_open()) logfile.open("log.txt",ios_base::app);
        logfile << "GirPot: error: nekladny argument!" << endl;
        return 0.0;
    }
    
    double Pot = 0.0;
    
    if( r1 < R[0] )
        Pot+= Q[0]*log(r1/R[0]);
    
    if( r2 < R[1] )
        Pot+= Q[1]*log(r2/R[1]);
    
    Pot*= 0.5/M_PI;    
    Pot+= Integral(H);
    
    return Pot;
}

//--------------------------------- H O T O V E -----------------------------------

double getK(double Z) // urci, jake K odpovida zadanemu z
{
	int idx = getLayer(Z);
	
	if(idx > -1)
		return K[idx];
	else
		return -1.0;
}

int getLayer(double Z)
{
	for(int i=0; i < N; i++)
		if( (z[i] <= Z) && (Z < z[i+1] )) //match
			return i;
	
	// jestli cyklus skoncil a my jsme dosli sem, ocividne se stala chyba - nesmyslne z
	return -1;
}

double track_point(double x0, double y0, double z0, double krok, vector<double> *X, vector<double> *Y, EndPoint *ep)
{
    double TT;
	
    track_point(x0, y0, z0, krok, X, Y, &TT, ep);
	
    return TT;
}

void track_point(double x0, double y0, double z0, double krok, vector<double> *X, vector<double> *Y, double *TT, EndPoint *ep)
{
	X->clear();
	Y->clear();
    *TT = 0;

    X->reserve(10000);
    Y->reserve(10000);

    if(!logfile.is_open())
        logfile.open("log.txt",ios_base::app);
/*    logfile << "Vstupni data pro proudnici:" << endl;
    logfile << "start: [ " << x0 << ", " << y0 << ", " << z0 << "]" << endl;
    logfile << "krok = " << krok << endl;
*/
    if(fabs(krok) == 0)
    {
        logfile << "track_point: varovani: krok = 0 (asi je Q = 0)." << endl;
        return;
    }
	
	double x = x0;
	double y = y0;
	double K = getK(z0);

    //unsigned int maxidx = int(10*L/fabs(krok));
    //logfile << "maxidx = 10*L/krok = " << maxidx << endl;

	double l[2];
	
	if (K < 0)
	{
        logfile << "track_point: error: Nepodarilo se najit odpovidajici K." << endl;
		return;
	}
	
    double vx, vy, vel, dt, dx, dy;
    bool podm_studna, podm_dosah;

	do {
		X->push_back(x);
		Y->push_back(y);
		
		v(x, y, K, &vx, &vy); // slozky obj. hustoty toku
        vel = sqrt(vx*vx+vy*vy);


        logfile << X->size() << "# \t v = (" << vx << ", " << vy << ")" << endl;


        /*if(vel < fabs(krok)/(3600*24*365.25)) // kdyz krok urazi za rok...
        {
            logfile << "track_point: trajectory terminated: negligible flow" << endl;
            logfile << "x = " << x << endl;
            logfile << "y = " << y << endl;
            logfile << "v = " << vel << endl;
            logfile << "Starting point: [ " << x0 << ", " << y0  << ", " << z0 << "]." << endl;
            if(ep != NULL)
                *ep = other;
            return;
        }
        */
        dt = krok / vel; // skalovat vektor v na delku kroku

        if((X->size()*fabs(krok) > 2*3.15*(10*L)) || (X->size() == X->max_size()))
        {
            logfile << "track_point: trajectory terminated: suspiciously long" << endl;
            logfile << "x = " << x << endl;
            logfile << "y = " << y << endl;
            logfile << "|v| = " << vel << endl;
            logfile << "Starting point: [ " << x0 << ", " << y0 << ", " << z0 << "]." << endl;
 /*           if(first_case) {
                for(int kk = 0; kk < X->size(); kk++)
                    logfile << "Point #" << kk << ": [ " << (*X)[X->size()-1] << ", " << (*Y)[Y->size()-1] << ", " << z0 << "]." << endl;
                first_case = false;
            }
*/            if(ep != NULL)
                *ep = other;
            return;
        }
		

        if(X->size() > 2) {
        if( sqrt(pow((*X)[X->size()-3]-(*X)[X->size()-1],2) + pow((*Y)[Y->size()-3]-(*Y)[Y->size()-1],2)) < krok) // ah, the evil of wrapping yourself around a watershed!
        {
//            logfile << "track_point: trajectory terminated: oscillating" << endl;
//            logfile << "Starting point: [ " << x0 << ", " << y0 << ", " << z0 << "]." << endl;
            // a odstranit posledni dva body:
            X->pop_back();
            X->pop_back();
            Y->pop_back();
            Y->pop_back();
            if(ep != NULL)
                *ep = watershed;
            break;
        }}
        dx = vx * dt;
        dy = vy * dt; // prevedeno na vektor delky "krok"
//    logfile << "vx = " << vx << ", vy = " << vy << endl;
    //logfile << "sgn "<< Q[0] << " = " << sgn(Q[0]) << ", sgn "<< Q[1] <<" = " << sgn(Q[1]) << endl;
//    logfile << "dx = " << dx << ", dy = " << dy << endl;
		x+= dx; // popojit o krok
		y+= dy;
        (*TT)+= dt;

		l[0] = sqrt(x*x+y*y); // vzdalenost sledovane castice od prvni/druhe studny
		l[1] = sqrt((x-L)*(x-L)+y*y);
//      logfile << "l[]: " << l[0] << ", " << l[1] << endl;
        podm_studna = (l[0]<r[0]) || (l[0] < fabs(krok)) || (l[1]<r[1]) || (l[0] < fabs(krok));
        podm_dosah = (l[0] > R[0]*.99) || (l[1] > R[1]*.99) || (l[0] > R[0]-1.5*krok) || (l[1] > R[1]-1.5*krok);

        if(ep != NULL) {

            if(podm_studna)
                *ep = well;

            if(podm_dosah)
                *ep = range;
        }
/*
        logfile << "ukonceni: ";
        switch(*ep) {
            case(well) : logfile << "well"; break;
            case(watershed) : cout << "watershed"; break;
            case(range) : cout << "range"; break;
            case(other) : cout << "other"; break;
        }
        logfile << " na souradnicich [" << (*X)[X->size() - 1] << ";" << (*Y)[Y->size() - 1] << "]" << endl;
*/
    } while ( !(podm_studna || podm_dosah) );

    if(X->size() < 3)
    {
        X->clear();
        Y->clear();
        *TT = 0;
    }
}

void simple_track_point(double x0, double y0, double krok, vector<double> *X, vector<double> *Y) // trajektorie konci, jakmile se dostaneme do vrtu
{
	X->clear();
	Y->clear();
	
	double x = x0;
	double y = y0;
	double dx, dy, koef;
	double l[2];
	
	do {
		X->push_back(x);
		Y->push_back(y);
		
		gradh(x, y, &dx, &dy); // pro anizotropii pouzit void v(...)!!!!!!!
		koef = krok / sqrt(dx*dx+dy*dy);
		dx*= koef;
		dy*= koef; // prevedeno na vektor delky "krok"
		
		x+= dx; // popojit o krok
		y+= dy;
		
		l[0] = sqrt(x*x+y*y); // vzdalenost sledovane castice od prvni/druhe studny
		l[1] = sqrt((x-L)*(x-L)+y*y);
	
#ifdef DEBUG
		cout << "l[0] = " << l[0] << endl;
		cout << "l[1] = " << l[1] << endl;
#endif
		
	} while ( !( (l[0]<r[0]) || (l[1]<r[1]) || ( (l[0]>R[0]) && (l[0]>R[0]) )   ));
}

double Integral(double h)
{
	bool konec = false;
	double Pot = 0.0;
    
    if(h < 0) {
        if(!logfile.is_open()) logfile.open("log.txt",ios_base::app);
        logfile << "Integral: warning: argument < 0" << endl;
    }
    
	for(int i=0; i<N; i++)
	{
		double a = z[i];
		double b;

		if ((h > z[i+1]) || (h < 0))
			b = z[i+1];
		else
		{
			b = h;
			konec = true;
		}

		Pot+= IntegralOverOneLayer(a,b,K[i],h);

		if (konec) break;
	}

	return Pot;
}

double IntegralOverOneLayer(double a, double b, double k, double h)
{
	return ( 0.5 * k * (b*b - a*a) - h * k * (b - a));
}

void gradG(double x, double y, double *dGdx, double *dGdy)
{
	double vzd2[2] = { (x*x+y*y), ((x-L)*(x-L)+y*y) }; // ctverce vzdalenosti od vrtu - hodi se i do vzorcu
	
	*dGdx = 0.0;
	*dGdy = 0.0;
	
	if(vzd2[0] < R[0]*R[0])
	{
		double pom = Q[0] / vzd2[0];
		*dGdx+= pom * x;
		*dGdy+= pom * y;
	}
	
	if(vzd2[1] < R[1]*R[1])
	{
		double pom = Q[1] / vzd2[1];
		*dGdx+= pom * (x-L);
		*dGdy+= pom * y;
	}
	
	double pom = 0.5/M_PI;
	*dGdx*= pom;
	*dGdy*= pom;
}

void gradh(double x, double y, double *dhdx, double *dhdy)
{
	gradG(x, y, dhdx, dhdy);
	*dhdx/= -T;
	*dhdy/= -T;
}

void v(double x, double y, double k, double *vx, double *vy)
{
	gradh(x,y,vx,vy);
    (*vx)*= -k; // tady by se zavedla anizotropie
    (*vy)*= -k;
}

void testovaci_input()
{
    N = 3;

    if(!logfile.is_open())
        logfile.open("log.txt",ios_base::ate);

    if(!logfile.is_open())
        cout << "Logfile is NOT open." << endl;

        // udaje o studnach:
    Q[0] =-.005; // m3/s Q<0 cerpani, Q>0 zasakovani
    Q[1] = .003; // m3/s
    r[0] = r[1] = .125; //m
    R[0] = 100;
    R[1] = 60; //m
    s[0] = .43;
    s[1] =-.2;

    L = 50.0; // m

    // udaje o vrstvach:
    z[0] =  0.0; // teren
    z[1] =  2.0;
    z[2] =  5.0;
    z[3] =  8.0;
    z[4] = 0.0;
    z[5] = 0.0; // podlozi

    K[0] = 3e-5; // m/s
    K[1] = 3e-3; // m/s
    K[2] = 3e-6; // m/s
    K[3] = 0; // m/s
    K[4] = 0; // m/s

    T = 0;
    for(int i = 0; i < N; i++)
    {
        d[i] = z[i+1] - z[i];
        T+= K[i]*d[i];
    }

    H = 4.0; // hloubka hpv

    // test logfile

    // tenhle kus kodu pripravi z-souradnice vrstevnich rozhrani - obrati osu, polozi z=0 na podlozi
    double Z_base = z[N];

    for(int i = 0; i < N+1; i++)
    {
        z[i] = Z_base - z[i];
        //logfile << "z[" << i << "] = " << z[i] << endl;
    }

    for(int i = 0; i < (N+1)/2; i++)
    {
        double pom;
        pom = z[i];
        z[i] = z[N-i];
        z[N-i] = pom;
        //logfile << "z[" << i << "] = " << z[i] << endl;
    }

    for(int i = 0; i < N/2; i++)
    {
        double pom;
        pom = K[i];
        K[i] = K[N-i-1];
        K[N-i-1] = pom;
        //logfile << "K[" << i << "] = " << K[i] << endl;
    }
    H = Z_base - H;
}

void logInput()
{
    if(!logfile.is_open())
        logfile.open("log.txt",ios_base::app);

    logfile << "I N P U T   D A T A - - - - - - - - - - - - - - - - - -" << endl << __DATE__ << " " <<__TIME__ << endl << endl;
    for(int i = 0; i < 2; i++)
    {
        logfile << "Studna c. " << i+1 << endl;
        logfile << "Q[" << i+1 <<"] = " << Q[i] << " m3/s" << endl;
        logfile << "s[" << i+1 <<"] = " << s[i] << " m" << endl;
        logfile << "r[" << i+1 <<"] = " << r[i] << " m" << endl;
        logfile << "R[" << i+1 <<"] = " << R[i] << " m" << endl << endl;
    }

    logfile << "Hydrogeologický profil (zdola nahoru, z=0 je podloží):" << endl;
    logfile << "Počet načtených vrstev: N = " << N << endl << endl;

    for(int i = N-1; i >= 0; i--)
    {
        logfile << "z[" << i+1 <<"] = " << z[i+1] << " m" << endl;
        logfile << "K[" << i+1 <<"] = " << K[i] << " m/s" << endl;
    }
    logfile << "z[0] = " << z[0] << " m" << endl;
    logfile << endl << "Puvodni/prirozena hpv: H = " << H << " m" << endl << "se nachazi ve vrstve c. " << getLayer(H)+1 << "." << endl;

    logfile << endl;
    logfile.flush();
}
