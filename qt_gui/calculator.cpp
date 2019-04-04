#include <QDebug>
#include "calc_main.h"
#include "../src/lcd.h"
#include "calculator.h"

Calculator::Calculator(QObject *parent) :
	QObject(parent),
	m_lcdText("calculator initial text"),
	m_timer(this)
{
	qDebug() << "Starting calculator thread";
	calc_thread.start();
	qDebug() << "calculator thread started";

	QObject::connect(&m_timer, &QTimer::timeout, this, &Calculator::updateLcd);
	m_timer.start(200);

	updateLcd();
}

Calculator::~Calculator(){
	ExitCalcMain = 1;
	while (!calc_thread.isFinished()); //TODO: timeout
}

void Calculator::buttonClicked(const QString& in) {
	QStringList split = in.split(",");
	int8_t row = split[0].toInt();
	int8_t col = split[1].toInt();
	//translate column from left indexed, to right indexed
	static const int NUM_COLS = 4;
	col = (NUM_COLS - 1) - col;
	//get keycode
	int8_t keycode = col + NUM_COLS*row;
//	qDebug() << "keycode: " << keycode;
//	qDebug() << " row: " << row << ", col: " << col;
	//push keycode
	KeyMutex.lock();
	#define INCR_NEW_KEY_I(i) i = (i + 1) & 3
	if (!NewKeyEmpty && (new_key_write_i == new_key_read_i)){
		printf("ERROR: key fifo full\n");
		KeyMutex.unlock();
		return;
	}
	NewKeyBuf[new_key_write_i] = keycode;
	INCR_NEW_KEY_I(new_key_write_i);
	NewKeyEmpty = 0;
	KeyMutex.unlock();
}

void Calculator::updateLcd() {
	QString tmp("lcd text:\n");
	const char* lcd_buf = get_lcd_buf();
	for (int i = 0; i < MAX_ROWS; i++){
		tmp += "|";
		for (int j = 0; j < MAX_CHARS_PER_LINE; j++){
			tmp += lcd_buf[j + i*MAX_CHARS_PER_LINE];
		}
		tmp += "|\n";
	}
//	qDebug() << "update lcd:" << tmp.toStdString().c_str();

	setLcdText(tmp);
}

void Calculator::setLcdText(const QString &lcdText){
	m_lcdText = lcdText;
	emit lcdTextChanged();
}



void CalcThread::run() {
	calc_main();
}

