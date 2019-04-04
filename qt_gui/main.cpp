#include <QApplication>
#include <QFontDatabase>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "calculator.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	//QML engine
	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	
	//calculator
	Calculator calculator;
	engine.rootContext()->setContextProperty("_calculator", &calculator);

	//fixed-width font for LCD
	QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	fixedFont.setStyleHint(QFont::TypeWriter);
	fixedFont.setPointSize(24);
	engine.rootContext()->setContextProperty("_fixedFont", fixedFont);

	//handle quit
	QObject::connect(&app, &QApplication::aboutToQuit, &calculator, &Calculator::quit);

	return app.exec();
}
