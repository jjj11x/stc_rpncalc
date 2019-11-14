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

#ifndef QTGUI_CALCULATOR_H
#define QTGUI_CALCULATOR_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QMutex>


class Calculator; //forward declare

class CalcMainThread : public QThread //thread runs code in main.c
{
	Q_OBJECT

protected:
	void run() override;
};


class CalcLcdThread : public QThread //thread just checks if we need to update lcd
{
	Q_OBJECT

public:
	explicit CalcLcdThread(Calculator& calc);
protected:
	void run() override;
private:
	Calculator& m_calc;
};




class Calculator : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString lcdText READ lcdText WRITE setLcdText NOTIFY lcdTextChanged)


public:
	explicit Calculator(QObject *parent = 0);
	~Calculator();

	inline QString lcdText(){return m_lcdText;}
	void setLcdText(const QString &lcdText);

signals:
	void lcdTextChanged();

public slots:
	void buttonClicked(const QString& in);
	void updateLcd();
	void quit();

private:
	CalcMainThread calc_thread;
	CalcLcdThread lcd_thread;
	QString m_lcdText;
};









#endif //include guard
