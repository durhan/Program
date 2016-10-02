#include "nactenidat.h"

//extern namespace Ui {}

nactenidat::nactenidat(MainWindow *ptr)
{ui = ptr;}

void nactenidat::refresh()
{



            // nacist data z gui do poli

            // hloubky rozhrani

                    z[0] = 0;
                    z[1] = ui->lineEdit->text().toDouble();
                    z[2] = ui->lineEdit_3->text().toDouble();
                    z[3] = ui->lineEdit_5->text().toDouble();
                    z[4] = ui->lineEdit_7->text().toDouble();
                    z[5] = ui->lineEdit_9->text().toDouble();

                    d[0] = z[1];
                    d[1] = z[2] - z[1];
                    d[2] = z[3] - z[2];
                    d[3] = z[4] - z[3];
                    d[4] = z[5] - z[4];


            // hydraulicke vodivosti
            K[0] = 0;
            K[1] = 0;
            K[2] = 0;
            K[3] = 0;
            K[4] = 0;

            // vydatnosti
            Q[0] = 0;
            Q[1] = 0;

            // polomery studni
            r[0] = 0;
            r[1] = 0;

            // polomery dosahu (odhad, nebo vnucena hodnota
            R[0] = 0;
            R[1] = 0;

            // snizeni
            s[0] = 0;
            s[1] = 0;

            // vzdalenost mezi studnami
            L = 0;

            // puvodni hydraulicka vyska (pred cerpanim - nebo hloubka hladiny podzemni vody? bylo by praktictejsi
            H = 0;

            // veci odvozene:

            // transmisivita kolektoru
            T = 0;
            for(int i=0; i < N; i++)
            {
                    T+= K[i]*d[i];
            }

            // tohle je moje interni vec, tuhle promennou budu kontrolovat, jestli je true, kdybych si nebyl jist, jestli uz je vsechno nacteno
            Initialized = true;
}


