#include "mainwindow.h"
#include "ui_mainwindow.h"

//开始读程序之前先认真把 **名词说明** 和 **概念说明** 慢慢看一遍！！！！！！！！

/*********************************名词说明*************************************
 * topic-----------------------------------------------------MQTT的主题
 * payload------------------------------------------------MQTT收到的消息
 * client--------------------------------------------------------客户端
 * gaugeweather-------------------------------------------温湿度控件对象
 * message-----------------MQTT的消息包，主要由主题+负载组成(topic+payload)
 * publish----------------------------------------------MQTT发布message
 * subscribe-----------------------------------------------MQTT订阅主题
 * 占空比/duty------------------风扇的转速，范围（0~100），数值越大，转速越快
*****************************************************************************/

/*************************************************************概念说明*******************************************************************************
 * 信号与槽：信号与槽通过connect函数连接，实现发射信号后立即执行槽函数，
 * 例如：    connect(client, SIGNAL(subscribed(const QString &)),        //将MQTT客户端的订阅成功信号与MainWindow的onMQTT_Subscribed()槽函数连接
                this, SLOT(onMQTT_Subscribed(QString)));                //也就是MQTT客户端一旦成功订阅一个主题，将会执行onMQTT_Subscribed()中的内容，
                                                                        //主题的名称将以参数的形式传递。
 * 执行的流程为：手动执行client->subscribe();函数，软件向服务端（阿里云服务器）发送订阅请求，若订阅成功，client对象自动
 * 发出subscribed信号，client发出信号后，根据之前执行过的connect函数，程序就会自动调用onMQTT_Subscribed函数，进行相关数据处理
 *
 * MQTT客户端：能够对服务端（阿里云）执行连接、断开连接、订阅与发布等操作，实例化原型为client，相关函数调用方法如下所示：
 *            client->setHost(QHostAddress(host));              //设置客户端要连接的目标服务器的ip，我们的ip为47.102.137.96
 *            client->setPort(port);                            //设置客户端要连接的目标服务器的端口，我们的端口为1883
 *            client->connectToHost();                          //根据上面两条的配置，连接目标服务器
 *            client->disconnectFromHost();                     //断开已连接的服务器
 *            client->subscribe(topic, qosSub);                 //订阅已连接的服务器的某个topic
 *            client->publish(message);                         //向服务器发布message包，数据包中包含topic与payload数据
 *            client->isConnectedToHost();                      //判断客户端是否已经连接上某个服务器，若已连接返回true，否则返回false
 *
 * message数据包：根据"qmqtt_message.h"中的对Message类的定义：
 *            explicit Message(const quint16 id, const QString &topic, const QByteArray &payload,
 *                  const quint8 qos = 0, const bool retain = false, const bool dup = false);
 * 一个message包包含：id，topic（消息的主题），payload（消息的负载，也就是消息的实际内容），pos（消息优先级），
 * retain与dup我也不懂啥意思，反正咱也用不着，我们需要用到的只是topic与payload两个部分
 * 总的来说，我们可以将message这个对象理解为一个结构体，通过message.topic或message.payload调用主题与负载，
 * 在发布消息或订阅消息时，将整个结构体（其实是对象，说结构体是为了方便理解）作为参数发送或接收处理。
 *
 * QString类：QT自定义的一个类，用于QT中各种控件需要用到字符串的地方，类似C++中的String类
 * 以下是这份程序中几处用到的该类的方法说明：
 *      QString str;                        //定义一个QString的对象"str"，可以理解为定义变量，类似char *str;
 *      str.toUShort();                     //将str转为无符号短整型，范围0~(2^16)-1，范围可以不用知道，简单来说就是"25"==>25
 *      str.toInt();                        //将str转为整型，范围-(2^31)~(2^31)-1，简单来说就是"25"==>25
 *      str.toDouble();                     //将str转为双精度浮点型，范围我也忘了，简单来说就是"25"==>25.000000
 *      QString::number(duty).toUtf8()      //将整型的duty转化为UTF8编码的字符串，简单来说就是25==》"25"
******************************************************************************************************************************************************/

//构造函数（初始化）
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //由于温湿度组件是第三方的组件动态库，无法在.ui设计文件中编辑，所以在代码中手动实例化进行添加
    /*******************************实例化一个温湿度组件（新建一个温湿度组件）*********************************************/
    gaugeweather=new GaugeWeather();                            //实例化对象
    gaugeweather->setOuterValue(0);                             //设置外环初始值为0
    gaugeweather->setInnerValue(0);                             //设置内环初始值为0
    gaugeweather->setAnimation(true);                           //开启内外环改变时的动画
    gaugeweather->setAnimationStep(1);                          //设置动画步进（速度）
    //    gaugeweather->setInnerRingBgColor(Qt::black);
    ui->verticalLayout_weather->addWidget(gaugeweather);                //在ui中的垂直布局组件将温湿度组件显示出来
    /*****************************************************************************************************************/

    colorpanel=new ColorPanelHSB();
    colorpanel->setBorderColor(Qt::gray);
    ui->verticalLayout_color->addWidget(colorpanel);

    ui->horizontalSlider->setMinimum(0);                        //设置水平滑动控件最小值为0
    ui->horizontalSlider->setMaximum(100);                      //设置水平滑动控件最大值为100
    ui->spinBox->setMinimum(0);                                 //设置微调框控件最小值为0
    ui->spinBox->setMaximum(100);                               //设置微调框控件最大值为100

    /******************************实例化一个MQTT对象（新建一个MQTT客户端），并绑定相关信号与槽*****************************************/
    client=new QMQTT::Client();                                 //实例化MQTT客户端
    connect(client, SIGNAL(connected()),                        //将MQTT客户端的连接成功信号与MainWindow的onMQTT_Connected()槽函数连接
            this, SLOT(onMQTT_Connected()));                    //也就是MQTT客户端一旦连接成功，就会执行onMQTT_Connected()中的内容
    connect(client, SIGNAL(disconnected()),                     //将MQTT客户端的断开成功信号与MainWindow的onMQTT_Disconnected()槽函数连接
            this, SLOT(onMQTT_Disconnected()));                 //也就是MQTT客户端一旦断开连接成功，就会执行onMQTT_Disconnected()中的内容
    connect(client, SIGNAL(subscribed(const QString &)),        //将MQTT客户端的订阅成功信号与MainWindow的onMQTT_Subscribed()槽函数连接
            this, SLOT(onMQTT_Subscribed(QString)));            //也就是MQTT客户端一旦成功订阅一个主题，将会执行onMQTT_Subscribed()中的内容，
    //主题的名称将以参数的形式传递。
    connect(client, SIGNAL(received(const QMQTT::Message &)),   //将MQTT客户端的接收消息成功信号与MainWindow的onMQTT_Received()槽函数连接
            this, SLOT(onMQTT_Received(QMQTT::Message)));       //也就是MQTT客户端一旦成功接收到任意一个message，将会执行onMQTT_Received()中的内容，
    //message将以参数的形式传递。

    connect(ui->spinBox, SIGNAL(valueChanged(int)),             //将微调框的变化信号与onFanDuty_changed槽函数相连接，在槽函数中发布message到服务器
            this, SLOT(onFanDuty_changed(int)));

    connect(colorpanel, SIGNAL(colorChanged(const QColor, double, double)),             //将微调框的变化信号与onFanDuty_changed槽函数相连接，在槽函数中发布message到服务器
            this, SLOT(onColorDuty_changed(const QColor, double, double)));

    /****************************************将横向滑动控件与微调框connect，两个控件同步变化******************************************/
    connect(ui->spinBox, SIGNAL(valueChanged(int)),
            ui->horizontalSlider, SLOT(setValue(int)));
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),
            ui->spinBox, SLOT(setValue(int)));
    /***************************************************************************************************************************/
}

MainWindow::~MainWindow()
{
    delete ui;
}

//连接/断开服务器的按键被按下
void MainWindow::on_pushButton_connect_clicked()
{
    if(!client->isConnectedToHost()){                                   //当客户端还没有连接上服务器
        QString host=ui->lineEdit_host->text();                         //获取lineEdit_host栏中ip数据，存储到host中
        quint16 port=ui->lineEdit_port->text().toUShort();              //获取lineEdit_port栏中的端口，存储到port中
        client->setHost(QHostAddress(host));                            //设置client要连接的目标服务器的ip地址
        client->setPort(port);                                          //设置client要连接的目标服务器的端口
        client->connectToHost();                                        //根据上两条设置，连接服务器，当服务器成功连接，执行onMQTT_Connected槽函数
    }
    else {                                                              //当客户端已经连接上了服务器
        client->disconnectFromHost();                                   //客户端与服务器断开连接，当客户端与服务器断开连接，执行onMQTT_Disconnected槽函数
    }
}

//风扇的风力（占空比）被改变
void MainWindow::onFanDuty_changed(int duty){                           //根据68行的connect，当横向滑动栏或微调框的值发生变化，执行该槽函数
    QMQTT::Message message;                                             //定义一个message（就是上面说的看成结构体）
    pubtopic=Fan;
    message.setTopic(topicPub[pubtopic]);                                         //设置message中的topic（例如fan(控制风扇的速度)）
    message.setPayload(QString::number(duty).toUtf8());                 //设置message中的payload（也就是具体的消息内容（0~100控制速度））
    qDebug()<<"风扇占空比被改变为："<<duty;                                //控制台输出风扇占空比（就和用vs里的cout一样）
    client->publish(message);                                           //客户端发布message到服务器
}

void MainWindow::onColorDuty_changed(const QColor &color, double hue, double sat)
{
    QMQTT::Message message;                                                       //定义一个message（就是上面说的看成结构体）
    long color_value=0;
    pubtopic=Led;
    ui->lineEdit_R->setText(QString::number(color.red()));
    ui->lineEdit_G->setText(QString::number(color.green()));
    ui->lineEdit_B->setText(QString::number(color.blue()));
    color_value=(color.red()<<16)|(color.green()<<8)|(color.blue());
    message.setTopic(topicPub[pubtopic]);                                         //设置message中的topic（例如fan(控制风扇的速度)）
    message.setPayload(QString::number(color_value).toUtf8());                    //设置message中的payload（也就是具体的消息内容（0~100控制速度））
    client->publish(message);
}

// 成功连接到MQTT服务器
void MainWindow::onMQTT_Connected(){                                    //根据79行的connect，当客户端与MQTT服务器成功连接，执行该槽函数
    qDebug()<<"服务器连接成功！";                                          //控制台输出（就和用vs里的cout一样）
    ui->pushButton_connect->setText("断开连接");                          //将ui界面中的按钮的值改成“断开连接”
    quint8 qosSub = 0;
    for (int i=0;i<topicSub_max;i++) {                                   //topicSub_max与topicSub[]在MainWindow.h中被定义
        client->subscribe(topicSub[i], qosSub);                          //订阅三个主题：wd（温度）、sd（湿度）、gz（光照）
    }
}

// 成功和MQTT服务器断开连接
void MainWindow::onMQTT_Disconnected(){                                   //根据70行的connect，当客户端与MQTT服务器成功断开连接，执行该槽函数
    qDebug()<<"服务器断开成功！";                                           //控制台输出（就和用vs里的cout一样）
    ui->pushButton_connect->setText("连接");                              //将ui界面中的按钮的值改成“连接”
}

// 成功订阅主题
void MainWindow::onMQTT_Subscribed(const QString &topic)                  //根据72行的connect，当客户端成功订阅了某个主题，执行该槽函数
{                                                                         //topic（主题）的名字以参数的方式传递给该槽函数
    qDebug()<<topic<<"主题订阅成功！";                                      //控制台输出（就和用vs里的cout一样）
}

// 接收到消息
void MainWindow::onMQTT_Received(const QMQTT::Message &message)           //根据75行的connect，当客户端成功接收了某个主题的message，执行该槽函数
{
    qDebug()<<"接收到来自 "<<message.topic()<<" 的消息："<<message.payload();      //通过qDebug输出接收到的message包的topic和payload
    if (!message.topic().compare("wd")) {                                       //如果收到的主题名为wd（温度）
        ui->label_wd->setText(message.payload());                               //设置温度的label文本控件
        gaugeweather->setInnerValue(message.payload().toDouble());              //设置温湿度显示控件的内环数值
    }
    else if (!message.topic().compare("sd")) {                                  //如果收到的主题名为sd（湿度）
        ui->label_sd->setText(message.payload());                               //设置湿度的label文本控件
        gaugeweather->setOuterValue(message.payload().toDouble());              //设置温湿度显示控件的外环数值
    }
    else if (!message.topic().compare("gz")) {                                  //如果收到的主题名为gz（光照）
        ui->label_gz->setText(message.payload());                               //设置光照的label文本控件
        if(message.payload().toInt()>50)                                        //如果光照的值大于50
        {
            gaugeweather->setCenterPixMapNegativeColor(QColor(252,81,90));      //设置温湿度控件中间太阳的颜色
            gaugeweather->setCenterPixMapPositiveColor(QColor(252,81,90));      //如果光照值大于50，设置为红色
        }
        else {
            gaugeweather->setCenterPixMapNegativeColor(QColor(34,163,169));     //如果光照值小于等于50，设置为青绿色
            gaugeweather->setCenterPixMapPositiveColor(QColor(34,163,169));     //QT中鼠标长时间放置在QColor(XX,XX,XX)上，可以预览颜色
        }
    }
}
