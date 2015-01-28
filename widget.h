#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = 0);
#define modelsize 1200
    short *ndvi;
    char *temp;
    float *smi;
    float ndvicount[256];
    float maxtemp[256];
    float mintemp[256];
    QImage* image;
    QImage* image2;
    QImage* imagediff;
    bool open();
    int openTemp();
    int openNDVI();
    void *openHDFChar(char *filename, char *requiredPart, int &x, int &y, int &fillValue);
    //    short *openHDFShort(char *filename, char *requiredPart, int &x, int &y, int &fillValue);
    float *open(char *tempfilename, char *temprequiredPart, char *ndvifilename, char *ndvirequiredPart,  int &x, int &y,bool &ok);
signals:

public slots:

protected:
    void paintEvent(QPaintEvent *);
};

#endif // WIDGET_H
