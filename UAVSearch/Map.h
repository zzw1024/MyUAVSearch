#pragma once

#include <QFrame>
#include <QMouseEvent>
#include <QVector>
#include <QLabel>
#include <QListWidget>
#include <qwebengineview.h>
#include <qwebchannel.h>
#include <iostream>
#include <algorithm>
#include <qmenu.h>
#include <qfile.h>
#include <qdialog.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <iostream>
#include <fstream> 
#include "engine.h"
#include "matrix.h"
#include "tmwtypes.h"

//�ǵý�lib�е�lib�ļ���ӿ���
using namespace std;


class Map : public QFrame
{
	Q_OBJECT

public:
	Map(QWidget *parent);
	~Map();

signals:
	void movepoint(QString lng, QString lat, QString containerX, QString containerY);
	void tablepoint(QString lng, QString lat);
	void savepoint(QVector<QPointF>, int);
	void sent(QPointF);

public:
	void SearchArea(bool millitary_model);
	void StartSearch(QVector<QString>matlab_uav_data, double scale, double minflight, double maxflight, QMap<int, QColor>LineColor);
	void StartKeyArea();
	void EndSearch();
	void ReStart();
	void dynamic_Rocover();
	void Dynanic_index(double index);
	void DeletePoint(int row);           //ɾ��ѡ��ľ�γ�ȵ�����
	bool WatchUAVLine(int line);  //�۲캽�ߺ���
	void LowModel();          //�;��Ⱥ͸߽���ģʽ�л�
	void HighModel();

private:
	void paintEvent(QPaintEvent * event);
	void timerEvent(QTimerEvent *event);
	// ���֧��
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void resizeEvent(QResizeEvent *event);
	void wheelEvent(QWheelEvent* event);
	void eventFilter(QEvent* event);
	bool event(QEvent * event);

public slots:
	int radar();        //�����״�
	int actiondanger_begin();		//����Σ������
	int actiondanger_end();
	//�������Ӧ�޸ģ����ɾ���ĺ���
	int actionpoint_change();
	int actionpoint_add();
	int actionpoint_delete();
	bool eventFilter(QObject *, QEvent *);          //��װinstallEventFilter�γɴ�͸Ч��
	void slotPopulateJavaScriptWindowObject();    //qt��jsע�����ĺ�������׼д��
	void onCall(QString lng, QString lat);         //js�ش����ݵ�qt�ĺ�����ע���ǲۺ���
	void container(QString x, QString y);
	void datatommap(QString x_lng, QString y_lat);               //�ӵ�ͼ�л�ȡ����Ӧ�����ص�����
	void mousemovedata(QString lng, QString lat, QString x, QString y);
	void sentbase(QString lng, QString lat, QString base);             //s�ֶ��������
	void GetBaseData(QString lng, QString lat, QString BaseX, QString BaseY, QString Base);             //��ȡ������Ϣ
	void GetPointData(QString lng, QString lat, QString BaseX, QString BaseY);                  //��ȡĿ������Ϣ�ӵ�ͼ��
	void ShowMapPointLnglat(QString lng, QString lat, QString containerX, QString containerY);

public:
	bool m_pressedFlag;
	bool m_matlabFlag;
	bool m_keyFlag;
	bool m_EndFlag;
	bool m_pathFlag;
	bool dynamic_status;   //�������˻���̬����
	QVector<QPoint> m_points;
	QVector<QPoint> m_keyPoints;
	QVector<QPointF>m_breakArea;   //�����ָ��;
	QVector<QPointF>m_UAV;
	QPolygonF polygon;
	QVector<int>m_number;
	QVector<int>m_length;
	QPoint  m_curPos; //  ��굱ǰλ��
	QPoint  m_start, m_end;
	int m_timeId;
	int dynamic_length = 100;
	int total_sum = 0;
	QVector<QPointF>dynamic_UAV;
	QVector<QPointF>temp_UAV;
	QVector<int>BaseData;           //��������

	float ratio;       //ͼƬ���ű�����С
	int action;          //����(�Ŵ�,��С,�ƶ�...)
	QPointF offset;              //һ�ε�ͼƬƫ��ֵ
	QPointF Alloffset;           //��ƫ��
	//��ͼ
	QPixmap pixmap;  //����ͼƬ
	int pixW;            //ͼƬ��
	int pixH;            //ͼƬ��
	//���»���
	QPixmap millitary_base;         //���ؾ��»���
	int militaryW;
	int militaryH;

	QPixmap radar_base;               //�����״�
	int radarW;
	int radarH;

	bool radar_status = false;     //�״�״̬
	QVector<float> radar_r;


	QVector<QVector<QPointF>>danger_area;    //������в�������
	bool danger_status = false;          //��в״̬�ر�
	int danger_number = 0;                //����Σ������

	QMap<int, QColor>GetLineColor;            //��ȡ������ɫ��ʶ
	bool GetLineColorStatus = false;                    //�Ƿ��ȡ����
	int CurChooseLine = 0;       //���û�ȡ��ȡ���ߵ�״̬
	int preChooseLine = -1;

	//�û��������޸ľ���
	QVector<QVector<QPointF>>ChoosePoint;
	bool pointchange_status = false;    //��ѡ��
	bool pointBeginChange_status = false;    //z�ҵ�Ҫ�޸ĺ����������޸�
	QPointF pointchange;            //��ȡ�޸ĺ�����
	//�û����������
	bool pointadd_status = false;        //���������״̬
	bool pointaddbegin_status = false;    //�������ѡ��
	QPoint pointadd;
	//�û�ɾ����·��
	bool pointdelete_status = false;
	bool pointdeletebegin_status = false;
	QPointF pointdelete;
	QVector<QString>Getlnglat;

	bool picture_model = false;
	QString picture_path;
	QVector<double>picture_lnglat;
	QString currentlng;
	QString currentlat;
	//��ȡ����״̬
public:
	enum  Type {
		None = 0,
		Amplification,
		Shrink,
		Lift,
		Right,
		Up,
		Down,
		Move
	};

private:
	QWebEngineView *view;   //���ص�ͼ

};
