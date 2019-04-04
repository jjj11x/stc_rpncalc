#ifndef QTGUI_CALCULATOR_H
#define QTGUI_CALCULATOR_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QTimer>
#include <QMutex>

class CalcThread : public QThread
{
	Q_OBJECT

protected:
	void run() override;
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
	CalcThread calc_thread;
	QString m_lcdText;
	QTimer m_timer;
};





#endif //include guard
