#include "myfork.h"


int
main()
{
  myfork fork_me;

  for (int i = 0; i < 5; i++) {
    cout << i << endl;
    sleep(1);
  }

  fork_me.rfork();
  

  for (int i = 5; i < 10; i++) {
    cout << i << endl;
    sleep(1);
  }

  return 0;
}