
#include <commoncpp/serial.h>

using namespace std;
using namespace ost;

static Serial *top;

string read_string()
{
 string r;
 while (1) {
   if (top->isPending(Serial::pendingInput, 10)) {
     char b = 0;
     int res = top->aRead(&b, 1);
     if (res == 1)
       r += b;
     else
       throw top;
     if (b == '\n')
       break;
   }
 }
 return r;
}

void app()
{
 if (top)
   delete top;
 top = 0;
 top = new Serial("/dev/ttyACM0");
 top->setSpeed(115200);
 top->setCharBits(8);
 top->setParity(Serial::parityNone);
 top->setStopBits(1);
 top->setFlowControl(Serial::flowNone);
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
      cout << "serial error: " << err << endl;
      if (err != 2)
        top->flushInput();
      else
        sleep(1);
   } 
 }
}
