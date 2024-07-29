/* Test interrupts to see if timers work
 *  properly with multiple timer interrupts.
 */

void setup() 
{
  initNode("TestInterrupts");
  
  initSensorsTimer();

  initOdomTimer();
}

void initNode(String id)
{
  Serial.begin(9600);

  while(!Serial);
  
  Serial.print("Starting ");
  Serial.print(id);
  Serial.println(".ino\n");
}

void initSensorsTimer()
{
  noInterrupts();           // disable all interrupts
  
  TCCR1A = 0;
  TCCR1B = 0;
  sensorsTmrCtr = 59286;   // preload timer 65536-16MHz/256/2Hz (34286 for 0.5sec) (59286 for 0.1sec)
  
  TCNT1 = sensorsTmrCtr;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  
  interrupts();             // enable all interrupts
}

void initOdomTimer()
{
  noInterrupts();           // disable all interrupts
  
  TCCR2A = 0;
  TCCR2B = 0;
  odomTmrCtr = 59286;   // preload timer 65536-16MHz/256/2Hz (34286 for 0.5sec) (59286 for 0.1sec)
  
  TCNT2 = odomTmrCtr;   // preload timer
  TCCR2B |= (1 << CS12);    // 256 prescaler 
  TIMSK2 |= (1 << TOIE1);   // enable timer overflow interrupt
  
  interrupts();             // enable all interrupts
}

void loop() {}

/* Run Sensor Loop at x (maybe 10-20) Hz, 
 * interrupt service routine - tick every 0.1 s (10 Hz)
 */
ISR(TIMER1_OVF_vect) // sensors loop!
{
  TCNT1 = sensorsTmrCtr; // set timer

  userTask(); // accelerate forward and maintain given speed

  arbitrator(); // send highest priority to motors
}

/* Run odom Loop at x (maybe 10-20) Hz, 
 * interrupt service routine - tick every 0.1 s (10 Hz)
 */
ISR(TIMER2_OVF_vect) // odom loop
{
  TCNT2 = odomTmrCtr; // set timer

  odomTask(); // accelerate forward and maintain given speed

  arbitrator(); // send highest priority to motors
}
