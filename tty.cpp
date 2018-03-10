
#include <commoncpp/serial.h>

using namespace std;
using namespace ost;

static Serial *stmcool;

string read_string()
{
 string r;
 while (1) {
   if (stmcool->isPending(Serial::pendingInput, 10)) {
     char b = 0;
     int res = stmcool->aRead(&b, 1);
     if (res == 1)
       r += b;
     else
       throw stmcool;
     if (b == '\n')
       break;
   }
 }
 return r;
}

void app()
{
 if (stmcool)
   delete stmcool;
 stmcool = 0;
 stmcool = new Serial("/dev/ttyACM0");
 stmcool->setSpeed(115200);
 stmcool->setCharBits(8);
 stmcool->setParity(Serial::parityNone);
 stmcool->setStopBits(1);
 stmcool->setFlowControl(Serial::flowNone);
 while(1) {
   string r = read_string();
   if (r.size())
     cout << r;
 }
}

int main()
{
 while(1) {
   try {
     app();
   } catch (Serial *e) {
      uint err = e->getErrorNumber();
      cout << "stmcool serial port error: " << err << endl;
      if (err != 2)
        stmcool->flushInput();
      else
        sleep(1);
   } 
 }
}
