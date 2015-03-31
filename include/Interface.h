#ifndef INTERFACE_H
#define INTERFACE_H

#include <QMainWindow>
#include <Transformer.h>
#include <QFile>
#include <QFileDialog>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QTextCodec>
#include <QDebug>
#include <QMessageBox>
#include "ui_transgui.h"

class PrInterface : public QMainWindow
{
	Q_OBJECT

public:
	PrInterface();

	QFileDialog * Fdialog;

	QAction * openAction;
	QAction * saveAction;
	QAction * exitAction;
	QAction * autorAction;
	QAction * aboutAction;
	QMenu * fileMenu;
	QMenu * helpMenu;
	QPushButton * btn;
	QCheckBox * chboxes[5];
	QRadioButton * rbut[3];
	QComboBox * combox;
	QSpinBox* spin;
	QDoubleSpinBox* dspin;
	QLabel * LayerN;
	QLabel * LayerW;
	QLabel * State;
	bool ready;
	transformer * Transform;

	QString * Input;
	QString * Output;

	//	void slot();

	public slots:
	void open();
	void save();
	void hideRb();
	void hideCb();
	void transformate();
	void showAbout();
	void showAutor();

public:
	Ui::PrInterfaceClass ui;
};

#endif // INTERFACE_H
