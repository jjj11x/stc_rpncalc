#include <QApplication>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "calculator.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	qmlRegisterType<Calculator>("calculator.lcd", 1, 0, "CLcd");

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	
	Calculator calculator;
	engine.rootContext()->setContextProperty("_calculator", &calculator);

	return app.exec();
}
