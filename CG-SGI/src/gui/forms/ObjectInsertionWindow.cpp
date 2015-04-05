#include "gui/forms/ObjectInsertionWindow.h"

ObjectInsertionWindow::ObjectInsertionWindow(ControladorUI* controladorUI, QWidget* parent,
							Qt::WindowFlags flags) : DefaultWindow(controladorUI, parent, flags) {
	this->setupUi(this);
	this->moveToCenter();
	this->connectSignalsAndSlots();
}

void ObjectInsertionWindow::connectSignalsAndSlots() {
	QObject::connect(btnColor, SIGNAL(clicked()), this, SLOT(btnSelectColorClicked()));
	QObject::connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(btnInsert, SIGNAL(clicked()), this, SLOT(insertObject()));
	QObject::connect(btnRemovePoint, SIGNAL(clicked()), this, SLOT(removePoligonPoint()));
	QObject::connect(btnInsertPoint, SIGNAL(clicked()), this, SLOT(insertPoligonPoint()));
}

void ObjectInsertionWindow::clearFields(){
	this->fieldName->clear();
	this->fieldPointX->clear();
	this->fieldPointY->clear();
	this->fieldPointZ->clear();
	this->fieldLineX1->clear();
	this->fieldLineX2->clear();
	this->fieldLineY1->clear();
	this->fieldLineY2->clear();
	this->fieldLineZ1->clear();
	this->fieldLineZ2->clear();

	while(this->tablePoligonPoints->rowCount() > 0)
		this->tablePoligonPoints->removeRow(0);

	this->tabObjects->setCurrentIndex(0);

	QString style = QString("QPushButton  {"
					"    border-radius: 5px;"
					"    background-color: black }");
	this->btnColor->setStyleSheet(style);
}

bool ObjectInsertionWindow::validateFields() {
	if(this->fieldName->text().isEmpty() || this->fieldName->text().contains(' '))
		return false;

	bool ok, ok1, ok2, ok3;

	switch(this->tabObjects->currentIndex()) {
		case 0:
			this->fieldPointX->text().toDouble(&ok1);
			this->fieldPointY->text().toDouble(&ok2);
			ok3 = true;//this->fieldPointZ->text().toDouble(&ok3);

			return ok1 && ok2 && ok3;
		case 1:
			this->fieldLineX1->text().toDouble(&ok1);
			this->fieldLineY1->text().toDouble(&ok2);
			ok3 = true;//this->fieldLineZ1->text().toDouble(&ok3);
			ok = ok1 && ok2 && ok3;

			this->fieldLineX2->text().toDouble(&ok1);
			this->fieldLineY2->text().toDouble(&ok2);
			ok3 = true;//this->fieldLineZ2->text().toDouble(&ok3);

			return ok && ok1 && ok2 && ok3;
		default:
			if(this->tablePoligonPoints->rowCount() < 3)
				return false;

			for(int i = 0; i < this->tablePoligonPoints->rowCount(); i++) {
				QTableWidgetItem *i1, *i2;//, *i3;
				i1 = this->tablePoligonPoints->item(i, 0);
				i2 = this->tablePoligonPoints->item(i, 1);
				//i3 = this->tablePoligonPoints->item(i, 2);

				if(!i1 || !i2)
					return false;

				i1->text().toDouble(&ok1);
				i2->text().toDouble(&ok2);
				//i3->text().toDouble(&ok3);

				if(!(ok1 && ok2))
					return false;
			}
			return true;
	}
}

void ObjectInsertionWindow::insertObject() {
	if(!this->validateFields()) {
		this->controladorUI->exibirMensagemErro("Algum campo encontra-se vazio ou contém informação inválida!");
		return;
	}

	QList<Ponto> points;
	Ponto p;
	double x, y, z;
	String name = this->fieldName->text().toStdString();

	if(this->controladorUI->contemObjeto(name)) {
		this->controladorUI->exibirMensagemErro("Um objeto com este nome já existe!");
		return;
	}

	switch(this->tabObjects->currentIndex()) {
		case 0:
			x = this->fieldPointX->text().toDouble();
			y = this->fieldPointY->text().toDouble();
			z = 1;//this->fieldPointZ->text().toDouble();
			p = Ponto(name, x, y, z);
			points.insert(0, p);
			break;
		case 1:
			x = this->fieldLineX1->text().toDouble();
			y = this->fieldLineY1->text().toDouble();
			z = 1;//this->fieldLineZ1->text().toDouble();
			p = Ponto("", x, y, z);
			points.insert(0, p);

			x = this->fieldLineX2->text().toDouble();
			y = this->fieldLineY2->text().toDouble();
			z = 1;//this->fieldLineZ2->text().toDouble();
			p = Ponto("", x, y, z);
			points.insert(1, p);
			break;
		default:
			for(int i = 0; i < this->tablePoligonPoints->rowCount(); i++) {
				String nomePonto = this->fieldName->text().toStdString();

				x = this->tablePoligonPoints->item(i, 0)->text().toDouble();
				y = this->tablePoligonPoints->item(i, 1)->text().toDouble();
				z = 1;//this->tablePoligonPoints->item(i, 2)->text().toDouble();

				p = Ponto(nomePonto, x, y, z);
				points.insert(i, p);
			}
			break;
	}

	QColor color = this->btnColor->palette().color(this->btnColor->backgroundRole());

	this->controladorUI->inserirObjeto(name, points, color);
	this->accept();
}

void ObjectInsertionWindow::insertPoligonPoint() {
	this->tablePoligonPoints->insertRow(this->tablePoligonPoints->rowCount());
}

void ObjectInsertionWindow::removePoligonPoint() {
	QItemSelectionModel *selectionModel = this->tablePoligonPoints->selectionModel();
	for(QModelIndex index : selectionModel->selectedRows())
	    tablePoligonPoints->removeRow(index.row());
}

void ObjectInsertionWindow::btnSelectColorClicked() {
	QColor color = QColorDialog::getColor(this->btnColor->palette().color(this->btnColor->backgroundRole()), this);

	QString style = QString("QPushButton  {"
					"    border-radius: 5px;"
					"    background-color: " + color.name() + " }");
	this->btnColor->setStyleSheet(style);
}
