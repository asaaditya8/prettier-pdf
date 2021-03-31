#include "images2pdf.h"
#include <QApplication>
#include <QRectF>
#include <QImage>
#include <QPainter>
#include <QPdfWriter>

#include <iostream>
#include <utility>

Images2PDF::Images2PDF(const QString& source_dir, const QString& target_pdf) :
    _aborted(false)
{
    _source_dir = new QDir(source_dir);
    _target_pdf = new QFileInfo(target_pdf);

    QStringList imageFilter;
    imageFilter << "*.jpg" << "*.jpeg" << "*.png";
    _images = _source_dir->entryList(imageFilter, QDir::Files | QDir::Readable, QDir::Name);
}

Images2PDF::Images2PDF(const QString& source_dir, QStringList filenames, const QString& target_pdf) :
        _aborted(false)
{
    _source_dir = new QDir(source_dir);
    _target_pdf = new QFileInfo(target_pdf);

    _images = std::move(filenames);
}

Images2PDF::~Images2PDF()
{
    delete _source_dir;
    delete _target_pdf;
}

int Images2PDF::imageCount()
{
    return _images.length();
}

const QString Images2PDF::error()
{
    return _error;
}

void Images2PDF::abort()
{
    _aborted = true;
}

void Images2PDF::start()
{
    if (imageCount() == 0) {
        _error = trUtf8("No images found!");
        emit finished(false);
        return;
    }

    QPdfWriter* printer = new QPdfWriter(_target_pdf->absoluteFilePath());
    printer->setPageSize(QPageSize(QPageSize::A4));

    QPainter* painter = new QPainter(printer);
    /// <summary>
    /// uses QPDFWriter as QPaintDevice
    /// with whatever default resolution there is.
    /// </summary>

    for (int idx = 0; !_aborted && idx < _images.length(); ++idx) {
        emit processing(_images[idx]);
        qApp->processEvents();

        if (idx > 0) {
            printer->newPage();
        }

        addImage(idx, painter);

        _processed << _images[idx];

        emit progress(idx + 1);
        qApp->processEvents();
    }

    painter->end();
    delete painter;
    delete printer;

    if (_aborted) {
        _error = trUtf8("Aborted after %1 images were added to PDF.").arg(_processed.length());
        emit finished(false);
        return;
    }
    emit finished(true);
}

void Images2PDF::addImage(int imageIndex, QPainter* painter)
{
    QImage img(_source_dir->filePath(_images[imageIndex]));

    if (img.isNull()) {
        return; // TODO: report skipped images somewhere?
    }

    float w = painter->device()->width();
    float h = painter->device()->height();
    QRect imageSize(0, 0, img.width(), img.height());

    float scale_x = w / ((float)imageSize.width());
    float scale_y = h / ((float)imageSize.height());
    float scale = (scale_x < scale_y) ? scale_x : scale_y;

    float offsetx = w - (imageSize.width() * scale);
    float offsety = h - (imageSize.height() * scale);
    offsetx /= 2;
    offsety /= 2;

    w = imageSize.width() * scale;
    h = imageSize.height() * scale;

    QRect sizeOnPage(offsetx, offsety, w, h);

    painter->drawImage(
        sizeOnPage,
        img,
        imageSize);
}
