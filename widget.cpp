#include "widget.h"
#include <QFile>
#include <QDebug>
#include <QPainter>

#include <stdio.h>
#include <stdarg.h>
#include <strings.h>
#include <mfhdf.h>
#include <hdf.h>

void check (int status, char * message, ...) {
  if (status < 0) {
    va_list(args);
    va_start(args, message);
    vprintf(message, args);
    exit(1);
  }
}

char* readHdfBytes(int32 sds_id, int* dimsizes) {
  char* data = (char*) malloc(sizeof(char) * dimsizes[0] * dimsizes[1]);
  int32 start[2] = {0, 0};

  int status = SDreaddata(sds_id, start, NULL, dimsizes, data);
  check(status, "Error reading chunk\n");
  return data;
}

short* readHdfShort(int32 sds_id, int* dimsizes) {
  short* data = (short*) malloc(sizeof(short) * dimsizes[0] * dimsizes[1]);
  int32 start[2] = {0, 0};

  int status = SDreaddata(sds_id, start, NULL, dimsizes, data);
  check(status, "Error reading chunk\n");
  return data;
}

int readAttrInt(int32 sds_id, int32 nt, int entry, int count, char * atrname, char * name) {

  int status;
  /* figure out the size of the attribute */
  int32 dsize = DFKNTsize(nt);
  check(dsize, "Error got invalid size for attribute %s in section %s\n", atrname, name);

  if(nt == 21) {
    char * tbuff = (char*) malloc(dsize * (count + 1));
    if(tbuff == NULL) {
      printf("Error could not allocate space for attribute\n");
      return 1;
    }

    status = SDreadattr(sds_id, entry, tbuff);
    check(status, "Error failed to read attribute %d from %d\n", atrname, name);
    return *tbuff;
  } else if(nt == 22) {

    /* allocate space for the attribute */
    short * tbuff = (short*) malloc(dsize * (count + 1));
    if(tbuff == NULL) {
      printf("Error could not allocate space for attribute\n");
      return 1;
    }

    status = SDreadattr(sds_id, entry, tbuff);
    check(status, "Error failed to read attribute %d from %d\n", atrname, name);
    return *tbuff;
  }
  else {
    printf("ERROR unknown datatype %d\n", nt);
    exit(1);
  }

}

Widget::Widget(QWidget *parent) :
    QWidget(parent)
{
//    ndvi = new char[modelsize*modelsize];
//    temp = new char[modelsize*modelsize];
//    smi = new char[modelsize*modelsize];
//    for (int i=0; i<256; i++)
//    {
//        ndvicount[i]=0;
//        maxtemp[i]=0;
//        mintemp[i]=256;
//    }
//    for (int y=0; y<modelsize; y++)
//    {
//        for (int x=0; x<modelsize; x++)
//        {
//            ndvi[x+y*modelsize]=0;
//            temp[x+y*modelsize]=0;
//            smi[x+y*modelsize]=100;
//        }
//    }
//    image=new QImage(modelsize,modelsize,QImage::Format_RGB32);
    if (open()==0);
        qDebug() << "error";
}

//void Widget::open()
//{
//    QFile file("/home/matt/ESA/NDVI.txt");
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//            return;

//    qDebug() << "File" << file.fileName() << "Opened";
//    QByteArray line;
//    while (!file.atEnd()) {
//         line = file.readLine();
//        if (line.contains("data:"))
//        {

//             line = file.readLine();
//            qDebug() << line;
//            line = file.readLine();
//            qDebug() << line;

////            line = file.readLine();
////            qDebug() << line;
//            break;
//        }

////        qDebug() << line;
////        process_line(line);
//    }

//    int overflow = 0;
//    int currentpixel = 0;
//    while (!file.atEnd()) {
//        line = file.readLine();
////        qDebug() << line;
//        QList<QByteArray> valuelist = line.split(',');
//        foreach(QByteArray value, valuelist)
//        {
//            value=value.trimmed();
//            if (!value.isEmpty())
//            {
//                int valueint = value.toInt();
//                if (valueint == -3000)
//                    valueint=0;
//                else
//                {
//                    valueint+=1000;
//                    valueint=valueint/(12000/256);
//                }
//                smi[currentpixel]=valueint;
//                //            if (valueint>0)
////                qDebug() << value << valueint;
//                currentpixel++;
////                if (currentpixel==30)
////                    return;
//                if (currentpixel>=modelsize*modelsize)
//                {
//                    qDebug() << "File contains too much data" << overflow;
//                    overflow++;
//                    //                break;
//                }
//            }
//        }
//    }

//    //Copy the buffer to an image.
//    for (int y=0; y<modelsize; y++)
//    {
//        for(int x=0; x<modelsize; x++)
//        {
////            int val = qBound(0,(int)smi[x+y*modelsize],255);
//            image->setPixel(x,y,qRgb(smi[x+y*modelsize],smi[x+y*modelsize],smi[x+y*modelsize]));
//        }
//    }
//    update();
//}

int Widget::openTemp()
{

}

int Widget::openNDVI()
{

}

void* Widget::openHDFChar(char * filename, char * requiredPart, int &x, int &y, int &fillValue)
{
    void* data=0;
    int32 nsds;
    int32 ngattr;
    int32 nattrs;
    int32 nt;
    int32 rank;
    int32 dimsizes[50];
    int32 count;
    char name[512];
    char atrname[512];

    fillValue = -1;


    for (int i = 0; i < 50; i++) {
      dimsizes[i] = 0;
    }

    int fid = SDstart(filename, DFACC_RDONLY);
    check(fid, "Error opening file %s returned code: %d\n", filename, fid);


    int status = SDfileinfo(fid, &nsds, &ngattr);
    check(status, "Error extracting attributes from %s returned %d\n", filename, fid);

    for (int i = 0; i < nsds; i++) {
      int32 sds_id = SDselect(fid, i);
      check(sds_id, "Error selecting section %d from file %s returned %d\n", i, filename, sds_id);

      status = SDgetinfo(sds_id, name, &rank, dimsizes, &nt, &nattrs);
      check(status, "Error getting info for section %d in %s returned %d\n", i, filename, sds_id);


      if (strcmp(name, requiredPart) == 0) {
        int datatype = nt;

        for (int j = 0; j < nattrs; j++) {
          status = SDattrinfo(sds_id, j, atrname, &nt, &count);
          check(status, "Error getting attribute info for %s", name);
          //printf("attr %s type %d value %d\n", atrname, nt, count);
          if (strcmp("_FillValue", atrname) == 0) {
            fillValue = readAttrInt(sds_id, nt, j, count, atrname, name);
          }

        }

        printf("%s x:%d y:%d fillValue:%d\n", name, dimsizes[0], dimsizes[1], fillValue);
        if(fillValue == -1) {
          printf("ERROR fill value not found\n");
          return 0;
        }

        x=dimsizes[0];
        y=dimsizes[1];
        if (datatype == 21) { // bytes
          char * data = readHdfBytes(sds_id, dimsizes);
//          for (int y = 0; y < dimsizes[0]; y++) {
//            for (int x = 0; x < dimsizes[1]; x++) {
//              int value = data[(y*dimsizes[0])+x];
//              if (value != fillValue) {
//                printf("#");
//              } else {
//                printf(" ");
//              }
//            }
//            printf("\n");
//          }
          return (void*)data;
        } else if (datatype == 22) {
          short * data = readHdfShort(sds_id, dimsizes);
//          for (int y = 0; y < dimsizes[0]; y++) {
//            for (int x = 0; x < dimsizes[1]; x++) {
//              int value = data[(y*dimsizes[0])+x];
//              if (value != fillValue) {
//                printf("#");
//              } else {
//                printf(" ");
//              }
//            }
//            printf("\n");
//          }
          return (void*)data;
        } else {
          printf("ERROR unknown datatype %d\n", datatype);
          return 0;
        }

      }

    }

    return 0;
}

bool Widget::open()
{
    bool ok = true;
    float* smi1;
    float* smi2;
    int xsize,ysize;
    char * tempfilename = "/home/matt/ESA/Final ESA/MYD11A2.A2013345.h22v10.005.2013355041029.hdf";
    char * temprequiredPart = "Clear_sky_days";
    char * ndvifilename = "/home/matt/ESA/Final ESA/MYD13A3.A2013335.h22v10.005.2014015124544.hdf";
    char * ndvirequiredPart = "1 km monthly NDVI";
    smi1 = open(tempfilename, temprequiredPart, ndvifilename, ndvirequiredPart, xsize,ysize,ok);
    tempfilename = "/home/matt/ESA/Final ESA/MYD11A2.A2014153.h22v10.005.2014162154620.hdf";
    temprequiredPart = "Clear_sky_days";
    ndvifilename = "/home/matt/ESA/Final ESA/MYD13A3.A2014152.h22v10.005.2014188182535.hdf";
    ndvirequiredPart = "1 km monthly NDVI";
    smi2 = open(tempfilename, temprequiredPart, ndvifilename, ndvirequiredPart, xsize,ysize,ok);

    //Copy the buffer to an image.
    for (int y=0; y<ysize; y++)
    {
        for(int x=0; x<xsize; x++)
        {

            char smival = (smi1[x+ysize*y])*255;
            image->setPixel(x,y,qRgb(smival,smival,smival));
        }
    }
    for (int y=0; y<ysize; y++)
    {
        for(int x=0; x<xsize; x++)
        {

            char smival = (smi2[x+ysize*y])*255;
            image2->setPixel(x,y,qRgb(smival,smival,smival));
        }
    }
    for (int y=0; y<ysize; y++)
    {
        for(int x=0; x<xsize; x++)
        {
            float smival1 = (smi1[x+ysize*y]);
            float smival2 = (smi2[x+ysize*y]);
            float smivaldiff = smival1-smival2;
            int pixelval = qRound((smivaldiff)*255);
            if (smivaldiff < 0)
                imagediff->setPixel(x,y,qRgb(0,0,0));
            else
                imagediff->setPixel(x,y,qRgb(pixelval,pixelval,pixelval));
        }
    }
    image->save("/home/matt/ESA/Final ESA/A2013345.png");
    image2->save("/home/matt/ESA/Final ESA/A2014153.png");
    imagediff->save("/home/matt/ESA/Final ESA/diff.png");
    update();
    return ok;
}

float* Widget::open(char * tempfilename, char * temprequiredPart, char * ndvifilename, char * ndvirequiredPart,  int &x, int &y, bool &ok)
{
    //TODO: this should be passed in  some how.
//    tempfilename = "/home/matt/ESA/MYD11A2.A2015009.h22v10.005.2015020130228.hdf";
//    temprequiredPart = "Clear_sky_days";
    int xsize,ysize, tempFillValue;
    temp = (char*) openHDFChar(tempfilename, temprequiredPart,xsize,ysize, tempFillValue);
    if (temp==0)
        return 0;

//    ndvifilename = "/home/matt/ESA/MYD13A3.A2014305.h22v10.005.2014349100449.hdf";
//    ndvirequiredPart = "1 km monthly NDVI";
    int ndvixsize, ndviysize, ndvifillValue;
    ndvi = (short*) openHDFChar(ndvifilename, ndvirequiredPart,ndvixsize,ndviysize, ndvifillValue);
    if (ndvi==0)
        return 0;

    x=xsize;
    y=ysize;

    //Reset the maths:
    smi = new float[modelsize*modelsize];
    for (int i=0; i<256; i++)
    {
        ndvicount[i]=0;
        maxtemp[i]=0;
        mintemp[i]=256;
    }
    for (int y=0; y<modelsize; y++)
    {
        for (int x=0; x<modelsize; x++)
        {
//            ndvi[x+y*modelsize]=0;
            //                temp[x+y*modelsize]=0;
            smi[x+y*modelsize]=0;
        }
    }
    image=new QImage(ndvixsize, ndviysize,QImage::Format_RGB32);
    image2=new QImage(ndvixsize, ndviysize,QImage::Format_RGB32);
    imagediff=new QImage(ndvixsize, ndviysize,QImage::Format_RGB32);

    uchar mintemps[10];//={255,255,255,255,255,255,255,255,255,255};
    uchar maxtemps[10];//={0,0,0,0,0,0,0,0,0,0};

    for(int ndviband = 0; ndviband<10; ndviband++)
    {
        mintemps[ndviband]=255;
        maxtemps[ndviband]=0;
        int bandmin=ndviband*1000-2000;
        int bandmax=ndviband*1000-1000;
        for (int y=0; y<ysize; y++)
        {
            for(int x=0; x<ysize; x++)
            {
                short ndvival = ndvi[x+ysize*y];
                char tempval = temp[x+ysize*y];
                if (ndvival!=ndvifillValue && ndvival>bandmin && ndvival<bandmax && tempval!=tempFillValue)
                {
                    uchar t1 = temp[x+ysize*y];
                    maxtemps[ndviband] = qMax(maxtemps[ndviband], t1);
                    mintemps[ndviband] = qMin(mintemps[ndviband], t1);
                }
            }
        }
        uchar max = maxtemps[ndviband];
        uchar min = mintemps[ndviband];
        qDebug() <<  "band" << ndviband << "Max" << max << "Min" << min;
        for (int y=0; y<ysize; y++)
        {
            for(int x=0; x<ysize; x++)
            {
                short ndvival = ndvi[x+ysize*y];
                char tempval = temp[x+ysize*y];
                if (ndvival!=ndvifillValue && ndvival>bandmin && ndvival<bandmax && tempval!=tempFillValue)
                {
                    uchar t1 = temp[x+ysize*y];
                    smi[x+ysize*y] = ((float)(max-t1))/((float)(max-min));
                }
            }
        }
    }

    return smi;
    free(temp);

}

void Widget::paintEvent(QPaintEvent *)
{

//    qDebug()<<"paintEvent()";
    QPainter painter(this);
    painter.drawImage(QRectF(0,0,modelsize,modelsize), *imagediff);
//    painter.drawImage(QPoint(0,0), *image);
}
