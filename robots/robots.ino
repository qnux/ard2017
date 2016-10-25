/**
 * This file is the main file of the operationnal embedded robot code.
 */
#include "RSP.h"
#include "Robot2017.h"

using namespace ard;

Robot2017 robot;


//------------------------------------------------------------------------------
void
setup ()
{
  robot.boot();
}

//------------------------------------------------------------------------------
void
loop ()
{
  //as looop is called in loop, protect the "first run section" to be executed once
  static bool once = false;
  if( !once )
    {
      robot.log.log(INFO, "Robot boot completed sucessfully.");
      once = true;
    }

  // idle loop has a very small, configMINIMAL_STACK_SIZE, stack
  // loop must never block
  // the heartbeat movement is a proof that the CPU is not overused as the minimal priority task is executed
  g_ArdOs.kickHeartbeat();
}
