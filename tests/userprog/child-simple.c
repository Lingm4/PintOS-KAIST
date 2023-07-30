/* Child process run by exec-multiple, exec-one, wait-simple, and
   wait-twice tests.
   Just prints a single message and terminates. */

#include <stdio.h>
#include "tests/lib.h"

int
main (void) 
{
  char tn[] = "child-simple"; 
  test_name = tn;
  msg ("run");
  return 81;
}
