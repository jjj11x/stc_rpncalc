#include <QDebug>

#include "calculator.h"

Calculator::Calculator(QObject *parent) :
	QObject(parent)
{
}

void Calculator::buttonClicked(const QString& in) {
	qDebug() << in;
}
