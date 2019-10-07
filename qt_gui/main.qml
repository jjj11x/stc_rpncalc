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
			color: "#ff9494"
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
						color: getBackgroundColor(parent.objectName, index)
						border { width: 1; color: "black" }
						Text {
							horizontalAlignment: Text.AlignHCenter
							font.pointSize: 16
							color: "gray"
							text: {getShiftedText(parent.parent.objectName, index) + "<br><br>"}
								textFormat: Text.RichText
								anchors.centerIn: parent
						}
						Text {
							horizontalAlignment: Text.AlignHCenter
							font.pointSize: 24
							text: {"<br>" + getText(parent.parent.objectName, index) + "<br>"}
							textFormat: Text.RichText
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

	function getText(row, col) {
		var keys = [
			["Shift", "x ⇄ y", "±",     "←"],
			["7",     "8",     "9",     "÷"],
			["4",     "5",     "6",     "×"],
			["1",     "2",     "3",     "-"],
			["0",     ".",     "Enter", "+"]
		]

		return "<b>" + keys[row][col] + "</b>"
	}
	
	function getShiftedText(row, col) {
		var shifted_keys = [
		["Shift", "1/x", " √<span style=\"text-decoration: overline\">x</span> ",   "CL<i>x</i>"],
		["y<sup>x</sup> ",   "ln(x)", "log(x)", ""],
		["",      "e<sup>x</sup>",   "10<sup>x</sup>",   ""],
		["",      "",      "",       ""],
		["off",   "",      "",       ""]
		]
		
		return "<small>" + shifted_keys[row][col] + "</small>"
	}
	
	function getBackgroundColor(row, col) {
		var background_color = [
			["white",   "white",   "white", "#ff9494"],
			["#eeeeee", "#eeeeee", "#eeeeee", "white"],
			["#eeeeee", "#eeeeee", "#eeeeee", "white"],
			["#eeeeee", "#eeeeee", "#eeeeee", "white"],
			["#eeeeee", "#eeeeee", "white",   "white"]
		]
		
		return background_color[row][col]
	}
}
