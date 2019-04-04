import QtQuick 2.0
import QtQuick.Controls 1.0

ApplicationWindow
{
	visible: true
	width:  1000
	height: 1000
	title: qsTr("RPN Calculator")
	
	//calculator
	Column {
		id: base;
		spacing: 5;
		width:  4 * (100 + 5)
		height: 5 * (100 + 5) + 200 + 30

		//quit button
		Rectangle{
			width: 100
			height: 25
			color: "red"
			Text {text: "Quit"}
			MouseArea {
				onClicked: Qt.quit()
				anchors.fill: parent
			}
		}
		
		//LCD
		Rectangle {
			objectName: "lcd";
			color: "lightgray"
			width: 4 * (100 + 5) - 5
			height: 200
			Text {
				objectName: "lcd_text";
				text: _calculator.lcdText
				font: _fixedFont
				anchors.centerIn: parent
			}
		}
		
		//Keyboard
		Repeater {
			model: 5;
			//row
			delegate: Row {
				id: key_row
				spacing: 5
				objectName: index
				//keys within row
				Repeater {
					model: 4;
					delegate: Rectangle {
						id: key_key;
						width: 100;
						height: 100;
						color: "gray"
						border { width: 1; color: "black" }
						Text {
							text: index
							anchors.centerIn: parent
						}
						MouseArea {
							onClicked: _calculator.buttonClicked(parent.parent.objectName + "," + index)
							anchors.fill: parent
						}
					}
				}
			}
		}
	}
}
