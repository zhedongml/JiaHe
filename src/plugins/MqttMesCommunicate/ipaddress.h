#ifndef IPADDRESS_H
#define IPADDRESS_H

#include <QWidget>

class QLabel;
class QLineEdit;

#ifdef quc
class Q_DECL_EXPORT IPAddress : public QWidget
#else
class IPAddress : public QWidget
#endif

{
    Q_OBJECT

    Q_PROPERTY(QString ip READ getIP WRITE setIP)

public:
    explicit IPAddress(QWidget *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QLabel *labDot1;    //第一个小圆点
    QLabel *labDot2;    //第二个小圆点
    QLabel *labDot3;    //第三个小圆点

    QLineEdit *txtIP1;  //IP地址网段输入框1
    QLineEdit *txtIP2;  //IP地址网段输入框2
    QLineEdit *txtIP3;  //IP地址网段输入框3
    QLineEdit *txtIP4;  //IP地址网段输入框4

    QString ip;         //IP地址
    QString bgColor;    //背景颜色
    QString borderColor;//边框颜色
    int borderRadius;   //边框圆角角度

private slots:
    void textChanged(const QString &text);

public:
    //默认尺寸和最小尺寸
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    //获取和设置IP地址
    QString getIP() const;
    void setIP(const QString &ip);

    //清空
    void clear();

    //设置背景颜色
    void setBgColor(const QString &bgColor);
    //设置边框颜色
    void setBorderColor(const QString &borderColor);
    //设置边框圆角角度
    void setBorderRadius(int borderRadius);
};

#endif // IPADDRESS_H
