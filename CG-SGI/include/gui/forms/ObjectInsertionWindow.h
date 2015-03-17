#ifndef OBJECTINSERTIONWINDOW_H_
#define OBJECTINSERTIONWINDOW_H_

#include <iostream>
#include <QtWidgets/qmessagebox.h>

#include "geometria/Ponto.h"
#include "gui/forms/ui/ui_ObjectInsertionWindow.h"
#include "gui/forms/Window.h"

class ObjectInsertionWindow : public Window, private Ui::ObjectInsertionWindow {
	Q_OBJECT

public:
	ObjectInsertionWindow(ControladorUI* controladorUI, QDialog* parent = 0, Qt::WindowFlags flags = Qt::Widget);
	void clearFields();

protected:
	void connectSignalsAndSlots();
	bool validateFields();

private slots:
	void insertObject();
	void insertPoligonPoint();
	void removePoligonPoint();

};

#endif /* OBJECTINSERTIONWINDOW_H_ */
