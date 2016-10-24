#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QFileDialog>
#include <QVector>
#include <qmath.h>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_color_map.h>
#include <qwt_scale_widget.h>

#include<iostream>

QwtPlot *rez=NULL, *mapa=NULL, *grafProudnice=NULL, *grafTracer=NULL;

class SpectrogramData: public QwtRasterData
{
public:
    SpectrogramData(double xmin = 0.0, double xmax = 1.1, double ymin = 0.0, double ymax = 1.1, double zmin = 0.0, double zmax = 1.1)
    {
        setInterval( Qt::XAxis, QwtInterval( xmin, xmax ) );
        setInterval( Qt::YAxis, QwtInterval( ymin, ymax ) );
        setInterval( Qt::ZAxis, QwtInterval( zmin, zmax ) );
    }

    virtual double value( double x, double y ) const
    {
        return hydraulic_head(x,y);
    }
};

void printout(double x[], int dim);
void ExportPlot(QwtPlot *arg, const char *filename);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lineEdit->setValidator(new QDoubleValidator(this));
    ui->lineEdit_2->setValidator(new QDoubleValidator(this));
    ui->lineEdit_3->setValidator(new QDoubleValidator(this));
    ui->lineEdit_4->setValidator(new QDoubleValidator(this));
    ui->lineEdit_5->setValidator(new QDoubleValidator(this));
    ui->lineEdit_6->setValidator(new QDoubleValidator(this));
    ui->lineEdit_7->setValidator(new QDoubleValidator(this));
    ui->lineEdit_8->setValidator(new QDoubleValidator(this));
    ui->lineEdit_9->setValidator(new QDoubleValidator(this));
    ui->lineEdit_10->setValidator(new QDoubleValidator(this));
    ui->lineEdit_11->setValidator(new QDoubleValidator(this));
    ui->lineEdit_12->setValidator(new QDoubleValidator(this));
    ui->lineEdit_13->setValidator(new QDoubleValidator(this));
    ui->lineEdit_14->setValidator(new QDoubleValidator(this));
    ui->lineEdit_15->setValidator(new QDoubleValidator(this));
    ui->lineEdit_16->setValidator(new QDoubleValidator(this));
    ui->lineEdit_17->setValidator(new QDoubleValidator(this));
    ui->lineEdit_18->setValidator(new QDoubleValidator(this));
    ui->lineEdit_19->setValidator(new QDoubleValidator(this));
    ui->lineEdit_20->setValidator(new QDoubleValidator(this));
    ui->lineEdit_21->setValidator(new QDoubleValidator(this));
}

MainWindow::~MainWindow()
{
    logfile.close();
    delete rez;
    delete mapa;
    delete grafProudnice;
    delete grafTracer;
    delete ui;
}


void MainWindow::on_pushButton_2_clicked()
{
QMessageBox::critical(NULL,"problem",QString::number(N)); // tohle asi nahradi smysluplny obsah... nebo to pujde pryc i s tlacitkem
}

void MainWindow::on_pushButton_clicked()
{
    //testovaci_input();
    readLayers();

    QGraphicsScene * schema = new QGraphicsScene();

    for (int i=0;i<6;i++)
    {
        QGraphicsLineItem * hloubka = new QGraphicsLineItem();
        hloubka->setLine(0,z[i],100,z[i]);
        schema->addItem(hloubka);
    }
    ui->graphicsView->setScene(schema);
    ui->graphicsView->fitInView(schema->sceneRect());

    for (int i=1; i <= N; i++)
    {
        rez = new QwtPlot(ui->FrameHH);
        rez->setTitle("Řez hydraulické výšky v linii vrtů");
        rez->setCanvasBackground(Qt::white);
        rez->setAxisScale(QwtPlot::yLeft, z[0], z[N]);
        rez->setAxisScale(QwtPlot::xBottom, -L/2 , L*1.5);
        rez->insertLegend(new QwtLegend);

        QwtPlotCurve *vrstva = new QwtPlotCurve("vrstva");
        //krivka->setSamples( x, rezh, xDim);
        vrstva->setPen( Qt::red, 1 );
        vrstva->attach(rez);

        QwtPlotGrid *mrizka = new QwtPlotGrid();
        mrizka->attach(rez);

        rez->resize(ui->FrameHH->width(), ui->FrameHH->height());
        rez->show();
    }
    
}

void MainWindow::on_lineEdit_returnPressed()
{
 //   ui->lineEdit->
}

void MainWindow::readData()
{
    readLayers();
    readWells();
}

void MainWindow::on_lineEdit_editingFinished()
{
    if( !ui->lineEdit->text().isEmpty() && !ui->lineEdit_2->text().isEmpty())
    {
        ui->lineEdit_3->setEnabled(true);
        ui->lineEdit_4->setEnabled(true);
        if(N==0)
            N++;
    }
    else
    {
        ui->lineEdit_3->setEnabled(false);
        ui->lineEdit_4->setEnabled(false);
        if(N==1)
            N--;
    }
}

void MainWindow::on_lineEdit_3_editingFinished()
{
    if( !ui->lineEdit_3->text().isEmpty() && !ui->lineEdit_4->text().isEmpty())
    {
        ui->lineEdit_5->setEnabled(true);
        ui->lineEdit_6->setEnabled(true);
        if(N==1)
            N++;
    }
    else
    {
        ui->lineEdit_5->setEnabled(false);
        ui->lineEdit_6->setEnabled(false);
        if(N==2)
            N--;
    }
}

void MainWindow::on_lineEdit_5_editingFinished()
{
    if( !ui->lineEdit_5->text().isEmpty() && !ui->lineEdit_6->text().isEmpty())
    {
        ui->lineEdit_7->setEnabled(true);
        ui->lineEdit_8->setEnabled(true);
        if(N==2)
            N++;
    }
    else
    {
        ui->lineEdit_7->setEnabled(false);
        ui->lineEdit_8->setEnabled(false);
        if(N==3)
            N--;
    }
}

void MainWindow::on_lineEdit_7_editingFinished()
{
    if( !ui->lineEdit_7->text().isEmpty() && !ui->lineEdit_8->text().isEmpty())
    {
        ui->lineEdit_9->setEnabled(true);
        ui->lineEdit_10->setEnabled(true);
        if(N==3)
            N++;
    }
    else
    {
        ui->lineEdit_9->setEnabled(false);
        ui->lineEdit_10->setEnabled(false);
        if(N==4)
            N--;
    }
}

void MainWindow::on_lineEdit_9_editingFinished()
{
    if( !ui->lineEdit_9->text().isEmpty() && !ui->lineEdit_10->text().isEmpty())
    {
        N=5;
    }
    else
    {
        N=4;
    }
}

void MainWindow::on_pushButton_3_clicked() // TAB: REZ HYDRAULICKE VYSKY
{
    if(!readLayers() || !readWells()) //zpusob, jak nacist data a rovnou skoncit, kdyz nejsou nactena
        return;

    logInput();

    //if(rez != NULL)
    //    delete rez;

    // rozmery pole: x: 2*L, y: whatever
    int xDim = int(2*L);
    double x[xDim], rezh[xDim];

    // mrizka souradnic a funkcni hodnoty
    for(int i = 0; i < xDim; i++) {
        x[i] = i - L/2;
        if(isnan(x[i]))
        {
            logfile << "ERROR: REZ: x[i] = nan" << endl;
            logfile << "i    = " << i << endl;
            logfile << "xDim = " << xDim << endl;
            return;
        }
        rezh[i] = hydraulic_head(x[i],0.0);
    }

    // a ted to cele vynest:
    if(rez==NULL)
    {
        rez = new QwtPlot(ui->FrameHH);
        rez->setTitle("Řez hydraulické výšky v linii vrtů");
        rez->setCanvasBackground(Qt::white);
        rez->setAxisScale(QwtPlot::xBottom, -L/2 , 1.5*L);
        rez->setAxisTitle(QwtPlot::xBottom,"x[m]");
        rez->setAxisScale(QwtPlot::yLeft, H-1, H+1);
        rez->setAxisTitle(QwtPlot::yLeft,"h[m nad bází]");
        rez->insertLegend(new QwtLegend);
    }
    rez->detachItems(QwtPlotItem::Rtti_PlotCurve); // zbavi graf vsech...

    QwtPlotCurve *krivka = new QwtPlotCurve("hydraulická výška");
    krivka->setSamples( x, rezh, xDim);
    krivka->setPen( Qt::blue, 1 );
    krivka->attach(rez);

    QwtPlotGrid *mrizka = new QwtPlotGrid();
    mrizka->setPen(Qt::gray,1.0,Qt::DotLine);
    mrizka->attach(rez);

    rez->resize(ui->FrameHH->width(), ui->FrameHH->height());
    rez->replot();
    rez->show();
}

void MainWindow::on_lineEdit_9_cursorPositionChanged(int arg1, int arg2)
{
    int k = arg1+arg2; // just to shut up that compile time warning 'bout unused params
    k++;
    // what?
}
    
void printout(double x[], int dim) // vypis pole pro manualni debug
{
    for(int i = 0; i < dim; i++)
        logfile << "["<< i << "] = " << x[i] << endl;
}

void MainWindow::on_pushButton_4_clicked() // TAB: REZ HYDRAULICKE VYSKY, zmena mezi na svisle ose
{
    if(rez==NULL)
        return;

    double ymin = ui->hhymin->text().toDouble();
    double ymax = ui->hhymax->text().toDouble();

    if(ui->hhymin->text().isEmpty())
        ymin = z[0];

    if(ui->hhymax->text().isEmpty())
        ymax = z[N];

    rez->setAxisScale(QwtPlot::yLeft, ymin, ymax);
    rez->replot();
}

void MainWindow::on_pushButton_5_clicked() // TAB: REZ HYDRAUL. V., export
{
    ExportPlot(rez, "rez_hydraul_vyska.png");
}

void ExportPlot(QwtPlot *arg, const char *filename)
{
    //QPixmap qPix = QPixmap::grabWidget(arg);
    QPixmap qPix = arg->grab();

    if(qPix.isNull()){
        logfile << "Export: Failed to capture the plot '" << filename << "' for saving." << endl;
        return;
    }

    //QFileDialog *saving = new QFileDialog(NULL,"Kepsn", "c:/");


    //QFileDialog(ui,"Export řezu hydraulické výšky", new QString("C:/"), new QString(".png"));
    qPix.save(filename,"PNG",85);
}

void MainWindow::on_pushButton_6_clicked() // TAB: MAPA: vypocet
{
    if(!readLayers() || !readWells()) //zpusob, jak nacist data a rovnou skoncit, kdyz nejsou nactena
        return;

    logInput();

    if(mapa != NULL)
        delete mapa;

    mapa = new QwtPlot(ui->widgetMapa);

    double xmin = -.5*L; //interval delky 2L
    double xmax = 1.5*L;

    double ymin =-1.0*ui->widgetMapa->height()/ui->widgetMapa->width() * L;
    double ymax = 1.0*ui->widgetMapa->height()/ui->widgetMapa->width() * L;

    //cerr << ymin << endl;
    //cerr << ymax << endl;

    //polointeligentni urceni mezi z:
    double arr[3] = { H, hydraulic_head(r[0],0), hydraulic_head(L+r[1],0) };

    for(int pruchod = 0; pruchod < 3; pruchod++)
        for(int k = 0; k < 2; k++)
            if(arr[k] > arr[k+1])
            {
                double p = arr[k];
                arr[k] = arr[k+1];
                arr[k+1] = p;
            }

    double zmin = arr[0];
    double zmax = arr[2];

    SpectrogramData *obsah = new SpectrogramData();
    obsah->setInterval( Qt::XAxis, QwtInterval( xmin, xmax ) );
    obsah->setInterval( Qt::YAxis, QwtInterval( ymin, ymax ) );
    obsah->setInterval( Qt::ZAxis, QwtInterval( zmin, zmax ) );

    QwtPlotSpectrogram *graf = new QwtPlotSpectrogram("Hydraulická výška");
    graf->setRenderThreadCount(4);
    graf->setCachePolicy(QwtPlotRasterItem::PaintCache);
    graf->setDisplayMode(QwtPlotSpectrogram::ContourMode, false);
    graf->setDisplayMode(QwtPlotSpectrogram::ImageMode, true);
    graf->setData(obsah);
    graf->attach(mapa);

    QwtLinearColorMap *barvy = new QwtLinearColorMap(Qt::darkBlue, Qt::cyan, QwtColorMap::RGB);
    graf->setColorMap(barvy);

    //kontury ----------------------------------------------------------
    // a zvlast na kontury, aby exit neprotestoval...
    SpectrogramData *obsah2 = new SpectrogramData();
    obsah2->setInterval( Qt::XAxis, QwtInterval( xmin, xmax ) );
    obsah2->setInterval( Qt::YAxis, QwtInterval( ymin, ymax ) );
    obsah2->setInterval( Qt::ZAxis, QwtInterval( zmin, zmax ) );

    QwtPlotSpectrogram *kontury = new QwtPlotSpectrogram("Hydroizohypsy");
    kontury->setRenderThreadCount(4);
    kontury->setCachePolicy(QwtPlotRasterItem::PaintCache);
    kontury->setDisplayMode(QwtPlotSpectrogram::ContourMode,true);
    kontury->setDisplayMode(QwtPlotSpectrogram::ImageMode, false);

    QList<double> contourLevels;
    for ( double level = ceil(10*zmin)/10; level < zmax + .05; level += .1 ) // puvodni kousek, kdyby se zas hodil: (zmax-zmin)/10
        contourLevels += level;
    kontury->setContourLevels( contourLevels );
    kontury->setDefaultContourPen(QPen(Qt::NoPen)); //Qt::NoPen
    kontury->setData(obsah2);
    kontury->attach(mapa);

    QwtLinearColorMap *barvy_kontur = new QwtLinearColorMap(Qt::green, Qt::darkGreen, QwtColorMap::RGB);    //barvy->addColorStop(H-.05,Qt::darkGreen);
    kontury->setColorMap(barvy_kontur);

    // legenda -----------------------------------------------------------
    mapa->setAxisScale( QwtPlot::yRight, zmin, zmax);
    mapa->enableAxis(QwtPlot::yRight);
    QwtLinearColorMap *barvy2 = new QwtLinearColorMap(Qt::darkBlue, Qt::cyan, QwtColorMap::RGB); // argh! podruhe ta sama QwtColorMap, ptz kopirovaci konstruktor je private a pouzit znova prvni mapu crashlo Skvost pri ukoncovani!
    mapa->axisWidget(QwtPlot::yRight)->setColorMap(graf->data()->interval(Qt::ZAxis), barvy2);
    mapa->axisWidget(QwtPlot::yRight)->setColorBarEnabled(true);
    mapa->setAxisTitle(QwtPlot::yRight, "h [m]");

    // rozsah na osach ---------------------------------------------------
    mapa->setAxisScale(QwtPlot::xBottom, xmin, xmax);
    mapa->setAxisTitle(QwtPlot::xBottom, "x [m]");
    mapa->setAxisScale(QwtPlot::yLeft, ymin, ymax);
    mapa->setAxisTitle(QwtPlot::yLeft, "y [m]");

    mapa->resize(ui->widgetMapa->width(), ui->widgetMapa->height());
    mapa->show();
}

void MainWindow::on_pushButton_7_clicked() // TAB: MAPA: export
{
    ExportPlot(mapa, "mapa.png");
}

void MainWindow::on_pushButton_8_clicked() //dopočítáme snížení
{
    if(!readLayers() || !readWells()) //zpusob, jak nacist data a rovnou skoncit, kdyz nejsou nactena
        return;

  if ( ui->lineEdit_11->text().isEmpty() || ui->lineEdit_14->text().isEmpty())
  {
      QMessageBox::warning(NULL,"Chyba!","Zadejte vydatnost!");
      return;
  }

    s[0]=wellDrawdown(0);
    s[1]=wellDrawdown(1);

  ui->lineEdit_13->setText(QString::number(s[0]));
  ui->lineEdit_16->setText(QString::number(s[1]));

}

void MainWindow::on_startProudnice_clicked() // TAB: PROUDNICE A TRACKING: grafy
{
    if(!readLayers() || !readWells()) //zpusob, jak nacist data a rovnou skoncit, kdyz nejsou nactena
        return;

    double krok = .03;

    // zaznamenavam cele proudnice plus casy
    vector<double> X,Y,T;

    //ciste pole
    if(grafProudnice!=NULL)
        delete grafProudnice;

    if(grafTracer!=NULL)
        delete grafTracer;

    //zalozit oba grafy
    grafProudnice = new QwtPlot(ui->widgetProudnice);
    grafTracer = new QwtPlot(ui->widgetTracer);

    ofstream outfile("proudnice.txt",ios_base::app);
    int poradove_cislo=1;

    outfile << __TIMESTAMP__ << endl;

    double krokfi = 2.0*M_PI/26;

    double z0 = 0.0;
    double x[2], y[2], t; //startovni body

    for(int j = 0; j < 2; j++)
    {
        outfile << "Trajektorie ze studny c. " << j+1 << endl;

        for(double fi = 0.0; fi < 2.0*M_PI; fi+= krokfi)
        {
            x[j] = j*L + (r[j]+.01) * cos(fi+.001);
            y[j] = (r[j]+.01) * sin(fi+.001);

            X.clear();
            Y.clear();

            t = track_point(x[j], y[j], z0, sgn(Q[j]) * krok, &X, &Y); //

            if(X.size() == 0)
                continue;

            //zaznamenat "cestovni cas" trajektorie, pokud skoncila v nekterem vrtu
            double ro[] = {sqrt(pow(X[X.size()-1],2) + pow(Y[Y.size()-1],2)), sqrt(pow(X[X.size()-1]-L,2) + pow(Y[Y.size()-1],2))};
            for(int k = 0; k < 2; k++)
                if(ro[k] < r[k])
                    T.push_back(t); // sbirame casy z obou studni

            // kazdou smysl majici trajektorii zapiseme do souboru
            if(X.size() > 1)
            {
                outfile << "proudnice c. " << poradove_cislo++ << "\tdoba [s]: " << t << endl;
                for(unsigned int i = 0; i < X.size(); i++)
                {
                    outfile << X[i] << '\t' << Y[i] << endl;
                }
            } // if

            //vynest proudnici:
            QwtPlotCurve *proudnice = new QwtPlotCurve();
            proudnice->setSamples(QVector<double>::fromStdVector(X), QVector<double>::fromStdVector(Y));

            Qt::GlobalColor barva;
            if(Q[j] >= 0)
                barva = Qt::red;
            if(Q[j] < 0)
                barva = Qt::blue;

            proudnice->setPen(barva,1.0);
            proudnice->attach(grafProudnice);
        } // for fi
    } //for j

    double pom = 1.0*grafProudnice->parentWidget()->height()/grafProudnice->parentWidget()->width();
    grafProudnice->setAxisScale(QwtPlot::xBottom, -L, 2*L);
    grafProudnice->setAxisTitle(QwtPlot::xBottom,"x [m]");
    grafProudnice->setAxisScale(QwtPlot::yLeft, -L*1.5*pom, L*1.5*pom);
    grafProudnice->setAxisTitle(QwtPlot::yLeft,"y [m]");
    grafProudnice->resize(grafProudnice->parentWidget()->width(),grafProudnice->parentWidget()->height());

    grafProudnice->show();

    pom = 1.0*grafTracer->parentWidget()->height()/grafTracer->parentWidget()->width();
    grafTracer->setAxisScale(QwtPlot::xBottom, -L, L);
    grafTracer->setAxisScale(QwtPlot::yLeft, -L*pom, L*pom);

    grafTracer->show();

    outfile.close();
}

//-------------------------------------------------------------------

void MainWindow::on_pushButton_9_clicked() //dopočítáme vydatnost
{
    if(!readLayers() || !readWells()) //zpusob, jak nacist data a rovnou skoncit, kdyz nejsou nactena
        return;

  if (ui->lineEdit_20->text().isEmpty() || ui->lineEdit_21->text().isEmpty())
  {
      QMessageBox::warning(NULL,"Chyba!","Zadejte oba dosahy!");
              return;
  }

  if ( ui->lineEdit_11->text().isEmpty() && ui->lineEdit_14->text().isEmpty())
  {
      QMessageBox::warning(NULL,"Chyba!","Zadejte jednu vydatnost!");
      return;
  }

  if (ui->lineEdit_14->text().isEmpty() && ui->lineEdit_16->text().isEmpty())
  {
      QMessageBox::warning(NULL,"Chyba!","Zadejte snížení ve studni, ve které chcete znát vydatnost!");
      return;
  }

  if (ui->lineEdit_11->text().isEmpty() && ui->lineEdit_13->text().isEmpty())
  {
      QMessageBox::warning(NULL,"Chyba!","Zadejte snížení ve studni, ve které chcete znát vydatnost!");
      return;
  }

  if (ui->lineEdit_11->text().isEmpty())
  {
      Q[0]=wellYield(0);
      ui->lineEdit_11->setText(QString::number(Q[0]*1000));
  }

  if (ui->lineEdit_14->text().isEmpty())
  {
      Q[1]=wellYield(1);
      ui->lineEdit_14->setText(QString::number(Q[1]*1000));
  }
}

void MainWindow::on_proudExport_clicked()
{
    ExportPlot(grafProudnice,"proudnice.png");
}

void MainWindow::on_pushButton_10_clicked() // odhad dosahu depr kuzele
{
    // empty :(((
}

bool MainWindow::readLayers()
{
    // nacist data z gui do promennych
    N = readN();

    if(N==0)
    {
        QMessageBox::critical(NULL,"Chyba!", "Co takhle zadat alespoň nějaké údaje o vrstvách?");
        return false;
    }

    if(!logfile.is_open())
        logfile.open("log.txt",ios_base::app);

    QLocale loc(QLocale::system());

    // hloubky rozhrani a hydraulicke vodivosti vrstev
    for(int i = 0; i < 6; i++)
    {
        K[i] = 0;
        z[i] = 0;
    }

    z[0] = 0; // v tehle fazi teren
    z[1] = loc.toDouble(ui->lineEdit->text()); // musi byt zadano aspon tohle
    K[0] = loc.toDouble(ui->lineEdit_2->text());

    if(N > 1) {
        z[2] = loc.toDouble(ui->lineEdit_3->text());
        K[1] = loc.toDouble(ui->lineEdit_4->text()); }
    if(N > 2) {
        z[3] = loc.toDouble(ui->lineEdit_5->text());
        K[2] = loc.toDouble(ui->lineEdit_6->text()); }
    if(N > 3) {
        z[4] = loc.toDouble(ui->lineEdit_7->text());
        K[3] = loc.toDouble(ui->lineEdit_8->text()); }
    if(N > 4) {
        z[5] = loc.toDouble(ui->lineEdit_9->text());
        K[4] = loc.toDouble(ui->lineEdit_10->text()); }

    for(int i = 1; i < N; i++)
    {
        if(z[i] < .001)
        {
            QMessageBox::critical(NULL,"Chyba!","Nepodařilo se načíst všechny hodnoty z!");
            return false;
        }
        if(K[i] < 1e-14) // mensi K uz snad nikdo nezada
        {
            QMessageBox::critical(NULL,"Chyba!","Nepodařilo se načíst všechny hodnoty K!");
            return false;
        }
    }

    // vzdalenost mezi studnami
    L = loc.toDouble(ui->lineEdit_17->text());

    if(L < .001)
    {
        QMessageBox::critical(NULL,"Chyba!","Nepodařilo se načíst hodnotu L!");
        return false;
    }
    // veci odvozene:

    // tenhle kus kodu pripravi z-souradnice vrstevnich rozhrani - obrati osu, polozi z=0 na podlozi
    double Z_base = z[N];

    for(int i = 0; i < N+1; i++)
    {
        logfile << "readLayers: z[" << i << "] = " << z[i] << "\t";
        z[i] = Z_base - z[i];
        logfile << z[i] << endl;
    }

    for(int i = 0; i < (N+1)/2; i++)
    {
        double pom, pom2;
        pom = z[i];
        z[i] = z[N-i];
        z[N-i] = pom;

        pom2 = K[i];
        K[i] = K[N-i];
        K[N-i]= pom2;
    }

    // puvodni hydraulicka vyska (pred cerpanim - nebo hloubka hladiny podzemni vody? bylo by praktictejsi
    H = loc.toDouble(ui->lineEdit_18->text());
    if(H < .000001)
    {
        QMessageBox::warning(NULL,"Varování!","Hodnota hladiny před čerpáním nebyla zadána, nebo je záporná.");
        //return false;
    }

    H = Z_base - H;

    // mocnosti vrstev
    d[0] = z[1];
    d[1] = z[2] - z[1];
    d[2] = z[3] - z[2];
    d[3] = z[4] - z[3];
    d[4] = z[5] - z[4];

    int n = getLayer(H);

    // pripad hodnoty H nad terenem / pod bazi - predpokladame napjate, tj. pocitame transmisivitu pres vsechny vrstvy:
    if(n==-1)
        n = N;

    // transmisivita kolektoru
    T = 0;
    for(int i=0; i < n; i++)
        T+= K[i]*d[i];

    double dT = K[n]*(H - z[n]);
    if(dT < 0)
    {
        logfile << "CHYBA při počítání transmisivity! dT = " << dT << endl;
    }
    T+=dT;

    return true;
}

bool MainWindow::readWells()
{
    QLocale loc(QLocale::system());

    // vydatnosti - do form se zadavaji l/s
    Q[0] = loc.toDouble(ui->lineEdit_11->text())/1000; // s prevodem na m3/s
    Q[1] = loc.toDouble(ui->lineEdit_14->text())/1000;

    if(ui->lineEdit_11->text().isEmpty() && ui->lineEdit_14->text().isEmpty())
    {
        QMessageBox::critical(NULL,"Chyba!","Musí být zadaná alespoň jedna vydatnost (druhá se dá spočítat, pokud pro ni zadáte snížení).");
        return false;
    }

    // polomery studni - do form se zadavaji PRUMERY V mm
    r[0] = loc.toDouble(ui->lineEdit_12->text())/2/1000;
    r[1] = loc.toDouble(ui->lineEdit_15->text())/2/1000;

    if((r[0] < .001) || (r[1] < .001))
    {
        QMessageBox::critical(NULL,"Chyba!","Musejí být zadané průměry obou studní.");
        return false;
    }

    // polomery dosahu (odhad, nebo vnucena hodnota]
    R[0] = loc.toDouble(ui->lineEdit_20->text());
    R[1] = loc.toDouble(ui->lineEdit_21->text());

    if(ui->lineEdit_20->text().isEmpty() || ui->lineEdit_21->text().isEmpty())
    {
        QMessageBox::warning(NULL,"Varování", "Nebyly zadány oba poloměry dosahu. Lze je spočítat z hodnot vydatností a snížení.");
    }

    // snizeni
    s[0] = loc.toDouble(ui->lineEdit_13->text());
    s[1] = loc.toDouble(ui->lineEdit_16->text());

    if(ui->lineEdit_13->text().isEmpty() || ui->lineEdit_16->text().isEmpty())
    {
        QMessageBox::warning(NULL,"Varování", "Nebyly zadána snížení v obou studnách. Lze je spočítat z hodnot vydatností a poloměrů dosahu.");
    }

    return true;
}

int MainWindow::readN()
{
    int n = 0;

    if(!ui->lineEdit->text().isEmpty() && !ui->lineEdit_2->text().isEmpty() && ui->lineEdit_2->isEnabled())
        n++;

    if(!ui->lineEdit_3->text().isEmpty() && !ui->lineEdit_4->text().isEmpty() && ui->lineEdit_4->isEnabled())
        n++;

    if(!ui->lineEdit_5->text().isEmpty() && !ui->lineEdit_6->text().isEmpty() && ui->lineEdit_6->isEnabled())
        n++;

    if(!ui->lineEdit_7->text().isEmpty() && !ui->lineEdit_8->text().isEmpty() && ui->lineEdit_8->isEnabled())
        n++;

    if(!ui->lineEdit_9->text().isEmpty() && !ui->lineEdit_10->text().isEmpty() && ui->lineEdit_10->isEnabled())
        n++;

    return n;
}


