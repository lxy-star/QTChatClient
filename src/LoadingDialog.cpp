#include "LoadingDialog.h"
#include <QVBoxLayout>
#include <QApplication>

LoadingDialog::LoadingDialog(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setModal(true);
    setWindowTitle("Loading...");

    movie = new QMovie(":/resources/loading.gif");
    loadingLabel = new QLabel;
    loadingLabel->setMovie(movie);
    loadingLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(loadingLabel);
    setLayout(layout);
    resize(120, 120);

    movie->start();
}

LoadingDialog::~LoadingDialog()
{
    movie->stop();
}
