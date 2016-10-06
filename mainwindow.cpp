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
    delete rez;
    delete mapa;
    delete ui;
}


void MainWindow::on_pushButton_2_clicked()
{
QMessageBox::critical(NULL,"problem",QString::number(N));
}

void MainWindow::on_pushButton_clicked()
{
   // readData();
testovaci_input();
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

            // vydatnosti
            Q[0] = 0;
            Q[1] = 0;

            // polomery studni
            r[0] = 0;
            r[1] = 0;

            // polomery dosahu (odhad, nebo vnucena hodnota
            R[0] = 0;//ui->lineEdit_20->text().toDouble();
            R[1] = 0;//ui->lineEdit_21->text().toDouble();;

            // snizeni
            s[0] = 0;
            s[1] = 0;
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
    testovaci_input();

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
        rez->setAxisScale(QwtPlot::yLeft, H-1, H+1);
        rez->setAxisScale(QwtPlot::xBottom, -L/2 , 1.5*L);
        rez->insertLegend(new QwtLegend);
    }
    rez->detachItems(QwtPlotItem::Rtti_PlotCurve); // zbavi graf vsech...

    QwtPlotCurve *krivka = new QwtPlotCurve("hydraulická výška");
    krivka->setSamples( x, rezh, xDim);
    krivka->setPen( Qt::blue, 1 );
    krivka->attach(rez);

    QwtPlotGrid *mrizka = new QwtPlotGrid();
    mrizka->attach(rez);

    rez->resize(ui->FrameHH->width(), ui->FrameHH->height());
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

/* jen kus kodu - ukazkovy zpusob, jak vynest jednoduchy graf:
    QwtPlot *plot = new QwtPlot(ui->ramecek);
    plot->setTitle( "Plot Demo" );
    plot->setCanvasBackground( Qt::white );
    plot->setAxisScale( QwtPlot::yLeft, 0.0, 10.0 );
    plot->insertLegend( new QwtLegend() );

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->attach( plot );

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setTitle( "Some Points" );
    curve->setPen( Qt::blue, 4 ),
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
        QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 4, 8 ) );
    curve->setSymbol( symbol );

    QPolygonF points;
    points << QPointF( 0.0, 4.4 ) << QPointF( 1.0, 3.0 )
        << QPointF( 2.0, 4.5 ) << QPointF( 3.0, 6.8 )
        << QPointF( 4.0, 7.9 ) << QPointF( 5.0, 7.1 );
    curve->setSamples( points );

    curve->attach( plot );

    plot->resize( ui->ramecek->width(), ui->ramecek->height() );
    plot->show();
*/

void MainWindow::on_pushButton_6_clicked() // TAB: MAPA: vypocet
{
    testovaci_input();

    if(mapa !=NULL)
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

    //kontury
    QList<double> contourLevels;
    for ( double level = zmin; level < zmax + .1; level += (zmax-zmin)/10 )
        contourLevels += level;
    graf->setContourLevels( contourLevels );

    graf->setData(obsah);
    graf->attach(mapa);

    //barvy
    QwtLinearColorMap *barvy = new QwtLinearColorMap(Qt::darkBlue, Qt::cyan, QwtColorMap::RGB);
    //barvy->addColorStop(H-.05,Qt::darkGreen);
    //barvy->addColorStop(H+.05,Qt::darkGreen);
    graf->setColorMap(barvy);
    graf->setDisplayMode( QwtPlotSpectrogram::ContourMode);

    //_
    mapa->setAxisScale( QwtPlot::yRight, zmin, zmax);
    mapa->enableAxis(QwtPlot::yRight);

    mapa->setAxisScale(QwtPlot::xBottom, xmin, xmax);
    mapa->setAxisScale(QwtPlot::yLeft, ymin, ymax);
    //mapa->setAxisAutoScale(QwtPlot::yLeft);
    //mapa->setAxisAutoScale(QwtPlot::xBottom);

    mapa->resize(ui->widgetMapa->width(), ui->widgetMapa->height());
    mapa->show();


}

void MainWindow::on_pushButton_7_clicked() // TAB: MAPA: export
{
    ExportPlot(mapa, "mapa.png");
}

void MainWindow::on_pushButton_8_clicked() //dopočítáme snížení
{
  //readData();
testovaci_input();
  if ( ui->lineEdit_11->text().isEmpty() || ui->lineEdit_14->text().isEmpty())
  {
      QMessageBox::warning(NULL,"problem","Zadejte vydatnost!");
      return;
  }

    s[0]=wellDrawdown(0);
    s[1]=wellDrawdown(1);
  ui->lineEdit_13->setText(QString::number(s[0]));
  ui->lineEdit_16->setText(QString::number(s[1]));

}

void MainWindow::on_startProudnice_clicked() // TAB: PROUDNICE A TRACKING: grafy
{
    readLayers();

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

    ofstream outfile("proudnice.txt",ios_base::out);
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

            t = track_point(x[j], y[j], z0, sgn(Q[j]) * krok, &X, &Y); //

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
    grafProudnice->setAxisScale(QwtPlot::yLeft, -L*1.5*pom, L*1.5*pom);
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
  if (ui->lineEdit_20->text().isEmpty() || ui->lineEdit_21->text().isEmpty())
  {
      QMessageBox::warning(NULL,"problem","Zadejte oba dosahy!");
              return;
  }

  if ( ui->lineEdit_11->text().isEmpty() && ui->lineEdit_14->text().isEmpty())
  {
      QMessageBox::warning(NULL,"problem","Zadejte jednu vydatnost!");
      return;
  }

  if (ui->lineEdit_14->text().isEmpty() && ui->lineEdit_16->text().isEmpty())
  {
      QMessageBox::warning(NULL,"problem","Zadejte snizeni ve studni,ve ktere chcete znad vydatnost!");
      return;
  }

  if (ui->lineEdit_11->text().isEmpty() && ui->lineEdit_13->text().isEmpty())
  {
      QMessageBox::warning(NULL,"problem","Zadejte snizeni ve studni,ve ktere chcete znad vydatnost!");
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

void MainWindow::readLayers()
{
    // nacist data z gui do poli

    // hloubky rozhrani

    z[0] = 0;
    z[1] = ui->lineEdit->text().toDouble();
    z[2] = ui->lineEdit_3->text().toDouble();
    z[3] = ui->lineEdit_5->text().toDouble();
    z[4] = ui->lineEdit_7->text().toDouble();
    z[5] = ui->lineEdit_9->text().toDouble();


    // hydraulicke vodivosti: ! CIST V ZDOLA NAHORU, ABY BYLA SPRAVNE, AZ SE ZPREHAZEJI Z-KA!
    K[4] = ui->lineEdit_2->text().toDouble();
    K[3] = ui->lineEdit_4->text().toDouble();
    K[2] = ui->lineEdit_6->text().toDouble();
    K[1] = ui->lineEdit_8->text().toDouble();
    K[0] = ui->lineEdit_10->text().toDouble();

    // vzdalenost mezi studnami
    L = ui->lineEdit_17->text().toDouble();

    // puvodni hydraulicka vyska (pred cerpanim - nebo hloubka hladiny podzemni vody? bylo by praktictejsi
    H = ui->lineEdit_18->text().toDouble();

    // veci odvozene:

    // tenhle kus kodu pripravi z-souradnice vrstevnich rozhrani - obrati osu, polozi z=0 na podlozi
    double Z_base = z[N];

    for(int i = 0; i < N+1; i++)
    {
        z[i] = Z_base - z[i];
        //cerr << "z[" << i << "] = " << z[i] << endl;
    }

    for(int i = 0; i < (N+1)/2; i++)
    {
        double pom;
        pom = z[i];
        z[i] = z[N-i];
        z[N-i] = pom;
        //cerr << "z[" << i << "] = " << z[i] << endl;
    }

    H = Z_base - H;

    // mocnosti vrstev
    d[0] = z[1];
    d[1] = z[2] - z[1];
    d[2] = z[3] - z[2];
    d[3] = z[4] - z[3];
    d[4] = z[5] - z[4];

    int n = getLayer(H);

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
}

void MainWindow::readWells()
{

}
