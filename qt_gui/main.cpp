// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
