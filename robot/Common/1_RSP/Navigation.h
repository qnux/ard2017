// Classe de gestion de la navigation (déplacements du robot sur la table)
#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "BSP.h"
#include "ArdOs.h"
#include "core/ArdMaths.h"
#include "GpioTools.h"
#include "CommonMsg.pb.h"

namespace ard
{
  /**
   * This class manage the robot position movements, and avoidance
   * It is isolated from the rest of the code to prevent something
   * to block the avoidance/localisation part.
   */
  class Navigation : public Thread
  {
  public:
    Navigation ();

    /**---------------------------------
     * Thread interface
     ---------------------------------*/

    //Implements Thread
    void run() override;

    //As the motor lib needs to be called
    void updateFromInterrupt();

    /**---------------------------------
     * User (= strategy) interface
     ---------------------------------*/

    /**
     * Force a new robot position
     * the position is automatically symetrized depending
     * on the value configured with setColor()
     */
    void
    setPosition (PointCap newPose);
    void
    setPosition(float x/*mm*/, float y/*mm*/, float h/*rad*/){setPosition(PointCap(x,y,h));};

    /**
     * Get the current robot position
     */
    PointCap
    getPosition ()
    {
      return m_pose;
    }

    /**
     Send a Goto (x,y) request, the robot will turn to face the target
     and use a straight line to go to it.
     If an order is already present the call is blocking as if a wait() where done.

     Note that the target is automatically symetrized depending
     on the color configured with setColor()

     In order to wait until the order is complete, use the wait()/targetReached() functions

     x,y in mm

     */
    void
    goTo (Point target, eDir sens = eDir_FORWARD);
    void
    goTo (float x /*mm*/, float y/*mm*/, eDir sens = eDir_FORWARD){goTo(Point(x,y), sens);};

    /**
     Send a Goto (x,y,heading) request, the robot will turn to face the target
     and use a straight line to go to it. Then it will turn to reach the
     target heading/
     If an order is already present the call is blocking as if a wait() where done.

     Note that the target is automatically symetrized depending
     on the color configured with setColor()

     In order to wait until the order is complete, use the wait()/targetReached() functions

     x,y in mm
     heading in degrees

     */
    void
    goToCap (PointCap target, eDir sens = eDir_FORWARD);
    void
    goToCap(float x/*mm*/, float y/*mm*/, float h/*°*/, eDir sens = eDir_FORWARD){goToCap(PointCap(x,y,h), sens);};

    /**
     * The robot will go ahead of the distance in parameter
     * note : the distance may be negative to go rear-way
     *
     * If an order is already present the call is blocking as if a wait() where done.
     */
    void
    goForward (float distanceMm);

    /**
     * The robot will turn of the angle in parameter (absolute or relative)
     * note : the angle may be negative to go clockwise
     *
     * If an order is already present the call is blocking as if a wait() where done.
     */
    void
    turnDelta(float angle);
    void
    turnTo (float angle);

    /**
     * The robot will turn to face the point in parameter
     * Note that the target is automatically symetrized depending
     * on the color configured with setColor()
     *
     * If an order is already present the call is blocking as if a wait() where done.
     */
    void
    faceTo (Point p);

    /**
     * Stops the robot. It interrupts current order.
     */
    void 
    stopMoving();

    /**
     * This is a blocking call until the current order is finished
     */
    void
    wait ();

    /**
     * This is the non blocking version of wait()
     */
    bool
    targetReached ();

    /**---------------------------------
     * Nav configuration
     ---------------------------------*/

    void
    setColor (eColor c);
    void
    setSpeed (float speed);
    void
    setSpeedVir (float s);

    /**---------------------------------
     * Publish state
     ---------------------------------*/

    apb_NavState getState() const;

  private:
    //Integrates the new displacement mesures with current position
    //This function modifies critical section variables with no protection
    //It's the caller responsibility to protect the call with portENTER_CRITICAL/portEXIT_CRITICAL from a thread, or call it in an interrupt context
    void
    compute_odom ();

    //used to send a straight line trajectory to the motors
    void
    straight (float distInMm);

    //used to send an on place rotation trajectory to the motors
    void
    turn (float angle);

    //interrupt the current movement
    void interruptCurrentMove();

    //used internally after a straight/turn/face order to check completeness
    bool
    subOrderFinished();

    String
    sensToString (eDir sens);
    String
    orderToString (eNavOrder order);
    String
    stateToString (eNavState state);

    //status
    PointCap m_pose;
    eNavState m_state;

    //target definition
    PointCap m_target;
    eDir m_sensTarget;
    eNavOrder m_order;
    float m_angleToTarget;
    float m_distanceToTarget;

    //HW interface
    AccelStepper stepperG;
    AccelStepper stepperD;
    FilteredInput omronFrontLeft;
    FilteredInput omronFrontRight;
    FilteredInput omronRearLeft;
    FilteredInput omronRearRight;

    //match color
    eColor m_color;

    //speed is reduced in turns to prevent drifting
    //hence we need 2 vars to switch from one to the other
    float m_speed;
    float m_speed_virage;

    Mutex m_mutex;
    Signal m_targetReached;
    
    long oldStepG;
    long oldStepD;
  };
}    //end namespace

#endif
