#include "IRmultitool.h"
#include <IRremote.h>


//----   PIN DECLARATIONS   ----------
const short int BUZZER_PIN = 5;
const short int RELAY_PIN = 9;
const short int RECV_PIN = 11;
const short int BUTTON_PIN = 12;


//----   IR   ------------------------
IRrecv irrecv(RECV_PIN);
decode_results prototype;
decode_results results;

const unsigned long IRR_REPEAT_CODE = 0xFFFFFFFF;
#define IRR_BUTTON1 0xFFA25D
#define IRR_BUTTON2 0xFF629D
#define IRR_BUTTON3 0xFFE21D
#define IRR_BUTTON4 0xFF22DD
#define IRR_BUTTON5 0xFF02FD
#define IRR_BUTTON6 0xFFC23D
#define IRR_BUTTON7 0xFFE01F
#define IRR_BUTTON8 0xFFA857
#define IRR_BUTTON9 0xFF906F
#define IRR_BUTTON10 0xFF6897
#define IRR_BUTTON11 0xFF9867
#define IRR_BUTTON12 0xFFB04F
#define IRR_BUTTON13 0xFF30CF
#define IRR_BUTTON14 0xFF18EF
#define IRR_BUTTON15 0xFF7A85
#define IRR_BUTTON16 0xFF10EF
#define IRR_BUTTON17 0xFF38CF
#define IRR_BUTTON18 0xFF5AA5
#define IRR_BUTTON19 0xFF42BD
#define IRR_BUTTON20 0xFF4AB5
#define IRR_BUTTON21 0xFF52AD


#define LIGHT_BUTTON  IRR_BUTTON2
#define HBRN_BUTTON   IRR_BUTTON1
#define MONOFF_BUTTON  IRR_BUTTON3
#define MUTE_BUTTON   IRR_BUTTON9
#define VOLDEC_BUTTON  IRR_BUTTON7
#define VOLINC_BUTTON  IRR_BUTTON8



unsigned long lastCommand=0x0;
//----   TIME CONSTANTS  ---------------
const unsigned int DEBOUNCE_DELAY = 100;
const unsigned int BUTTON_HOLDInterval=1500;
const long unsigned int LIGHT_COUNTDOWNInterval=60000;  

const unsigned int BRTInterval=300;
const unsigned int HRTInterval=250;
const unsigned int RHETInterval=500;

//----  TIME MARKS -------------------
struct TimerStruct {
	unsigned int interval;
	unsigned long int mark;
	boolean enabled;
};
TimerStruct timers[5];


//----  FINITE STATE MACHINE  ----------
enum ButtonStateType {NOT_PRESSED,
	PRESSED,
	HELD
};

ButtonStateType buttonState=NOT_PRESSED;

//-----
enum RemoteStateType {IRR_IDLE,
	IRR_PRESSED,
	IRR_HELD };

RemoteStateType IRRState=IRR_IDLE;


boolean relayState=false;


//----------------------------------------------------------------------------------


void NECcommandReceived(unsigned long value){
	//Serial.println(value,HEX);
	switch(IRRState){
	case IRR_IDLE:
		//Serial.println("");
		if(!isRepeatCode(&value)){
			//Serial.println("new command detected!");
			startTimer(RHET);
			startTimer(BRT);
			lastCommand=value;
			irrButtonPush(value); //<<initial irr button push detected
			IRRState=IRR_PRESSED;
		}
		else{
			playTone(1,5);       
		}
		
		break;
	case IRR_PRESSED:
		//Serial.println("");
		if(isRepeatCode(&value)){ 
			startTimer(BRT);
		}
		
		break;
	case IRR_HELD:
		//Serial.println("");
		if(isRepeatCode(&value)){ 
			startTimer(BRT);
		}
		break;

	}
}
//---------------------------------------------------------
void irrUpdate(){
	//Serial.println(value,HEX);
	switch(IRRState){
	case IRR_IDLE:
		//Serial.println("");
		break;
	case IRR_PRESSED:
		//Serial.println("");
		if(timePassed(BRT)){   
			IRRState=IRR_IDLE;
		}
		else if(timePassed(RHET)){
			//hold detected     //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  TO DO
			irrButtonHold(lastCommand);
			startTimer(HRT);
			//Serial.println("IR-hold detected");
			IRRState=IRR_HELD;
			
		}
		break;
	case IRR_HELD:
		//Serial.println("");
		if(timePassed(BRT)){ 
			IRRState=IRR_IDLE;
		}
		else if(timePassed(HRT)){
			//hold persist detected   //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  TO DO
			irrButtonHold(lastCommand);
			//Serial.println("hold persist detected");
			startTimer(HRT);
		}
		break;

	}
}
//---------------------------------------------------------
void irrButtonPush(unsigned long irCode){
	switch(irCode){
	case HBRN_BUTTON:
		sendPrefix();
		Serial.print("HBRN\n");
		break;
	case MONOFF_BUTTON:
		sendPrefix();
		Serial.print("MONOFF\n");
		break;
	case MUTE_BUTTON:
		sendPrefix();
		Serial.print("MUTE\n");
		break;
	case VOLDEC_BUTTON:
		sendPrefix();
		Serial.print("VOLDEC\n");
		break;
	case LIGHT_BUTTON://LIGHT... different scenario
		toggleRelay();
		break;
	case VOLINC_BUTTON:
		sendPrefix();
		Serial.print("VOLINC\n");
		break;
	default:
		
		break;
	}

}
//---------------------------------------------------------
void irrButtonHold(unsigned long irCode){
	switch(irCode){
	case VOLDEC_BUTTON:
		sendPrefix();
		Serial.print("VOLDEC\n");
		break;
	case VOLINC_BUTTON:
		sendPrefix();
		Serial.print("VOLINC\n");
		break;
	default:
		
		break;
	}  
}
//---------------------------------------------------------

void sendPrefix(){
	Serial.print("__CMD_"); 
}

//---------------------------------------------------------
boolean isRepeatCode(unsigned long *value){
	//Serial.println(*value == IRR_REPEAT_COMMAND);
	return (*value == IRR_REPEAT_CODE);
}
//------------------------------------


void setup()
{
	Serial.begin(9600);
	irrecv.enableIRIn(); // Start the receiver

	pinMode(BUZZER_PIN,OUTPUT);
	pinMode(RELAY_PIN,OUTPUT);
	pinMode(BUTTON_PIN,INPUT);

	timers[RHET].mark=0;
	timers[RHET].enabled=true;
	timers[RHET].interval=RHETInterval;
	
	timers[BRT].mark=0;
	timers[BRT].enabled=true;
	timers[BRT].interval=BRTInterval;
	
	timers[HRT].mark=0;
	timers[HRT].enabled=true;
	timers[HRT].interval=HRTInterval;

	timers[LIGHT_COUNTDOWN].mark=0;
	timers[LIGHT_COUNTDOWN].enabled=false;
	timers[LIGHT_COUNTDOWN].interval=LIGHT_COUNTDOWNInterval;

	timers[BUTTON_HOLD].mark=0;
	timers[BUTTON_HOLD].enabled=false;
	timers[BUTTON_HOLD].interval=BUTTON_HOLDInterval;

	setRelayState(false);


}
//-------------------------------------------------------
void loop() {


	if (irrecv.decode(&results)) {
		//Serial.println("loop: got IR code -> ");
		
		if(results.decode_type==NEC){
			//Serial.println("nec received!");
			NECcommandReceived(results.value);
		}
		irrecv.resume(); // Receive the next value
	}

	irrUpdate();

	boolean buttonRead=digitalRead(BUTTON_PIN); 
	boolean buttonTimePassed;
	switch (buttonState){
		
		
	case NOT_PRESSED:
		//Serial.println("NOT PRESSED CASE");
		if(buttonRead){
			//  Serial.println("NOT PRESSED -> PRESSED");
			//setButtonTimer();  
			startTimer(BUTTON_HOLD);
			delay(DEBOUNCE_DELAY);
			buttonState=PRESSED;  
		}
		break;
		
		
	case PRESSED:
		// Serial.println("KEPT PRESSED CASE");
		buttonTimePassed=timePassed(BUTTON_HOLD);
		if(buttonRead && buttonTimePassed){
			//   Serial.println("PRESSED -> HELD");
			buttonHold();
			buttonState=HELD;
		}
		else if(buttonRead && !buttonTimePassed){
			delay(DEBOUNCE_DELAY);
		}
		else{
			//   Serial.println("momentary press detected.. switching relay state");
			//  Serial.println("PRESSED -> NOT_PRESSED");
			enableTimer(LIGHT_COUNTDOWN,false);
			//disableCountdown();
			toggleRelay();
			delay(DEBOUNCE_DELAY);
			buttonState=NOT_PRESSED;
		}
		break;
		
	case HELD:
		if(!buttonRead){
			//  Serial.println("HELD -> NOT_PRESSED");
			delay(DEBOUNCE_DELAY);
			buttonState=NOT_PRESSED;
		}
		break;         

	}//end switch

	if(isEnabled(LIGHT_COUNTDOWN) && timePassed(LIGHT_COUNTDOWN)){
		setRelayState(false);
		enableTimer(LIGHT_COUNTDOWN,false);
	}

}
//---------------------------------------------------------
