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

private:
	CalcMainThread calc_thread;
	CalcLcdThread lcd_thread;
	QString m_lcdText;
};









#endif //include guard
