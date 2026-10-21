#include <QApplication>

#include "janela.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Janela janela;
    janela.show();
    return app.exec();
}