#include "Map.h"
#include <qdir.h>
#include <qfile.h>


Map::Map(QWidget *parent)
	: QFrame(parent)
{
	setMouseTracking(true);//���������
	setAcceptDrops(true);//�����ק����

	view = new QWebEngineView(this);//��������
	view->load(QUrl(QDir::currentPath() + "/loadMap/index.html")); //���ص�ǰ·���µ�html��ͼ
	connect(view->page(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(slotPopulateJavaScriptWindowObject()));
	view->resize(600, 400);
	view->show();   //����֮����Ҫshow����
	
	picture_path = "./image/map.png";
	pixmap.load(picture_path);
	millitary_base.load("./image/military_base.png");
	radar_base.load("./image/radar.png");
	radarW = radar_base.width();
	radarH = radar_base.height();
	//��ͼ
	pixW = pixmap.width();
	pixH = pixmap.height();
	militaryW = millitary_base.width();
	militaryH = millitary_base.height();
	//���»���
	//���庽��������ľ����С
	ChoosePoint.resize(11);
	ratio = 1.0;
	Alloffset.setX(0);
	Alloffset.setY(0);
	m_start = m_end = QPoint(0, 0);
	m_pressedFlag = false;
	m_matlabFlag = false;
	m_keyFlag = false;
	m_EndFlag = false;
	m_pathFlag = false;
	dynamic_status = true;
	m_timeId = startTimer(500);
	danger_area.resize(20);
}

Map::~Map()
{
}

bool Map::eventFilter(QObject *target, QEvent *event)
{
	static bool press = false;
	static QPoint PreDot;
	if (event->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent *mouse = dynamic_cast<QMouseEvent*>(event);

		//�ж�����Ƿ����������,�����λ���Ƿ��ڻ滭����
		if (mouse->button() == Qt::LeftButton &&rect().contains(mouse->pos()))
		{
			press = true;
			setCursor(Qt::OpenHandCursor);   //���������ʽ

			PreDot = mouse->pos();
			qDebug() << PreDot;
			qDebug() << this->width();
			qDebug() << this->height();
		}

	}
	return 0;

}

void Map::resizeEvent(QResizeEvent *event)
{
	int width = this->width();        //��ȡʵʱ����
	int height = this->height();
	view->resize(width, height);       //����QFrame�Ĵ�С������ͼ��С
}

bool Map::event(QEvent* event)
{
	if (((!m_keyFlag && !radar_status) && !danger_status) && !pointchange_status)      //��ȡѡ������״��Լ�������ʱ����ֹ�ƶ������ź�point��״̬ͬ��
	{
		static bool press = false;
		static QPoint PreDot;

		if (event->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent *mouse = dynamic_cast<QMouseEvent*>(event);

			//�ж�����Ƿ����������,�����λ���Ƿ��ڻ滭����
			if (mouse->button() == Qt::LeftButton &&rect().contains(mouse->pos()))
			{
				press = true;
				setCursor(Qt::OpenHandCursor);   //���������ʽ

				PreDot = mouse->pos();
			}

		}
		else if (event->type() == QEvent::MouseButtonRelease)
		{
			QMouseEvent *mouse = dynamic_cast<QMouseEvent*>(event);

			//�ж�����Ƿ�������ͷ�,��֮ǰ���ڻ滭����
			if (mouse->button() == Qt::LeftButton && press)
			{
				setCursor(Qt::ArrowCursor);//�Ļ������ʽ 
				press = false;
			}
		}

		if (event->type() == QEvent::MouseMove)              //�ƶ�ͼƬ
		{
			if (press)
			{
				QMouseEvent *mouse = dynamic_cast<QMouseEvent*>(event);
				offset.setX(mouse->x() - PreDot.x());
				offset.setY(mouse->y() - PreDot.y());
				qDebug() << offset;
				PreDot = mouse->pos();
				action = Map::Move;
				this->update();
			}
		}
	}
	return QFrame::event(event);
	return 0;
}


void Map::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);

	//��ͼ�ı�����ص�
	int NowW = ratio * pixW;
	int NowH = ratio * pixH;
	//���»��ظı����ص�
	int NowMilitaryW = ratio * militaryW;
	int NowMilitaryH = ratio * militaryH;
	//�״�ĸı�
	int NowRadarW = ratio * radarW;
	int NowRadarH = ratio * radarH;

	int width = this->width();
	int height = this->height();
	if (action == Map::Amplification)           //��С
	{
		ratio -= 0.1*ratio;
		if (ratio < 0.018)
			ratio = 0.01;
	}
	else  if (action == Map::Shrink)           //�Ŵ�
	{

		ratio += 0.1*ratio;
		if (ratio > 4.5)
			ratio = 5.000;
	}
	if (action == Map::Amplification || action == Map::Shrink)      //����ͼƬ
	{
		NowW = ratio * pixW;
		NowH = ratio * pixH;

		NowMilitaryW = ratio * militaryW;
		NowMilitaryH = ratio * militaryH;

		NowRadarW = ratio * radarW;
		NowRadarH = ratio * radarH;

		pixmap.load(picture_path);      //����װ��,��Ϊ֮ǰ��ͼƬ�Ѿ������Ź�
		millitary_base.load("./image/military_base.png");
		radar_base.load("./image/radar.png");
		pixmap = pixmap.scaled(NowW, NowH, Qt::KeepAspectRatio);
		millitary_base = millitary_base.scaled(NowMilitaryW, NowMilitaryH, Qt::KeepAspectRatio);
		radar_base = radar_base.scaled(NowRadarW, NowRadarH, Qt::KeepAspectRatio);
		action = Map::None;
	}
	if (action == Map::Move)                    //�ƶ�
	{
		float offsetx = Alloffset.x() + offset.x();
		Alloffset.setX(offsetx);

		float offsety = Alloffset.y() + offset.y();
		Alloffset.setY(offsety);
		action = Map::None;
	}

	QRect target(0, 0, width, height);
	QRect source(-Alloffset.x(), -Alloffset.y(), width, height);
	painter.drawPixmap(target, pixmap, source);

	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setBrush(QColor(0xff, 0x00, 0x00));
	painter.translate(Alloffset.x(), Alloffset.y());         // ʵ�����ź�ƽ�ƵĹؼ�����ƽ�ƺ�����
	painter.scale(ratio, ratio);
	for (int i = 0; i < BaseData.size() / 3; i++)
	{
		float x = BaseData[i * 3];
		float y = BaseData[i * 3 + 1];
		QRect target1(x, y, 32, 32);
		QRect source1(0, 0, 32, 32);
		painter.drawPixmap(target1, millitary_base, source1);
	}
	for (int i = 0; i < radar_r.size() / 3; i++)
	{
		float x = radar_r[i * 3];
		float y = radar_r[i * 3 + 1];
		QRect target2(x, y, 16, 16);
		QRect source2(0, 0, 16, 16);
		painter.drawPixmap(target2, radar_base, source2);
	}


	QFont font("Times", 12, QFont::Bold, true);
	painter.setFont(font);
	painter.setPen(Qt::red);
	for (int i = 0; i < BaseData.size() / 3; i++)
	{
		painter.drawText(BaseData[i * 3] + 10, BaseData[i * 3 + 1] + 10, QString::number(BaseData[i * 3 + 2]) + QString::fromLocal8Bit("��"));
		painter.drawEllipse(BaseData[i * 3], BaseData[i * 3 + 1], 5, 5);
	}
	painter.setBrush(QColor(0x00, 0x00, 0xff));
	painter.setPen(QColor(0x00, 0x00, 0xff));
	for (int i = 0; i < m_keyPoints.size(); i++)
	{
		QPoint p1 = m_points[i];
		painter.drawEllipse(p1, 5, 5);
	}
	if (m_EndFlag == true && m_keyFlag == false)
	{
		painter.setBrush(QColor(0x00, 0xff, 0x00, 100));
	}
	painter.setPen(QColor(0xff, 0xb7, 0xdd));
	painter.setBrush(QColor(0, 0, 0, 100));
	for (int i = 0; i < radar_r.size() / 3; i++)       //�״���췶Χ
	{
		int x = radar_r[i * 3];
		int y = radar_r[i * 3 + 1];
		float r = radar_r[i * 3 + 2];
		QPointF p;
		p.setX(x);
		p.setY(y);
		painter.drawEllipse(p, r, r);
	}
	if (radar_status == true)       //��̬�����״���췶Χ
	{
		int x = radar_r[(radar_r.size() / 3) * 3] - 3;
		int y = radar_r[(radar_r.size() / 3) * 3 + 1] - 3;
		float r = sqrt(pow(abs(x - m_curPos.x()), 2) + pow(abs(x - m_curPos.x()), 2));
		QPointF p;
		p.setX(x);
		p.setY(y);
		painter.drawEllipse(p, r, r);
	}
	for (int i = 0; i < danger_area.size(); i++)
	{
		QPointF p[20];          //���һ�������20����
		int j = 0;
		for (; j < danger_area[i].size(); j++)
		{
			painter.setPen(QColor(0xff, 0xb7, 0xdd));
			p[j] = danger_area[i][j];
			painter.drawEllipse(p[j], 3, 3);
		}
		painter.setPen(QColor(0xff, 0xb7, 0xdd));
		painter.setBrush(QColor(0, 0, 0, 100));
		if (j < 20)
		{
			painter.drawPolygon(p, danger_area[i].size());
		}
		if (j > 20)
		{
			painter.drawPolygon(p, 20);
		}
	}
	if (!m_breakArea.empty())    //���Ʒָ������
	{
		QPointF p = m_breakArea[0];
		painter.setPen(QPen(QColor(0xff, 0x00, 0x00), 2, Qt::SolidLine));
		QPointF p1;
		QPointF p2;
		for (int i = 0; i < m_breakArea.size() - 1; i++)
		{
			p1 = m_breakArea[i];
			p2 = m_breakArea[i + 1];
			painter.drawLine(p1, p2);
		}
		painter.drawLine(p2, p);
	}
	painter.setPen(QPen(QColor(0x00, 0x00, 0xff)));
	for (int i = 0; i < m_UAV.size(); i++)
	{
		painter.drawEllipse(m_UAV[i], 2, 2);
	}
	painter.setPen(QPen(QColor(0x00, 0x00, 0x00)));

	QVector<QPointF>path;
	painter.setBrush((QColor(0x00, 0x00, 0x00)));
	if (m_pathFlag&&dynamic_status)
	{
		path = m_UAV;
		for (int i = 0; i < m_length.size(); i++)
		{
			QVector<QPointF>temp;
			int uav;
			painter.setPen(QPen(GetLineColor[m_number[i]]));
			for (int j = 0; j < m_length[i] / 2 - 1; j++)
			{
				QPointF& p1 = path[j];
				QPointF& p2 = path[j + 1];
				temp.push_back(p1);
				temp.push_back(p2);
				painter.drawLine(p1, p2);
			}
			path.erase(path.begin(), path.begin() + m_length[i] / 2);
			temp.clear();
		}
	}
	if (GetLineColorStatus == true)
	{
		painter.setPen(QPen(GetLineColor[CurChooseLine], 3));
		if (ChoosePoint[CurChooseLine].empty())
		{
			return;
		}
		//��ȡҪ��ʾ�ĺ���
		int number = -1;
		int need_number = 0;
		temp_UAV.clear();
		for (int i = 0; i < m_number.size(); i++)
		{
			if (CurChooseLine == m_number[i])
			{
				number = i;
				break;
			}
		}
		if (number == -1)
		{
			return;
		}
		for (int j = 0; j <= number; j++)
		{
			need_number = need_number + m_length[j] / 2;
		}
		for (int i = need_number - m_length[number] / 2; i < need_number; i++)
		{
			temp_UAV.push_back(m_UAV[i]);
		}
		QPointF p = temp_UAV[0];
		QPointF p1;
		QPointF p2;
		for (int i = 0; i < temp_UAV.size() - 1; i++)
		{
			p1 = temp_UAV[i];
			p2 = temp_UAV[i + 1];
			painter.drawLine(p1, p2);
		}
		painter.drawLine(p2, p);
	}

	if ((!dynamic_status&&m_pathFlag))
	{
		painter.setPen(QPen(QColor(0x00, 0x00, 0xff)));
		for (int i = 0; i < dynamic_UAV.size() - 1; i++)
		{
			QPointF& p1 = dynamic_UAV[i];
			QPointF& p2 = dynamic_UAV[i + 1];
			painter.drawLine(p1, p2);
		}
	}
}

void Map::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_timeId)
	{
		if (!dynamic_status&&m_pathFlag)
		{
			for (int i = 0; i < 1; i++)
			{
				total_sum++;
				if (total_sum > dynamic_length)
				{
					break;
				}
				QPointF& p1 = temp_UAV[i];
				dynamic_UAV.push_back(p1);
			}
			if (!temp_UAV.empty())
			{
				if (temp_UAV.size() >= 1)
				{
					temp_UAV.erase(temp_UAV.begin(), temp_UAV.begin() + 1);
				}
				else
				{
					temp_UAV.erase(temp_UAV.begin(), temp_UAV.end());
				}
			}
			update();
		}
	}
}

void Map::Dynanic_index(double index)
{
	if (ChoosePoint[index].empty())
	{
		return;
	}
	int number = -1;
	int need_number = 0;
	temp_UAV.clear();
	dynamic_UAV.clear();
	dynamic_length = 0;
	total_sum = 0;
	for (int i = 0; i < m_number.size(); i++)
	{
		if (index == m_number[i])
		{
			number = i;
			break;
		}
	}
	if (number == -1)
	{
		return;
	}
	for (int j = 0; j <= number; j++)
	{
		need_number = need_number + m_length[j] / 2;
	}
	for (int i = need_number - m_length[number] / 2; i < need_number; i++)
	{
		temp_UAV.push_back(m_UAV[i]);
	}
	dynamic_length = temp_UAV.size();
	dynamic_status = false;
}

void Map::dynamic_Rocover()
{
	temp_UAV.clear();
	dynamic_UAV.clear();
	dynamic_length = 0;
	total_sum = 0;
	dynamic_status = true;
	update();
}

bool Map::WatchUAVLine(int line)          //�۲캽�ߵĺ���
{
	CurChooseLine = line;
	if (preChooseLine == CurChooseLine)
	{
		if (GetLineColorStatus == false)
		{
			GetLineColorStatus = true;
		}
		else
		{
			GetLineColorStatus = false;
		}
	}
	else
	{
		GetLineColorStatus = true;
	}
	preChooseLine = CurChooseLine;
	update();
	return 0;
}

void Map::mouseMoveEvent(QMouseEvent * event)
{

	//ͼ�д�������ƽ��ʱ��Ҫ�ڴ���������ʵ�֣��������漰���������ź��ƶ�״̬�¡�
	m_curPos = event->pos();
	m_curPos.setX(m_curPos.x() - Alloffset.x());
	m_curPos.setY(m_curPos.y() - Alloffset.y());
	m_curPos = m_curPos / ratio;
	if (picture_model == false)
	{
		if (m_curPos.x() < 0 || m_curPos.x() > pixW)
		{
			emit movepoint("0", "0", QString::number(m_curPos.x()), QString::number(m_curPos.y()));
		}
		else if (m_curPos.y() < 0 || m_curPos.y() > pixH)
		{
			emit movepoint("0", "0", QString::number(m_curPos.x()), QString::number(m_curPos.y()));
		}
		else
		{
			QString cmd = QString("pictureGetLnglat(%1,%2)").arg(QString::number(m_curPos.x()), QString::number(m_curPos.y()));
			view->page()->runJavaScript(cmd);
		}
	}
	else
	{
		if (m_curPos.x() < 0 || m_curPos.x() > pixW)
		{
			emit movepoint("0", "0", QString::number(m_curPos.x()), QString::number(m_curPos.y()));
		}
		else if (m_curPos.y() < 0 || m_curPos.y() > pixH)
		{
			emit movepoint("0", "0", QString::number(m_curPos.x()), QString::number(m_curPos.y()));
		}
		else
		{
			double maxLon = max(picture_lnglat[0], picture_lnglat[2]);
			double minLon = min(picture_lnglat[0], picture_lnglat[2]);
			double maxLat = max(picture_lnglat[1], picture_lnglat[3]);
			double minLat = min(picture_lnglat[1], picture_lnglat[3]);
			int y = pixmap.width();
			int h = pixmap.height();
			double scaleX = ((maxLon - minLon) * 3600) / y;
			double scaleY = ((maxLat - minLat) * 3600) / h;
			double lng = m_curPos.x() * scaleX / 3600 + minLon;
			double lat = maxLat - m_curPos.y()* scaleY / 3600;
			emit movepoint(QString::number(lng, 10, 6), QString::number(lat, 10, 6),QString::number(m_curPos.x()), QString::number(m_curPos.y()));
		}
	}
	if ((pointchange_status == true || pointadd_status == true) || pointdelete_status == true)
	{
		for (int i = 0; i < m_length.size(); i++)
		{
			for (int j = 0; j < ChoosePoint[m_number[i]].size(); j++)
			{
				if (fabs(m_curPos.x() - ChoosePoint[m_number[i]][j].x()) <= 5 && fabs(m_curPos.y() - ChoosePoint[m_number[i]][j].y()) <= 5)
				{
					setCursor(Qt::CrossCursor);
				}
				else
				{
					setCursor(Qt::ArrowCursor);
				}
			}
		}
	}
	if (pointBeginChange_status == true)
	{
		ChoosePoint[pointchange.x()][pointchange.y()] = m_curPos;
	}
	update();
}

void Map::mousePressEvent(QMouseEvent * event)
{

	m_start = m_end = event->pos();
	m_start.setX(m_start.x() - Alloffset.x());
	m_start.setY(m_start.y() - Alloffset.y());
	m_end.setX(m_end.x() - Alloffset.x());
	m_end.setY(m_end.y() - Alloffset.y());
	m_start = m_end = m_start / ratio; // ��¼��ʼ�ĵ�

	m_curPos = event->pos();
	m_curPos.setX(m_curPos.x() - Alloffset.x());
	m_curPos.setY(m_curPos.y() - Alloffset.y());
	m_curPos = m_curPos / ratio;
	if (event->button() & Qt::LeftButton)
	{
		if (m_keyFlag)
		{
			m_points.append(m_curPos);
			m_keyPoints.append(m_curPos);
			polygon.push_back(m_curPos);
			if (picture_model == false)
			{
				QString cmd = QString("pictureGetLnglat(%1,%2)").arg(QString::number(m_curPos.x()), QString::number(m_curPos.y()));
				view->page()->runJavaScript(cmd);
				Getlnglat.push_back(currentlng);
				Getlnglat.push_back(currentlat);
				emit tablepoint(currentlng, currentlat);
				//emit tablepoint(QString::number(m_curPos.x()), QString::number(m_curPos.y()));
			}
			else
			{
				double maxLon = max(picture_lnglat[0], picture_lnglat[2]);
				double minLon = min(picture_lnglat[0], picture_lnglat[2]);
				double maxLat = max(picture_lnglat[1], picture_lnglat[3]);
				double minLat = min(picture_lnglat[1], picture_lnglat[3]);
				int y = pixmap.width();
				int h = pixmap.height();
				double scaleX = ((maxLon - minLon) * 3600) / y;
				double scaleY = ((maxLat - minLat) * 3600) / h;
				double lng = m_curPos.x() * scaleX / 3600 + minLon;
				double lat = maxLat - m_curPos.y()* scaleY / 3600;
				currentlng = QString::number(lng, 10, 8);
				currentlat = QString::number(lat, 10, 8);
				Getlnglat.push_back(currentlng);
				Getlnglat.push_back(currentlat);
				emit tablepoint(currentlng, currentlat);
			}
		}
	}
	if (!m_keyFlag)        //ѡ���ǲ�������״�
	{
		if (event->button() & Qt::RightButton)
		{
			// ���˵�
			QMenu Mainmenu(this);
			QAction *aciton_radar = new QAction(&Mainmenu);
			QAction *aciton_danger = new QAction(&Mainmenu);
			QAction *action_point = new QAction(&Mainmenu);
			aciton_radar->setText(QString::fromLocal8Bit("�״�"));
			aciton_danger->setText(QString::fromLocal8Bit("��в��"));
			action_point->setText(QString::fromLocal8Bit("������"));
			QList<QAction*> actionList;
			actionList << aciton_radar << aciton_danger << action_point;
			//���������˵�
			Mainmenu.addActions(actionList);

			//Σ�������Ӳ˵�
			QMenu childMenu_danger;
			//����Σ������
			QAction *danger_begin = new QAction(&childMenu_danger);
			danger_begin->setText(QString::fromLocal8Bit("��ʼ"));
			//����Σ������
			QAction *danger_end = new QAction(&childMenu_danger);
			danger_end->setText(QString::fromLocal8Bit("����"));
			//����Ӳ˵���Ŀ
			QList<QAction *> childActionList;
			childActionList << danger_begin << danger_end;
			childMenu_danger.addActions(childActionList);
			//���˵�����Ӳ˵�
			aciton_danger->setMenu(&childMenu_danger);

			//�������Ӳ˵�
			QMenu childMenu_point;
			//�޸ĺ�����λ��
			QAction *point_change = new QAction(&childMenu_point);
			point_change->setText(QString::fromLocal8Bit("�ƶ�"));
			//��Ӻ�����
			QAction *point_add = new QAction(&childMenu_point);
			point_add->setText(QString::fromLocal8Bit("���"));
			//ɾ��������
			QAction *point_delete = new QAction(&childMenu_point);
			point_delete->setText(QString::fromLocal8Bit("ɾ��"));
			//����Ӳ˵���Ŀ
			QList<QAction*>childpointList;
			childpointList << point_change << point_add << point_delete;
			childMenu_point.addActions(childpointList);
			//���˵�����Ӳ˵�
			action_point->setMenu(&childMenu_point);

			connect(aciton_radar, SIGNAL(triggered()), this, SLOT(radar()));
			connect(danger_begin, SIGNAL(triggered()), this, SLOT(actiondanger_begin()));
			connect(danger_end, SIGNAL(triggered()), this, SLOT(actiondanger_end()));

			connect(point_change, SIGNAL(triggered()), this, SLOT(actionpoint_change()));
			connect(point_add, SIGNAL(triggered()), this, SLOT(actionpoint_add()));
			connect(point_delete, SIGNAL(triggered()), this, SLOT(actionpoint_delete()));

			Mainmenu.exec(QCursor::pos());
		}
	}
	if ((event->button() & Qt::LeftButton) && pointchange_status == true)
	{
		for (int i = 0; i < m_length.size(); i++)
		{
			for (int j = 0; j < ChoosePoint[m_number[i]].size(); j++)
			{
				if (fabs(m_curPos.x() - ChoosePoint[m_number[i]][j].x()) <= 5 && fabs(m_curPos.y() - ChoosePoint[m_number[i]][j].y()) <= 5)
				{
					pointBeginChange_status = true;
					pointchange.setX(m_number[i]);      //��ȡ�޸ĺ����������
					pointchange.setY(j);
					setCursor(Qt::CrossCursor);

				}
				else
				{
					setCursor(Qt::ArrowCursor);
				}
			}
		}
	}
	if ((event->button() & Qt::LeftButton) && pointadd_status == true)
	{
		for (int i = 0; i < m_length.size(); i++)
		{
			for (int j = 0; j < ChoosePoint[m_number[i]].size(); j++)
			{
				if (fabs(m_curPos.x() - ChoosePoint[m_number[i]][j].x()) <= 5 && fabs(m_curPos.y() - ChoosePoint[m_number[i]][j].y()) <= 5)
				{
					pointaddbegin_status = true;
					pointadd_status = false;
					pointadd.setX(m_number[i]);
					pointadd.setY(j);
					setCursor(Qt::CrossCursor);

				}
				else
				{
					setCursor(Qt::ArrowCursor);
				}
			}
		}
	}

	if ((event->button() & Qt::LeftButton) && pointdelete_status == true)
	{
		for (int i = 0; i < m_length.size(); i++)
		{
			for (int j = 0; j < ChoosePoint[m_number[i]].size(); j++)
			{
				if (fabs(m_curPos.x() - ChoosePoint[m_number[i]][j].x()) <= 5 && fabs(m_curPos.y() - ChoosePoint[m_number[i]][j].y()) <= 5)
				{
					pointdeletebegin_status = true;
					pointdelete_status = false;
					pointdelete.setX(m_number[i]);
					pointdelete.setY(j);
					setCursor(Qt::CrossCursor);

				}
				else
				{
					setCursor(Qt::ArrowCursor);
				}
			}
		}
	}
}

void Map::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (radar_status == true)
	{
		int x = radar_r[(radar_r.size() / 3) * 3];
		int y = radar_r[(radar_r.size() / 3) * 3 + 1];
		float r = sqrt(pow(abs(x - m_curPos.x()), 2) + pow(abs(x - m_curPos.x()), 2));
		radar_r.push_back(r);
		radar_status = false;
	}
	if ((event->button() & Qt::LeftButton) && pointaddbegin_status == true)
	{
		m_UAV.clear();
		QVector<QPointF>::iterator index = ChoosePoint[pointadd.x()].begin() + pointadd.y();
		ChoosePoint[pointadd.x()].insert(index, m_curPos);
		int m = 0;
		for (0; m < m_number.size(); m++)
		{
			if (m_number[m] == pointadd.x())
			{
				break;
			}
		}
		m_length[m] = m_length[m] + 2;
		for (int i = 0; i < m_length.size(); i++)
		{
			for (int j = 0; j < ChoosePoint[m_number[i]].size(); j++)
			{
				m_UAV.push_back(ChoosePoint[m_number[i]][j]);
			}
		}
		pointaddbegin_status = false;
		pointadd_status = false;
		setCursor(Qt::ArrowCursor);
	}
	update();
}

void Map::mouseReleaseEvent(QMouseEvent * event)
{
	if (danger_status == true)
	{
		danger_area[danger_number].push_back(m_curPos);
	}
	if (pointchange_status == true && pointBeginChange_status == true)
	{
		m_UAV.clear();
		for (int i = 0; i < m_length.size(); i++)
		{
			for (int j = 0; j < ChoosePoint[m_number[i]].size(); j++)
			{
				m_UAV.push_back(ChoosePoint[m_number[i]][j]);
			}
		}
		pointBeginChange_status = false;
		pointchange_status = false;
		setCursor(Qt::ArrowCursor);
	}
	if (pointdeletebegin_status == true)
	{
		m_UAV.clear();
		int y = pointdelete.y();
		QVector<QPointF>::iterator index = ChoosePoint[pointdelete.x()].begin() + y;
		ChoosePoint[pointdelete.x()].erase(index);
		int m = 0;
		for (0; m < m_number.size(); m++)
		{
			if (m_number[m] == pointdelete.x())
			{
				break;
			}
		}
		m_length[m] = m_length[m] - 2;
		for (int i = 0; i < m_length.size(); i++)
		{
			for (int j = 0; j < ChoosePoint[m_number[i]].size(); j++)
			{
				m_UAV.push_back(ChoosePoint[m_number[i]][j]);
			}
		}
		pointdeletebegin_status = false;
	}
	update();
}

void Map::wheelEvent(QWheelEvent* event)
{
	if (event->delta() > 0)
	{
		action = Map::Shrink;
		update();
	}
	else
	{
		action = Map::Amplification;
		update();
	}
}

void Map::DeletePoint(int row)
{
	//ɾ��tablewwidget������ʱ��ͬ������ͼ����
	auto index1 = m_points.begin() + row;
	auto index2 = m_keyPoints.begin() + row;
	if (m_points.empty())
	{
		return;
	}
	if (m_keyPoints.empty())
	{
		return;
	}
	m_points.erase(index1);
	m_keyPoints.erase(index2);
	QString cmd = QString("Deletepointlnglat(%1)").arg(QString::number(row));
	view->page()->runJavaScript(cmd);
	update();
}

int Map::radar()            //�״�Ч��
{
	radar_r.push_back(m_curPos.x());
	radar_r.push_back(m_curPos.y());

	radar_status = true;
	return 0;
}

int Map::actiondanger_begin()          //Σ��������
{
	danger_status = true;
	return 0;
}

int Map::actiondanger_end()          //Σ��������
{
	danger_status = false;
	danger_number++;
	return 0;
}

int Map::actionpoint_change()       //�������λ���޸�
{
	pointchange_status = true;
	return 0;
}

int Map::actionpoint_add()        //��Ӻ�����
{
	pointadd_status = true;
	return 0;
}

int Map::actionpoint_delete()         //ɾ��������
{
	pointdelete_status = true;
	return 0;
}

void Map::SearchArea(bool mmillitary_model)
{
	if (mmillitary_model == false)
	{
		container("300", "300");
		picture_model = false;
	}
	else
	{
		picture_model = true;
		QString filepath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ���ļ�"));
		qDebug() << filepath;
		if (filepath.isEmpty())
		{
			QMessageBox mesg;
			mesg.warning(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("��ͼƬʧ��!"));
			return;
		}
		else
		{
			picture_path = filepath;
			view->hide();

			pixmap.load(picture_path);
			pixW = pixmap.width();
			pixH = pixmap.height();
		}
		double lng1 = 118.06114196;
		double lat1 = 24.557116164;
		double lng2 = 118.19246292;
		double lat2 = 24.475743901;
		picture_lnglat.push_back(lng1);
		picture_lnglat.push_back(lat1);
		picture_lnglat.push_back(lng2);
		picture_lnglat.push_back(lat2);
	}
	//��ʱδ�õ�
}
void Map::StartKeyArea()
{
	m_keyFlag = true;
	m_EndFlag = false;
	QString cmd = "OpenPointStatus()";
	view->page()->runJavaScript(cmd);
}

void Map::EndSearch()
{
	m_EndFlag = true;
	m_keyFlag = false;
	QString cmd = "ClosePointStatus()";
	view->page()->runJavaScript(cmd);
	update();
}

void Map::StartSearch(QVector<QString>matlab_uav_data, double scale, double minflight, double maxflight, QMap<int, QColor>LineColor)
{
	if (!Getlnglat.empty())
	{
		double EARTH_RADIUS = 6378137;       //����뾶
		double PI = 3.141592654;              // PI
		int a = m_points[0].x();
		int b = m_points[1].x();
		double a_lng = Getlnglat[0].toDouble()*PI / 180;
		double b_lng = Getlnglat[2].toDouble()*PI / 180;
		double a_lat = Getlnglat[1].toDouble()*PI / 180;
		double b_lat = Getlnglat[3].toDouble()*PI / 180;
		double ab_lng = a_lng - b_lng;
		double ab_lat = a_lat - b_lat;                 //�������㾭γ�������
		double s = 2 * asin(sqrt(pow(sin(ab_lat / 2), 2) + cos(a_lat)*cos(b_lat)*pow(sin(ab_lng / 2), 2)));
		s = s * EARTH_RADIUS;
		scale = s / abs(a - b);

	}
	if (scale == 0)
	{
		QMessageBox::information(NULL, "title", "No Scale");
		return;
	}
	if (!m_UAV.empty())
	{
		m_UAV.clear();
		ChoosePoint.clear();
		ChoosePoint.resize(11);
		m_breakArea.clear();
		m_number.clear();
		m_length.clear();
	}
	if (m_keyPoints.size() < 4 && m_points.size() < 4)
	{
		QMessageBox::information(NULL, "title", QString::fromLocal8Bit("�ؼ��ڵ���Ϣ����"));
		return;
	}
	vector<double>data_key;       //ѡ��ĵ��λ��  
	vector<double>uav_data;        //���˻�����
	vector<double>search_data;    //��Ӧ��������ͻ���λ��
	for (int i = 0; i < matlab_uav_data.size(); i++)
	{
		QString result;
		QString temp = matlab_uav_data[i];
		for (int j = 0; j < temp.size(); j++)
		{
			if ((temp[j] >= '0'&&temp[j] <= '9') || temp[j] == '.')
			{
				result = result + temp[j];
			}
		}
		uav_data.push_back(result.toDouble());
	}
	for (int i = 0; i < m_keyPoints.size(); i++)
	{
		QPoint &p = m_keyPoints[i];
		data_key.push_back(p.x());
		data_key.push_back(p.y());
		qDebug("%d,%d", p.x(), p.y());
	}
	if (data_key.empty())
	{
		QMessageBox::information(NULL, "title", "No Area Data");
		return;
	}
	int width = this->width();
	int height = this->height();
	qDebug() << width;
	qDebug() << height;
	search_data.push_back(width);
	search_data.push_back(height);
	search_data.push_back(0);
	search_data.push_back(0);
	search_data.push_back(0);
	search_data.push_back(0);
	search_data.push_back(0);
	search_data.push_back(0);
	for (int i = 0; i < BaseData.size() / 3; i++)
	{
		int j = BaseData[i * 3 + 2];
		search_data[j * 2] = BaseData[i * 3];
		search_data[j * 2 + 1] = BaseData[i * 3 + 1];
	}
	if (uav_data.empty())
	{
		QMessageBox::information(NULL, "title", "No UAV Data");
		return;
	}
	QMessageBox msgBox(QMessageBox::Warning, "title", "Start Just Waiting", QMessageBox::NoButton);
	GetLineColor = LineColor;
	msgBox.setModal(false);
	msgBox.show();
	Engine *ep;
	mxArray *UAV_path;
	mxArray *UAV_length;
	mxArray *UAV_number;
	mxArray *UAV_warning;
	mxArray *UAV_Dis;
	mxArray *Run_Time;
	mxArray *BreakArea;
	mxArray *resRate;
	mxArray *T1 = mxCreateDoubleMatrix(1, data_key.size(), mxREAL);
	mxArray *T2 = mxCreateDoubleMatrix(1, uav_data.size(), mxREAL);
	mxArray *T3 = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *T4 = mxCreateDoubleMatrix(1, search_data.size(), mxREAL);
	mxArray *T5 = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *T6 = mxCreateDoubleMatrix(1, 1, mxREAL);
	if (!(ep = engOpen("\0")))
	{
		fprintf(stderr, "\ncan't start matlab engine\n");
	}
	bool nstatus = engSetVisible(ep, true);
	if (nstatus != 0)
	{
		cout << "����MATLAB��ʾʧ��" << endl;
	}
	else
	{
		engEvalString(ep, "state = enableservice('AutomationServer', true);");
		memcpy((void *)mxGetPr(T1), (void *)(&data_key[0]), data_key.size() * sizeof(double));
		memcpy((void *)mxGetPr(T2), (void *)(&uav_data[0]), uav_data.size() * sizeof(double));
		memcpy((void *)mxGetPr(T3), (void *)(&scale), sizeof(double));
		memcpy((void *)mxGetPr(T4), (void *)(&search_data[0]), search_data.size() * sizeof(double));
		memcpy((void *)mxGetPr(T5), (void *)(&minflight), sizeof(double));
		memcpy((void *)mxGetPr(T6), (void *)(&maxflight), sizeof(double));
		engPutVariable(ep, "areaPosition_", T1);
		engPutVariable(ep, "UAVData_", T2);
		engPutVariable(ep, "mapScale_", T3);
		engPutVariable(ep, "searchData_", T4);
		engPutVariable(ep, "minFlightLen_", T5);
		engPutVariable(ep, "maxFlightLen_", T6);
		QString path = QDir::currentPath();
		path = "cd " + path + "/MyFile";
		string cstr;
		cstr = string((const char *)path.toLocal8Bit());
		const char* CurrentPAHT = cstr.c_str();
		engEvalString(ep, CurrentPAHT);
		engEvalString(ep, "[isWaring,cutAreaSpots,spotsNums,UAVNums, UAVFlyKeySpots,UAVFlyDis,runTime,resRate] = main(areaPosition_,UAVData_,mapScale_,searchData_,minFlightLen_,maxFlightLen_);");

		UAV_length = engGetVariable(ep, "spotsNums");
		UAV_number = engGetVariable(ep, "UAVNums");
		UAV_path = engGetVariable(ep, "UAVFlyKeySpots");
		UAV_warning = engGetVariable(ep, "isWaring");
		UAV_Dis = engGetVariable(ep, "UAVFlyDis");
		BreakArea = engGetVariable(ep, "cutAreaSpots");
		Run_Time = engGetVariable(ep, "runTime");
		resRate = engGetVariable(ep, "resRate");
		if (UAV_path == NULL)
		{
			QMessageBox::information(NULL, "title", "False Try again");
			return;
		}
		double *p1 = mxGetPr(UAV_path);
		double *p2 = mxGetPr(UAV_length);
		double *p3 = mxGetPr(UAV_number);
		double *p4 = mxGetPr(UAV_warning);
		double *p5 = mxGetPr(UAV_Dis);
		double *p6 = mxGetPr(Run_Time);
		double *p7 = mxGetPr(BreakArea);
		double *p8 = mxGetPr(resRate);
		engEvalString(ep, "clear;clc;");   //���matlab������

		size_t K = mxGetN(UAV_number);
		if (!m_number.empty())
		{
			m_number.clear();

		}
		if (!m_length.empty())
		{
			m_length.clear();
		}
		for (int i = 0; i < K; i++)
		{
			m_number.push_back(*p3);
			p3++;
		}
		for (int i = 0; i < m_number.size(); i++)
		{
			m_length.push_back(0);
		}
		size_t M = mxGetN(UAV_length);
		for (int i = 0; i < M; i++)
		{
			m_length[i] = (*p2) * 2;
			qDebug() << *p2 * 2;
			p2++;
		}

		size_t warning_n = mxGetN(UAV_warning);
		for (int i = 0; i < warning_n; i++)
		{
			int index = *p4;
			if (index % 10 == 1)
			{
				QMessageBox::information(NULL, "title", QString::fromLocal8Bit("��������"));
				return;
			}
			if (index % 10 == 2)
			{
				QString temp = QString::number(m_number[i]);
				QMessageBox::information(NULL, "title", QString::fromLocal8Bit("���˻�") + temp + QString::fromLocal8Bit("�ĺ���̫��"));
				return;
			}
			if (index % 10 == 3)
			{
				QString temp = QString::number(m_number[i]);
				QMessageBox::information(NULL, "title", QString::fromLocal8Bit("���˻�") + temp + QString::fromLocal8Bit("�ĺ��̲���"));
				return;
			}
			p4++;
		}
		size_t N = mxGetN(UAV_path);
		if (!m_UAV.empty())
		{
			m_UAV.clear();
		}
		for (int i = 0; i < N / 2; i++)
		{
			QPointF temp;
			temp.setX(*p1);
			p1++;
			temp.setY(*p1);
			p1++;
			m_UAV.append(temp);
		}
		QVector<int>save_pointLength;
		N = mxGetN(UAV_length);
		for (int i = 0; i < N; i++)
		{
			save_pointLength.push_back(m_length[i]);
		}
		int k = 0;
		for (int i = 0; i < m_number.size(); i++)
		{
			for (int j = 0; j < m_length[i] / 2; j++)
			{
				ChoosePoint[m_number[i]].push_back(m_UAV[k]);
				k++;
			}
		}

		//��ȡ�ָ�������
		size_t R = mxGetN(BreakArea);
		if (!m_breakArea.empty())
		{
			m_breakArea.clear();
		}
		for (int i = 0; i < R / 2; i++)
		{
			QPointF temp;
			temp.setX(*p7);
			p7++;
			temp.setY(*p7);
			p7++;
			m_breakArea.append(temp);
		}
		size_t efficiency = mxGetN(resRate);
		if (efficiency > 0)
		{
			ofstream EfficiencyFile;
			EfficiencyFile.open("Ч�ʶԱ�.txt", ios::out);
			EfficiencyFile << "�Ա�Ч��: " << *p8;
			EfficiencyFile << "\n";
			EfficiencyFile.close();
			qDebug() << *p8;
		}
		for (int i = 0; i < m_number.size(); i++)
		{
			QVector<QPointF>temp;
			int uav;
			temp = ChoosePoint[m_number[i]];
			uav = m_number[i];
			if (picture_model == false)
			{
				emit savepoint(temp, uav);
			}
			else
			{
				QVector<QPointF>lnglatSet;
				double maxLon = max(picture_lnglat[0], picture_lnglat[2]);
				double minLon = min(picture_lnglat[0], picture_lnglat[2]);
				double maxLat = max(picture_lnglat[1], picture_lnglat[3]);
				double minLat = min(picture_lnglat[1], picture_lnglat[3]);
				int y = pixmap.width();
				int h = pixmap.height();
				double scaleX = ((maxLon - minLon) * 3600) / y;
				double scaleY = ((maxLat - minLat) * 3600) / h;
				double lng;
				double lat;
				for (int i = 0; i < temp.size(); i++)
				{
					double lng = temp[i].x() * scaleX / 3600 + minLon;
					double lat = maxLat - temp[i].y()* scaleY / 3600;
					QPointF pixeltolnglat;
					pixeltolnglat.setX(lng);
					pixeltolnglat.setY(lat);
					lnglatSet.push_back(pixeltolnglat);
				}
				emit savepoint(lnglatSet, uav);
				lnglatSet.clear();
			}
			temp.clear();
		}
		ofstream outFile;
		outFile.open("data.txt", ios::out);
		outFile << "�㷨����ʱ��: " << *p6 << "s";
		outFile << "\n";
		outFile.close();
		m_pathFlag = true;
		update();
	}
}


void Map::slotPopulateJavaScriptWindowObject()    //QT�̶��ķ���,��ɵ�JS�Ķ���ע��
{

	
	view->page()->setWebChannel(new QWebChannel(this));	
	view->page()->webChannel()->registerObject(QStringLiteral("mywork"), this);
}
void Map::GetBaseData(QString lng, QString lat, QString BaseX, QString BaseY, QString Base)
{
	//��ȡ��������
	if (picture_model == true)
	{
		double maxLon = max(picture_lnglat[0], picture_lnglat[2]);
		double minLon = min(picture_lnglat[0], picture_lnglat[2]);
		double maxLat = max(picture_lnglat[1], picture_lnglat[3]);
		double minLat = min(picture_lnglat[1], picture_lnglat[3]);
		int y = pixmap.width();
		int h = pixmap.height();
		double scaleX = ((maxLon - minLon) * 3600) / y;
		double scaleY = ((maxLat - minLat) * 3600) / h;
		double templng = lng.toDouble();
		double templat = lat.toDouble();
		double X = (templng - minLon) * 3600 / scaleX;
		double Y = (maxLat - templat) * 3600 / scaleY;
		X = round(X);
		Y = round(Y);
		BaseData.push_back(X);
		BaseData.push_back(Y);
	}
	else
	{
		BaseData.push_back(BaseX.toInt());
		BaseData.push_back(BaseY.toInt());
	}
	BaseData.push_back(Base.toInt());
}
void Map::GetPointData(QString lng, QString lat, QString BaseX, QString BaseY)
{
	QPoint temp;
	temp.setX(BaseX.toInt());
	temp.setY(BaseY.toInt());
	m_points.push_back(temp);
	m_keyPoints.push_back(temp);
	Getlnglat.push_back(lng);
	Getlnglat.push_back(lat);
}
void Map::ShowMapPointLnglat(QString lng, QString lat, QString containerX, QString containerY)
{
	lng = lng.mid(0, 12);    //��ʾ��γ�ȱ���С����λ��  qstring��mid����
	lat = lat.mid(0, 12);
	qDebug() << lng;
	qDebug() << lat;
	QPoint temp;
	temp.setX(containerX.toInt());
	temp.setY(containerY.toInt());
	m_points.push_back(temp);
	m_keyPoints.push_back(temp);
	emit tablepoint(lng, lat);
}
void Map::container(QString x, QString y)      //��ͼƬ��������
{
	m_points.clear();
	m_keyPoints.clear();
	QPixmap p = this->grab(QRect(0, 0, this->width(), this->height()));
	QString filename = "map";
	filename += ".png";
	p.save(filename, "png");
	view->hide();
	picture_path = "./map.png";
	pixmap.load(picture_path);
	pixW = pixmap.width();
	pixH = pixmap.height();
	datatommap(x, y);
	QString cmd = "sentbase()";
	view->page()->runJavaScript(cmd);
}

void Map::datatommap(QString x_lng, QString y_lat)
{
	QString cmd = QString("callfromqt(%1,%2)").arg(x_lng, y_lat);
	view->page()->runJavaScript(cmd);
}
void Map::sentbase(QString lng, QString lat, QString base)
{
	QString cmd = QString("basefromqt(%1,%2,%3)").arg(lng, lat, base);
	view->page()->runJavaScript(cmd);
}
void Map::onCall(QString lng, QString lat)         //JS����QT�ĺ������������ݵ�QT��
{
	QPointF temp;
	temp.setX(lng.toDouble());
	temp.setY(lat.toDouble());
	emit sent(temp);
}
void Map::mousemovedata(QString lng, QString lat, QString x, QString y)
{
	qDebug() << lng;
	qDebug() << lat;
	currentlng = lng.mid(0, 12);
	currentlat = lat.mid(0, 12);
	lng = lng.mid(0, 10);    //��ʾ��γ�ȱ���С����λ��  qstring��mid����
	lat = lat.mid(0, 10);
	emit movepoint(lng, lat, x, y);
}
void Map::LowModel()
{
	view->hide();
	QString cmd = QString("recoverView()");
	view->page()->runJavaScript(cmd);
}
void Map::HighModel()
{
	view->show();
	for (int i = 0; i < m_number.size(); i++)
	{
		for (int j = 0; j < ChoosePoint[m_number[i]].size(); j++)
		{
			QString x = QString::number(ChoosePoint[m_number[i]][j].x());
			QString y = QString::number(ChoosePoint[m_number[i]][j].y());
			QString cmd = QString("getpolyline(%1,%2)").arg(x, y);
			view->page()->runJavaScript(cmd);
		}
		QString cmd = QString("drawpolyline()");
		view->page()->runJavaScript(cmd);
	}
}
void Map::ReStart()
{
	dynamic_UAV.clear();
	m_points.clear();
	polygon.clear();
	m_keyPoints.clear();
	m_UAV.clear();
	m_number.clear();
	m_length.clear();
	total_sum = 0;
	m_pressedFlag = false;
	m_matlabFlag = false;
	m_keyFlag = false;
	m_EndFlag = false;
	m_pathFlag = false;
	temp_UAV.clear();
	dynamic_UAV.clear();
	dynamic_length = 0;
	total_sum = 0;
	dynamic_status = true;
	BaseData.clear();
	pixmap.load(picture_path);
	millitary_base.load("military_base.png");
	radar_base.load("radar.png");
	ratio = 1.0;
	Alloffset.setX(0);
	Alloffset.setY(0);


	radar_status = false;      //�״�״̬
	radar_r.clear();           //��¼�״����ð뾶
	danger_area.clear();         //��в�����ÿ�
	danger_status = false;
	danger_number = 0;              //��в�������ظ�0��
	danger_area.resize(20);
	m_breakArea.clear();        //�رշ�������
	GetLineColor.clear();          //���߹۲�ȡ��
	GetLineColorStatus = false;
	int CurChooseLine = 0;       //���û�ȡ��ȡ���ߵ�״̬
	int preChooseLine = -1;
	ChoosePoint.clear();
	ChoosePoint.resize(11);
	pointchange_status = false;            //�����޸�״̬
	pointBeginChange_status = false;       //����������ѡ���޸�
	pointchange.setX(0); //����޸ĺ�����λ��
	pointchange.setY(0);
	//�û���������ӽ���
	pointadd_status = false;
	pointaddbegin_status = false;
	pointadd.setX(0);
	pointadd.setY(0);
	//�û���·��ɾ��
	pointdelete_status = false;
	pointdeletebegin_status = false;
	pointdelete.setX(0);
	pointdelete.setY(0);
	view->load(QUrl::fromLocalFile(QDir::currentPath() + "/index.html"));
	view->show();
	Getlnglat.clear();
	picture_model = false;
	currentlng.clear();
	currentlat.clear();
	update();
}