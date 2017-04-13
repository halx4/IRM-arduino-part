//---------------------------------------------------------

void toggleRelay(){
	setRelayState(!relayState);
}

//------------------------------------------------------------
void setRelayState(boolean val){
	//true ->relay on
	//false ->relay off
	relayState=val;
	digitalWrite(RELAY_PIN,!val);


}
//-----
