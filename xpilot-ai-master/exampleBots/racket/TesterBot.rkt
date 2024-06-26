;Justin Anderson - May 2012
;Run: racket TesterBot.rkt
#lang racket
(require "rktAI.rkt")
(define PI 3.14159)
(define (loop) 
  ;(turnLeft 1)
  ;(turnRight 1)
  ;(turn 20)
  ;(turnToDeg 20)
  ;(thrust 1)
  ;(setTurnSpeed 1.0)
  ;(setPower 1.0)
  ;(fasterTurnrate)
  ;(slowerTurnrate)
  ;(morePower)
  ;(lessPower)
  ;(fireShot)
  ;(fireMissile)
  ;(fireTorpedo)
  ;(fireHeat)
  ;(dropMine)
  ;(detachMine)
  ;(detonateMines)
  ;(fireLaser)
  ;(tankDetach)
  ;(cloak)
  ;(ecm)
  ;(transporter)
  ;(tractorBeam 1)
  ;(pressorBeam 1)
  ;(phasing)
  ;(shield)
  ;(emergencyShield)
  ;(hyperjump)
  ;(nextTank)
  ;(prevTank)
  ;(toggleAutopilot)
  ;(emergencyThrust)
  ;(deflector)
  ;(selectItem)
  ;(loseItem)
  ;(lockNext)
  ;(lockPrev)
  ;(lockClose)
  ;(lockNextClose)
  ;(loadLock1)
  ;(loadLock2)
  ;(loadLock3)
  ;(loadLock4)
  ;(toggleNuclear)
  ;(togglePower)
  ;(toggleVelocity)
  ;(toggleCluster)
  ;(toggleMini)
  ;(toggleSpread)
  ;(toggleLaser)
  ;(toggleImplosion)
  ;(toggleUserName)
  ;(loadModifiers1)
  ;(loadModifiers2)
  ;(loadModifiers3)
  ;(loadModifiers4)
  ;(clearModifiers)
  ;(connector 1)
  ;(dropBall)
  ;(refuel 1)
  ;(keyHome)
  ;(selfDestruct)
  ;(pauseAI)
  ;(swapSettings)
  ;(quitAI)
  ;(talkKey)
  ;(toggleCompass)
  ;(toggleShowMessage)
  ;(toggleShowItems)
  ;(repair)
  ;(reprogram)
  ;(talk "Hello:")
  ;(display "scanMsg : ")
  ;(displayln (scanMsg 0))
  ;(display "selfX : ")
  ;(displayln (selfX))
  ;(display "selfY : ")
  ;(displayln (selfY))
  ;(display "selfRadarX : ")
  ;(displayln (selfRadarX))
  ;(display "selfRadarY : ")
  ;(displayln (selfRadarY))
  ;(display "selfVelX : ")
  ;(displayln (selfVelX))
  ;(display "selfVelY : ")
  ;(displayln (selfVelY))
  ;(display "selfSpeed : ")
  ;(displayln (selfSpeed))
  ;(display "lockHeadingDeg : ")
  ;(displayln (lockHeadingDeg))
  ;(display "lockHeadingRad : ")
  ;(displayln (lockHeadingRad))
  ;(display "selfLockDist : ")
  ;(displayln (selfLockDist))
  ;(display "selfReload : ")
  ;(displayln (selfReload))
  ;(display "selfID : ")
  ;(displayln (selfID))
  ;(display "selfAlive : ")
  ;(displayln (selfAlive))
  ;(display "selfTeam : ")
  ;(displayln (selfTeam))
  ;(display "selfLives : ")
  ;(displayln (selfLives)) 
  ;(display "selfTrackingRad : ")
  ;(displayln (selfTrackingRad))
  ;(display "selfTrackingDeg : ")
  ;(displayln (selfTrackingDeg))
  ;(display "selfHeadingDeg : ")
  ;(displayln (selfHeadingDeg))
  ;(display "selfHeadingRad : ")
  ;(displayln (selfHeadingRad))
  ;(display "hudName : ")
  ;(displayln (hudName))
  ;(display "hudScore : ")
  ;(displayln (hudScore))
  ;(display "hudTimeLeft : ")
  ;(displayln (hudTimeLeft))
  ;(display "getTurnSpeed : ")
  ;(displayln (getTurnSpeed))
  ;(display "getPower : ")
  ;(displayln (getPower))
  ;(display "selfShield : ")
  ;(displayln (selfShield))
  ;(display "selfName : ")
  ;(displayln (selfName))
  ;(display "selfScore : ")
  ;(displayln (selfScore))
  ;(display "closestRadarX : ")
  ;(displayln (closestRadarX))
  ;(display "closestRadarY : ")
  ;(displayln (closestRadarY))
  ;(display "closestItemX : ")
  ;(displayln (closestItemX))
  ;(display "closestItemY : ")
  ;(displayln (closestItemY))
  ;(display "closestShipId : ")
  ;(displayln (closestShipId))
  ;(display "enemySpeedId : ")
  ;(displayln (enemySpeedId (closestShipId)))
  ;(display "enemyTrackingRadId : ")
  ;(displayln (enemyTrackingRadId (closestShipId)))
  ;(display "enemyTrackingDegId : ")
  ;(displayln (enemyTrackingDegId (closestShipId)))
  ;(display "enemyReloadId : ")
  ;(displayln (enemyReloadId (closestShipId)))
  ;(display "screenEnemyXId : ")
  ;(displayln (screenEnemyXId (closestShipId)))
  ;(display "screenEnemyYId : ")
  ;(displayln (screenEnemyYId (closestShipId)))
  ;(display "enemyHeadingDegId : ")
  ;(displayln (enemyHeadingDegId (closestShipId)))
  ;(display "enemyHeadingRadId : ")
  ;(displayln (enemyHeadingRadId (closestShipId)))
  ;(display "enemyShieldId : ")
  ;(displayln (enemyShieldId (closestShipId)))
  ;(display "enemyLivesId : ")
  ;(displayln (enemyLivesId (closestShipId)))
  ;(display "enemyNameId : ")
  ;(displayln (enemyNameId (closestShipId)))
  ;(display "enemyScoreId : ")
  ;(displayln (enemyScoreId (closestShipId)))
  ;(display "enemyTeamId : ")
  ;(displayln (enemyTeamId (closestShipId)))
  ;(display "enemyDistanceId : ")
  ;(displayln (enemyDistanceId (closestShipId)))
  ;(display "enemyDistance : ")
  ;(displayln (enemyDistance 0))
  ;(display "enemySpeed : ")
  ;(displayln (enemySpeed 0))
  ;(display "enemyReload : ")
  ;(displayln (enemyReload 0))
  ;(display "enemyTrackingRad : ")
  ;(displayln (enemyTrackingRad 0))
  ;(display "enemyTrackingDeg : ")
  ;(displayln (enemyTrackingDeg 0))
  ;(display "screenEnemyX : ")
  ;(displayln (screenEnemyX 0))
  ;(display "screenEnemyY : ")
  ;(displayln (screenEnemyY 0))
  ;(display "enemyHeadingDeg : ")
  ;(displayln (enemyHeadingDeg 0)) 
  ;(display "enemyHeadingRad : ")
  ;(displayln (enemyHeadingRad 0))
  ;(display "enemyShield : ")
  ;(displayln (enemyShield 0))
  ;(display "enemyLives : ")
  ;(displayln (enemyLives 0))
  ;(display "enemyTeam : ")
  ;(displayln (enemyTeam 0))
  ;(display "enemyName : ")
  ;(displayln (enemyName 0))
  ;(display "enemyScore : ")
  ;(displayln (enemyScore 0))
  ;(display "degToRad : ")
  ;(displayln (degToRad 180))
  ;(display "radToDeg : ")
  ;(displayln (radToDeg (/ PI 2)))
  ;(display "angleDiff : ")
  ;(displayln (angleDiff 1 3))
  ;(display "angleAdd : ")
  ;(displayln (angleAdd 1 3))
  ;(display "wallFeeler : ")
  ;(displayln (wallFeeler 100 0 1 1))
  ;(display "wallFeelerRad : ")
  ;(displayln (wallFeelerRad 100 0.0 1 1))
  ;(display "wallBetween : ")
  ;(displayln (wallBetween 87 600 87 1000 1 1))
  ;(display "shotAlert : ")
  ;(displayln (shotAlert 0))
  ;(display "shotX : ")
  ;(displayln (shotX 0))
  ;(display "shotY : ")
  ;(displayln (shotY 0))
  ;(display "shotDist : ")
  ;(displayln (shotDist 0))
  ;(display "shotVel : ")
  ;(displayln (shotVel 0))
  ;(display "shotVelDir : ")
  ;(displayln (shotVelDir 0))
  ;(display "aimdir : ")
  ;(displayln (aimdir 0))
  ;(display "ballX : ")
  ;(displayln (ballX))
  ;(display "ballY : ")
  ;(displayln (ballY))
  ;(display "connectorX0 : ")
  ;(displayln (connectorX0))
  ;(display "connectorX1 : ")
  ;(displayln (connectorX1))
  ;(display "connectorY0 : ")
  ;(displayln (connectorY0))
  ;(display "connectorY1 : ")
  ;(displayln (connectorY1))
  (displayln "Test Complete...yeah!"))
(start loop '("-name" "Tester Bot"))
