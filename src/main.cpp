#include <fstream>
#include <Transformer.h>
#include <Interface.h>
#include <QApplication>
using namespace std;
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Interface w;
	w.show();
	return a.exec();
}
