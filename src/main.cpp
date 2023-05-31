#include <Arduino.h>
#include <limits.h>
// Macro pin def
#define PHOTORESISTOR_ADC_PIN 0
#define REF_ADC_PIN 2
#define SELECT_MODE_PIN 27
#define ON_BOARD_LED_PIN 32
#define LIGHT_OUT_PIN 17
// Const value
const uint32_t TICK_TO_MINUTE = 60;
const uint32_t MINUTES_TO_TURN_OFF_LIGHT = 120;   //Default value 2h = 120min
const uint32_t MINUTES_TO_BLANKING_LIGHT = 10;  //Default value 10min
const uint32_t COUNT_OF_SAMPLES = 5;
const float MIN_DUTY_CYCLE = 0.5f;
//Enum
enum State
{
    Off,
    Blanking,
    On
};
State current_state = State::Off; // Default state

//Variable
uint16_t photoresistor_adc_val;
uint16_t reference_adc_val;
float median_photoresistor_adc_val;
float median_reference_adc_val;

uint8_t tick_count;
uint16_t minutes_count;

bool setup_ref_val_mode;
bool is_dark;
bool was_active;

// Local dunctions declaration 
bool compare_values();
void turn_off_light();
void turn_on_light();
void turn_blanking_light();
void dynamic_light_blanking();

// Setup
void setup()
{   
    //Initialize serial 
    Serial.begin(115200);

    //Initialize GPIO
    pinMode(PHOTORESISTOR_ADC_PIN, INPUT_ANALOG);
    pinMode(REF_ADC_PIN, INPUT_ANALOG);
    pinMode(LIGHT_OUT_PIN, OUTPUT);
    pinMode(ON_BOARD_LED_PIN, OUTPUT);
    pinMode(SELECT_MODE_PIN, INPUT_PULLDOWN);
    digitalWrite(LIGHT_OUT_PIN, LOW);   //Default lights are off
    digitalWrite(ON_BOARD_LED_PIN, HIGH);

    // Initialize variable
    setup_ref_val_mode = false;
    was_active = false; 
    tick_count = TICK_TO_MINUTE; // Set max value to tick count
}

void loop()
{
    //Decrease tick count 
    tick_count -= 1;
    //Read select pin mode
    setup_ref_val_mode = ! digitalRead(SELECT_MODE_PIN);
    // Reset tick count and decrease minute count
    if (tick_count == 0)
    {
        tick_count = TICK_TO_MINUTE;
        if (current_state != State::Off)    //When system is active
        {
            minutes_count -= 1; //Decrease minutes count 
            if (minutes_count == 0)
                switch (current_state)
                {
                case State::On: 
                    turn_blanking_light();
                    break;
                case State::Blanking: 
                    turn_off_light();
                    break;               
                default:
                    turn_off_light();
                    break;
                }
        }
    }
    //Check values 
    if (tick_count == TICK_TO_MINUTE ||  setup_ref_val_mode )
    {
        is_dark =  compare_values();   //Return false when is dark
        //Switch between diffrent programs
        switch (current_state)
        {
            case State::Off :
                    if (is_dark == true && was_active == false)
                        turn_on_light();
                    if (is_dark == false)
                        was_active = false;
                break;
            case State::On :
                    if (is_dark == false)
                        turn_off_light();   
                break;
            case State::Blanking :
                    if (is_dark == false)
                        turn_off_light();
                    else
                        dynamic_light_blanking();
                break;
            default:
                break;
        }

        //Print ADC values on serial
        Serial.print(median_photoresistor_adc_val);
        Serial.print('\t');
        Serial.println(median_reference_adc_val);
    }

    //Delay for 1s
    delay(1000);
}

// Return true when reference voltage is lower than photoresistor voltage 
bool compare_values(){
    median_photoresistor_adc_val = 0;
    median_reference_adc_val = 0;
    for (int i =0; i < COUNT_OF_SAMPLES; ++i){
        median_photoresistor_adc_val += 1.0f/COUNT_OF_SAMPLES * analogRead(PHOTORESISTOR_ADC_PIN);
        median_reference_adc_val += 1.0f/COUNT_OF_SAMPLES * analogRead(REF_ADC_PIN);
    }
    if (median_photoresistor_adc_val < median_reference_adc_val )
        return true;
    else
        return false;
}
void turn_off_light(){
    minutes_count = 0;
    current_state = State::Off;
    was_active = true;
    pinMode(LIGHT_OUT_PIN, OUTPUT);
    digitalWrite(LIGHT_OUT_PIN, LOW);
    digitalWrite(ON_BOARD_LED_PIN, HIGH);
}
void turn_on_light(){
    minutes_count = MINUTES_TO_TURN_OFF_LIGHT;
    current_state = State::On;
    pinMode(LIGHT_OUT_PIN, OUTPUT);
    digitalWrite(LIGHT_OUT_PIN, HIGH);  
    digitalWrite(ON_BOARD_LED_PIN, LOW);
}
void turn_blanking_light(){
    pinMode(LIGHT_OUT_PIN, PWM);
    minutes_count = MINUTES_TO_BLANKING_LIGHT;
    current_state = State::Blanking;
    dynamic_light_blanking();
}

void dynamic_light_blanking(){
    uint16_t max_duty_cycle_value = numeric_limits<uint16_t>::max();
    uint16_t diff = (uint16_t)max_duty_cycle_value *float(1 - MIN_DUTY_CYCLE);
    float fill_factor = 1 - float(minutes_count)/MINUTES_TO_BLANKING_LIGHT;
    uint16_t duty_cycle  = max_duty_cycle_value - fill_factor * diff;
    Serial.println(duty_cycle);
    pwmWrite(LIGHT_OUT_PIN, duty_cycle);
}