void buttonHold(){
    //Serial.println("button hold detected...");
    if(relayState){
      if(isEnabled(LIGHT_COUNTDOWN)){
          //disableCountdown();        
          enableTimer(LIGHT_COUNTDOWN,false);
          playTone(1,300); 
        
      }
      else{
          startTimer(LIGHT_COUNTDOWN);
          playTone(1,70); 
      }
      
    }

}
//---------------------------------------------------------
