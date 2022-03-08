#ifndef PID_H_
#define PID_H_

void pid_constant(double Kp_, double Ki_, double Kd_);
void pid_reference(float reference_);
double pid_control(double saida_medida);

#endif /* PID_H_ */

