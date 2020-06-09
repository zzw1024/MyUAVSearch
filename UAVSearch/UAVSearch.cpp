#include "UAVSearch.h"
#include <qwebengineview.h>
#include <Qstring>
#include <qstringlist.h>
#include <qdir.h>
#include <qdebug.h>
#include <qfiledialog.h>
#include <qmenu.h>

UAVSearch::UAVSearch(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	QString str = QString(u8"�����˻�ϵͳ");
	this->setWindowTitle(str);//���ô��ڱ���

	connect(ui.btnSearch, SIGNAL(clicked()), this, SLOT(OnSettingSearch())); //ͼƬ����
	connect(ui.btnStart, SIGNAL(clicked()), this, SLOT(OnSettingStart()));//��ʼ����
	connect(ui.btnKey, SIGNAL(clicked()), this, SLOT(OnSettingKeyArea()));//����ѡ��
	connect(ui.btnNo, SIGNAL(clicked()), this, SLOT(OnSettingEndSearch()));//��������
	connect(ui.btnRestart, SIGNAL(clicked()), this, SLOT(OnSettingReStart()));//��������
	connect(ui.AddBase, SIGNAL(clicked()), this, SLOT(OnSettingBase()));//����
	connect(ui.AddUAV, SIGNAL(clicked()), this, SLOT(OnAddUAV()));//������˻�
	connect(ui.AddEnd,&QPushButton::clicked,this,&UAVSearch::OnAddEnd);
	connect(ui.DeleteUAV, SIGNAL(clicked()), this, SLOT(OnDeleteUAV()));//ɾ�����˻�


	connect(ui.tableWidgetUAV, SIGNAL(cellChanged(int, int)), this, SLOT(OnSettingUAV(int, int)));//���˻��������
	connect(ui.tableWidgetUAV, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(UAVLineStatus()));

	//�������˻������tablewidget����
	ui.tableWidgetUAV->setShowGrid(true);
	ui.tableWidgetUAV->setSortingEnabled(true);
	ui.tableWidgetUAV->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidgetUAV->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");
	ui.tableWidgetUAV->setColumnCount(12);//12��
	ui.tableWidgetUAV->resizeColumnsToContents();
	ui.tableWidgetUAV->resizeRowsToContents();
	ui.tableWidgetUAV->verticalHeader()->setDefaultSectionSize(70);
	ui.tableWidgetUAV->horizontalHeader()->setDefaultSectionSize(140);

	QStringList headersUAV;//������
	headersUAV.append(QString(u8"�ͺ�"));
	headersUAV.append(QString(u8"ʱ��: h"));
	headersUAV.append(QString(u8"�߶�: km"));
	headersUAV.append(QString(u8"�ٶ�: km/h"));
	headersUAV.append(QString(u8"����"));
	headersUAV.append(QString(u8"�������: mm"));
	headersUAV.append(QString(u8"�����Ȧ: mm"));
	headersUAV.append(QString(u8"��Сת��Ƕ�: o"));
	headersUAV.append(QString(u8"��Сת��뾶: km"));
	headersUAV.append(QString(u8"ת���ӳ�����: km"));
	headersUAV.append(QString(u8"�ص���: %"));
	headersUAV.append(QString(u8"������ɫ��ʶ"));
	ui.tableWidgetUAV->setHorizontalHeaderLabels(headersUAV);//��ֹ����

	connect(ui.btn_scale, SIGNAL(clicked()), this, SLOT(OnSettingScale()));//������
	connect(ui.btn_dynamic, SIGNAL(clicked()), this, SLOT(OnSettingDynamicUAV()));//��̬
	connect(ui.btn_recover, SIGNAL(clicked()), this, SLOT(OnSettingDynamicRecover()));//�ָ�
	connect(ui.M_model, SIGNAL(clicked()), this, SLOT(OnMillitary_model()));//����ģʽ
	connect(ui.HL_model, SIGNAL(clicked()), this, SLOT(OnLowToHighModel()));//�;���ģʽ

	//��ͼ����
	connect(ui.frame, SIGNAL(movepoint(QString, QString, QString, QString)), this, SLOT(OnMouseTrace(QString, QString, QString, QString)));         //��Ļ��ʾ�����ص�
	connect(ui.frame, SIGNAL(tablepoint(QString, QString)), this, SLOT(OnTableWidget(QString, QString)));
	connect(ui.frame, SIGNAL(savepoint(QVector<QPointF>, int)), this, SLOT(OnSavePoint(QVector<QPointF>, int)));
	connect(ui.frame, SIGNAL(sent(QPointF)), this, SLOT(datafromweb(QPointF)));

	//���û�ȡ��γ�ȵ�tablewidget����
	ui.tableWidget->setShowGrid(true);
	ui.tableWidget->setSortingEnabled(true);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableWidget->setColumnCount(2);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	// ˮƽ����ı���
	QStringList headers;
	headers.append(QString(u8"����"));  // ����
	headers.append(QString(u8"γ��")); // ά��
	ui.tableWidget->setHorizontalHeaderLabels(headers);

	connect(ui.tableWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(OnListContextMenu(const QPoint&)));
	connect(ui.actionDelete, SIGNAL(triggered()),
		this, SLOT(OnMenuRemove()));
}

//����ѡ��
bool UAVSearch::OnSettingKeyArea()
{
	ui.btnKey->setStyleSheet("QPushButton{color:red;background:yellow}");//���ð�����ɫ
	ui.btnKey->setText(QString::fromLocal8Bit("�ѿ�ʼ"));
	ui.frame->StartKeyArea();
	return 0;
}
//����ѡ��
bool UAVSearch::OnSettingEndSearch()
{
	ui.btnKey->setStyleSheet(NULL);
	ui.btnKey->setText(QString::fromLocal8Bit("����ѡ��"));
	ui.btnKey->setEnabled(false);//ѡ���˾Ͳ����ٰ���
	ui.frame->EndSearch();
	return 0;
}

//ͼƬģʽ
bool UAVSearch::OnSettingSearch()
{
	low_to_high = true;//�ض�ģʽ
	ui.HL_model->setText(QString::fromLocal8Bit("�;���ģʽ"));//����û���ã��ؼ����ֱ����������
	int width = this->width();
	int height = this->height();

	ui.frame->SearchArea(millitary_model);  //���õ�ͼ����ʾ��С
	return 0;
}

//�ֶ����ݾ�γ����ӻ���
bool UAVSearch::OnSettingBase()
{
	Plan dlg(this);
	int ret = dlg.exec();
	QString baseX;
	QString baseY;
	QString base;
	if (ret == QDialog::Accepted)
	{
		baseX = dlg.B_X;
		baseY = dlg.B_Y;
		base = dlg.BASE;
		bool OneBase = true;   //�ж��Ƿ����ظ�����
		for (int i = 0; i < BaseNumber.size(); i++)
		{
			if (base.toInt() == BaseNumber[i])
			{
				OneBase = false;
				QMessageBox::information(NULL, "title", "Repeat base");
				return 0;
			}
		}
		if (OneBase == true)
		{
			BaseNumber.push_back(base.toInt());
		}
		if (millitary_model == true)
		{
			ui.frame->GetBaseData(baseX, baseY, baseX, baseY, base);
		}
		else
		{
			ui.frame->sentbase(baseX, baseY, base);
		}

	}
	return 0;
}

//��ʼ����
bool UAVSearch::OnSettingStart()
{
	matlab_uav_data.clear();//�洢MATLAB�������еĽ��
	int row = ui.tableWidgetUAV->rowCount();
	for (int i = 0; i < row; i++)
	{//���δ洢Ҫ���͵�����
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 4)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 1)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 2)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 3)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 5)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 6)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 7)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 8)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 9)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 10)->text());
		//���˻����и߶�
		uavtoheight[ui.tableWidgetUAV->item(i, 4)->text().toInt()] = ui.tableWidgetUAV->item(i, 2)->text();
		//���˻��ı��
		int line = ui.tableWidgetUAV->item(i, 4)->text().toInt();
		//��ȡ��ɫѡ��
		QComboBox *combox = (QComboBox*)(ui.tableWidgetUAV->cellWidget(i, 11));
		int color = combox->currentIndex();
		if (color == 0)
		{
			uav_color[line] = QColor(0xff, 0xb6, 0xc1);
		}
		if (color == 1)
		{
			uav_color[line] = QColor(0x00, 0xdd, 0x00);
		}
		if (color == 2)
		{
			uav_color[line] = QColor(0xff, 0xff, 0x00);
		}
		if (color == 3)
		{
			uav_color[line] = QColor(0xff, 0x00, 0xff);
		}
		if (color == 4)
		{
			uav_color[line] = QColor(0x00, 0x00, 0xff);
		}
		qDebug() << uav_color[line];
	}

	double minflight = ui.minflight->text().toDouble();
	double maxflight = ui.maxfight->text().toDouble();
	ui.frame->StartSearch(matlab_uav_data, current_scale, minflight, maxflight, uav_color);
	return 0;
}

//���¿�ʼ�������������
bool UAVSearch::OnSettingReStart()
{
	int width = this->width();
	int height = this->height();

	//��γ���������
	ui.mouseTracelng->clear();
	ui.mouseTracelat->clear();
	BaseNumber.clear();
	int countrow = ui.tableWidget->rowCount();
	for (int i = 0; i < countrow; i++)
	{
		ui.tableWidget->removeRow(0);
	}
	int uavrow = ui.tableWidgetUAV->rowCount();
	for (int i = 0; i < uavrow; i++)
	{
		ui.tableWidgetUAV->removeRow(0);
	}
	uavnumber.clear();
	ui.btnKey->setStyleSheet(NULL);
	ui.btnKey->setText(QString::fromLocal8Bit("����ѡ��"));
	//���������������
	matlab_uav_data.clear(); //��մ���matilab�����˻�����
	ui.scale_text->clear();
	ui.dynamic_text->clear();
	uav_color.clear();             //���˻���������ɫ���
	current_scale = 0;
	ui.frame->ReStart();
	uavtoheight.clear();         //���˻���Ӧ�ĸ߶�
	millitary_model = false;
	low_to_high = false;
	ui.HL_model->setText(QString::fromLocal8Bit("�;���ģʽ"));
	ui.minflight->clear();
	ui.maxfight->clear();
	return 0;
}


///////////////////////////////////////////
//������˻�
void UAVSearch::OnAddUAV()
{
	ui.tableWidgetUAV->setEditTriggers(QAbstractItemView::DoubleClicked);
	int row = ui.tableWidgetUAV->rowCount();//��ȡ��ǰ��
	if (row > 9)//ֻ�����10�����˻�
	{
		QMessageBox::information(this, "this", QString::fromLocal8Bit("���˻�����"));
	}

	ui.tableWidgetUAV->insertRow(row);//��row�������һ��
	for (int i = 0; i < 11; i++)//11��Ԫ�ظ�ֵΪ�գ�Ȼ�������д
	{
		ui.tableWidgetUAV->setItem(row, i, new QTableWidgetItem(""));
	}
	//���ú�����ɫ
	QStringList list;
	list.append(QString::fromLocal8Bit("��ɫ"));
	list.append(QString::fromLocal8Bit("��ɫ"));
	list.append(QString::fromLocal8Bit("��ɫ"));
	list.append(QString::fromLocal8Bit("��ɫ"));
	list.append(QString::fromLocal8Bit("��ɫ"));
	QComboBox *combox = new QComboBox();//�½�������
	combox->addItems(list);
	ui.tableWidgetUAV->setCellWidget(row, 11, combox);
}

void UAVSearch::OnAddEnd()
{
	int row = ui.tableWidgetUAV->rowCount();//��ȡ��ǰ��
	for (int i = 0; i < 11; i++)//11��Ԫ�ظ�ֵΪ�գ�Ȼ�������д
	{
		bool isRightAdd = true;
		ui.tableWidgetUAV->item(row-1, i)->text().toInt(&isRightAdd);
		if (!isRightAdd)
		{
			QMessageBox::warning(this, u8"����", u8"��ӵ����˻�����������������д��",
				QMessageBox::Ok);
			ui.tableWidgetUAV->removeRow(row);
			return;				
		}
		//QTableWidgetItem* item = new QTableWidgetItem;
		//item->setFlags(item->flags() &= (~Qt::ItemIsEditable));
		//ui.tableWidgetUAV->setItem(row - 1, i, item);
		//delete item;
	}
	QMessageBox::information(this, u8"֪ͨ", u8"��ӵ����˻������ɹ���",
		QMessageBox::Ok);
	ui.tableWidgetUAV->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

//ɾ�����˻�
void UAVSearch::OnDeleteUAV()
{
	int row = ui.tableWidgetUAV->currentRow();
	QTableWidgetItem* item = ui.tableWidgetUAV->takeItem(row, 0);
	if (item)//�鿴�Ƿ��������µ����˻���Ϣ
	{
		ui.tableWidgetUAV->removeRow(row);
	}
}

/////////////////////////////////////////
//�������ص�;�γ��
int UAVSearch::OnMouseTrace(QString lng, QString lat, QString containerX, QString containerY)
{
	//��ʾ�ھ���γ�ȵ�QLine��
	ui.mouseTracelng->setText(lng);
	ui.mouseTracelat->setText(lat);
	ui.mouseX->setText(containerX);
	ui.mouseY->setText(containerY);
	return 0;
}
//���ñ�����
bool UAVSearch::OnSettingScale()
{
	double index;
	if (ui.scale_text->text() == "")
	{
		index = 0;
	}
	else
	{
		index = ui.scale_text->text().toDouble();
	}
	current_scale = index;
	return 0;
}
//���ö�̬·�ߣ�������̬·��
bool UAVSearch::OnSettingDynamicUAV()
{
	double index = ui.dynamic_text->text().toDouble();
	ui.frame->Dynanic_index(index);
	return 0;
}
//�ָ�������Ĩ����̬����
bool UAVSearch::OnSettingDynamicRecover()
{
	ui.frame->dynamic_Rocover();
	return 0;
}
bool UAVSearch::OnMillitary_model()
{
	millitary_model = true;
	return 0;
}

void UAVSearch::OnLowToHighModel()
{
	if (low_to_high == true)
	{
		low_to_high = false;
		ui.HL_model->setText(QString::fromLocal8Bit("�߾���ģʽ"));
		ui.frame->HighModel();
	}
	else
	{
		ui.HL_model->setText(QString::fromLocal8Bit("�;���ģʽ"));
		low_to_high = true;
		ui.frame->LowModel();
	}
}
//////////////////////////////////////
//ѡ���˹ؼ������Ӿ�γ�ȵ�table��
void UAVSearch::AddData(const QString &x, const QString &y)
{
	int row = ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(row);//���һ�У�rowΪ�к�

	// x
	QTableWidgetItem* item = new QTableWidgetItem();
	item->setText(x);
	ui.tableWidget->setItem(row, 0, item);

	// y
	item = new QTableWidgetItem();
	item->setText(y);
	ui.tableWidget->setItem(row, 1, item);
}

//��γ�ȱ����ʾ�˵�
int UAVSearch::OnListContextMenu(const QPoint &pt)
{
	// ��ǰѡ����
	QTableWidgetItem* selected = ui.tableWidget->itemAt(pt);

	// ����˵����˵���
	QMenu menu;
	if (selected)
	{
		menu.addAction(ui.actionDelete);
	}

	// ��ʾ�˵�
	menu.exec(ui.tableWidget->mapToGlobal(pt));

	return 0;
}

//���˻��������
void UAVSearch::OnSettingUAV(int x, int y)
{
	if (y != 4)
	{
		return;     //ֻ���ݵ��������жϻ���
	}

	QString tempuav = ui.tableWidgetUAV->item(x, y)->text();
	if (BaseNumber.empty() && ui.frame->BaseData.empty())
	{
		QMessageBox::information(NULL, "this", "no base");
		return;
	}

	//��ȡ�ӵ;���ģʽ������Ļ���
	if (!ui.frame->BaseData.empty())
	{
		BaseNumber.clear();
		for (int i = 0; i < ui.frame->BaseData.size() / 3; i++)
		{
			BaseNumber.push_back(ui.frame->BaseData[i * 3 + 2]);
		}
	}

	int index;
	if (tempuav.isEmpty())
	{
		index = 0;
	}
	index = tempuav.toInt();

	if (index > 10 || index < 0)
	{
		QMessageBox::information(NULL, "title", "wrong base");
		return;
	}
	std::sort(uavnumber.begin(), uavnumber.end());
	for (int i = 0; i < uavnumber.size(); i++)
	{
		if (index == uavnumber[i])
		{
			QMessageBox::information(NULL, "this", QString::fromLocal8Bit("���˻��Ѿ�����"));
			return;
		}
	}
	if (uavnumber.size() > 10)
	{
		QMessageBox::information(NULL, "this", QString::fromLocal8Bit("��������"));
		return;
	}
	bool basestatus = false;
	std::sort(BaseNumber.begin(), BaseNumber.end());
	for (int i = 0; i < BaseNumber.size(); i++)
	{
		//�ж����˻��Ƿ��ж�Ӧ�Ļ���
		int baseNu = BaseNumber[i];
		if (index == 0)
		{
			basestatus = true;
			break;
		}
		if ((index == 1 || index == 2 || index == 3) && baseNu == 1)
		{
			basestatus = true;
			break;
		}
		if ((index == 4 || index == 5 || index == 6) && baseNu == 2)
		{
			basestatus = true;
			break;
		}
		if ((index == 7 || index == 8 || index == 9 || index == 10) && baseNu == 3)
		{
			basestatus = true;
			break;
		}
	}
	if (basestatus == false)
	{
		QMessageBox::information(NULL, "this", QString::fromLocal8Bit("�޴˻��أ�ע��1��2��3��Ӧ1�Ż���"
			"4��5��6��Ӧ2�Ż��أ�7��8��9��10��Ӧ���Ż���"));
		return;
	}
	if (index != 0)
	{
		uavnumber.push_back(index);    //����indexΪ��
	}
}


//����������Ӿ�γ��
int UAVSearch::OnTableWidget(QString, QString)
{
	AddData(lng, lat);
	return 0;
}
//�������˻���·����
bool UAVSearch::OnSavePoint(QVector<QPointF> uav, int index)
{
	ofstream outFile;
	QString name = "uav" + QString::number(index) + ".txt";
	string Filename;
	Filename = string((const char *)name.toLocal8Bit());
	outFile.open(Filename, ios::out);

	// 	outFile.open("./data.txt",ios::app);
	// 	int i = index;
	// 	string s = "���˻�";
	// 	outFile << s << i << "������: \n";
	// 	outFile << "��ɵ�: " << uav[0].x() << " " << uav[0].y()<<"\n";
	// 	outFile << "����ʱ�䣺 " << uav.size() << "\n";
	// 	outFile << "���й켣����" << "\n";
	for (int i = 0; i < uav.size(); i++)
	{
		string lat;
		string lng;
		if (millitary_model == false)
		{
			ui.frame->datatommap(QString::number(uav[i].x()), QString::number(uav[i].y()));
			lng = QString::number(fromweb.x(), 10, 8).toLocal8Bit();
			lat = QString::number(fromweb.y(), 10, 8).toLocal8Bit();
		}
		else
		{
			lng = QString::number(uav[i].x(), 10, 8).toLocal8Bit();
			lat = QString::number(uav[i].y(), 10, 8).toLocal8Bit();
		}
		//ת�����ȱ���ģʽ
		double data_lng = atof(lng.c_str());
		int d_lng = int(data_lng);
		int m_lng = int((data_lng - d_lng) * 60);
		double s_lng = ((data_lng - d_lng) * 60 - m_lng) * 60;
		//ת��γ�ȱ���ģʽ
		double data_lat = atof(lat.c_str());
		int d_lat = int(data_lat);
		int m_lat = int((data_lat - d_lat) * 60);
		double s_lat = ((data_lat - d_lat) * 60 - m_lat) * 60;
		outFile << index << "\t";
		outFile << i << "\t";
		outFile << "0" << "\t";
		outFile << d_lng << " " << m_lng << " " << fixed << setprecision(2) << s_lng << "\t";
		outFile << d_lat << " " << m_lat << " " << fixed << setprecision(2) << s_lat << "\t";
		outFile << uavtoheight[index].toInt() * 1000 << "\n";
	}
	outFile.close();
	return 0;
}


//�������ߣ��������˻���Ŀ��
bool UAVSearch::UAVLineStatus()
{
	QList<QTableWidgetItem*> items = ui.tableWidgetUAV->selectedItems();
	QTableWidgetItem *item = items.at(4);
	int line = item->text().toInt(); //��ȡ����
	if (line == 0)
	{
		return 0;
	}
	ui.frame->WatchUAVLine(line);
	return 0;
}


//ɾ��ѡ�еĵ�
int UAVSearch::OnMenuRemove()
{
	int row = ui.tableWidget->currentRow();//��ǰ��
	QTableWidgetItem* item = ui.tableWidget->takeItem(row, 0);//��ȡ��ǰ��Ŀ
	ui.frame->DeletePoint(row);//ɾ���õ�ı��
	if (item)
	{
		ui.tableWidget->removeRow(row);//����ɾ��
	}
	return 0;
}

void UAVSearch::datafromweb(QPointF pppp)
{
	fromweb = pppp;
}

void UAVSearch::setValueFromWeb(const QString &weblng, const QString &weblat)
{
	lng = weblng;
	lat = weblat;
}










