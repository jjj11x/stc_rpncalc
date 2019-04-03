#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "calculator.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	
	Calculator calculator;
	engine.rootContext()->setContextProperty("_calculator", &calculator);
	
	return app.exec();
}
