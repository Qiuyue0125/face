import QtQuick 2.0
import QtQuick.Controls
import client 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 480
    height: 680
    flags: Qt.FramelessWindowHint | Qt.Window
    color: "transparent"
    title: qsTr("人脸识别")

    FaceDetector {//单例检测类(ImageProvider)
        id: faceDetector
        onImageCaptured:  {
            faceImage.imageId = (faceImage.imageId + 1) % 500
        }
        onIdentitySuccess: function(name) {
            message.name = name
            message.visible = true
        }

    }//单例检测类(ImageProvider)

    Rectangle {//主窗矩形
        id: mainRec
        anchors.fill: parent
        radius: 20
        color: "black"
        smooth: true

        Image {//背景图(采集图像)
            id: faceImage
            property real radius: mainRec.radius
            property int imageId: -1
            width: mainRec.width - 5
            height: mainRec.height - 5
            anchors.centerIn: mainRec
            anchors.margins: 50
            fillMode: Image.PreserveAspectCrop
            source: "image://provider/" + imageId
            layer.enabled: true
            smooth: true
            layer.effect: ShaderEffect {
                fragmentShader: Qt.resolvedUrl("qrc:/shaders/round_corner.frag.qsb")
                property real radius: faceImage.radius
                property variant sourceTexture: faceImage.source
                property var size: Qt.size(faceImage.width, faceImage.height)
            }
        }//背景图(采集图像)

        MouseArea {//拖动区域
            width: root.width
            height: 100
            property int dragX: 0
            property int dragY: 0
            onPressed: function(mouse) {
                dragX = mouse.x
                dragY = mouse.y
            }
            onPositionChanged: function(mouse) {
                root.x += mouse.x - dragX
                root.y += mouse.y - dragY
            }
        }//拖动区域

        Text {//显示时间文本
            id: timeDisplay
            text: ""
            color: "black"
            font.pixelSize: 23
            x: 10
            y: 10
            anchors.margins: 10
        }//显示时间文本

        Text {//显示地址文本
            id: locationDisplay
            text: ""
            color: "black"
            font.pixelSize: 23
            anchors.left: timeDisplay.right
            anchors.verticalCenter: timeDisplay.verticalCenter
            anchors.margins: 30
        }//显示地址文本

        Rectangle{//关闭按钮
            id: close
            x: root.width - 40
            y: 5
            width: 30
            height: 30
            radius: 5
            color: "transparent"
            Text{
                text: qsTr("✕")
                font.pixelSize: 22
                anchors.centerIn: parent
            }
            MouseArea{
                hoverEnabled: true
                anchors.fill: close
                onEntered: {
                    close.color = "#CCCCCC";
                }
                onExited: {
                    close.color = "transparent"
                }
                onClicked: {
                    if (timeDisplay.text === "") {
                        Qt.quit()
                    }
                    faceDetector.detectQuit()
                    detectionButton.visible = true
                    inputAdress.visible = true
                    faceImage.imageId = -1
                    faceBorder.visible = false
                    timeTimer.stop()
                    timeDisplay.text = ""
                    locationDisplay.text = ""
                }
            }
        }//关闭按钮

        Image{//头部轮廓
            id: faceBorder
            source: "qrc:/resources/face_border.png"
            visible: false
            width: mainRec.width * 0.95
            height: width * (sourceSize.height / sourceSize.width)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 50
        }//头部轮廓

        Rectangle{//显示认证信息
            id: message
            radius: mainRec.radius
            height: 150
            width: mainRec.width
            color: "black"
            y: root.height - message.height
            property string name: qsTr("张齐宇")
            visible: false
            opacity: 1
            onVisibleChanged: {
                if (visible) {
                    message.opacity = 1;
                    messageAnima.restart()
                }
            }
            NumberAnimation on y{
                id: messageAnima
                from: message.y + 100
                to: message.y
                duration: 300
                easing.type: Easing.OutQuad
                onStopped: {
                    var timer = Qt.createQmlObject('import QtQuick 2.15; Timer { running: false; interval: 2000; repeat: false; onTriggered: function() {
                        opacityAnimation.start();
                    } }', root);
                    timer.restart();
                }
            }
            NumberAnimation on opacity {
                id: opacityAnimation
                from: 1
                to: 0
                duration: 300
                easing.type: Easing.InQuad
                onStopped: {
                    message.visible = false;
                }
            }
            Image{
                id: messageBackground
                smooth: true
                property real radius: mainRec.radius
                width: message.width - 4
                height: message.height - 4
                anchors.centerIn: message
                layer.enabled: true
                source: "qrc:/resources/Back.png"
                fillMode: Image.PreserveAspectCrop
                layer.effect: ShaderEffect {
                    fragmentShader: Qt.resolvedUrl("qrc:/shaders/round_corner.frag.qsb")
                    property real radius: messageBackground.radius
                    property variant sourceTexture: messageBackground.source
                    property var size: Qt.size(messageBackground.width, messageBackground.height)
                }
            }
            Text{//认证信息
                id: messageText
                anchors.centerIn: messageBackground
                text: qsTr("  认证成功\n姓名:") + message.name
                font.pixelSize: 23
                wrapMode: Text.WordWrap
            }//认证信息
        }//显示认证信息

        Timer {//更新时间定时器
            id: timeTimer
            interval: 1000
            running: false
            repeat: true
            onTriggered: {
                timeDisplay.text = Qt.formatDateTime(new Date(), "yyyy-MM-dd hh:mm:ss")
            }
        }//更新时间定时器

        Button {//开始检测按钮
            id: detectionButton
            width: 130
            height: 40
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
            enabled: inputAdress.text.length > 0
            hoverEnabled: inputAdress.text.length > 0
            onClicked: {
                detectionButton.visible = false
                inputAdress.visible = false
                faceDetector.startDetection(inputAdress.text)
                locationDisplay.text = inputAdress.text
                timeDisplay.text = Qt.formatDateTime(new Date(), "yyyy-MM-dd hh:mm:ss")
                faceBorder.visible = true
                timeTimer.start()
            }
            background: Rectangle {
                border.color: "#26282a"
                border.width: 1
                radius: 4
                color: enabled ? "white" : "#EFEFEF"
            }
            Text{
                text: qsTr("开始检测")
                anchors.centerIn: parent
                color: "black"
                font.pixelSize: 20
            }
        }//开始检测按钮

        TextField{//输入地址
            id: inputAdress
            visible: true
            font.pixelSize: 20
            width: detectionButton.width
            height: detectionButton.height
            text: ""
            placeholderText: qsTr("请输入地址")
            placeholderTextColor: "gray"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            y: detectionButton.y - height - 10
            anchors.horizontalCenter: parent.horizontalCenter
            background: Rectangle {
                color: "white"
                radius: 5
                border.width: 1
                border.color: "black"
            }
        }//输入地址

    }//主窗矩形
}
