#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_UAVSearch.h"
#include <qpoint.h>
#include <qmap.h>
#include <qcolor.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qtablewidget.h>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include "Plan.h"
using namespace std;

class UAVSearch : public QMainWindow
{
	Q_OBJECT

public:
	UAVSearch(QWidget *parent = Q_NULLPTR);

private:
	Ui::UAVSearchClass ui;

private slots:
	bool OnSettingSearch();     //���������������������
	bool OnSettingStart();
	bool OnSettingKeyArea();
	bool OnSettingEndSearch();
	bool OnSettingReStart();
	bool OnSettingScale();
	bool OnSettingDynamicRecover();

	//void OnSettingUAV(QTableWidgetItem* item);
	void OnSettingUAV(int, int);
	bool OnSettingDynamicUAV();
	int OnMouseTrace(QString lng, QString lat, QString containerX, QString containerY);
	int OnTableWidget(QString, QString);
	bool OnSavePoint(QVector<QPointF> uav, int index);      //��������
	bool OnSettingBase();
	void OnAddUAV();
	void OnAddEnd();
	void OnDeleteUAV();

	bool UAVLineStatus();           //��ȡ������ɫ
	int OnListContextMenu(const QPoint& pt);   //����Ҽ��˵�
	int OnMenuRemove();
	void datafromweb(QPointF pppp);
	bool OnMillitary_model();
	void OnLowToHighModel();          //�߽��Ⱥ͵;���ģʽ�л�

public:
	QVector<QString>matlab_uav_data;
	double current_scale = 0;
	void AddData(const QString& x, const QString& y);
	void setValueFromWeb(const QString &weblng, const QString &weblat);

private:
	QString lng;
	QString lat;
	QString position;
	QVector<QString>uav_data;

	QVector<int>BaseNumber;          //���˻�����
	QVector<int>uavnumber;               //�ж����˻��Ƿ�����
	QMap<int, QColor>uav_color;           //���˻���������ɫƥ��
	QPointF fromweb;                  //��ȡ����ҳ�ľ�γ������
	QMap<int, QString> uavtoheight;    //���˻���Ӧ�ĸ߶�

	bool millitary_model = false;
	bool low_to_high = false;
};