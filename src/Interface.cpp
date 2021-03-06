﻿#include <Interface.h>
#include <Transformer.h>
#include <QTextCodec>
#include <QShortcut>
#include <string>
#include <stdexcept>


Interface::Interface(){
	// Указываем кодировку этого файла для правильного отображения кириллицы.
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    // Инициализация интерфейса
	QShortcut* openFile = new QShortcut(QKeySequence("Ctrl+O"), this);
	QShortcut* saveFile = new QShortcut(QKeySequence("Ctrl+S"), this);
	QShortcut* quitProgram = new QShortcut(QKeySequence("Ctrl+Q"), this);
	openAction = new QAction(trUtf8("Открыть (ctrl+o)"), this);
	saveAction = new QAction(trUtf8("Сохранить как (ctrl+s)"), this);
	exitAction = new QAction(trUtf8("Выход (ctrl+q)"), this);
	autorAction = new QAction(trUtf8("Об авторе"), this);
	aboutAction = new QAction(trUtf8("О программе"), this);
    fileMenu = menuBar()->addMenu(trUtf8("Файл"));
	fileMenu->addAction(openAction);
	fileMenu->addAction(saveAction);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAction);

	helpMenu = menuBar()->addMenu(trUtf8("Помощь"));
	helpMenu->addAction(aboutAction);
	helpMenu->addAction(autorAction);

	static const int y = 30;

	// Левая колонка
    chboxes[0] = new QCheckBox(trUtf8("Сортировать\n координаты"), this);
	chboxes[0]->setGeometry(10, y, 130, 40);
    rbut[0] = new QRadioButton("X", this);
	rbut[0]->move(30, y+35);
	rbut[0]->hide();
	rbut[1] = new QRadioButton("Y", this);
	rbut[1]->move(30, y+50);
	rbut[1]->hide();
	rbut[2] = new QRadioButton("Z", this);
	rbut[2]->move(30, y+65);
	rbut[2]->hide();
	chboxes[4] = new QCheckBox(trUtf8("Отразить"), this);
	chboxes[4]->setGeometry(10, y+95, 100, 20);
    combox = new QComboBox(this);
	combox->move(10, y+120);
	combox->hide();
	combox->addItem(trUtf8("180°"), QVariant(QVariant::String));
	combox->addItem(trUtf8("270°"), QVariant(QVariant::String));
	combox->addItem(trUtf8("360°"), QVariant(QVariant::String));

    // Правая колонка
    LayerN = new QLabel(trUtf8("Количество слоев:"), this);
    LayerN->setGeometry(150, y, 140, 25);
    spin = new QSpinBox(this);
	spin->setMinimum(1);
	spin->setGeometry(160, y+20, 40, 25);
    spin->setToolTip(trUtf8("в фигуре"));
    airCapLength = new QSpinBox(this);
    airCapLength->setMinimum(0);
    airCapLength->setGeometry(220, y+20, 40, 25);
    airCapLength->setToolTip(trUtf8("под 'воздушные шапки' (с одной стороны)"));

	LayerW = new QLabel(trUtf8("Толщина слоя:"), this);
	LayerW->move(150, y + 40);
	dspin = new QDoubleSpinBox(this);
	dspin->setMinimum(0.01);
	dspin->setValue(1);
	dspin->setGeometry(175, y + 65, 60, 25);

	chboxes[1] = new QCheckBox(trUtf8("Отцентровать по X"), this);
	chboxes[1]->setGeometry(150, y + 95, 160, 20);
	chboxes[2] = new QCheckBox(trUtf8("Отцентровать по Y"), this);
	chboxes[2]->setGeometry(150, y + 115, 160, 20);
	chboxes[3] = new QCheckBox(trUtf8("Отцентровать по Z"), this);
	chboxes[3]->setGeometry(150, y + 135, 160, 20);

    // Нижняя часть
	State = new QLabel(this);
	State->setGeometry(10, 220, 155, 50);
	btn = new QPushButton(trUtf8("Приступить!"), this);
	btn->move(160, 230);

	Input = new QString("");
	Output = new QString("");

	ready = 0;

	this->setFixedSize(350, 280);
	this->setWindowTitle(trUtf8("Transformer"));


	bool res;
	res = connect(openFile, SIGNAL(activated()), this, SLOT(open()));
	Q_ASSERT(res);
	res = connect(saveFile, SIGNAL(activated()), this, SLOT(save()));
	Q_ASSERT(res);
	res = connect(quitProgram, SIGNAL(activated()), qApp, SLOT(quit()));
	Q_ASSERT(res);

	res = connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
	Q_ASSERT(res);
	res = connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
	Q_ASSERT(res);
	res = connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
	Q_ASSERT(res);

	res = connect(chboxes[0], SIGNAL(clicked()), this, SLOT(hideRb()));
	Q_ASSERT(res);
	res = connect(chboxes[4], SIGNAL(clicked()), this, SLOT(hideCb()));
	Q_ASSERT(res);
	res = connect(btn, SIGNAL(clicked()), this, SLOT(transformate()));
	Q_ASSERT(res);
	res = connect(autorAction, SIGNAL(triggered()), this, SLOT(showAutor()));
	Q_ASSERT(res);
	res = connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));
	Q_ASSERT(res);


}

void Interface::open()
{
	qDebug() << Input;
	QString fname = QFileDialog::getOpenFileName(this, trUtf8("Открыть"), "",
		trUtf8("Файл разбиения(*.pmd)"));
	if (fname != "") {
		Input = new QString(fname);
        State->setText(trUtf8("Выбран файл"));
    } else
        State->setText(trUtf8("Файл не выбран"));
}

void Interface::hideRb(){
	if (!chboxes[0]->isChecked()){
		rbut[0]->hide();
		rbut[1]->hide();
		rbut[2]->hide();
	} else {
		rbut[0]->show();
		rbut[1]->show();
		rbut[2]->show();
	}

}

void Interface::hideCb(){
	if (!chboxes[4]->isChecked())
		combox->hide();
	else
		combox->show();
}

void Interface::save()
{
	if (ready){
		QString fname = QFileDialog::getSaveFileName(this, trUtf8("Сохранить"), "",
			trUtf8("*.transf"));

		if (fname != ""){
			Output = new QString(fname + ".transf");


			QByteArray oqb = Output->toUtf8();
			char* out = oqb.data();
			Transform->SaveFile(out);
			State->setText(trUtf8("Сохранено!"));
		}
	}
	else
		State->setText(trUtf8("Не сохранено"));
}

void Interface::showAutor(){
	QMessageBox::information(this, trUtf8("Об авторе"),
		trUtf8("Автор программы: Ермаков Даниэль.\n"
		"  E-Mail: dan-ermakov@rambler.ru\n"
		"        НТУ ХПИ. Кафедра СПУ. \n"
		"                 Харьков 2014"), QMessageBox::Cancel);

}

void Interface::showAbout(){
	QMessageBox::information(this, trUtf8("О Программе"),
		trUtf8("Данная программа служит для перехода от двумерного разбиения треугольниками к трехмерному разбиению прямыми призмами. \n\n")
		, QMessageBox::Cancel);
}

void Interface::transformate(){

	if (Input->compare("") != 0){
		State->setText(trUtf8("Обработка данных"));

		int a = 0;
		if (chboxes[4]->isChecked())
			a = 90 * combox->currentIndex() + 180;
		Transform = new transformer(spin->value(), dspin->value(), a, airCapLength->value());


		QByteArray iqb = Input->toUtf8();
		char* in = iqb.data();


		State->setText(trUtf8("Чтение из файла..."));
		
		QString errDescript;
		switch(Transform->LoadFile(in)){
		case 0: 
			// Все нормально
			break;
		case 1:
			errDescript = trUtf8("Файл нарушен в секции [inds]: узлов меньше, чем было объявлено");
			break;
		case 2:
			errDescript = trUtf8("Файл нарушен в секции [koor]: элементов меньше, чем было объявлено");
			break;
		case 3:
			errDescript = trUtf8("Файл нарушен в секции [materials]: узлов меньше, чем было объявлено");
			break;
		case 4:
			errDescript = trUtf8("Файл нарушен в секции [inds]: индексация узлов начинается не с 1");
			break;
		}
		if( !errDescript.isNull() ) {
			QMessageBox::critical(this, trUtf8("Ошибка чтения"), errDescript);
			return;
		}

		State->setText(trUtf8("Разбиение..."));
		Transform->Partition();

		if (chboxes[0]->isChecked()){
			State->setText(trUtf8("Сортировка..."));
			if (rbut[0]->isChecked())
				a = 0;
			else if (rbut[1]->isChecked())
				a = 1;
			else
				a = 2;
			Transform->Sort_Koor(a);
		}

		if (chboxes[4]->isChecked()){
			State->setText(trUtf8("Отражение..."));
			Transform->turn();
		}

		State->setText(trUtf8("Создание слоев..."));
		if( !Transform->CreateLayers() ){
			QMessageBox::critical(this, trUtf8("Ошибка разбиения"), trUtf8("Разбиение породило больше узлов, чем было предсказано"));
            return;
        }

		if (chboxes[1]->isChecked())
			Transform->makeCentral(0);
		if (chboxes[2]->isChecked())
			Transform->makeCentral(1);
		if (chboxes[3]->isChecked())
			Transform->makeCentral(2);

		State->setText(trUtf8("Сохранение..."));
		Transform->SaveFile("data.transf");

		State->setText(trUtf8("Завершено"));
		ready = true;
	}
	else
		State->setText(trUtf8("Файл не выбран"));

}
