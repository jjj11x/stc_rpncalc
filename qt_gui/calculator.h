#ifndef QTGUI_CALCULATOR_H
#define QTGUI_CALCULATOR_H

#include <QObject>

class Calculator : public QObject
{
	Q_OBJECT
	
public:
	explicit Calculator(QObject *parent = 0);
	
public slots:
	void buttonClicked(const QString& in);
	
};



#endif //include guard
