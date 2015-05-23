#include <Interface.h>
#include <Transformer.h>
#include <QTextCodec>
#include <string>


PrInterface::PrInterface(){
	// Указываем кодировку этого файла для правильного отображения кириллицы
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8")); 
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName ("UTF-8"));

    // Инициализация интерфейса
	openAction = new QAction(tr("Открыть"), this);
	saveAction = new QAction(tr("Сохранить"), this);
	exitAction = new QAction(tr("Выход"), this);
	autorAction = new QAction(tr("Об авторе"), this);
	aboutAction = new QAction(tr("О программе"), this);

	btn = new QPushButton("Приступить!", this);
	btn->move(150, 220);

	chboxes[0] = new QCheckBox(tr("Сортировать\n координаты"), this);
	chboxes[0]->setGeometry(10, 20, 130, 40);
	chboxes[1] = new QCheckBox(tr("Отцентровать по X"), this);
	chboxes[1]->setGeometry(150, 130, 160, 20);
	chboxes[2] = new QCheckBox(tr("Отцентровать по Y"), this);
	chboxes[2]->setGeometry(150, 150, 160, 20);
	chboxes[3] = new QCheckBox(tr("Отцентровать по Z"), this);
	chboxes[3]->setGeometry(150, 170, 160, 20);
	chboxes[4] = new QCheckBox(tr("Отразить"), this);
	chboxes[4]->setGeometry(10, 125, 100, 20);

	combox = new QComboBox(this);
	combox->move(10, 160);
	combox->hide();
	//FIXME combox->addItem("", QVariant(QVariant::String));
	combox->addItem(tr("180°"), QVariant(QVariant::String));
	combox->addItem(tr("270°"), QVariant(QVariant::String));
	combox->addItem(tr("360°"), QVariant(QVariant::String));

	spin = new QSpinBox(this);
	spin->setMinimum(1);
	spin->setGeometry(150, 50, 40, 25);

	dspin = new QDoubleSpinBox(this);
	dspin->setMinimum(0.01);
	dspin->setValue(1);
	dspin->setGeometry(150, 90, 60, 25);

	rbut[0] = new QRadioButton("X", this);
	rbut[0]->move(30, 55);
	rbut[0]->hide();
	rbut[1] = new QRadioButton("Y", this);
	rbut[1]->move(30, 70);
	rbut[1]->hide();
	rbut[2] = new QRadioButton("Z", this);
	rbut[2]->move(30, 85);
	rbut[2]->hide();

	LayerN = new QLabel("Количество слоев:", this);
	LayerN->move(150, 25);
	LayerW = new QLabel("Толщина слоя:", this);
	LayerW->move(150, 67);


	fileMenu = menuBar()->addMenu(tr("Файл"));
	fileMenu->addAction(openAction);
	fileMenu->addAction(saveAction);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAction);

	helpMenu = menuBar()->addMenu(tr("Помощь"));
	helpMenu->addAction(aboutAction);
	helpMenu->addAction(autorAction);

	State = new QLabel(this);
	State->setGeometry(10, 210, 150, 50);

	Input = new QString("");
	Output = new QString("");

	ready = 0;

	this->setFixedSize(350, 280);
	this->setWindowTitle(tr("Transformer"));


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

void PrInterface::open()
{
	qDebug() << Input;
	QString fname = QFileDialog::getOpenFileName(this, tr("Открыть"), "",
		tr("*.pmd"));
	if (fname != "") {
		Input = new QString(fname);
        State->setText(tr("Выбран файл"));
    } else
        State->setText(tr("Файл не выбран"));
}

void PrInterface::hideRb(){

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

void PrInterface::hideCb(){
	if (!chboxes[4]->isChecked())
		combox->hide();
	else
		combox->show();
}

void PrInterface::save()
{
	if (ready){
		QString fname = QFileDialog::getSaveFileName(this, tr("Сохранить"), "",
			tr("*.transf"));

		if (fname != ""){
			Output = new QString(fname);


			QByteArray oqb = Output->toUtf8();
			char* out = oqb.data();
			Transform->SaveFile(out);
			State->setText("Сохранено!");
		}
	}
	else
		State->setText("Не сохранено");
}

void PrInterface::showAutor(){
	QMessageBox::information(this, tr("Об авторе"),
		tr("Автор программы: Ермаков Даниэль.\n"
		"  E-Mail: dan-ermakov@rambler.ru\n"
		"        НТУ ХПИ. Кафедра СПУ. \n"
		"                 Харьков 2014"), QMessageBox::Cancel);

}

void PrInterface::showAbout(){
	QMessageBox::information(this, tr("О Программе"),
		tr("Данная программа служит для перехода от двумерного разбиения треугольниками к трехмерному разбиению прямыми призмами. \n\n")
		, QMessageBox::Cancel);
}


void PrInterface::transformate(){

	if (Input->compare("") != 0){
		State->setText(tr("Обработка данных"));

		int a = 0;
		if (chboxes[4]->isChecked())
			a = 90 * combox->currentIndex() + 180;
		Transform = new transformer(spin->value(), dspin->value(), a);


		QByteArray iqb = Input->toUtf8();
		char* in = iqb.data();


		State->setText(tr("Чтение из файла..."));
		Transform->LoadFile(in);

		State->setText(tr("Разбиение..."));
		Transform->Partition();

		if (chboxes[0]->isChecked()){
			State->setText(tr("Сортировка..."));
			if (rbut[0]->isChecked())
				a = 0;
			else if (rbut[1]->isChecked())
				a = 1;
			else
				a = 2;
			Transform->Sort_Koor(a);
		}

		if (chboxes[4]->isChecked()){
			State->setText(tr("Отражение..."));
			Transform->turn();
		}

		State->setText(tr("Создание слоев..."));
		if( !Transform->MakeLayer() ){
            State->setText(tr("Проблемы с созданием слоев"));
            return;
        }

		if (chboxes[1]->isChecked())
			Transform->makeCentral(0);
		if (chboxes[2]->isChecked())
			Transform->makeCentral(1);
		if (chboxes[3]->isChecked())
			Transform->makeCentral(2);

		State->setText(tr("Сохранение..."));
		Transform->SaveFile("data.transf");

		State->setText(tr("Завершено"));
		ready = true;
	}
	else
		State->setText(tr("Файл не выбран"));

}
