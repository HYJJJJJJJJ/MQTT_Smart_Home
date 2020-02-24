#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qt_image_include/gaugeweather.h"
#include "qt_image_include/colorpanelhsb.h"
#include <QSlider>
#include <QElapsedTimer>
#include "QDebug"
#include "QString"
#include "mqtt/qmqtt.h"

#define topicSub_max    3
#define topicPub_max    2

enum Pubtopic{
    Fan=0,Led
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);    
    ~MainWindow();

private slots:// 槽函数定义
    void onMQTT_Connected();
    void onMQTT_Disconnected();
    void onMQTT_Subscribed(const QString &topic);
    void onMQTT_Received(const QMQTT::Message &message);
    void on_pushButton_connect_clicked();
    void onFanDuty_changed(int duty);
    void onColorDuty_changed(const QColor &color, double hue, double sat);
    void on_pushButton_setColor_clicked();

private:
    Ui::MainWindow *ui;
    GaugeWeather *gaugeweather;
    ColorPanelHSB *colorpanel;
    QMQTT::Client *client;
    Pubtopic pubtopic=Fan;

protected:
    QString topicSub[topicSub_max] = {"wd","sd","gz"};
    QString topicPub[topicPub_max] = {"fan","led"};

};

#endif // MAINWINDOW_H
