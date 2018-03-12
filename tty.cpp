
#include <commoncpp/serial.h>
#include <termios.h>

using namespace std;
using namespace ost;

void set_echo(bool on)
{
 struct termios t;
 tcgetattr(STDIN_FILENO, &t);
 if (on)
   t.c_lflag |= (ECHO | ICANON);
 else
   t.c_lflag &= ~(ECHO | ICANON);
 tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void sigquit_handler(int sig)
{
 if ((sig != SIGINT) && (sig != SIGTERM) && (sig != SIGQUIT))
   return;
 set_echo(1);
 cout << "\rbye..\n";
 ::exit(0);
}

static Serial *stmcool;

string read_stdin()
{
 fd_set read_fds;
 FD_ZERO(&read_fds);
 FD_SET(STDIN_FILENO, &read_fds);
 struct timeval tv;
 tv.tv_sec = 0;
 tv.tv_usec = 10000; // 10ms
 int result = select(1, &read_fds, NULL, NULL, &tv);
 if ((result == -1) && (errno != EINTR))
   cerr << "read_stdin: select: " << strerror(errno) << endl;
 else if ((result == -1) && (errno == EINTR))
   // we've received and interrupt - handle this
   cerr << "read_stdin: got interrupt\n";
 else {
   if (FD_ISSET(STDIN_FILENO, &read_fds)) {
     char b[3] = { 0 };
     int res = read(0, &b, 1);
     if (!b[0]) // ctrl-d
       sigquit_handler(SIGQUIT);
     if (b[0] == 0xa)
       b[0] = 0xd;
     if (b[0] == 0x7f)
       b[0] = 0x8;
     if (b[0] == 0x5b) // cursor esc seq
       res = read(0, &(b[1]), 1);
     if (res >= 1)
       return string(b);
   } // timeout
 }
 return string();
}

string process_input(string &o)
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
     if (r.size())
       break;
   } else {
       o = read_stdin();
       if (o.size())
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
 stmcool = new Serial("/dev/stmcool");
 stmcool->setSpeed(115200);
 stmcool->setCharBits(8);
 stmcool->setParity(Serial::parityNone);
 stmcool->setStopBits(1);
 stmcool->setFlowControl(Serial::flowNone);
 while(1) {
   string i, o = process_input(i);
   if (o.size()) {
     if (o.c_str()[0] != 0xc)
       cerr << o;
     else {
       // TODO: add clear screen [0xc] support
     }
   }
   if (i.size())
     stmcool->aWrite((char *)i.c_str(), i.size());
 }
}

void set_signals()
{
 ::signal(SIGPIPE, SIG_IGN);
 ::signal(SIGINT, &sigquit_handler);
 ::signal(SIGTERM, &sigquit_handler);
 ::signal(SIGQUIT, &sigquit_handler);
}

int main()
{
 set_signals();
 set_echo(0);
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
