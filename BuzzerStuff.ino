



void playTone(unsigned int halfPeriod,unsigned int repetitions){
	for(unsigned int i=0;i<repetitions;i++){
		digitalWrite(BUZZER_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
		delay(halfPeriod);               // wait for a second
		digitalWrite(BUZZER_PIN, LOW);    // turn the LED off by making the voltage LOW
		delay(halfPeriod);               // wait for a second

	} 
}
//---------------------------------------------------
