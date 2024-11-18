import QtQuick
import Qt.labs.platform
import QtQuick.Controls
import QtQuick.Layouts
import ServerCPP 1.0

ApplicationWindow {
    id: root
    width: 700
    height: 600
    minimumWidth: 700
    visible: true
    title: qsTr("Server")

    Rectangle{//背景矩形
        id: backgroundRec
        anchors.fill: parent
        MouseArea{
            anchors.fill: parent
            onClicked: {
                parent.focus = true;
            }
        }
        Image {
            id: background
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
            source: "qrc:/resources/094 Cloudy Apple - trans.png"
            layer.enabled: true
            smooth: true
            layer.effect: ShaderEffect {
                fragmentShader: Qt.resolvedUrl("qrc:/shaders/round_corner.frag.qsb")
                property real radius: 0
                property variant sourceTexture: background.source
                property var size: Qt.size(background.width, background.height)
            }
        }
    }//背景矩形

    ColumnLayout {//主垂直布局
        anchors.fill: parent
        spacing: 10

        RowLayout {//水平布局(按钮)
            spacing: 20
            Layout.alignment: Qt.AlignLeft
            Button {
                text: qsTr("开始监听")
                font.pixelSize: attendanceButtom.font.pixelSize
                Layout.preferredHeight: attendanceButtom.myHeight
                Layout.preferredWidth: attendanceButtom.myWidth
                background: Rectangle {
                    opacity: 1
                    border.color: "#26282a"
                    border.width: 1
                    radius: 4
                }
                onClicked: {
                    if(text === qsTr("开始监听")){
                        cpp.listen()
                        text = qsTr("停止监听")
                    }
                    else{
                        cpp.stopListen()
                        text = qsTr("开始监听")
                    }
                }
            }
            Button {
                text: qsTr("刷新表格")
                font.pixelSize: attendanceButtom.font.pixelSize
                Layout.preferredHeight: attendanceButtom.myHeight
                Layout.preferredWidth: attendanceButtom.myWidth
                background: Rectangle {
                    opacity: 1
                    border.color: "#26282a"
                    border.width: 1
                    radius: 4
                }
                onClicked: {
                    cpp.refresh()
                }
            }
            Item {
                Layout.fillWidth: true
            }
            Button {
                id:attendanceButtom
                text: qsTr("展示考勤记录")
                Layout.alignment: Qt.AlignRight
                property int myHeight: 30
                property int myWidth: 140
                font.pixelSize: 18
                Layout.preferredHeight: myHeight
                Layout.preferredWidth: myWidth
                background: Rectangle {
                    opacity: 1
                    border.color: "#26282a"
                    border.width: 1
                    radius: 4
                }
                onClicked: {
                    myTable.model = cpp.attendanceModel
                    title0.text = qsTr("打卡姓名")
                    title1.text = qsTr("打卡地点")
                    title2.text = qsTr("打卡时间")
                }
            }
            Button {
                id: deviceButton
                text: qsTr("展示设备信息")
                Layout.alignment: Qt.AlignRight
                font.pixelSize: attendanceButtom.font.pixelSize
                Layout.preferredHeight: attendanceButtom.myHeight
                Layout.preferredWidth: attendanceButtom.myWidth
                background: Rectangle {
                    opacity: 1
                    border.color: "#26282a"
                    border.width: 1
                    radius: 4
                }
                onClicked: {
                    myTable.model = cpp.deviceModel
                    title0.text = qsTr("设备序号")
                    title1.text = qsTr("设备名称")
                    title2.text = qsTr("安装地点")
                }
            }
        }//水平布局(按钮)

        RowLayout {//表头水平布局
            spacing: 0
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            Text {
                id: title0
                text: qsTr("打卡姓名")
                font.pixelSize: 18
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                id: title1
                text: qsTr("打卡地点")
                font.pixelSize: 18
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                id: title2
                text: qsTr("打卡时间")
                font.pixelSize: 18
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
            Item{
                width: scro.width
            }
        }//表头水平布局

        Rectangle {//显示表格的矩形
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "transparent"
            border.color: "transparent"
            radius: 5
            StackLayout {
                anchors.fill: parent
                Layout.fillWidth: true
                Layout.fillHeight: true
                TableView {
                    id: myTable
                    model: cpp.attendanceModel
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    visible: true
                    ScrollBar.vertical: ScrollBar {
                        id: scro
                        width: 15
                    }
                    delegate: Item {
                        id: modelItem
                        implicitWidth: root.width / 3 - scro.width / 3
                        width: implicitWidth
                        implicitHeight: 50
                        height: implicitHeight
                        Rectangle {
                            anchors.fill: parent
                            color: Qt.rgba(1.0, 1.0, 1.0, 0.25)
                            border.color: "#26282a"
                            border.width: 1
                            radius: 10
                            Text {
                                anchors.centerIn: parent
                                text: model.modelData
                                color: "#282828"
                                font.pixelSize: 18
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }
                }
            }
        }//显示表格的矩形

        RowLayout {//水平布局(插入设备信息)
            spacing: 30
            Layout.alignment: Qt.AlignLeft
            TextField{
                id: deviceId
                visible: true
                Layout.preferredHeight: 30
                Layout.preferredWidth: 180
                font.pixelSize: 18
                text: ""
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                placeholderText: qsTr("请输入设备名称")
                placeholderTextColor: "gray"
                background: Rectangle {
                    color: "white"
                    radius: 5
                    border.width: 1
                    border.color: "black"
                }
            }
            TextField{
                id: deviceLocation
                visible: true
                Layout.preferredHeight: 30
                Layout.preferredWidth: 180
                font.pixelSize: 18
                text: ""
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                placeholderText: qsTr("请输入设备地址")
                placeholderTextColor: "gray"
                background: Rectangle {
                    color: "white"
                    radius: 5
                    border.width: 1
                    border.color: "black"
                }
            }
            Item {
                Layout.fillWidth: true
            }
            Button {
                id: addDeviceButton
                text: qsTr("添加设备信息")
                Layout.alignment: Qt.AlignRight
                font.pixelSize: 18
                Layout.preferredHeight: 40
                Layout.preferredWidth: 120
                enabled: deviceId.text.length > 0 && deviceLocation.length > 0
                hoverEnabled: deviceId.text.length > 0 && deviceLocation.length > 0
                background: Rectangle {
                    border.color: "#26282a"
                    border.width: 1
                    radius: 4
                    color: enabled ? "white" : "#EFEFEF"
                }
                onClicked: {
                    deviceConfirmDialog.open()
                }
            }
        }//水平布局(插入设备信息)

        RowLayout {//水平布局(插入人脸信息)
            spacing: 30
            Layout.alignment: Qt.AlignLeft
            TextField{
                id: faceName
                visible: true
                Layout.preferredHeight: 30
                Layout.preferredWidth: 180
                font.pixelSize: 18
                text: ""
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                placeholderText: qsTr("请输入姓名")
                placeholderTextColor: "gray"
                background: Rectangle {
                    color: "white"
                    radius: 5
                    border.width: 1
                    border.color: "black"
                }
            }
            TextField{
                id: faceLocation
                visible: true
                Layout.preferredHeight: 30
                Layout.preferredWidth: 180
                font.pixelSize: 18
                text: ""
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                placeholderText: qsTr("请输入地址")
                placeholderTextColor: "gray"
                background: Rectangle {
                    color: "white"
                    radius: 5
                    border.width: 1
                    border.color: "black"
                }
            }
            Button {
                id: faceImageButton
                property bool faceFlag: false
                property bool faceChooingFlag: false
                text: qsTr("选择资源")
                font.pixelSize: 18
                Layout.preferredHeight: 30
                Layout.preferredWidth: 90
                Text{
                    id: faceImageLogo
                    font.bold: true
                    anchors.left: faceImageButton.right
                    anchors.leftMargin: 10
                    anchors.verticalCenter: faceImageButton.verticalCenter
                    font.pixelSize: 20
                    text: "X"
                    color: "red"
                }
                background: Rectangle {
                    border.color: "#26282a"
                    border.width: 1
                    radius: 4
                    color: "white"
                }
                onClicked: {
                    openFileDialog.open()
                }
                onEnabledChanged: {
                    if (enabled) {
                        faceImageButton.faceChooingFlag = false
                    } else {
                        faceImageButton.faceChooingFlag = true
                    }
                }
                onFaceFlagChanged: {
                    if(faceFlag){
                        faceImageLogo.text = "√"
                        faceImageLogo.color = "green"
                    }
                    else{
                        faceImageLogo.text = "X"
                        faceImageLogo.color = "red"
                    }
                }
            }
            Item {
                Layout.fillWidth: true
            }
            Button {
                id: addFaceButton
                text: qsTr("添加人脸信息")
                Layout.alignment: Qt.AlignRight
                font.pixelSize: 18
                Layout.preferredHeight: 40
                Layout.preferredWidth: 120
                enabled: faceLocation.text.length > 0 && faceName.length > 0 && faceImageButton.faceFlag && !faceImageButton.faceChooingFlag
                hoverEnabled: faceLocation.text.length > 0 && faceName.length > 0 && faceImageButton.faceFlag && !faceImageButton.faceChooingFlag
                background: Rectangle {
                    border.color: "#26282a"
                    border.width: 1
                    radius: 4
                    color: enabled ? "white" : "#EFEFEF"
                }
                onClicked: {
                    faceConfirmDialog.open()
                }
            }
        }//水平布局(插入人脸信息)

        RowLayout {//水平布局(执行数据库操作)
            spacing: 30
            Layout.alignment: Qt.AlignLeft
            TextField{
                id: qry
                visible: true
                Layout.preferredHeight: 30
                Layout.fillWidth: true
                font.pixelSize: 18
                text: ""
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                placeholderText: qsTr("请输入sql语句")
                placeholderTextColor: "gray"
                background: Rectangle {
                    color: "white"
                    radius: 5
                    border.width: 1
                    border.color: "black"
                }
            }
            Button {
                id: qryButton
                text: qsTr("执行sql操作")
                Layout.alignment: Qt.AlignRight
                font.pixelSize: 18
                Layout.preferredHeight: 40
                Layout.preferredWidth: 120
                enabled: qry.text.length > 0
                hoverEnabled: qry.text.length > 0
                background: Rectangle {
                    border.color: "#26282a"
                    border.width: 1
                    radius: 4
                    color: enabled ? "white" : "#EFEFEF"
                }
                onClicked: {
                    cpp.sql(qry.text)
                }
            }
        }//水平布局(执行数据库操作)

    }//主垂直布局

    ServerCPP {//cpp类
        id: cpp
        onFail: {
            messageDialog.tit = qsTr("操作失败")
            messageDialog.mes = qsTr("操作执行失败")
            messageDialog.open()
        }
        onSucceed: {
            messageDialog.tit = qsTr("操作成功")
            messageDialog.mes = qsTr("操作执行成功")
            messageDialog.open()
        }
        onLogoFail: {
            faceImageButton.enabled = true
            faceImageButton.faceFlag = false
            messageDialog.tit = qsTr("图片失败")
            messageDialog.mes = qsTr("识别图片失败")
            messageDialog.open()
            faceImageLogo.text = "X"
            faceImageLogo.color = "red"
        }
        onLogoSucceed: {
            faceImageButton.enabled = true
            faceImageButton.faceFlag = true
            messageDialog.tit = qsTr("识别成功")
            messageDialog.mes = qsTr("识别图片成功")
            messageDialog.open()
        }
    }//cpp类

    MessageDialog {//添加设备 确认窗口
        id: deviceConfirmDialog
        title: qsTr("添加设备信息")
        text: qsTr("您确定要添加这个设备信息吗？")
        onAccepted: {
            cpp.addDevice(deviceId.text, deviceLocation.text)
            deviceId.text = ""
            deviceLocation.text = ""
        }
    }//添加设备 确认窗口

    MessageDialog {//添加人脸 确认窗口
        id: faceConfirmDialog
        title: qsTr("添加人脸信息")
        text: qsTr("您确定要添加这个人脸信息吗？")
        onAccepted: {
            cpp.addFace(faceName.text, faceLocation.text)
            faceName.text = ""
            faceLocation.text = ""
            faceImageButton.faceFlag = false
        }
    }//添加人脸 确认窗口

    FileDialog {//选择人脸图片窗口
        id: openFileDialog
        title: qsTr("选择人脸图片")
        onAccepted: {
            faceImageLogo.text = qsTr("···")
            faceImageLogo.color = "black"
            faceImageButton.enabled = false//图片处理完毕才可以再次点击
            cpp.chooseFaceImage(openFileDialog.currentFile)
        }
    }//选择人脸图片窗口

    MessageDialog {//确认提示窗口
        id: messageDialog
        property string tit: qsTr("操作成功")
        property string mes: qsTr("操作执行成功")
        title: tit
        text: mes
    }//确认提示窗口
}
