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

import QtQuick 2.0
import QtQuick.Controls 1.0

ApplicationWindow
{
	visible: true
	width:  4 * (100 + 5)
	height: 5 * (100 + 5) + 200 + 30
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

			//handle key presses from physical keyboard
			focus: true //handle keypresses here
			//numbers
			Keys.onDigit0Pressed: _calculator.buttonClicked("4,0")
			Keys.onDigit1Pressed: _calculator.buttonClicked("3,0")
			Keys.onDigit2Pressed: _calculator.buttonClicked("3,1")
			Keys.onDigit3Pressed: _calculator.buttonClicked("3,2")
			Keys.onDigit4Pressed: _calculator.buttonClicked("2,0")
			Keys.onDigit5Pressed: _calculator.buttonClicked("2,1")
			Keys.onDigit6Pressed: _calculator.buttonClicked("2,2")
			Keys.onDigit7Pressed: _calculator.buttonClicked("1,0")
			Keys.onDigit8Pressed: _calculator.buttonClicked("1,1")
			Keys.onDigit9Pressed: _calculator.buttonClicked("1,2")
			//swap
			Keys.onRightPressed: _calculator.buttonClicked("0,1")
			//enter
			Keys.onEnterPressed:  _calculator.buttonClicked("4,2")
			Keys.onReturnPressed: _calculator.buttonClicked("4,2")
			Keys.onSpacePressed:  _calculator.buttonClicked("4,2")
			Keys.onPressed: {
				if ((event.key == Qt.Key_Q) && (event.modifiers == Qt.ControlModifier))
					Qt.quit()
				else if ((event.key == Qt.Key_S))
					_calculator.buttonClicked("0,0")
				else if ((event.key == Qt.Key_Backspace))
					_calculator.buttonClicked("0,3")
				else if ((event.key == Qt.Key_Slash))
					_calculator.buttonClicked("1,3")
				else if ((event.key == Qt.Key_division))
					_calculator.buttonClicked("1,3")
				else if ((event.key == Qt.Key_Asterisk))
					_calculator.buttonClicked("2,3")
				else if ((event.key == Qt.Key_Minus))
					_calculator.buttonClicked("3,3")
				else if ((event.key == Qt.Key_Plus))
					_calculator.buttonClicked("4,3")
				else if ((event.key == Qt.Key_N)) //negate
					_calculator.buttonClicked("0,2")
				else if ((event.key == Qt.Key_Equal)) //enter
					_calculator.buttonClicked("4,2")
				else if ((event.key == Qt.Key_Period)) // "."
					_calculator.buttonClicked("4,1")
				else if ((event.key == Qt.Key_E)) // also "."
					_calculator.buttonClicked("4,1")
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
							//get row/column
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
		["🔃",      "e<sup>x</sup>",   "10<sup>x</sup>",   ""],
		["",      "",      "",       ""],
		["off",   "STO",      "RCL",       "LAST<i>x</i>"]
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
