/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#endif

#include "imageviewer.h"

typedef struct PNT {
    int x;
    int y;
}DES_PT;

//! [0]
/*for MyView using*/
typedef enum
{
    None,
    CalculateDistance,
    RegionGrowing
}ViewMode;

class MyView:public QGraphicsView
{
public:
    MyView( ImageViewer *parent=0 ) :  QGraphicsView(parent = 0)
    {
        this->Parent = parent;
        qlPixelInfo = new QLabel(this);
//        qlPixelInfo->setStyleSheet("QLabel { color : red; }");
        this->PointCount = 0;
    }

    ~MyView()
    {
        this->destroy(true, true);
    }

    void SetImage(QImage img)
    {
        this->Qimage = img.convertToFormat(QImage::Format_RGB888);
    }

    void Display()
    {
        setScene( new QGraphicsScene( this ) );
        this->scene()->addPixmap(QPixmap::fromImage(this->Qimage)); // this->image is a QImage
        this->show();
    }

    void setViewMode(ViewMode mode)
    {
        this->Mode = mode;
    }

    void drawColorDot(QImage* img, int rgb, int x, int y)
    {
        for (int r = x-2; r < x+3; r++)
        {
            for (int c = y-2; c < y+3; c++)
            {
                img->setPixel(r, c, rgb);
            }
        }
    }

public slots:
    void mousePressEvent( QMouseEvent* event )
    {
        QPointF pos =  mapToScene( event->pos() );
        int x = ( int )pos.x();
        int y = ( int )pos.y();
        if (PointCount > 5) return;
        ClickPointX[PointCount] = x;
        ClickPointY[PointCount] = y;
        PointCount++;
        QRgb rgb = this->Qimage.pixel( x, y );
        drawColorDot(&Qimage, qRgb(0,255,0), x, y);
        if (this->Mode == CalculateDistance && PointCount % 2 == 0)
        {
            QLabel *qlDistance = new QLabel(this);
            QPainter pt(&Qimage);
            if (PointCount == 2)
            {
                qlDistance->setStyleSheet("QLabel { color : red; }");
                pt.setPen(Qt::red);
            }
            if (PointCount == 4)
            {
                qlDistance->setStyleSheet("QLabel { color : green; }");
                pt.setPen(Qt::green);
            }
            if (PointCount == 6)
            {
                qlDistance->setStyleSheet("QLabel { color : blue; }");
                pt.setPen(Qt::blue);
            }
            pt.drawLine(ClickPointX[PointCount-1],ClickPointY[PointCount-1],
                    ClickPointX[PointCount-2],ClickPointY[PointCount-2] );
            pt.end();
            qlDistance->setGeometry(0, 0+(PointCount/2)*30, 300, 30);
//                qlDistance->setGeometry(x, y, 300, 30);
            QString line_info;
            double distance = sqrt(pow(ClickPointX[PointCount-1] - ClickPointX[PointCount-2], 2) +
                    pow(ClickPointY[PointCount-1] - ClickPointY[PointCount-2], 2));
            line_info.sprintf("(%d,%d) to (%d,%d) is %lf",
                              ClickPointX[PointCount-1],ClickPointY[PointCount-1],
                    ClickPointX[PointCount-2],ClickPointY[PointCount-2], distance);
            qlDistance->setText(line_info);
            qlDistance->show();
        }

        this->Display();
        QString info;
        info.sprintf("(%d,%d)=(%d,%d,%d)", x, y, qRed(rgb), qGreen(rgb), qBlue(rgb));

        qlPixelInfo->hide();
        qlPixelInfo->setGeometry(0, 0, 300, 30);
        qlPixelInfo->setText(info);
        qlPixelInfo->show();
    }

    void mouseMoveEvent(QMouseEvent* event)
    {
        QPointF pos =  mapToScene( event->pos() );
        int x = ( int )pos.x();
        int y = ( int )pos.y();
        QRgb rgb = this->Qimage.pixel( x, y );
        QString info;
        info.sprintf("(%d,%d)=(%d,%d,%d)", x, y, qRed(rgb), qGreen(rgb), qBlue(rgb));

        qlPixelInfo->setText(info);
        qlPixelInfo->show();
    }

private slots:
private:
    QImage Qimage;
    QLabel *qlPixelInfo;
    ImageViewer *Parent;
    int PointCount;
    //to recording 6 clicked point
    int ClickPointX[6], ClickPointY[6];
    ViewMode Mode;
};

MyView *mv;

ImageViewer::ImageViewer()
{
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    setCentralWidget(scrollArea);

    createActions();
    createMenus();

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);

    int btn_hight = 50;
    QPushButton *btnOpen = new QPushButton(this);
    btnOpen->setGeometry(0,btn_hight,100,30);
    btnOpen->setText("Open");
    connect(btnOpen, SIGNAL (released()),this, SLOT (open()));

    QPushButton *btnReset = new QPushButton(this);
    btnReset->setGeometry(0,btn_hight*2,100,30);
    btnReset->setText("Reset");
    connect(btnReset, SIGNAL (released()),this, SLOT (slotReset()));

    QPushButton *btnSave = new QPushButton(this);
    btnSave->setGeometry(0,btn_hight*3,100,30);
    btnSave->setText("Save");
    connect(btnSave, SIGNAL (released()),this, SLOT (slotSave()));
}

void ImageViewer::slotReset()
{
    mv->~MyView();
    mv = new(std::nothrow) MyView(this);
    mv->setGeometry(500,200, 500, 500);
    mv->SetImage(S_Image);
    mv->setViewMode(CalculateDistance);
    mv->Display();
}

void ImageViewer::slotSave()
{
    QPixmap save_image = QPixmap::grabWindow(mv->winId());
    save_image.save("save_image.bmp");
}

//! [0]
//! [2]

bool ImageViewer::loadFile(const QString &fileName)
{
    QImage image(fileName);
    if (image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
        setWindowFilePath(QString());
        imageLabel->setPixmap(QPixmap());
        imageLabel->adjustSize();
        return false;
    }
//! [2] //! [3]
    S_Image = image;
//    imageLabel->setPixmap(QPixmap::fromImage(S_Image));
    mv = new(std::nothrow) MyView(this);
    mv->setGeometry(500,200, 500, 500);
    mv->SetImage(S_Image);
    mv->setViewMode(CalculateDistance);
    mv->Display();

//! [3] //! [4]
    scaleFactor = 1.0;

    printAct->setEnabled(true);
    fitToWindowAct->setEnabled(true);
    updateActions();

    if (!fitToWindowAct->isChecked())
        imageLabel->adjustSize();

    setWindowFilePath(fileName);
    return true;
}

//! [4]

//! [2]

//! [1]
void ImageViewer::open()
{
    QStringList mimeTypeFilters;
    foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QFileDialog dialog(this, tr("Open File"),
                       picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first());
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/bmp");

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}
//! [1]

//! [5]
void ImageViewer::print()
//! [5] //! [6]
{
    Q_ASSERT(imageLabel->pixmap());
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
//! [6] //! [7]
    QPrintDialog dialog(&printer, this);
//! [7] //! [8]
    if (dialog.exec()) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = imageLabel->pixmap()->size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(imageLabel->pixmap()->rect());
        painter.drawPixmap(0, 0, *imageLabel->pixmap());
    }
#endif
}
//! [8]

//! [9]
void ImageViewer::zoomIn()
//! [9] //! [10]
{
//    scaleImage(1.25);
//    scaleImage_NNI(1.25);
    scaleImage_Bilinear(1.25);
}

void ImageViewer::zoomOut()
{
//    scaleImage(0.8);
    scaleImage_Bilinear(0.8);
}

//! [10] //! [11]
void ImageViewer::normalSize()
//! [11] //! [12]
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}
//! [12]

//! [13]
void ImageViewer::fitToWindow()
//! [13] //! [14]
{
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow) {
        normalSize();
    }
    updateActions();
}
//! [14]


//! [15]
void ImageViewer::about()
//! [15] //! [16]
{
    QMessageBox::about(this, tr("About Image Viewer"),
            tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
               "and QScrollArea to display an image. QLabel is typically used "
               "for displaying a text, but it can also display an image. "
               "QScrollArea provides a scrolling view around another widget. "
               "If the child widget exceeds the size of the frame, QScrollArea "
               "automatically provides scroll bars. </p><p>The example "
               "demonstrates how QLabel's ability to scale its contents "
               "(QLabel::scaledContents), and QScrollArea's ability to "
               "automatically resize its contents "
               "(QScrollArea::widgetResizable), can be used to implement "
               "zooming and scaling features. </p><p>In addition the example "
               "shows how to use QPainter to print an image.</p>"));
}
//! [16]

//! [17]
void ImageViewer::createActions()
//! [17] //! [18]
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    printAct = new QAction(tr("&Print..."), this);
    printAct->setShortcut(tr("Ctrl+P"));
    printAct->setEnabled(false);
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    zoomInAct = new QAction(tr("Zoom &In (NNI)"), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setEnabled(false);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (NNI)"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setEnabled(false);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

//    zoomInAct = new QAction(tr("Zoom &In (Bilinear)"), this);
//    zoomInAct->setShortcut(tr("Ctrl++"));
//    zoomInAct->setEnabled(false);
//    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

//    zoomOutAct = new QAction(tr("Zoom &Out (Bilinear%)"), this);
//    zoomOutAct->setShortcut(tr("Ctrl+-"));
//    zoomOutAct->setEnabled(false);
//    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAct = new QAction(tr("&Normal Size"), this);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    fitToWindowAct = new QAction(tr("&Fit to Window"), this);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));
    connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}
//! [18]

//! [19]
void ImageViewer::createMenus()
//! [19] //! [20]
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);
    viewMenu->addSeparator();
    viewMenu->addAction(fitToWindowAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(helpMenu);
}
//! [20]

//! [21]
void ImageViewer::updateActions()
//! [21] //! [22]
{
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}
//! [22]

//! [23]
void ImageViewer::scaleImage(double factor)
//! [23] //! [24]
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void ImageViewer::scaleImage_NNI(double factor)
{
    float row, col;
    int x,y,px;
    QImage outImg = QImage(S_Image.width() * factor, S_Image.height() * factor, QImage::Format_ARGB32);
    for(row = 0; row < outImg.height(); row++)
    {
        for (col = 0; col<outImg.width(); col++)
        {
            x = col/factor+0.5;
            y = row/factor+0.5;
            if(x>=S_Image.width())  x--;
            if(y>=S_Image.height()) y--;

            px = S_Image.pixel(x, y);
            outImg.setPixel(col, row, qRgb(qRed(px), qGreen(px), qBlue(px)));
        }
    }
    S_Image = outImg;
    imageLabel->setPixmap(QPixmap::fromImage(S_Image));
    imageLabel->adjustSize();
}

void ImageViewer::scaleImage_Bilinear(double factor)
{
    double alpha, beta;
    float row, col;
    //p[0]    p[1]
    //**      **
    //**      **
    //p[2]    p[3]
    DES_PT p[4];
    int xa, xb, xc, xd, ya, yb, yc, yd;
    int pa_red, pb_red, pc_red, pd_red, pa_green, pb_green, pc_green, pd_green, pa_blue, pb_blue, pc_blue, pd_blue, outp_red, outp_green, outp_blue;
    QImage outImg = QImage(S_Image.width() * factor, S_Image.height() * factor, QImage::Format_ARGB32);
    for(row = 0; row < outImg.height(); row++)
    {
//        qDebug("j = %f", j);
        for (col = 0; col<outImg.width(); col++)
        {
            xa = col/factor;	ya = row/factor;
            xb = xa+1;	yb = ya;
            xc = xa;	yc = ya + 1;
            xd = xa+1;	yd = ya+1;
            if (xb>=S_Image.width())  xb--;
            if (xd>=S_Image.width())	xd--;
            if (yc>=S_Image.height())	yc--;
            if (yd>=S_Image.height())	yd--;
            alpha = col/factor - xa;
            beta = row/factor -ya;
            //caculate the red color
            pa_red = qRed(S_Image.pixel(xa, ya));
            pb_red = qRed(S_Image.pixel(xb, yb));
            pc_red = qRed(S_Image.pixel(xc, yc));
            pd_red = qRed(S_Image.pixel(xd, yd));
            outp_red = (1-alpha)*(1-beta)*pa_red + alpha*(1-beta)*pb_red + (1-alpha)*beta*pc_red + alpha*beta*pd_red + 0.5;

            pa_green = qGreen(S_Image.pixel(xa, ya));
            pb_green = qGreen(S_Image.pixel(xb, yb));
            pc_green = qGreen(S_Image.pixel(xc, yc));
            pd_green = qGreen(S_Image.pixel(xd, yd));
            outp_green = (1-alpha)*(1-beta)*pa_green + alpha*(1-beta)*pb_green + (1-alpha)*beta*pc_green + alpha*beta*pd_green + 0.5;

            pa_blue = qBlue(S_Image.pixel(xa, ya));
            pb_blue = qBlue(S_Image.pixel(xb, yb));
            pc_blue = qBlue(S_Image.pixel(xc, yc));
            pd_blue = qBlue(S_Image.pixel(xd, yd));
            outp_blue = (1-alpha)*(1-beta)*pa_blue + alpha*(1-beta)*pb_blue + (1-alpha)*beta*pc_blue + alpha*beta*pd_blue + 0.5;

            outImg.setPixel(col, row, qRgb(outp_red, outp_green, outp_blue));
        }
    }
    S_Image = outImg;
    imageLabel->setPixmap(QPixmap::fromImage(S_Image));
    imageLabel->adjustSize();
}

//! [24]

//! [25]
void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
//! [25] //! [26]
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}
//! [26]
