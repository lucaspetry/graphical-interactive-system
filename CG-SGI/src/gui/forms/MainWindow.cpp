#include "gui/forms/MainWindow.h"
#include "geometria/Poligono.h"
#include "geometria/Reta.h"

MainWindow::MainWindow(ControladorUI* controladorUI, QDialog* parent,
						Qt::WindowFlags flags) : DefaultWindow(controladorUI, parent, flags) {
	this->setupUi(this);
	this->moveToCenter();
	this->connectSignalsAndSlots();
	this->zoomControl->setValue(50);
    this->viewportWidth = 475;
    this->viewportHeight = 475;
    this->zoomValue = this->zoomControl->value();
}

MainWindow::~MainWindow() {}

void MainWindow::updateObjects(const QList<ObjetoGeometrico*>& objects) {
	// Remover todos os objetos da tabela
	while(this->tableObjects->rowCount() > 0)
		this->tableObjects->removeRow(0);

	// Inserir todos os objetos atualizados na tabela
	for(ObjetoGeometrico* obj : objects) {
		this->tableObjects->insertRow(this->tableObjects->rowCount());
		QTableWidgetItem* type = new QTableWidgetItem(QString::fromStdString(obj->getTipoString()));
		QTableWidgetItem* name = new QTableWidgetItem(QString::fromStdString(obj->getNome()));
		this->tableObjects->setItem(this->tableObjects->rowCount() - 1, 0, type);
		this->tableObjects->setItem(this->tableObjects->rowCount() - 1, 1, name);
	}

	QGraphicsView* g = this->graphicsView;
	QGraphicsScene* scene = g->scene();
	QList<ObjetoGeometrico*> objs = this->viewportTransformation(objects);

	if(scene)
		delete scene;

	scene = new QGraphicsScene(0, 0, this->viewportWidth - 4,
								this->viewportHeight - 6, this->graphicsView);

	for(int i = 0; i < objs.size(); i++) {
		QList<Ponto> pontos = objs.at(i)->getPontos();
		Ponto first = pontos.at(0);

		QPen pen(objs.at(i)->getCor());
		QLineF line;

		if(pontos.size() > 1) {
			Ponto ant = first;

			for(int i = 1; i < pontos.size(); i++) {
				line = QLineF(ant.getX(), ant.getY(), pontos.at(i).getX(), pontos.at(i).getY());
				scene->addLine(line, pen);
				ant = pontos.at(i);
			}
			line = QLineF(ant.getX(), ant.getY(), first.getX(), first.getY());
			scene->addLine(line, pen);
		} else {
			scene->addEllipse(first.getX(), first.getY(), 3, 3, pen, QBrush(objs.at(i)->getCor()));
		}
	}

	while(objs.size() > 0)
		objs.removeAt(0);

	this->graphicsView->setScene(scene);
	this->graphicsView->repaint();
}

QList<ObjetoGeometrico*> MainWindow::viewportTransformation(const QList<ObjetoGeometrico*>& objects) {
	QList<ObjetoGeometrico*> newObjects;
	//double altura = this->controladorUI->getAlturaWindow();
	//double largura = this->controladorUI->getLarguraWindow();

	double xwMin = -1;
	double xwMax = 1;
	double ywMin = -1;
	double ywMax = 1;

	for(int i = 0; i < objects.size(); i++) {
		ObjetoGeometrico* obj = objects.at(i);
		QList<Ponto> points = obj->getPontos();
		QList<Ponto> newPoints;
		Ponto p1, p2;

		switch(obj->getTipo()) {
			case ObjetoGeometrico::PONTO:
				p1 = this->pointTransformation(points.at(0), xwMin, xwMax, ywMin, ywMax);
				p1.setCor(obj->getCor());
				newObjects.insert(i, new Ponto(p1));
				break;
			case ObjetoGeometrico::RETA:
				p1 = this->pointTransformation(points.at(0), xwMin, xwMax, ywMin, ywMax);
				p2 = this->pointTransformation(points.at(1), xwMin, xwMax, ywMin, ywMax);
				newObjects.insert(i, new Reta(obj->getNome(), p1, p2, obj->getCor()));
				break;
			case ObjetoGeometrico::POLIGONO:
				for(int j = 0; j < points.size(); j++) {
					newPoints.insert(j, this->pointTransformation(points.at(j), xwMin, xwMax, ywMin, ywMax));
				}
				newObjects.insert(i, new Poligono(obj->getNome(), newPoints, obj->getCor()));
				break;
			default:
				break;
		}
	}

	return newObjects;
}

Ponto MainWindow::pointTransformation(const Ponto& point, const double xwMin, const double xwMax,
									const double ywMin, const double ywMax) {
	double xp = ((point.getX() - xwMin) / (xwMax - xwMin)) * this->viewportWidth;
	double yp = (1 - (point.getY() - ywMin) / (ywMax - ywMin)) * this->viewportHeight;
	Ponto newPoint(point.getNome(), xp, yp, 1);
	return newPoint;
}

void MainWindow::connectSignalsAndSlots() {
	QObject::connect(btnZoomIn, SIGNAL(pressed()), this, SLOT(btnZoomInPressed()));
	QObject::connect(btnZoomOut, SIGNAL(pressed()), this, SLOT(btnZoomOutPressed()));
	QObject::connect(zoomControl, SIGNAL(valueChanged(int)), this, SLOT(zoomControlValueChanged(int)));
	QObject::connect(btnUp, SIGNAL(clicked()), this, SLOT(btnNavigationUpPressed()));
	QObject::connect(btnLeft, SIGNAL(clicked()), this, SLOT(btnNavigationLeftPressed()));
	QObject::connect(btnCenter, SIGNAL(clicked()), this, SLOT(btnNavigationCenterPressed()));
	QObject::connect(btnRight, SIGNAL(clicked()), this, SLOT(btnNavigationRightPressed()));
	QObject::connect(btnDown, SIGNAL(clicked()), this, SLOT(btnNavigationDownPressed()));
	QObject::connect(btnRotateLeft, SIGNAL(clicked()), this, SLOT(btnRotateLeftPressed()));
	QObject::connect(btnRotateRight, SIGNAL(clicked()), this, SLOT(btnRotateRightPressed()));
	QObject::connect(btnInsertObject, SIGNAL(clicked()), this, SLOT(btnInsertObjectClicked()));
	QObject::connect(btnRemoveObject, SIGNAL(clicked()), this, SLOT(btnRemoveObjectClicked()));
	QObject::connect(btnTransformObject, SIGNAL(clicked()), this, SLOT(btnTransformObjectClicked()));
}

void MainWindow::btnZoomInPressed() {
	int position = this->zoomControl->value();
	this->zoomControl->setValue(position + 5);
}

void MainWindow::btnZoomOutPressed() {
	int position = this->zoomControl->value();
	this->zoomControl->setValue(position - 5);
}

void MainWindow::zoomControlValueChanged(int currentValue) {
	int factor = currentValue - this->zoomValue;
	double zoomFactor = (double) 1 / ((factor * 0.05) + 1);

	if(factor < 0)
		zoomFactor = (double) (factor * -0.05) + 1;

	this->zoomValue = currentValue;
	this->controladorUI->redimensionarWindow(zoomFactor);
}

void MainWindow::btnNavigationUpPressed() {
	this->controladorUI->navegarNoMundo(Mundo::CIMA, 0.1);
}

void MainWindow::btnNavigationLeftPressed() {
	this->controladorUI->navegarNoMundo(Mundo::ESQUERDA, 0.1);
}

void MainWindow::btnNavigationCenterPressed() {
	// Nothing
}

void MainWindow::btnNavigationRightPressed() {
	this->controladorUI->navegarNoMundo(Mundo::DIREITA, 0.1);
}

void MainWindow::btnNavigationDownPressed() {
	this->controladorUI->navegarNoMundo(Mundo::BAIXO, 0.1);
}

void MainWindow::btnRotateLeftPressed() {
	this->controladorUI->rotacionarWindow(-30);
}

void MainWindow::btnRotateRightPressed() {
	this->controladorUI->rotacionarWindow(30);
}

void MainWindow::btnInsertObjectClicked() {
	this->controladorUI->exibirObjectInsertionWindow();
}

void MainWindow::btnRemoveObjectClicked() {
	QItemSelectionModel *selectionModel = this->tableObjects->selectionModel();
	String objectName = "";

	if(selectionModel->selectedRows().size() == 0) {
		QMessageBox messageBox;
		messageBox.critical(this, "Erro", "É necessário selecionar um objeto para ser removido!");
		return;
	}

	for(QModelIndex index : selectionModel->selectedRows()) {
		QTableWidgetItem *item = this->tableObjects->item(index.row(), 1);

		if(item)
			objectName = item->text().toStdString();

	    this->tableObjects->removeRow(index.row());
	}

	this->controladorUI->removerObjeto(objectName);
}

void MainWindow::btnTransformObjectClicked() {
	QItemSelectionModel *selectionModel = this->tableObjects->selectionModel();
	String objectName = "";

	if(selectionModel->selectedRows().size() == 0) {
		QMessageBox messageBox;
		messageBox.critical(this, "Erro", "É necessário selecionar um objeto para aplicar uma transformação!");
		return;
	}

	for(QModelIndex index : selectionModel->selectedRows()) {
		QTableWidgetItem *item = this->tableObjects->item(index.row(), 1);

		if(item)
			objectName = item->text().toStdString();
	}

	this->controladorUI->exibirObjectTransformationWindow(objectName);
}
