import QtQuick 2.0
import QtQuick.Controls 1.0
import Qt3D.Input 2.0

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
		height: 5 * (100 + 5) + 200
		
		//LCD
		Rectangle {
			id: lcd;
			color: "gray"
			width: 4 * (100 + 5) - 5
			height: 200
		}
		
		//Keyboard
		Repeater {
			model: 5;
			//row
			delegate: Row {
				id: key_row;
				spacing: 5;
				//keys within row
				Repeater {
					model: 4;
					delegate: Rectangle {
						id: key_key;
						width: 100;
						height: 100;
						color: "blue"
						border { width: 1; color: "black" }
						Text {
							text: index
							anchors.centerIn: parent
						}
						MouseHandler {
							onClicked: _calculator.buttonClicked(index)
						}
					}
				}
			}
		}
	}
}
