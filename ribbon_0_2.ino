

//funciones clear bit y set bit
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

// constants won't change. Used here to 
// set pin numbers:
const byte clockpin =  11;      // the number of the LED pin
const byte entrada = 0 ;  //pin de entrada del ribbon
const int numlectura = 30;  //definimos el tamaño de readings (un array de 10 valores)


// Variables will change:

volatile boolean pin_salida_clock=0;  // almacen del estado actual de la salida de sincronia
boolean pin_salida_clock_old;
unsigned long previous_micros = 0;        // will store last time LED was updated
unsigned long current_micros;
int lectura_alta;
int lectura_baja;
int lectura_suma;
int timer_3=0;
int lectura_l[numlectura]; // creamos un array de 10 valores 
int lectura_r[numlectura]; // creamos un array de 10 valores 
int index_l = 0;             // el indice de la posicin del array
int index_r = 0;             // el indice de la posicin del array
unsigned int total_l = 0;             // the running total
unsigned int total_r = 0;             // the running total
int average_l = 0;           // la media de los valores almacenados
int average_r = 0;           // la media de los valores almacenados


void setup() {
  // set the digital pin as output:
  pinMode(clockpin, OUTPUT); 
  pinMode (entrada,INPUT);
  Serial.begin(57600);
  // set prescale to 16 (1MHz ADC clock)
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;
  pinMode(12, OUTPUT);
  Setup_timer2();
  for (int thisReading = 0; thisReading < numlectura; thisReading++)
  {
    lectura_l[thisReading] = 0;
    lectura_r[thisReading] = 0;
  }  
   
}

void loop()
{
    
  
  if (pin_salida_clock) //procedemos a la lectura del valor alto
  {
    current_micros= micros();
    if (current_micros>(previous_micros+350))
    {
      total_l= total_l - lectura_l[index_l]; // subtract the last reading:
      lectura_l[index_l]=analogRead(entrada);
      total_l= total_l + lectura_l[index_l]; // add the reading to the total:  
      index_l++;
      if (index_l >= numlectura) index_l = 0;
      
      average_l = total_l / numlectura; // calculate the average:
    }
  }
  
  if (!pin_salida_clock)//procedemos a la lectura del valor bajo
  {
    current_micros= micros();
    if (current_micros>(previous_micros+350))
    {
      total_r= total_r - lectura_r[index_r]; // subtract the last reading:
      lectura_r[index_r]=analogRead(entrada);
      total_r= total_r + lectura_r[index_r]; // add the reading to the total:   
      index_r++;
      if (index_r >= numlectura) index_r = 0;
      average_r = total_r / numlectura; // calculate the average:
    }
  }


 
 
 


  

 
   
  if (timer_3 > 100) {
    timer_3= 0;
    
    Serial.print("average_l: ");
    Serial.print(average_l,DEC);
    
    Serial.print("    average_r: ");
    Serial.print(average_r,DEC);
    
    lectura_suma=average_l+average_r;
    
    Serial.print("    lecturasuma");
    Serial.println(lectura_suma,DEC);
  }

 
}

// timer2 setup
// prescaler a 32, CTC mode
void Setup_timer2() {
 
  OCR2A = 0xF0;
  OCR2B = 0x00;
  TCCR2A = 0x42;    //CTC mode
  TCCR2B = 0x03;   // CTC mode, prescaler /32
  TCNT2 = 0x00;
  TIMSK2 = 0x02;
  
}

// interrumpimos a 1kHz

ISR(TIMER2_COMPA_vect) {
  
  cli();

  OCR2A = 0xF0;
  
  pin_salida_clock=  !pin_salida_clock; // invierto conforme se invierte OC2A para saber el estado actual de la salida

  previous_micros=micros();

  timer_3++;
    
 

  sei();
  
}

