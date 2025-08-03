#ifndef TimerRoutines_h
#define TimerRoutines_h

#include <Arduino.h>
#include <data.h>

class TimerRoutines
{
    private:
        Data *data;
    public:
        TimerRoutines(Data *dataPtr) {
            data = dataPtr;
        }

        void updateMidiTimer(int bpm) {
            float pulsesPerSecond = (bpm / 60.0f) * 24.0f;
            uint32_t compareValue = (uint32_t)(48000000.0 / 64.0 / pulsesPerSecond);
            TC3->COUNT16.CC[0].reg = compareValue;
            while (TC3->COUNT16.STATUS.bit.SYNCBUSY);
        }

        void setupMidiTimer() {
            // compare match value
            float pps = (data->bpm / 60.0) * 24.0; // pulses per second at given bpm
            uint32_t compareValue = (uint32_t)(48000000.0 / 64.0 / pps); // 48MHz / prescaler value

            // Enable only TC4 peripheral clock
            PM->APBCMASK.reg |= PM_APBCMASK_TC4;

            // Enable GCLK for TC4 and TC5 (GCLK0 = 48 MHz usually)
            GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_TC4_TC5 |
                                GCLK_CLKCTRL_GEN_GCLK0 |
                                GCLK_CLKCTRL_CLKEN;
            while (GCLK->STATUS.bit.SYNCBUSY) {}

            // Reset TC4
            TC4->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
            while (TC4->COUNT16.STATUS.bit.SYNCBUSY || TC4->COUNT16.CTRLA.bit.SWRST) {}

            // Set TC4 to 16-bit mode with prescaler
            TC4->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 |
                                     TC_CTRLA_PRESCALER_DIV64 |
                                     TC_CTRLA_WAVEGEN_MFRQ;
            while (TC4->COUNT16.STATUS.bit.SYNCBUSY) {}

            TC4->COUNT16.CC[0].reg = compareValue;
            while (TC4->COUNT16.STATUS.bit.SYNCBUSY) {}

            // Enable interrupt on overflow
            TC4->COUNT16.INTENSET.reg = TC_INTENSET_MC0;
            NVIC_EnableIRQ(TC4_IRQn);

            // Start TC4
            TC4->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
            while (TC4->COUNT16.STATUS.bit.SYNCBUSY) {} 
        }

        void setupLedTimer(uint16_t freq) {
            // Enable TC5 peripheral clock
            PM->APBCMASK.reg |= PM_APBCMASK_TC5;          

            // Reset TC5
            TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
            while (TC5->COUNT16.STATUS.bit.SYNCBUSY || TC5->COUNT16.CTRLA.bit.SWRST) {}

            // Set TC5 to 16-bit mode with prescaler
            TC5->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 |
                                     TC_CTRLA_PRESCALER_DIV64 |
                                     TC_CTRLA_WAVEGEN_MFRQ;
            while (TC5->COUNT16.STATUS.bit.SYNCBUSY) {}

            // Calculate CC value: CC = 48,000,000 / (prescaler * frequency)
            uint32_t ccValue = 48000000 / (64 * freq);
            TC5->COUNT16.CC[0].reg = ccValue;
            while (TC5->COUNT16.STATUS.bit.SYNCBUSY) {}

            // Enable interrupt on overflow
            TC5->COUNT16.INTENSET.reg = TC_INTENSET_MC0;
            NVIC_EnableIRQ(TC5_IRQn);

            // Start TC5
            TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
            while (TC5->COUNT16.STATUS.bit.SYNCBUSY) {} 
        }
};

#endif