#pragma once
#include <Arduino.h>
#include "EEPROM.h"

#define EEPROM_SIZE 255

namespace arming
{
    bool isEnabled = 0;
    volatile bool timeKeeper = 0;
    volatile int interruptCounter;
    hw_timer_t *timer = NULL;
    portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

    void IRAM_ATTR onTimer()
    {
        portENTER_CRITICAL_ISR(&timerMux);
        timeKeeper = 1;
        portEXIT_CRITICAL_ISR(&timerMux);
    }

    int nihrom = 33;            //p20 OUTPUT
    int nihrom2 = 32;           //p19 OUTPUT
    int ParachuteBattery1 = 34; //p18 INPUT
    int ParachuteBattery2 = 35; //p17 INPUT

    int USBcheck = 37;     //p14 INPUT
    int SecondSwitch = 38; //p16 INPUT //!previously was 39
    int ThirdSwitch = 39;  //p15 INPUT //!previously was 38 (Changed from testing with Arvids)

    const int ParachutePower = 0; //!JADEFINĒ!
    const int LopyPower = 0;      //!JĀDEFINĒ!
    int out = 26;                 //p21 lampiņa/buzzer

    bool fail = 0;
    bool AlreadyCalibrated = 0;
    int USBir = 0;

    void setup()
    {
        // *NEW pin defining and settuping varetu ielikt setup funkcija AM wrapper
        pinMode(nihrom, OUTPUT);           //1. nihroma
        pinMode(nihrom2, OUTPUT);          // 2. nihromam
        pinMode(ParachuteBattery1, INPUT); //MOSFET shēmas baterijai
        pinMode(ParachuteBattery2, INPUT); //MOSFET shēmas baterijai

        pinMode(ThirdSwitch, INPUT);
        pinMode(SecondSwitch, INPUT);
        pinMode(USBcheck, INPUT);

        pinMode(out, OUTPUT); //? buzzer

        //sak timeri
        timer = timerBegin(0, 80, true);
        timerAttachInterrupt(timer, &onTimer, true);
        timerAlarmWrite(timer, 5000000, false); //! 5sek - safety Note: code goes through all sensor initializations and calibration while timer is going, should create seperate timer start function
        timerAlarmEnable(timer);

        //EEPROM setup
        EEPROM.begin(EEPROM_SIZE);
        Serial.println("Arming setup complete!");
    }

    bool isConnectedUSB()
    {
        /*
        parbauda vai ir usb rezims, vai ir pieslegta baterija (ja ir Low tad no baterijas nenak strava, jo nav izvilkts stienis 
        (ja nav izvilkts stienis tad logiski lopy runo kodu no USB jo savadak nebutu strava) un nav pieslegta ari pati baterija)
        */
        //USB check principa ir pirmais sledzis
        if (digitalRead(USBcheck) == LOW)
        {
            return 1; //USB pieslegts
        }
        else
        {
            return 0;
        }
    }

    bool checkFirstSwitch()
    {
        if (digitalRead(USBcheck) == HIGH && digitalRead(SecondSwitch) == LOW && digitalRead(ThirdSwitch) == HIGH)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    bool checkSecondSwitch()
    {
        if (digitalRead(SecondSwitch) == HIGH)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    bool checkThirdSwitch()
    {
        if (digitalRead(ThirdSwitch) == LOW)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    bool armingSuccess()
    {
        if (AlreadyCalibrated == 1 && fail == 0)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    void nihromActivate()
    {
        //if (APOGY==DETECTED){
        digitalWrite(nihrom, HIGH); //pirmais nihroms                   //* jaizskata nihrom activation - apogee detection
                                    //*POSSIBLE PROBLEM WITH TIMER INTERRUPT - SHOULD USE DIFFERENT INTERRUPT HANDLING FUNCTION (otherwise when checking timeKeeper it's already 1)
        timer = timerBegin(0, 80, true);
        timerAttachInterrupt(timer, &onTimer, true);
        timerAlarmWrite(timer, 1000000, false); //1sek
        timerAlarmEnable(timer);
        if (timeKeeper)
        {
            Serial.println("nihromi palaisti");
            digitalWrite(nihrom2, HIGH); //otrais nihroms
        }
    }

}