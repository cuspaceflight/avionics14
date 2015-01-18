#ifndef QUEST_ESTIMATOR_H
#define QUEST_ESTIMATOR_H

void quest_estimator_new_accel(float accel[3]);

void quest_estimator_new_mag(float mag[3]);

void quest_estimator_update(float q[4]);

#endif /* QUEST_ESTIMATOR_H */