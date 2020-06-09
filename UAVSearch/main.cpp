#include "UAVSearch.h"
#include <QtWidgets/QApplication>
#include <qsplashscreen.h>
#include <qthread.h>
#include <qlabel.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	UAVSearch w;

	QSplashScreen *splash = new QSplashScreen();
	//    splash->setPixmap(QPixmap("D:\\MyFiles\\ITWorld\\MyUAVSearch\\UAVSearch\\back.jpg").scaled(1200, 700, Qt::KeepAspectRatio));
	splash->setPixmap(QPixmap("./image/back.jpg").scaled(1200, 700, Qt::KeepAspectRatio));

	QFont ft;
	ft.setPointSize(15);
	QPalette pa;
	pa.setColor(QPalette::WindowText, Qt::white);

	QLabel *text1 = new QLabel(splash);
	text1->setFont(ft);
	text1->setPalette(pa);
	text1->setText(QString(u8"���Ŵ�ѧ"));

	QLabel *text2 = new QLabel(splash);
	text2->setFont(ft);
	text2->setPalette(pa);
	text2->move(0, splash->height() - 30);
	text2->setText(QString(u8"��������..."));

	QLabel *text3 = new QLabel(splash);
	ft.setPointSize(30);
	text3->setFont(ft);
	text3->setPalette(pa);
	text3->move(140, 200);
	text3->setText(QString(u8"���˻�����Эͬ�滮���"));

	splash->show();
	a.processEvents();//ȥ���̵߳ȴ���ͼ��
	QThread::sleep(1);//�ӳ�ʱ��
   // splash->close();//�����رյ���


	w.resize(1200, 800);
	w.show();
	splash->finish(&w);//��������ʾ�󣬵���������ʧ
	return a.exec();
}
