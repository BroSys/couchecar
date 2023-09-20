// Pins die den Startup verhindern: 12 (low on reset?), 2 (LED)

int links_speed = 25;         // Geschwindigkeit linker motor
int links_dir = 21;           // drehrichtung linker motor
// int links_enable = ;       // noch ohne Fkt. - stop linker motor
int links_break = 27;         // bremse linker motor
int links_sp = 18;            // Speed Pulse Tachosignal

int rechts_speed = 26;        // Geschwindigkeit linker motor
int rechts_dir = 22;          // drehrichtung linker motor
// int rechts_enable = ;      // noch ohne Fkt. - stop linker motor
int rechts_break = 14;        // bremse linker motor
int rechts_sp = 19;           // Speed Pulse Tachosignal

// Joystick - (u)p, (d)own, (l)eft, (r)ight
int joystick_u = 35;
int joystick_d = 34;
int joystick_l = 36;
int joystick_r = 39;

// onboard Status LED
int led = 2;
bool led_status = 0;

const bool vorw = 1;
const bool zurr = 0;

// fuer stop - ist letztlich nur ein "enable" 
// TODO:
// for enable, set GPIO to INPUT
// for disable, set GPIO to OUTPUT and LOW
// putting it to OUTPUT and HIGH causes a malfunction resulting in fullspeed
const bool ausschalten = 1;
const bool einschalten = 0;

// fuer break
const bool bremsen = 1;
const bool loesen  = 0;

// Geschwindigkeit 8bit: 0-255
int geschwindigkeit = 0;
int geschwindigkeit_l = 0;
int geschwindigkeit_r = 0;

// Pulse counter
int pulse_li = 0;
int pulse_re = 0;

void IRAM_ATTR isr_li() {
	pulse_li++;
}

void IRAM_ATTR isr_re() {
	pulse_re++;
}

void setup() {
  Serial.begin(115200);

  // LED
  pinMode(led, OUTPUT);

  pinMode(links_speed, OUTPUT);
  pinMode(links_dir, OUTPUT);
//  pinMode(links_enable, INPUT); //todo
  pinMode(links_break, OUTPUT);

  pinMode(rechts_speed, OUTPUT);
  pinMode(rechts_dir, OUTPUT);
 // pinMode(rechts_enable, INPUT); //todo
  pinMode(rechts_break, OUTPUT);

  // Joystick
  pinMode(joystick_u, INPUT_PULLDOWN);
  pinMode(joystick_d, INPUT_PULLDOWN);
  pinMode(joystick_l, INPUT_PULLDOWN);
  pinMode(joystick_r, INPUT_PULLDOWN);

  // Controller Ausgang Speed Pulse Out - Tachosignal
  pinMode(links_sp, INPUT);
  pinMode(rechts_sp, INPUT);

  analogWrite(links_speed, geschwindigkeit_l);
  analogWrite(rechts_speed, geschwindigkeit_r);

  digitalWrite(links_dir, vorw);
  digitalWrite(rechts_dir, !vorw); //motoren gegenlaeufig

  digitalWrite(links_break, loesen);
  digitalWrite(rechts_break, loesen);
  
  attachInterrupt(links_sp, isr_li, FALLING);
  attachInterrupt(rechts_sp, isr_re, FALLING);
}

void loop() {
  /*
  if (led_status==1) {
    led_status=0;
  } else {
    led_status=1;
  }
  */

  if (digitalRead(joystick_u) == HIGH) {
    led_status=1;
    // vorwaerts
    if (geschwindigkeit < 255) {
      geschwindigkeit += 32;
    } else {
      geschwindigkeit = 255;
    }
    if (digitalRead(joystick_r) && !digitalRead(joystick_l)) {
      geschwindigkeit_l = geschwindigkeit;
      geschwindigkeit_r = geschwindigkeit - 32;
    } else {
      if (!digitalRead(joystick_r) && digitalRead(joystick_l)) {
        geschwindigkeit_r = geschwindigkeit;
        geschwindigkeit_l = geschwindigkeit - 32;
      } else {
        geschwindigkeit_l = geschwindigkeit;
        geschwindigkeit_r = geschwindigkeit;
      }
    }
  } else {
    if (digitalRead(joystick_r) && !digitalRead(joystick_l)) {
      geschwindigkeit_l = 64;
      geschwindigkeit_r = 0;
    } else {
      if (!digitalRead(joystick_r) && digitalRead(joystick_l)) {
        geschwindigkeit_r = 64;
        geschwindigkeit_l = 0;
      } else {
        if (digitalRead(joystick_d) && (pulse_li == 0) && (pulse_re == 0)) {
          Serial.printf("Rueck\n");
          digitalWrite(links_dir, zurr);
          digitalWrite(rechts_dir, !zurr); //motoren gegenlaeufig
          analogWrite(links_speed, 64);
          analogWrite(rechts_speed, 64);
          while (digitalRead(joystick_d)){
            delay(50);
          }
          digitalWrite(links_dir, vorw);
          digitalWrite(rechts_dir, !vorw); //motoren gegenlaeufig
          geschwindigkeit_l = 0;
          geschwindigkeit_r = 0;
          geschwindigkeit = 0;
        } else {
          geschwindigkeit_l = 0;
          geschwindigkeit_r = 0;
          geschwindigkeit = 0;
          led_status = 0;
        }
      }
    }
    
  }
  
  analogWrite(links_speed, geschwindigkeit_l);
  analogWrite(rechts_speed, geschwindigkeit_r);

  digitalWrite(led, led_status);
  Serial.printf("Motorpulse links %u rechts %u \n", pulse_li, pulse_re);
  pulse_re = 0;
  pulse_li = 0;

  delay(250);
}
