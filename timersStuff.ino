

void startTimer(timerNames timer){

	timers[timer].mark=millis();
	timers[timer].enabled=true;

}
//----------
void enableTimer(timerNames timer,boolean enabling){
	timers[timer].enabled=enabling;
}
//----------
boolean timePassed(timerNames timer){
	return timers[timer].enabled && ( millis()- timers[timer].mark >=  timers[timer].interval);

}
//------------------------------------------
boolean isEnabled(timerNames timer){
	return timers[timer].enabled; 
}
