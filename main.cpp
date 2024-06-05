#include <QCoreApplication>
#include <QCommandLineParser>
#include <QElapsedTimer>
#include <QTimer>
#include <QDebug>
#include <QSerialPortInfo>

#include "modbus/modbus.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("Neuron Extension Bus Test");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Neuron extension bus test");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption serialPortOption(QStringList() << "s" << "serial port", "System path to serial port", "serial port");
    parser.addOption(serialPortOption);

    QCommandLineOption parityOption(QStringList() << "p" << "parity", "Set parity even|none, 'none' is default", "parity");
    parser.addOption(parityOption);

    QCommandLineOption baudOption(QStringList() << "b" << "baudrates", "Set baudrate, 115200 default", "baudrate");
    parser.addOption(baudOption);

    QCommandLineOption addressOption(QStringList() << "a" << "address", "Set address, [1, 254] default 15", "address");
    parser.addOption(addressOption);

    QCommandLineOption requestCountOption(QStringList() << "c" << "count", "Set request count, default 1000", "count");
    parser.addOption(requestCountOption);


    // Process the actual command line arguments given by the user
    parser.process(app);
    QTextStream qtin(stdin);

    QString portName;
    if (parser.isSet(serialPortOption)) {
        portName = parser.value(serialPortOption);
    } else {
        while (1) {
            qDebug() << "Select the serial port [1, "+QString::number(QSerialPortInfo::availablePorts().count())+"]:";

            int i = 0;
            QList<QSerialPortInfo> infoList;
            foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
                i++;
                qDebug() << i << info.portName();
                infoList.append(info);
            }
            qDebug() << "Enter number:";
            int j; qtin >> j;
            if (j < 1 || j > i) {
                qWarning() << "Selected number not available." << QString::number(j);
                continue;
            }

            if (infoList.at(j-1).isBusy()) {
                qDebug() << "Selected serial port is busy, select another one.";
                continue;
            }
            qDebug() << "The selected port is" << infoList.at(j-1).systemLocation();
            portName = infoList.at(j-1).systemLocation();
            break;
        }
    }

    int baudrate = 19200;
    if (parser.isSet(baudOption)) {
        bool ok;
        baudrate = parser.value(baudOption).toInt(&ok);
        if (!ok) {
            qDebug() << "Baudrate is not valid" << parser.value(baudOption);
        }
    } else {
        qDebug() << "Default baudrate:" << baudrate;
    }

    char parity = 'N';
    if (parser.isSet(parityOption)) {
        QString parityValue = parser.value(parityOption);
        if (parityValue.startsWith("none")) {
            parity = 'N';
        } else if (parityValue.startsWith("even")) {
            parity = 'E';
        } else if (parityValue.startsWith("odd")) {
            parity = 'O';
        } else {
            qDebug() << "Parity" << parityValue << "is not supported, must be 'even','odd' or 'none'";
        }
    } else {
        qDebug() << "Default parity:" << parity;
    }

    int address = 1;
    if (parser.isSet(addressOption)) {
        bool ok;
        address = parser.value(addressOption).toInt(&ok);
        if (!ok) {
            qDebug() << "Address is not valid" << parser.value(addressOption);
            return -1;
        }
    }

    QElapsedTimer elapsedTimer;

    int requestCount = 1000;
    if (parser.isSet(requestCountOption)) {
        bool ok;
        requestCount = parser.value(requestCountOption).toInt(&ok);
        if (!ok) {
            qDebug() << "Request count is not valid" << parser.value(requestCountOption);
            return -1;
        }
    }

    modbus_t *mb;
    mb = modbus_new_rtu(portName.toUtf8(), baudrate, parity, 8, 1);
    if (mb == nullptr) {
        qDebug() << "Could not create modbus rtu";
        return -1;
    }
    modbus_connect(mb);
    if (modbus_set_slave(mb, address) == -1) {
        modbus_close(mb);
        modbus_free(mb);
        qDebug() << "Could not set slave";
        return -1;
    }

    int failureCount = 0;
    int registerCount = 5;
    QVector<uint16_t> data;
    data.resize(requestCount);

    elapsedTimer.start();
    for (int i=0; i < requestCount; i++) {
        if (modbus_read_registers(mb, 0, registerCount, data.data()) == -1) {
            failureCount++;
            qDebug() << "Error" << modbus_strerror(errno);
        }
    }

    qDebug() << "Elapsed time for" << requestCount << "is" << elapsedTimer.elapsed() << "ms. With" << failureCount << "errors.";
    modbus_close(mb);
    modbus_free(mb);

    return app.exec();
}
