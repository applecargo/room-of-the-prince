#pragma once

// stepper
#define RPM_MAX               (18.0)
#define STEPS_PER_REV         (2048.0)

//
// speed (rpm) * steps-per-revolution == speed (steps per minute)
// speed (steps per minute) / 60 == speed (steps per second)
// speed (steps per second) / 1000 == speed (steps per millisec)
//
// speed (steps per millisec) * 60 * 1000 / steps-per-revolution == speed (rpm)
#define STEPS_PER_MILLISEC_TO_RPM (60.0 * 1000.0 / STEPS_PER_REV)
//
// speed (rpm) * steps-per-revolution / 60 / 1000 == speed (steps per millisec)
#define RPM_TO_STEPS_PER_MILLISEC (STEPS_PER_REV / 60.0 / 1000.0)

//
#define STEPS_PER_MILLISEC_MAX (RPM_MAX * RPM_TO_STEPS_PER_MILLISEC)
