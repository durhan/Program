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
#include <qwt_plot_histogram.h>
#include <qwt_samples.h>
#include <qwt_plot_barchart.h>
#include <QColor>
#include <QPen>

#include<iostream>

QwtPlot *rez=NULL, *mapa=NULL, *grafProudnice=NULL, *grafTracer=NULL, *prurez=NULL;
vector<double> casy,delky;
vector<double> newT;

double zet[6], Ha;
double Z_base;

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
    ui->lineEdit_19->setValidator(new QIntValidator(1,99999,this));
    ui->lineEdit_20->setValidator(new QDoubleValidator(this));
    ui->lineEdit_21->setValidator(new QDoubleValidator(this));
    ui->lineEdit_a->setValidator(new QDoubleValidator(0,10,1000,this));
    ui->lineEdit_maxt->setValidator(new QDoubleValidator(1e-6,1e40,1000,this));
    ui->lineEdit_pkat->setValidator(new QIntValidator(2,99999,this));
    ui->lineEdit_starty->setValidator(new QIntValidator(1,1000,this));
    ui->lineEdit_n->setValidator(new QDoubleValidator(0,1,2,this));
    ui->pushButton_9->setVisible(false);
    ui->pushButton_10->setVisible(false);
    ui->pushButton_9->setEnabled(false);
    ui->pushButton_10->setEnabled(false);
    ui->pushButton_2->setEnabled(false);

    // vyplnit vychozi hodnoty (jen ty, kt. nejsou celociselne; kvuliva desetinne carce)

    QLocale loc(QLocale::system());

    ui->lineEdit_a->setText(loc.toString(0.1,'g',1));
    ui->lineEdit_n->setText(loc.toString(0.2,'g',1));
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
    QLocale loc(QLocale::system());
/*
    s[0] = wellDrawdown(0);
    s[1] = wellDrawdown(1);
*/
    ui->lineEdit_13->setText(loc.toString(s[0],'f',2));
    ui->lineEdit_16->setText(loc.toString(s[1],'f',2));
}

void MainWindow::on_pushButton_clicked() // HYDROGEOLOGICKE SCHEMA
{
    //testovaci_input();
    if(!readLayers())
    {
        cerr << "HG schema: !readLayers() == true" << endl;
        return;
    }

    // musime si sami nacist polomery, jinak vazne neni co vynaset:
    // lineedit 12 a 15

    QLocale loc(QLocale::system());

    bool ok = true;

    r[0] = loc.toDouble(ui->lineEdit_12->text(), &ok)/1000/2;

    if(!ok)
        r[0] = .1;

    r[1] = loc.toDouble(ui->lineEdit_15->text(), &ok)/1000/2;

    if(!ok)
        r[1] = .1;

    readWells();

    if(ui->lineEdit_17->text().isEmpty())
    {
        cout << "L = 0" << endl;
        L = 0;
    }

    if(ui->lineEdit_18->text().isEmpty())
    {
        cout << "H = 0" << endl;
        H = 0;
    }

    //cout << "L = " << L << " m" << endl << "H = " << H << " m" << endl;
    //-------udelame to vse jako histogram

    if(rez == NULL) {
        rez = new QwtPlot(ui->FrameHH);
        rez->setTitle("Řez hydraulické výšky v linii vrtů");
        rez->setCanvasBackground(Qt::white);
        rez->setAxisTitle(QwtPlot::xBottom,"x [m]");

        rez->setAxisTitle(QwtPlot::yLeft,"h [m p. t.]");
        //rez->insertLegend(new QwtLegend);

        QwtPlotGrid *mrizka = new QwtPlotGrid();
        mrizka->setPen(Qt::gray,1.0,Qt::DotLine);
        mrizka->attach(rez);
    }

    rez->detachItems(QwtPlotItem::Rtti_PlotHistogram); // zbavi graf vsech...
    rez->setAxisScale(QwtPlot::xBottom, -L/2 , 1.5*L);
    rez->setAxisScale(QwtPlot::yLeft, zet[N], zet[0]);

   QVector <QwtIntervalSample> X,Y,S1,S2;

//--------geol rez
   for (int i=0; i<N; i++)
   {
   X.push_back(QwtIntervalSample((1.5*L)+5,zet[i],zet[i+1]));

   QwtPlotHistogram *vrstva = new QwtPlotHistogram("vrstva");
   vrstva->setSamples(X);
   vrstva->setBaseline((-L/2)-5);
   vrstva->setOrientation(Qt::Orientation(1));
   vrstva->setPen(QColor(139,69,19,255),5);
   vrstva->attach(rez);
   }

//----------pridam hladinu
   Y.push_back(QwtIntervalSample((1.5*L)+5,-0.1,Ha));

   QwtPlotHistogram *hladina = new QwtPlotHistogram ("hladina");
   hladina->setSamples(Y);
   hladina->setBaseline((-L/2)-5);
   hladina->setOrientation(Qt::Orientation(1));
   hladina->setPen(QColor(64,224,208),2,Qt::DashLine);
   hladina->attach(rez);

//-----pridam studny
   //cerr << "zet[N] = " << zet[N] << endl;

   S1.push_back(QwtIntervalSample(zet[N],0-r[0],0+r[0]));
   S2.push_back(QwtIntervalSample(zet[N],L-r[1],L+r[1]));

   QwtPlotHistogram *studna1 = new QwtPlotHistogram ("studna 1.");
   QwtPlotHistogram *studna2 = new QwtPlotHistogram ("studna 2.");
   studna1->setSamples(S1);
   studna2->setSamples(S2);
   studna1->attach(rez);
   studna2->attach(rez);
/*

*/
   rez->resize(ui->FrameHH->width(),ui->FrameHH->height());
   rez->replot();
   rez->show();

   ui->tabWidget->setCurrentIndex(2);
}

void MainWindow::on_lineEdit_returnPressed()
{
 //   ui->lineEdit->
}

void MainWindow::readData()
{
    readLayers();
    readWells();

    QMessageBox::warning(NULL,"XXX",ui->label_10->text());
}

/* abandoned functions

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

*/

void MainWindow::on_pushButton_3_clicked() // TAB: REZ HYDRAULICKE VYSKY
{
    if(!readLayers() || !readWells()) //zpusob, jak nacist data a rovnou skoncit, kdyz nejsou nactena
        return;

    on_pushButton_clicked(); //experimentalni

    logInput();

    //if(rez != NULL)
    //    delete rez;

    // rozmery pole: x: 2*L, y: whatever
    //int xDim = int(4*L)+1+4; // 4 body navic na plastich studen
    vector<double> x;

    // mrizka souradnic a funkcni hodnoty 
    for(double xx = -L/2; xx <= 1.5*L; xx+= .25)
        x.push_back(xx);

    x.push_back( -r[0]);
    x.push_back(  r[0]);
    x.push_back(L-r[1]);
    x.push_back(L+r[1]);

    std::sort(x.begin(),x.end());

    vector<double> rezh;

    for(unsigned int i = 0; i < x.size(); i++) {
        rezh.push_back(zet[N] - hydraulic_head(x[i],0.0));
    }

    // a ted to cele vynest:
    if(rez==NULL)
    {
        rez = new QwtPlot(ui->FrameHH);
        rez->setTitle("Řez hydraulické výšky v linii vrtů");
        rez->setCanvasBackground(Qt::white);
        // set axis scale nesmi byt podminene novym grafem - jinak to ignoruje zmenu L ve vstupnich datech
        rez->setAxisTitle(QwtPlot::xBottom,"x [m]");
        rez->setAxisTitle(QwtPlot::yLeft,"h [m p. t.]");
        rez->insertLegend(new QwtLegend);

        QwtPlotGrid *mrizka = new QwtPlotGrid();
        mrizka->setPen(Qt::gray,1.0,Qt::DotLine);
        mrizka->attach(rez);
    }
    rez->detachItems(QwtPlotItem::Rtti_PlotCurve); // zbavi graf vsech...
    rez->setAxisScale(QwtPlot::xBottom, -L/2 , 1.5*L);
    rez->setAxisScale(QwtPlot::yLeft, zet[N], zet[0]);

    QwtPlotCurve *krivka = new QwtPlotCurve("hydraulická výška");
    krivka->setSamples( QVector<double>::fromStdVector(x), QVector<double>::fromStdVector(rezh));
    krivka->setPen( Qt::blue, 1 );
    krivka->attach(rez);

    rez->resize(ui->FrameHH->width(), ui->FrameHH->height());
    rez->replot();
    rez->show();

  /*if ( ui->lineEdit_11->text().isEmpty() || ui->lineEdit_14->text().isEmpty())
  {
      QMessageBox::warning(NULL,"Chyba!","Zadejte obě vydatnosti!");
      return;
  }
  */

    // spocitat a zapsat snizeni:
    s[0]=wellDrawdown(0);
    s[1]=wellDrawdown(1);

    QLocale loc(QLocale::system());

    ui->label_43->setText(loc.toString(s[0],'f',2));
    ui->label_52->setText(loc.toString(s[1],'f',2));

    ui->pushButton_2->setEnabled(true);

    // spocitat a zapsat maximum/minimum hydraulicke vysky...

    if(sgn(Q[0]) != sgn(Q[1])) // studny nejedou ve stejnem rezimu (cerpani/nalev/neaktivni)
        return;

    if((sgn(Q[0]) == 0) || (sgn(Q[1]) == 0)) // jedna ze studni neni aktivni
        return;

    double x0 = extrem();
    double hydv = zet[N] - hydraulic_head(x0,0);

    ui->label_x->setText(loc.toString(x0,'f',1));
    ui->label_hydv->setText(loc.toString(hydv,'f',2));
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

    QLocale loc(QLocale::system());

    double dolni = loc.toDouble(ui->hhymin->text());
    double horni = loc.toDouble(ui->hhymax->text());

    if(ui->hhymin->text().isEmpty())
        dolni = zet[N];

    if(ui->hhymax->text().isEmpty())
        horni = zet[0];

    if(dolni == horni)
    {
        QMessageBox::warning(NULL,"Varování","Zadaná dolní mez je stejná jako horní.");
        return;
    }

    if(dolni < horni) // horni ma mensi hloubku
    {
        QMessageBox::warning(NULL,"Varování","Zadaná dolní mez je NAD zadanou horní mezí. Zaměňuji meze.");
        double pom = dolni;
        dolni = horni;
        horni = pom;

        QString str = ui->hhymin->text();
        ui->hhymin->setText(ui->hhymax->text());
        ui->hhymax->setText(str);
    }

    rez->setAxisScale(QwtPlot::yLeft, dolni, horni);
    rez->replot();
}

void MainWindow::on_pushButton_5_clicked() // TAB: REZ HYDRAUL. V., export
{
    ExportPlot(rez, "rez_hydraul_vyska.png");
}

void ExportPlot(QwtPlot *arg, const char *filename)
{
    //QPixmap qPix = QPixmap::grabWidget(arg);
    if(arg == NULL)
        return;

    QPixmap qPix = arg->grab();

    if(qPix.isNull()){
        if(!logfile.is_open())
            logfile.open("log.txt",ios_base::app);
        logfile << "ExportPlot: Failed to capture the plot '" << filename << "' for saving." << endl;
        return;
    }

    //QFileDialog *saving = new QFileDialog(NULL,"Kepsn", "c:/");
    QString fname = QFileDialog::getSaveFileName(NULL,"Uložit jako",".","*.png");


    //QFileDialog(ui,"Export řezu hydraulické výšky", new QString("C:/"), new QString(".png"));
    if(!fname.isEmpty())
        qPix.save(fname,"PNG",85);
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

    //polointeligentni urceni mezi z:
    double arr[3] = { H, hydraulic_head(r[0],0), hydraulic_head(L+r[1],0) };

//    cout << "H = " << arr[0] << "\t" << "h[0] = " << arr[1] << "\t" << "h[1] = " << arr[2] << endl;
    std::sort(arr,arr+3);

    double zmin = arr[0];
    double zmax = arr[2];

//    cout << "zmin = " << zmin << endl << "zmax = " << zmax << endl;
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

    //barvy
    QwtLinearColorMap *barvy = new QwtLinearColorMap(Qt::darkBlue, Qt::cyan, QwtColorMap::RGB);
    //barvy->addColorStop(H-.05,Qt::darkGreen);
    //barvy->addColorStop(H+.05,Qt::darkGreen);
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
    for ( double level = ceil(10*zmin)/10; level < zmax + .05; level += .05 ) // puvodni kousek, kdyby se zas hodil: (zmax-zmin)/10
        contourLevels += level;
    kontury->setContourLevels( contourLevels );
    kontury->setDefaultContourPen(QPen(Qt::NoPen)); //Qt::NoPen
    kontury->setData(obsah2);
    kontury->attach(mapa);

    QwtLinearColorMap *barvy_kontur = new QwtLinearColorMap(Qt::green, Qt::darkGreen, QwtColorMap::RGB);    //barvy->addColorStop(H-.05,Qt::darkGreen);
    kontury->setColorMap(barvy_kontur);

    // legenda -----------------------------------------------------------
    //_
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
    mapa->repaint();
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
/*
  if ( ui->lineEdit_11->text().isEmpty() || ui->lineEdit_14->text().isEmpty())
  {
      QMessageBox::warning(NULL,"Chyba!","Zadejte obě vydatnosti!");
      return;
  }*/
    s[0]=wellDrawdown(0);
    s[1]=wellDrawdown(1);

    QLocale loc(QLocale::system());

    ui->lineEdit_13->setText(loc.toString(s[0],'f',2));
    ui->lineEdit_16->setText(loc.toString(s[1],'f',2));

}

void MainWindow::on_startProudnice_clicked() // TAB: PROUDNICE A TRACKING: grafy
{

    ui->startProudnice->setText("Čekejte...");
    this->repaint();

    if(!readLayers() || !readWells()) //zpusob, jak nacist data a rovnou skoncit, kdyz nejsou nactena
        return;

    logInput();

    casy.clear();
    delky.clear();

    double krok = .01 * ui->lineEdit_19->text().toInt();

    // zaznamenavam cele proudnice plus casy
    vector<double> X,Y;
    casy.reserve(3000);

    //ciste pole
    if(grafProudnice!=NULL)
        delete grafProudnice;

//    if(grafTracer!=NULL)
//        delete grafTracer;

    grafProudnice = new QwtPlot(ui->widgetProudnice);
    //grafTracer = new QwtPlot(ui->widgetTracer);

    ofstream outfile("proudnice.txt",ios_base::out);
    int pocet_proudnic = 0;
    int pocet_dorazivsich = 0;
    int cislo_proudnice = 0;

    outfile << __DATE__ << " " << __TIME__ << endl;
    QLocale loc(QLocale::system());

    bool ok = false;
    int pocet_startovnich_bodu = loc.toInt(ui->lineEdit_starty->text(), &ok);

    if(ui->lineEdit_starty->text().isEmpty() || !ok)
        pocet_startovnich_bodu = 47;

    double krokfi = 2.0*M_PI/pocet_startovnich_bodu;

    double z0 = 0.0; // pocitam jen pro prvni vrstvu
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
            EndPoint konec = other;

            t = track_point(x[j], y[j], z0, sgn(Q[j]) * krok, &X, &Y, &konec); //

            if(X.size() == 0)
                continue;

            //zaznamenat "cestovni cas" trajektorie, pokud skoncila v nekterem vrtu
            if ((Q[j] > 0) && (Q[1-j] < 0)) { // sbirame casy jen ze zdrojove studny a jen pokud ta druha je nor
                pocet_proudnic++;
                if(konec == well) {
                    //casy.push_back(t/3600/24); //rovnou prevod na dny
                    casy.push_back(t); // zadny prevod na dny
                    delky.push_back(X.size()*krok);
                    pocet_dorazivsich++;
                }
            }

            // kazdou smysl majici trajektorii zapiseme do souboru
            if(X.size() > 1)
            {
                outfile << "proudnice c. " << ++cislo_proudnice << "\tdoba [s]: " << t << endl;
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
        } // for i
    } //for j

    double pom = 1.0*grafProudnice->parentWidget()->height()/grafProudnice->parentWidget()->width();
    grafProudnice->setAxisScale(QwtPlot::xBottom, -L, 2*L);
    grafProudnice->setAxisTitle(QwtPlot::xBottom,"x [m]");
    grafProudnice->setAxisScale(QwtPlot::yLeft, -L*1.5*pom, L*1.5*pom);
    grafProudnice->setAxisTitle(QwtPlot::yLeft,"y [m]");
    grafProudnice->resize(grafProudnice->parentWidget()->width(),grafProudnice->parentWidget()->height());
    QwtPlotGrid *mrizka = new QwtPlotGrid;
    mrizka->setPen(Qt::gray,1.0,Qt::DotLine);
    mrizka->attach(grafProudnice);
    grafProudnice->show();                                            

    outfile.close();

    if((sgn(Q[0]) != sgn(Q[1])) && (sgn(Q[0]) != 0) && (sgn(Q[1]) != 0))
        ui->label_navratnost->setText(QString::number(100.0*pocet_dorazivsich/pocet_proudnic,'f',1)+" %");
        //ui->label_navratnost->setText(QString::number(pocet_dorazivsich)+"/"+QString::number(pocet_proudnic));
    else
        ui->label_navratnost->setText("%");


    ui->startProudnice->setText("Zobrazit proudnice");
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
    // upraveny Kusakinuv vzorec (je empiricky a pro zvoden s volnou hladinou)
    if(!readLayers())
    {
        QMessageBox::critical(NULL,"Chyba","Nepodařilo se načíst údaje o vrstvách.");
        return;
    }

    if(ui->lineEdit_13->text().isEmpty())
    {
        s[0] = wellDrawdown(0);
        ui->lineEdit_13->setText(QString::number(s[0],'g',2));
    }
    if(ui->lineEdit_16->text().isEmpty())
    {
        s[1] = wellDrawdown(1);
        ui->lineEdit_16->setText(QString::number(s[0],'g',2));
    }

    R[0] = 575 * fabs(s[0]) * sqrt(T);
    R[1] = 575 * fabs(s[1]) * sqrt(T);

    ui->lineEdit_20->setText(QString::number(R[0],'f',1));
    ui->lineEdit_21->setText(QString::number(R[1],'f',1));
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
    for(int i = 0; i < 5; i++)
    {
        K[i] = 0;
        z[i] = zet[i] = 0;
    }
    z[5] = zet[5] = 0;


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

    for(int i = 0; i < 6; i++)
        zet[i] = z[i];

    // vzdalenost mezi studnami
    L = loc.toDouble(ui->lineEdit_17->text());

    if(L < .001)
    {
        QMessageBox::critical(NULL,"Chyba!","Nepodařilo se načíst hodnotu L!");
        return false;
    }

    // puvodni hydraulicka vyska (pred cerpanim - nebo hloubka hladiny podzemni vody? bylo by praktictejsi
    H = Ha = loc.toDouble(ui->lineEdit_18->text());
    if(H < .000001)
    {
        QMessageBox::warning(NULL,"Varování!","Hodnota hladiny před čerpáním nebyla zadána, nebo je záporná.");
        //return false;
    }
    //cerr << H << endl << z[0] << endl << z[N] << endl;

    H = z[N] - H;

    int n = getLayer(H);

    // pripad hodnoty H nad terenem / pod bazi - predpokladame napjate, tj. pocitame transmisivitu pres vsechny vrstvy:
    if(n==-1)
        n = N;

    if(H < z[0])
    {
        QMessageBox::critical(NULL,"Chyba","Zadaná hladina p. v. je pod bází kolektoru!");
        return false;
    }


    // veci odvozene:

    // tenhle kus kodu pripravi z-souradnice vrstevnich rozhrani - obrati osu, polozi z=0 na podlozi
    Z_base = z[N];

    for(int i = 0; i < N+1; i++)
    {
        logfile << "readLayers: z[" << i << "] = " << z[i] << "\t";
        z[i] = Z_base - z[i];
        logfile << z[i] << endl;
    }

    for(int i = 0; i < (N+1)/2; i++)
    {
        double pom;
        pom = z[i];
        z[i] = z[N-i];
        z[N-i] = pom;
    }

    for(int i = 0; i < N/2; i++) // protoze pole K je o prvek kratsi nez pole z - prohazovani jednim vrzem neni cesta
    {
        double pom;
        pom = K[i];
        K[i] = K[N-i-1];
        K[N-i-1]= pom;
    }


    // mocnosti vrstev
    d[0] = z[1];
    d[1] = z[2] - z[1];
    d[2] = z[3] - z[2];
    d[3] = z[4] - z[3];
    d[4] = z[5] - z[4];

    // transmisivita kolektoru

    T = 0;
    for(int i=0; i <= n; i++)
        T+= K[i]*d[i];

    double dT = K[n]*(z[n+1] - H);

    if(dT < 0)
    {
        logfile << "CHYBA při počítání transmisivity! dT = " << dT << endl;
    }
    T-=dT;

    ui->label_transmisivita->setText(loc.toString(T*24*3600)); // prevod na m2/d

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
        QMessageBox::warning(NULL,"Varování!","Pro odhad snížení by měla být zadaná vydatnost.");
        //return false;
    }

    // polomery studni - do form se zadavaji PRUMERY V mm
    r[0] = loc.toDouble(ui->lineEdit_12->text())/2/1000;
    r[1] = loc.toDouble(ui->lineEdit_15->text())/2/1000;

    if((r[0] < .001) || (r[1] < .001))
    {
        QMessageBox::warning(NULL,"Chyba!","Musejí být zadané průměry obou studní. Dosazuji výchozí hodnotu 200 mm.");
        //return false;
        r[0] = r[1] = .1; //m
        ui->lineEdit_12->setText("200");
        ui->lineEdit_15->setText("200");
    }

    // polomery dosahu (odhad, nebo vnucena hodnota]
    R[0] = loc.toDouble(ui->lineEdit_20->text());
    R[1] = loc.toDouble(ui->lineEdit_21->text());

    if(ui->lineEdit_20->text().isEmpty() || ui->lineEdit_21->text().isEmpty())
    {
        QMessageBox::warning(NULL,"Varování", "Nebyly zadány poloměry dosahu depresních/elevačních kuželů! Dosazuji výchozí hodnotu 200 m.");
        //return false;
        R[0] = R[1] = 200; //m
        ui->lineEdit_20->setText("200");
        ui->lineEdit_21->setText("200");
    }

    // snizeni
    s[0] = loc.toDouble(ui->lineEdit_13->text());
    s[1] = loc.toDouble(ui->lineEdit_16->text());

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



void MainWindow::on_pushButton_11_clicked() // GRAF: STOPOVACI ZKOUSKA
{


    if((sgn(Q[0]) != -sgn(Q[1])) || ((Q[0] == 0) && (Q[1] == 0)))
    {
        QMessageBox::critical(NULL,"Chyba","Tuto akci lze provést, jen pokud je jeden z vrtů čerpaný a druhý nalévaný.");
        return;
    }

    if(casy.size() == 0)
    {
        // zadne proudnice nemame
        QMessageBox::critical(NULL,"Chyba","Nejsou spočítané žádné proudnice. Klikněte na tlačítko \"Zobrazit proudnice\".");
        return;
    }

    // spocitat hodnoty do grafu pro tracer test:


    bool ok = false;

    QLocale loc(QLocale::system());
    double a = loc.toDouble(ui->lineEdit_a->text(), &ok); // hodnota podelne disperzivity [m] ... asi bych ji mel skalovat podle L, nebo jeste lip delky te ktere proudnice

    if(ui->lineEdit_a->text().isEmpty() || !ok)
        a = .1;

    double porovitost = loc.toDouble(ui->lineEdit_n->text(), &ok);

    if(ui->lineEdit_n->text().isEmpty() || !ok)
        porovitost = .2;

    newT.clear();
    newT.reserve(casy.size() * 9 * fabs(z[0] - z[N])*10 + 1); // *9; +1 kvuli std::sort

    for(unsigned int i = 0; i < casy.size(); i++)
    {

    // rozpocitat casy podle hodnoty koef. hydrodyn. disperze

    // budeme to povazovat za 1D
    // rychlost proudeni berem prumernou (z celkoveho casu a celkove drahy)
    // normalizovane normalni rozdeleni:
    // - po 1 castici na +/- 2*sigma
    // - po 2 casticich na +/- 1*sigma
    // - 3 castice na 0*sigma, tj. T[i]
    // ... takze v intervalu < -sigma; sigma> je 2+3+2 = 7 bodu z 9 (ma odpovidat 68%)

    //  ( N E ) F U N G U J E
        //double D = a * delky[i]/casy[i]; // m^2/d
        //double sigma = sqrt(4*D*casy[i]); // v metrech
        double sigma = sqrt(4 * a * delky[i]);
        //cout << i << "\t" << casy[i] << "\t" << delky[i] << "\t" << sigma << endl;
        double kobrfaktor = 1; // very unproudly
        porovitost*= kobrfaktor;

        newT.push_back(casy[i] * porovitost * delky[i] / (delky[i] + 2*sigma));
        newT.push_back(casy[i] * porovitost * delky[i] / (delky[i] + sigma));
        newT.push_back(casy[i] * porovitost * delky[i] / (delky[i] + sigma));
        newT.push_back(casy[i] * porovitost);

        newT.push_back(casy[i] * porovitost);

        newT.push_back(casy[i] * porovitost);
        newT.push_back(casy[i] * porovitost * delky[i] / (delky[i] - sigma));
        newT.push_back(casy[i] * porovitost * delky[i] / (delky[i] - sigma));
        newT.push_back(casy[i] * porovitost * delky[i] / (delky[i] - 2*sigma));
    } // for casy[i]

//*
    // spocitat casy pro ostatni hloubky (prepocet na jine K)
    // opakovane projizdime puvodne spocitane casy (pro z = 0)
    unsigned int pocet_casu = newT.size();
    for(double z = 0.1; z < H; z+=.1) // protoze pro z=0 uz to mame v T
    { // spis nez do H by bylo lepsi do max(h)
        double k = getK(z);
        for(unsigned int i = 0; i < pocet_casu; i++)
            newT.push_back(newT[i]*K[0]/k);
    }

    // setridit
//    cout << "size pred tridenim: " << newT.size() << endl;
    std::sort(newT.begin(),newT.end());
//    cout << "size po trideni: " << newT.size() << endl;

    // prevod na dny
    for(unsigned int i = 0; i < newT.size(); i++)
        newT[i]/= 24 * 3600;

    // trim:
    double max_T = loc.toDouble(ui->lineEdit_maxt->text(), &ok);// * 24 * 3600;
//    cout << "max_T pred = " << max_T << endl;

    if(ui->lineEdit_maxt->text().isEmpty() || !ok)
        max_T = newT[newT.size()-1]; // nic neuseknem

//    cout << "max_T po   = " << max_T << endl;

    unsigned int old_size = newT.size();

//    cout << "old_size = " << old_size << endl;

    int popped = 0;
    while(newT[newT.size()-1] > max_T) {
        popped++;
        newT.pop_back();
    }

//    cout << "popped = " << popped << endl;

    if(newT.size() == 0)
    {
        QMessageBox::critical(NULL, "Chyba", "Maximální čas pro graf je příliš malý - zvětšete ho.");
        return;
    }

//    cout << "new size = " << newT.size() << endl;

    // spocitat histogram:

    int pocet_kategorii = loc.toInt(ui->lineEdit_pkat->text(), &ok);
//    cout << "pocet_kategorii = " << pocet_kategorii << endl;

    if(ui->lineEdit_pkat->text().isEmpty() || !ok)
        pocet_kategorii = 60;

//    cout << "pocet_after     = " << pocet_kategorii << endl;

//    for(int i = 0; i < newT[newT.size()-1]; i++)
//        cout << "newT [" << i << "] = " << newT[i] << endl;

    int hist[pocet_kategorii] = {}; // {[0..pocet_kategorii] = 0}

//    cout << "min = " << newT[0] << endl;
//    cout << "max = " << newT[newT.size()-1] << endl;

    //double rozpeti = newT[newT.size()-1] - newT[0];
    double rozpeti = newT[newT.size()-1];
/*
    cout << endl << "histogram -------------------" << endl;
    cout << "rozpeti = " << rozpeti << endl;
    cout << "pocet_kategorii = " << pocet_kategorii << endl;
//*/

    for(unsigned int i = 0; i < newT.size(); i++)
    {
        // zjistit index kategorie, do ktere prvek spada:
        int idx = floor(newT[i]/rozpeti * pocet_kategorii);
//        if(i % 20 == 0)
//            cout << "i=" << i << "\t idx = " << idx << endl;

        hist[idx]++;
    }

    // prevest na relativni cetnosti:
    QVector <QwtIntervalSample> H;
    for(int kategorie = 0; kategorie < pocet_kategorii; kategorie++)
    {
        double min = rozpeti / pocet_kategorii * kategorie;
        double max = rozpeti / pocet_kategorii * (kategorie+1);

        double val = 100.0 * hist[kategorie]/old_size;
        H.push_back(QwtIntervalSample(val, min, max));
    }

//    for(unsigned int i = 0; i < newT.size()-1; i++)
//        H.push_back(QwtIntervalSample(2,newT[i],newT[i+1]));

    // vynest tracer test


    if(grafTracer == NULL) {
        //grafTracer = new QwtPlot(QwtText("Graf hypotetické stopovací zkoušky"));
        grafTracer = new QwtPlot(ui->widgetTracer);
        grafTracer->setTitle("Graf hypotetické stopovací zkoušky");
        grafTracer->setCanvasBackground(Qt::white);
        grafTracer->setAxisTitle(QwtPlot::xBottom,"čas [d]");
        grafTracer->setAxisTitle(QwtPlot::yLeft,"relativní četnost [%]");
        //this->layout()->addWidget(grafTracer);
        //ui->widgetTracer->layout()->addWidget(grafTracer);
    }

    grafTracer->detachItems(QwtPlotItem::Rtti_PlotHistogram);

    //grafTracer->setAxisScale(QwtPlot::yLeft, 0, 100);
    //grafTracer->setAxisScale(QwtPlot::xBottom, 0, 15);

    // sem prijde kod pro vyneseni histogramu
    QwtPlotHistogram *Histo = new QwtPlotHistogram;
    Histo->setSamples(H);
    Histo->setPen(Qt::darkBlue,1,Qt::SolidLine);
    Histo->setBrush(QBrush(Qt::blue, Qt::SolidPattern));
    Histo->attach(grafTracer);

    grafTracer->resize(grafTracer->parentWidget()->width(), grafTracer->parentWidget()->height());
    grafTracer->replot();
    grafTracer->show();
}

void MainWindow::on_pushButton_12_clicked()
{
    ExportPlot(grafTracer, "tracertest.png");
}

void MainWindow::on_pushButton_13_clicked()
{
    vector<double> *ptr = &newT;

    if(newT.size() < 3) // kdyz nejsou spocitane zadne lepsi casy, ulozime aspon zaklad - stredni casy na proudnicich
        ptr = &casy;

    if(ptr->size() == 0)
    {
        QMessageBox::critical(NULL,"Chyba","Není co ukládat - vrty nespojuje žádná proudnice.");
        return;
    }

    QString fname = QFileDialog::getSaveFileName(this,"Uložit jako",".","*.txt");

    ofstream komplet;
    komplet.open(fname.toStdString().data(), ios_base::out);

    for(unsigned int i = 0; i < ptr->size(); i++)
        komplet << (*ptr)[i] << endl;

    komplet.close();
}

void write(QLineEdit *w, double val)
{
  QLocale loc(QLocale::system());

  w->setText(loc.toString(val));
}

/* postarsi funkce - a Bany si s tim dal tolik prace :(

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    if(!ui->lineEdit->text().isEmpty() && !ui->lineEdit_2->text().isEmpty())
    {
        ui->lineEdit_3->setEnabled(true);
        ui->lineEdit_4->setEnabled(true);
    }
}

void MainWindow::on_lineEdit_2_textChanged(const QString &arg1)
{
    if(!ui->lineEdit->text().isEmpty() && !ui->lineEdit_2->text().isEmpty())
    {
        ui->lineEdit_3->setEnabled(true);
        ui->lineEdit_4->setEnabled(true);
    }
}

void MainWindow::on_lineEdit_3_textChanged(const QString &arg1)
{
    if(!ui->lineEdit_3->text().isEmpty() && !ui->lineEdit_4->text().isEmpty())
    {
        ui->lineEdit_5->setEnabled(true);
        ui->lineEdit_6->setEnabled(true);
    }
}

void MainWindow::on_lineEdit_4_textChanged(const QString &arg1)
{
    if(!ui->lineEdit_3->text().isEmpty() && !ui->lineEdit_4->text().isEmpty())
    {
        ui->lineEdit_5->setEnabled(true);
        ui->lineEdit_6->setEnabled(true);
    }
}

void MainWindow::on_lineEdit_5_textChanged(const QString &arg1)
{
    if(!ui->lineEdit_5->text().isEmpty() && !ui->lineEdit_6->text().isEmpty())
    {
        ui->lineEdit_7->setEnabled(true);
        ui->lineEdit_8->setEnabled(true);
    }
}

void MainWindow::on_lineEdit_6_textChanged(const QString &arg1)
{
    if(!ui->lineEdit_5->text().isEmpty() && !ui->lineEdit_6->text().isEmpty())
    {
        ui->lineEdit_7->setEnabled(true);
        ui->lineEdit_8->setEnabled(true);
    }
}

void MainWindow::on_lineEdit_7_textChanged(const QString &arg1)
{
    if(!ui->lineEdit_7->text().isEmpty() && !ui->lineEdit_8->text().isEmpty())
    {
        ui->lineEdit_9->setEnabled(true);
        ui->lineEdit_10->setEnabled(true);
    }
}

void MainWindow::on_lineEdit_8_textChanged(const QString &arg1)
{
    if(!ui->lineEdit_7->text().isEmpty() && !ui->lineEdit_8->text().isEmpty())
    {
        ui->lineEdit_9->setEnabled(true);
        ui->lineEdit_10->setEnabled(true);
    }
}
*/


