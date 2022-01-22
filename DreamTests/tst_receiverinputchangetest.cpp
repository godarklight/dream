#include <QtTest>
#include <QCoreApplication>

// add necessary includes here
#include "../src/GlobalDefinitions.h"
#include "../src/DrmReceiver.h"
#include "../src/util/Settings.h"

class ReceiverInputChangeTest : public QObject
{
    Q_OBJECT

public:
    ReceiverInputChangeTest();
    ~ReceiverInputChangeTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
private:
    CSettings Settings;
};

ReceiverInputChangeTest::ReceiverInputChangeTest()
{
    int argc=1;
    char* argv[1];
    Settings.Load(argc, argv);
}

ReceiverInputChangeTest::~ReceiverInputChangeTest()
{

}

void ReceiverInputChangeTest::initTestCase()
{
}

void ReceiverInputChangeTest::cleanupTestCase()
{

}

void ReceiverInputChangeTest::test_case1()
{
    CDRMReceiver rx(&Settings);
    rx.LoadSettings();
    ERunState eRunState = RESTART;
    int n=50;
    do
    {
        rx.InitReceiverMode();
        rx.SetInStartMode();
        eRunState = RUNNING;
        do
        {
            qDebug("process");
            if(--n <= 0) eRunState = STOP_REQUESTED;
            rx.process();
        }
        while (eRunState == RUNNING);
    }
    while (eRunState == RESTART);
    rx.CloseSoundInterfaces();
}

QTEST_MAIN(ReceiverInputChangeTest)

#include "tst_receiverinputchangetest.moc"
