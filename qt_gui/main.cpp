#include <QApplication>
#include <QFontDatabase>
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

	QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	fixedFont.setStyleHint(QFont::TypeWriter);
	engine.rootContext()->setContextProperty("_fixedFont", fixedFont);

	return app.exec();
}
