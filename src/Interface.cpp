#include <Interface.h>
#include <Transformer.h>
#include <QTextCodec>
#include <string>


Interface::Interface(){
	// Указываем кодировку этого файла для правильного отображения кириллицы.
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    // Инициализация интерфейса
	openAction = new QAction(trUtf8("Открыть"), this);
	saveAction = new QAction(trUtf8("Сохранить"), this);
	exitAction = new QAction(trUtf8("Выход"), this);
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

	// Левая колонка
    chboxes[0] = new QCheckBox(trUtf8("Сортировать\n координаты"), this);
	chboxes[0]->setGeometry(10, 10, 130, 40);
    rbut[0] = new QRadioButton("X", this);
	rbut[0]->move(30, 45);
	rbut[0]->hide();
	rbut[1] = new QRadioButton("Y", this);
	rbut[1]->move(30, 60);
	rbut[1]->hide();
	rbut[2] = new QRadioButton("Z", this);
	rbut[2]->move(30, 75);
	rbut[2]->hide();
	chboxes[4] = new QCheckBox(trUtf8("Отразить"), this);
	chboxes[4]->setGeometry(10, 105, 100, 20);
    combox = new QComboBox(this);
	combox->move(10, 130);
	combox->hide();
	combox->addItem(trUtf8("180°"), QVariant(QVariant::String));
	combox->addItem(trUtf8("270°"), QVariant(QVariant::String));
	combox->addItem(trUtf8("360°"), QVariant(QVariant::String));

    // Правая колонка
    LayerN = new QLabel(trUtf8("Количество слоев:"), this);
    LayerN->setGeometry(150, 10, 140, 25);
    spin = new QSpinBox(this);
	spin->setMinimum(1);
	spin->setGeometry(160, 30, 40, 25);
    spin->setToolTip(trUtf8("в фигуре"));
    airCapLength = new QSpinBox(this);
    airCapLength->setMinimum(0);
    airCapLength->setGeometry(220, 30, 40, 25);
    airCapLength->setToolTip(trUtf8("под 'воздушные торцы'"));

	LayerW = new QLabel(trUtf8("Толщина слоя:"), this);
	LayerW->move(150, 50);
	dspin = new QDoubleSpinBox(this);
	dspin->setMinimum(0.01);
	dspin->setValue(1);
	dspin->setGeometry(175, 75, 60, 25);

	chboxes[1] = new QCheckBox(trUtf8("Отцентровать по X"), this);
	chboxes[1]->setGeometry(150, 105, 160, 20);
	chboxes[2] = new QCheckBox(trUtf8("Отцентровать по Y"), this);
	chboxes[2]->setGeometry(150, 125, 160, 20);
	chboxes[3] = new QCheckBox(trUtf8("Отцентровать по Z"), this);
	chboxes[3]->setGeometry(150, 145, 160, 20);

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
		trUtf8("*.pmd"));
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
	}
	else{
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
			Output = new QString(fname);


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
		Transform->LoadFile(in);

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
            State->setText(trUtf8("Проблемы с созданием слоев"));
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
